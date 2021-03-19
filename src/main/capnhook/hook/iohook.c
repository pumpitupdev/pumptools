#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hook/common.h"
#include "capnhook/hook/iohook.h"
#include "capnhook/hook/lib.h"

#include "util/time.h"

#define INVALID_FILE_DEVICE (-1)

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Real funcs typedefs */
/* ------------------------------------------------------------------------------------------------------------------
 */

typedef int (*cnh_iohook_open_t)(const char *path, int oflag);
typedef FILE *(*cnh_iohook_fdopen_t)(int fd, const char *mode);
typedef ssize_t (*cnh_iohook_write_t)(int fd, const void *buf, size_t count);
typedef ssize_t (*cnh_iohook_read_t)(int fd, void *buf, size_t count);
typedef off_t (*cnh_iohook_lseek_t)(int fildes, off_t offset, int whence);
typedef int (*cnh_iohook_ioctl_t)(int fd, int request, void *data);
typedef int (*cnh_iohook_close_t)(int fd);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private helpers */
/* ------------------------------------------------------------------------------------------------------------------
 */

static void _cnh_iohook_init(void);

static enum cnh_result _cnh_iohook_invoke_real(struct cnh_iohook_irp *irp);
static enum cnh_result _cnh_iohook_invoke_real_open(struct cnh_iohook_irp *irp);
static enum cnh_result
_cnh_iohook_invoke_real_fdopen(struct cnh_iohook_irp *irp);
static enum cnh_result
_cnh_iohook_invoke_real_close(struct cnh_iohook_irp *irp);
static enum cnh_result _cnh_iohook_invoke_real_read(struct cnh_iohook_irp *irp);
static enum cnh_result
_cnh_iohook_invoke_real_write(struct cnh_iohook_irp *irp);
static enum cnh_result _cnh_iohook_invoke_real_seek(struct cnh_iohook_irp *irp);
static enum cnh_result
_cnh_iohook_invoke_real_ioctl(struct cnh_iohook_irp *irp);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------
 */

static cnh_iohook_open_t _cnh_iohook_real_open;
static cnh_iohook_fdopen_t _cnh_iohook_real_fdopen;
static cnh_iohook_close_t _cnh_iohook_real_close;
static cnh_iohook_read_t _cnh_iohook_real_read;
static cnh_iohook_write_t _cnh_iohook_real_write;
static cnh_iohook_lseek_t _cnh_iohook_real_lseek;
static cnh_iohook_ioctl_t _cnh_iohook_real_ioctl;

static const cnh_iohook_fn_t _cnh_iohook_real_handlers[7] = {
    [CNH_IOHOOK_IRP_OP_OPEN] = _cnh_iohook_invoke_real_open,
    [CNH_IOHOOK_IRP_OP_FDOPEN] = _cnh_iohook_invoke_real_fdopen,
    [CNH_IOHOOK_IRP_OP_CLOSE] = _cnh_iohook_invoke_real_close,
    [CNH_IOHOOK_IRP_OP_READ] = _cnh_iohook_invoke_real_read,
    [CNH_IOHOOK_IRP_OP_WRITE] = _cnh_iohook_invoke_real_write,
    [CNH_IOHOOK_IRP_OP_SEEK] = _cnh_iohook_invoke_real_seek,
    [CNH_IOHOOK_IRP_OP_IOCTL] = _cnh_iohook_invoke_real_ioctl,
};

static atomic_int _cnh_iohook_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_iohook_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_iohook_lock;
static cnh_iohook_fn_t *_cnh_iohook_handlers;
static size_t _cnh_iohook_nhandlers;

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

enum cnh_result cnh_iohook_push_handler(cnh_iohook_fn_t fn)
{
  cnh_iohook_fn_t *new_array;
  size_t new_size;
  enum cnh_result result;

  assert(fn != NULL);

  _cnh_iohook_init();
  pthread_mutex_lock(&_cnh_iohook_lock);

  new_size = _cnh_iohook_nhandlers + 1;
  new_array = realloc(_cnh_iohook_handlers, new_size * sizeof(cnh_iohook_fn_t));

