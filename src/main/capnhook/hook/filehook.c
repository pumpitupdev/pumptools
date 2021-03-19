#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hook/common.h"
#include "capnhook/hook/filehook.h"
#include "capnhook/hook/lib.h"

#include "util/time.h"

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Real funcs typedefs */
/* ------------------------------------------------------------------------------------------------------------------
 */

typedef FILE *(*cnh_filehook_fopen_t)(const char *filename, const char *mode);
typedef int (*cnh_filehook_fclose_t)(FILE *stream);
typedef size_t (*cnh_filehook_fread_t)(
    void *ptr, size_t size, size_t nmemb, FILE *stream);
typedef size_t (*cnh_filehook_fwrite_t)(
    const void *ptr, size_t size, size_t nmemb, FILE *stream);
typedef char *(*cnh_filehook_fgets_t)(char *s, int size, FILE *stream);
typedef int (*cnh_filehook_fseek_t)(FILE *stream, long offset, int whence);
typedef long (*cnh_filehook_ftell_t)(FILE *stream);
typedef int (*cnh_filehook_feof_t)(FILE *stream);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private helpers */
/* ------------------------------------------------------------------------------------------------------------------
 */

static void _cnh_filehook_init(void);

static enum cnh_result _cnh_filehook_invoke_real(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fopen(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fclose(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fread(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fwrite(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fgets(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_fseek(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_ftell(struct cnh_filehook_irp *irp);
static enum cnh_result
_cnh_filehook_invoke_real_feof(struct cnh_filehook_irp *irp);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------
 */

static cnh_filehook_fopen_t _cnh_filehook_real_fopen;
static cnh_filehook_fclose_t _cnh_filehook_real_fclose;
static cnh_filehook_fread_t _cnh_filehook_real_fread;
static cnh_filehook_fwrite_t _cnh_filehook_real_fwrite;
static cnh_filehook_fgets_t _cnh_filehook_real_fgets;
static cnh_filehook_fseek_t _cnh_filehook_real_fseek;
static cnh_filehook_ftell_t _cnh_filehook_real_ftell;
static cnh_filehook_feof_t _cnh_filehook_real_feof;

static const cnh_filehook_fn_t _cnh_filehook_real_handlers[8] = {
    [CNH_FILEHOOK_IRP_OP_OPEN] = _cnh_filehook_invoke_real_fopen,
    [CNH_FILEHOOK_IRP_OP_CLOSE] = _cnh_filehook_invoke_real_fclose,
    [CNH_FILEHOOK_IRP_OP_READ] = _cnh_filehook_invoke_real_fread,
    [CNH_FILEHOOK_IRP_OP_WRITE] = _cnh_filehook_invoke_real_fwrite,
    [CNH_FILEHOOK_IRP_OP_FGETS] = _cnh_filehook_invoke_real_fgets,
    [CNH_FILEHOOK_IRP_OP_SEEK] = _cnh_filehook_invoke_real_fseek,
    [CNH_FILEHOOK_IRP_OP_TELL] = _cnh_filehook_invoke_real_ftell,
    [CNH_FILEHOOK_IRP_OP_EOF] = _cnh_filehook_invoke_real_feof,
};

static atomic_int _cnh_filehook_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_filehook_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_filehook_lock;
static cnh_filehook_fn_t *_cnh_filehook_handlers;
static size_t _cnh_filehook_nhandlers;

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

enum cnh_result cnh_filehook_push_handler(cnh_filehook_fn_t fn)
{
  cnh_filehook_fn_t *new_array;
  size_t new_size;
  enum cnh_result result;

  assert(fn != NULL);

  _cnh_filehook_init();
  pthread_mutex_lock(&_cnh_filehook_lock);

  new_size = _cnh_filehook_nhandlers + 1;
  new_array =
      realloc(_cnh_filehook_handlers, new_size * sizeof(cnh_filehook_fn_t));

  if (new_array != NULL) {
    _cnh_filehook_handlers = new_array;
    _cnh_filehook_handlers[_cnh_filehook_nhandlers++] = fn;
    result = CNH_RESULT_SUCCESS;
  } else {
    result = CNH_RESULT_OUT_OF_MEMORY;
  }

  pthread_mutex_unlock(&_cnh_filehook_lock);

  return result;
}

enum cnh_result cnh_filehook_invoke_next(struct cnh_filehook_irp *irp)
{
  cnh_filehook_fn_t handler;
  enum cnh_result result;

  assert(irp != NULL);
  assert(_cnh_filehook_initted > 0);

  pthread_mutex_lock(&_cnh_filehook_lock);

  assert(irp->next_handler <= _cnh_filehook_nhandlers);

  if (irp->next_handler < _cnh_filehook_nhandlers) {
    handler = _cnh_filehook_handlers[irp->next_handler];
    irp->next_handler++;
  } else {
    handler = _cnh_filehook_invoke_real;
    irp->next_handler = (size_t) -1;
  }

  pthread_mutex_unlock(&_cnh_filehook_lock);

  result = handler(irp);

  if (result != CNH_RESULT_SUCCESS) {
    irp->next_handler = (size_t) -1;
  }

  return result;
}

FILE *cnh_filehook_open_dummy_file_handle()
{
  _cnh_filehook_init();

  /* We need to return some sort of valid FD to the
     caller, since we're simulating a device that
     doesn't exist. Open an FD on /dev/null. */

  return _cnh_filehook_real_fopen("/dev/null", "r");
}

void cnh_filehook_close_dummy_file_handle(FILE *file)
{
  _cnh_filehook_init();

  /* Avoid hooking pipeline when using dummy handles */
  _cnh_filehook_real_fclose(file);
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Hooked functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

FILE *fopen(const char *filename, const char *mode)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (filename == NULL || mode == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return NULL;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_OPEN;
  irp.file = NULL;
  irp.open_filename = filename;
  irp.open_mode = mode;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return NULL;
  }

  errno = 0;

  return irp.file;
}

int fclose(FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return EOF;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_CLOSE;
  irp.file = stream;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return EOF;
  }

  errno = 0;

  return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (ptr == NULL || stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return 0;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_READ;
  irp.file = stream;
  irp.read.bytes = ptr;
  irp.read.nbytes = size * nmemb;
  irp.read.pos = 0;
  /* To ensure we don't mess up any calls that are just passed through, we have
     to drag along the original size and nmemb parameters and apply them to the
     real call at the end of the chain if the iobuffer was not modified */
  irp.orig_read_write_size = size;
  irp.orig_read_write_nmemb = nmemb;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return 0;
  }

  errno = 0;

  return irp.read.pos / size;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_WRITE;
  irp.file = stream;
  irp.write.bytes = ptr;
  irp.write.nbytes = size * nmemb;
  irp.write.pos = 0;
  /* To ensure we don't mess up any calls that are just passed through, we have
     to drag along the original size and nmemb parameters and apply them to the
     real call at the end of the chain if the iobuffer was not modified */
  irp.orig_read_write_size = size;
  irp.orig_read_write_nmemb = nmemb;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return 0;
  }

  errno = 0;

  return irp.write.pos / size;
}

char *fgets(char *s, int size, FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (s == NULL || stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return NULL;
  }

  /* Redirect this to fread */
  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_FGETS;
  irp.file = stream;
  irp.read.bytes = (uint8_t *) s;
  irp.read.nbytes = (size_t) size;
  irp.read.pos = 0;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return NULL;
  }

  errno = 0;

  return (char *) irp.read.bytes;
}

int fseek(FILE *stream, long offset, int whence)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_SEEK;
  irp.file = stream;
  irp.seek_origin = whence;
  irp.seek_offset = offset;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  errno = 0;

  return 0;
}

void rewind(FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return;
  }

  /* Seek to beginning */
  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_SEEK;
  irp.file = stream;
  irp.seek_origin = SEEK_SET;
  irp.seek_offset = 0;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return;
  }
}

long ftell(FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (stream == NULL) {
    errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_TELL;
  irp.file = stream;
  /* Return value */
  irp.tell_offset = 0;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    errno = cnh_result_to_errno(result);
    return -1;
  }

  errno = 0;

  return (long) irp.tell_offset;
}

int feof(FILE *stream)
{
  struct cnh_filehook_irp irp;
  enum cnh_result result;

  /* Ensure module is initialized */
  _cnh_filehook_init();

  if (stream == NULL) {
    /* Must be EBADF according to man pages */
    errno = EBADF;
    return -1;
  }

  memset(&irp, 0, sizeof(irp));
  irp.op = CNH_FILEHOOK_IRP_OP_EOF;
  irp.file = stream;
  /* Return value */
  irp.eof = false;

  result = cnh_filehook_invoke_next(&irp);

  if (result != CNH_RESULT_SUCCESS) {
    /* Function is never supposed to fail and EBADF is the only allowed error
     * code */
    errno = EBADF;
    return -1;
  }

  errno = 0;

  return irp.eof ? 1 : 0;
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

static void _cnh_filehook_init(void)
{
  int expected;

  if (atomic_load(&_cnh_filehook_initted) > 0) {
    return;
  }

  expected = 0;

  if (!atomic_compare_exchange_strong(
          &_cnh_filehook_init_in_progress, &expected, 1)) {
    while (atomic_load(&_cnh_filehook_init_in_progress) > 1) {
      util_time_sleep_us(100);
    }

    return;
  }

  /* Check again to ensure the current thread yielded between the init'd check
   * and setting init in progress */
  if (atomic_load(&_cnh_filehook_initted) > 0) {
    /* Revert init in progress, because nothing to do anymore */
    atomic_store(&_cnh_filehook_init_in_progress, 0);
    return;
  }

  _cnh_filehook_real_fopen =
      (cnh_filehook_fopen_t) cnh_lib_get_func_addr("fopen");
  _cnh_filehook_real_fclose =
      (cnh_filehook_fclose_t) cnh_lib_get_func_addr("fclose");
  _cnh_filehook_real_fread =
      (cnh_filehook_fread_t) cnh_lib_get_func_addr("fread");
  _cnh_filehook_real_fwrite =
      (cnh_filehook_fwrite_t) cnh_lib_get_func_addr("fwrite");
  _cnh_filehook_real_fgets =
      (cnh_filehook_fgets_t) cnh_lib_get_func_addr("fgets");
  _cnh_filehook_real_fseek =
      (cnh_filehook_fseek_t) cnh_lib_get_func_addr("fseek");
  _cnh_filehook_real_ftell =
      (cnh_filehook_ftell_t) cnh_lib_get_func_addr("ftell");
  _cnh_filehook_real_feof = (cnh_filehook_feof_t) cnh_lib_get_func_addr("feof");

  pthread_mutex_init(&_cnh_filehook_lock, NULL);

  atomic_store(&_cnh_filehook_initted, 1);
  atomic_store(&_cnh_filehook_init_in_progress, 0);
}

static enum cnh_result _cnh_filehook_invoke_real(struct cnh_filehook_irp *irp)
{
  cnh_filehook_fn_t handler;

  assert(irp != NULL);
  assert(irp->op < _countof(_cnh_filehook_real_handlers));

  handler = _cnh_filehook_real_handlers[irp->op];

  assert(handler != NULL);

  return handler(irp);
}

static enum cnh_result
_cnh_filehook_invoke_real_fopen(struct cnh_filehook_irp *irp)
{
  FILE *file;

  assert(irp != NULL);

  file = _cnh_filehook_real_fopen(irp->open_filename, irp->open_mode);

  if (!file) {
    return cnh_errno_to_result(errno);
  }

  irp->file = file;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_fclose(struct cnh_filehook_irp *irp)
{
  int res;

  assert(irp != NULL);

  res = _cnh_filehook_real_fclose(irp->file);

  if (res == EOF) {
    return cnh_errno_to_result(errno);
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_fread(struct cnh_filehook_irp *irp)
{
  size_t read;

  assert(irp != NULL);

  /* Check if any previous hook modified the buffer and use original parameters
     otherwise to ensure the original call is correctly applied to the real
     function */
  if (irp->read.pos != 0 ||
      irp->read.nbytes !=
          irp->orig_read_write_nmemb * irp->orig_read_write_size) {
    read = _cnh_filehook_real_fread(
        &irp->read.bytes[irp->read.pos],
        irp->read.nbytes - irp->read.pos,
        1,
        irp->file);

    if (read < 1 && ferror(irp->file) != 0) {
      return cnh_errno_to_result(errno);
    }

    irp->read.pos += irp->read.nbytes - irp->read.pos;
  } else {
    read = _cnh_filehook_real_fread(
        irp->read.bytes,
        irp->orig_read_write_size,
        irp->orig_read_write_nmemb,
        irp->file);

    if (read < 1 && ferror(irp->file) != 0) {
      return cnh_errno_to_result(errno);
    }

    irp->read.pos = read * irp->orig_read_write_size;
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_fwrite(struct cnh_filehook_irp *irp)
{
  size_t written;

  assert(irp != NULL);

  /* Check if any previous hook modified the buffer and use original parameters
     otherwise to ensure the original call is correctly applied to the real
     function */
  if (irp->write.pos != 0 ||
      irp->write.nbytes !=
          irp->orig_read_write_nmemb * irp->orig_read_write_size) {
    written = _cnh_filehook_real_fwrite(
        &irp->write.bytes[irp->write.pos],
        irp->write.nbytes - irp->write.pos,
        1,
        irp->file);

    if (written < 1 && ferror(irp->file) != 0) {
      return cnh_errno_to_result(errno);
    }

    irp->write.pos += irp->write.nbytes - irp->write.pos;
  } else {
    written = _cnh_filehook_real_fwrite(
        irp->write.bytes,
        irp->orig_read_write_size,
        irp->orig_read_write_nmemb,
        irp->file);

    if (written < 1 && ferror(irp->file) != 0) {
      return cnh_errno_to_result(errno);
    }

    irp->write.pos += written * irp->orig_read_write_size;
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_fgets(struct cnh_filehook_irp *irp)
{
  char *res;

  res = _cnh_filehook_real_fgets(
      (char *) irp->read.bytes, irp->read.nbytes, irp->file);

  if (res == NULL) {
    /* eof */
    if (errno == 0) {
      irp->read.bytes = NULL;
      return CNH_RESULT_SUCCESS;
    } else {
      return cnh_errno_to_result(errno);
    }
  } else {
    return CNH_RESULT_SUCCESS;
  }
}

static enum cnh_result
_cnh_filehook_invoke_real_fseek(struct cnh_filehook_irp *irp)
{
  int res;

  assert(irp != NULL);

  res = _cnh_filehook_real_fseek(
      irp->file, (long) irp->seek_offset, irp->seek_origin);

  if (res != 0) {
    return cnh_errno_to_result(errno);
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_ftell(struct cnh_filehook_irp *irp)
{
  long res;

  assert(irp != NULL);

  res = _cnh_filehook_real_ftell(irp->file);

  if (res < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->tell_offset = (uint64_t) res;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_filehook_invoke_real_feof(struct cnh_filehook_irp *irp)
{
  int res;

  assert(irp != NULL);

  res = _cnh_filehook_real_feof(irp->file);

  if (res < 0) {
    return cnh_errno_to_result(errno);
  }

  irp->eof = res > 0;

  return CNH_RESULT_SUCCESS;
}