  if (new_array != NULL) {
    _cnh_iohook_handlers = new_array;
    _cnh_iohook_handlers[_cnh_iohook_nhandlers++] = fn;
    result = CNH_RESULT_SUCCESS;
  } else {
    result = CNH_RESULT_OUT_OF_MEMORY;
  }

  pthread_mutex_unlock(&_cnh_iohook_lock);

  return result;
}

enum cnh_result cnh_iohook_invoke_next(struct cnh_iohook_irp *irp)
{
  cnh_iohook_fn_t handler;
  enum cnh_result result;

  assert(irp != NULL);
  assert(_cnh_iohook_initted > 0);

  pthread_mutex_lock(&_cnh_iohook_lock);

  assert(irp->next_handler <= _cnh_iohook_nhandlers);

  if (irp->next_handler < _cnh_iohook_nhandlers) {
    handler = _cnh_iohook_handlers[irp->next_handler];
    irp->next_handler++;
  } else {
    handler = _cnh_iohook_invoke_real;
    irp->next_handler = (size_t) -1;
  }

  pthread_mutex_unlock(&_cnh_iohook_lock);

  result = handler(irp);

  if (result != CNH_RESULT_SUCCESS) {
    irp->next_handler = (size_t) -1;
  }

  return result;
}

int cnh_iohook_open_dummy_fd()
{
  _cnh_iohook_init();

  /* We need to return some sort of valid FD to the
     caller, since we're simulating a device that
     doesn't exist. Open an FD on /dev/null. */

  /* Open read only */
  return _cnh_iohook_real_open("/dev/null", 0);
}

void cnh_iohook_close_dummy_fd(int fd)
{
  _cnh_iohook_init();

  /* Avoid hooking pipeline when using dummy handles */
  _cnh_iohook_real_close(fd);
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Hooked functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

int open(const char *path, int oflag)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (path == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return INVALID_FILE_DEVICE;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_OPEN;
  irp.fd = INVALID_FILE_DEVICE;
  irp.open_filename = path;
  irp.open_flags = oflag;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return INVALID_FILE_DEVICE;
  }

  errno = 0;

  return irp.fd;
}

FILE *fdopen(int fd, const char *mode)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == -1 || mode == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return NULL;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_FDOPEN;
  irp.fdopen_fd = fd;
  irp.fdopen_mode = mode;
  irp.fdopen_res = NULL;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return NULL;
  }

  errno = 0;

  return irp.fdopen_res;
}

int close(int fd)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == INVALID_FILE_DEVICE) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_CLOSE;
  irp.fd = fd;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  errno = 0;

  return 0;
}

ssize_t read(int fd, void *buf, size_t count)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == INVALID_FILE_DEVICE || buf == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_READ;
  irp.fd = fd;
  irp.read.bytes = buf;
  irp.read.nbytes = count;
  irp.read.pos = 0;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  assert(irp.read.pos <= irp.read.nbytes);

  errno = 0;

  return irp.read.pos;
}

ssize_t write(int fd, const void *buf, size_t count)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == INVALID_FILE_DEVICE || buf == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_WRITE;
  irp.fd = fd;
  irp.write.bytes = buf;
  irp.write.nbytes = count;
  irp.write.pos = 0;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  assert(irp.write.pos <= irp.write.nbytes);

  errno = 0;

  return irp.write.pos;
}

off_t lseek(int fd, off_t offset, int whence)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == INVALID_FILE_DEVICE) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_SEEK;
  irp.fd = fd;
  irp.seek_origin = whence;
  irp.seek_offset = offset;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  errno = 0;

  return (off_t) irp.seek_pos;
}

int ioctl(int fd, int request, void *data)
{
  struct cnh_iohook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_iohook_init();

  if (fd == INVALID_FILE_DEVICE) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_IOHOOK_IRP_OP_IOCTL;
  irp.fd = fd;
  irp.ioctl_req = request;
  irp.ioctl.bytes = data;

  result = cnh_iohook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  errno = 0;

  return irp.ioctl.pos;
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

static void _cnh_iohook_init(void)
{
  int expected;

  if (atomic_load(&_cnh_iohook_initted) > 0) {
    return;
  }

  expected = 0;

  if (!atomic_compare_exchange_strong(
          &_cnh_iohook_init_in_progress, &expected, 1)) {
    while (atomic_load(&_cnh_iohook_init_in_progress) > 1) {
      util_time_sleep_us(100);
    }

    return;
  }

  /* Check again to ensure the current thread yielded between the init'd check
   * and setting init in progress */
  if (atomic_load(&_cnh_iohook_initted) > 0) {
    /* Revert init in progress, because nothing to do anymore */
    atomic_store(&_cnh_iohook_init_in_progress, 0);
    return;
  }

  _cnh_iohook_real_open = (cnh_iohook_open_t) cnh_lib_get_func_addr("open");
  _cnh_iohook_real_fdopen =
      (cnh_iohook_fdopen_t) cnh_lib_get_func_addr("fdopen");
  _cnh_iohook_real_close = (cnh_iohook_close_t) cnh_lib_get_func_addr("close");
  _cnh_iohook_real_read = (cnh_iohook_read_t) cnh_lib_get_func_addr("read");
  _cnh_iohook_real_write = (cnh_iohook_write_t) cnh_lib_get_func_addr("write");
  _cnh_iohook_real_lseek = (cnh_iohook_lseek_t) cnh_lib_get_func_addr("lseek");
  _cnh_iohook_real_ioctl = (cnh_iohook_ioctl_t) cnh_lib_get_func_addr("ioctl");

  pthread_mutex_init(&_cnh_iohook_lock, NULL);

  atomic_store(&_cnh_iohook_initted, 1);
  atomic_store(&_cnh_iohook_init_in_progress, 0);
}

static enum cnh_result _cnh_iohook_invoke_real(struct cnh_iohook_irp *irp)
{
  cnh_iohook_fn_t handler;

  assert(irp != NULL);
  assert(irp->op < _countof(_cnh_iohook_real_handlers));

  handler = _cnh_iohook_real_handlers[irp->op];

  assert(handler != NULL);

  return handler(irp);
}

static enum cnh_result _cnh_iohook_invoke_real_open(struct cnh_iohook_irp *irp)
{
  int fd;

  assert(irp != NULL);

  fd = _cnh_iohook_real_open(irp->open_filename, irp->open_flags);

  if (fd == INVALID_FILE_DEVICE) {
    return cnh_errno_to_result(errno);
  }

  irp->fd = fd;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_iohook_invoke_real_fdopen(struct cnh_iohook_irp *irp)
{
  FILE *file;

  assert(irp != NULL);

  file = _cnh_iohook_real_fdopen(irp->fdopen_fd, irp->fdopen_mode);

  if (file == NULL) {
    return cnh_errno_to_result(errno);
  }

  irp->fdopen_res = file;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_iohook_invoke_real_close(struct cnh_iohook_irp *irp)
{
  int ok;

  assert(irp != NULL);

  ok = _cnh_iohook_real_close(irp->fd);

  if (ok < 0) {
    return cnh_errno_to_result(errno);
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_iohook_invoke_real_read(struct cnh_iohook_irp *irp)
{
  ssize_t read;

  assert(irp != NULL);

  read = _cnh_iohook_real_read(
      irp->fd,
      &irp->read.bytes[irp->read.pos],
      irp->read.nbytes - irp->read.pos);

  if (read < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->read.pos += read;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_iohook_invoke_real_write(struct cnh_iohook_irp *irp)
{
  ssize_t written;

  assert(irp != NULL);

  written = _cnh_iohook_real_write(
      irp->fd,
      &irp->write.bytes[irp->write.pos],
      irp->write.nbytes - irp->write.pos);

  if (written < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->write.pos += written;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_iohook_invoke_real_seek(struct cnh_iohook_irp *irp)
{
  off_t result;

  assert(irp != NULL);

  result = _cnh_iohook_real_lseek(
      irp->fd, (off_t) irp->seek_offset, irp->seek_origin);

  if (result < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->seek_pos = result;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_iohook_invoke_real_ioctl(struct cnh_iohook_irp *irp)
{
  int result;

  assert(irp != NULL);

  result = _cnh_iohook_real_ioctl(irp->fd, irp->ioctl_req, irp->ioctl.bytes);

  if (result < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->ioctl.pos = (size_t) result;

  return CNH_RESULT_SUCCESS;
}