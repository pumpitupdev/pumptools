#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>
#include <util/mem.h>

#include "capnhook/hooklib/usb-emu.h"

#include "util/log.h"
#include "util/time.h"

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private helpers */
/* ------------------------------------------------------------------------------------------------------------------
 */

static void _cnh_usb_emu_init(void);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Hooks to usbhook module */
/* ------------------------------------------------------------------------------------------------------------------
 */

static enum cnh_result
_cnh_usb_emu_usbhook_default(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usb_emu_usbhook_noop(struct cnh_usbhook_irp *irp);
static enum cnh_result
_cnh_usb_emu_usbhook_find_busses(struct cnh_usbhook_irp *irp);
static enum cnh_result
_cnh_usb_emu_usbhook_find_devices(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usb_emu_usbhook_open(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usb_emu_usbhook_close(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usb_emu_usbhook_reset(struct cnh_usbhook_irp *irp);
static enum cnh_result
_cnh_usb_emu_usbhook_ctrl_msg(struct cnh_usbhook_irp *irp);

static const cnh_usbhook_fn_t _cnh_usb_emu_usb_handlers[10] = {
    [CNH_USBHOOK_IRP_OP_INIT] = _cnh_usb_emu_usbhook_default,
    [CNH_USBHOOK_IRP_OP_FIND_BUSSES] = _cnh_usb_emu_usbhook_find_busses,
    [CNH_USBHOOK_IRP_OP_FIND_DEVICES] = _cnh_usb_emu_usbhook_find_devices,
    [CNH_USBHOOK_IRP_OP_OPEN] = _cnh_usb_emu_usbhook_open,
    [CNH_USBHOOK_IRP_OP_CLOSE] = _cnh_usb_emu_usbhook_close,
    [CNH_USBHOOK_IRP_OP_RESET] = _cnh_usb_emu_usbhook_reset,
    [CNH_USBHOOK_IRP_OP_SET_ALTINTERFACE] = _cnh_usb_emu_usbhook_noop,
    [CNH_USBHOOK_IRP_OP_SET_CONFIGURATION] = _cnh_usb_emu_usbhook_noop,
    [CNH_USBHOOK_IRP_OP_CLAIM_INTERFACE] = _cnh_usb_emu_usbhook_noop,
    [CNH_USBHOOK_IRP_OP_CTRL_MSG] = _cnh_usb_emu_usbhook_ctrl_msg,
};

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------
 */

struct usb_dev_handle {
  struct cnh_usb_emu_virtdev *virtdev;
};

struct cnh_usb_emu_virtdev {
  const struct cnh_usb_emu_virtdev_ep *ep;
  bool real_dev_avail;
  bool enumerated;
  struct usb_dev_handle usb_dev_handle;
};

static atomic_int _cnh_usb_emu_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_usb_emu_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_usb_emu_virtdevs_lock;
static struct cnh_usb_emu_virtdev *_cnh_usb_emu_virtdevs;
static size_t _cnh_usb_emu_nvirtdevs;

static struct cnh_usb_emu_virtdev *
_cnh_usb_emu_get_virtdev(uint16_t vid, uint16_t pid);
static struct cnh_usb_emu_virtdev *
_cnh_usb_emu_get_virtdev_by_handle(usb_dev_handle *handle);
static struct usb_device *
_cnh_usb_emu_create_fakedev(uint16_t vid, uint16_t pid);

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

void cnh_usb_emu_add_virtdevep(const struct cnh_usb_emu_virtdev_ep *virtdevep)
{
  struct cnh_usb_emu_virtdev *new_array;
  size_t new_size;

  assert(virtdevep->enumerate);
  assert(virtdevep->open);
  assert(virtdevep->reset);
  assert(virtdevep->control_msg);
  assert(virtdevep->close);

  /* Ensure initialized */
  _cnh_usb_emu_init();

  pthread_mutex_lock(&_cnh_usb_emu_virtdevs_lock);

  new_size = _cnh_usb_emu_nvirtdevs + 1;
  new_array = realloc(
      _cnh_usb_emu_virtdevs, new_size * sizeof(struct cnh_usb_emu_virtdev));

  if (new_array != NULL) {
    _cnh_usb_emu_virtdevs = new_array;

    _cnh_usb_emu_virtdevs[_cnh_usb_emu_nvirtdevs].enumerated = false;
    _cnh_usb_emu_virtdevs[_cnh_usb_emu_nvirtdevs].real_dev_avail = false;
    _cnh_usb_emu_virtdevs[_cnh_usb_emu_nvirtdevs].ep = virtdevep;
    _cnh_usb_emu_virtdevs[_cnh_usb_emu_nvirtdevs].usb_dev_handle.virtdev =
        &_cnh_usb_emu_virtdevs[_cnh_usb_emu_nvirtdevs];
    _cnh_usb_emu_nvirtdevs++;
  } else {
    log_die("Out of memory");
  }

  pthread_mutex_unlock(&_cnh_usb_emu_virtdevs_lock);
}

enum cnh_result cnh_usb_emu(struct cnh_usbhook_irp *irp)
{
  cnh_usbhook_fn_t handler;

  /* Ensure initialized */
  _cnh_usb_emu_init();

  handler = _cnh_usb_emu_usb_handlers[irp->op];

  assert(handler != NULL);

  return handler(irp);
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Hooked functions to usbhook module */
/* ------------------------------------------------------------------------------------------------------------------
 */

static enum cnh_result _cnh_usb_emu_usbhook_default(struct cnh_usbhook_irp *irp)
{
  return cnh_usbhook_invoke_next(irp);
}

static enum cnh_result _cnh_usb_emu_usbhook_noop(struct cnh_usbhook_irp *irp)
{
  struct cnh_usb_emu_virtdev *virtdev;

  assert(irp->handle);

  virtdev = _cnh_usb_emu_get_virtdev_by_handle(irp->handle);

  /* Not a virtual device handle, probably real handle from libusb */
  if (virtdev == NULL) {
    return cnh_usbhook_invoke_next(irp);
  }

  /* No-op */

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result
_cnh_usb_emu_usbhook_find_busses(struct cnh_usbhook_irp *irp)
{
  enum cnh_result result;

  result = cnh_usbhook_invoke_next(irp);

  // inject another custom bus for virtual devices
  if (result == CNH_RESULT_SUCCESS) {
    log_debug("Injecting usb bus for virtual devices");
    irp->find_busses_res_num_busses++;
  }

  return result;
}

static enum cnh_result
_cnh_usb_emu_usbhook_find_devices(struct cnh_usbhook_irp *irp)
{
  enum cnh_result result;
  struct usb_bus *it;
  int fakedev_count;

  fakedev_count = 0;

  /* first, populate the list with real devices */
  result = cnh_usbhook_invoke_next(irp);

  /* Iterate to the end of the list and add our stuff keeping real devices */
  it = usb_get_busses();

  /* Iterate all busses */
  do {
    /* First, iterate all devices on each bus and check for all real
       devices connected which are also part of our hook list */
    if (it != NULL) {
      /* Iterate devices on the current bus */
      struct usb_device *dev = it->devices;

      while (dev != NULL) {
        struct cnh_usb_emu_virtdev *virt_dev = _cnh_usb_emu_get_virtdev(
            (uint16_t) dev->descriptor.idVendor,
            (uint16_t) dev->descriptor.idProduct);

        /* Real device connected for a hook */
        if (virt_dev) {
          log_debug(
              "Detected real %04X:%04X device for virtual one",
              dev->descriptor.idVendor,
              dev->descriptor.idProduct);
          virt_dev->real_dev_avail = true;
        }

        dev = dev->next;
      }
    }

    /* it == NULL, nothing found on current bus which is our last bus
       we added on usb_find_busses. We put our fakedevs on that bus */
    if (it == NULL || it->next == NULL) {
      struct usb_bus *fakebus;
      struct usb_device *fakebus_dev_tail;

      /* Create an additional bus for our fakedevs */
      fakebus = (struct usb_bus *) util_xmalloc(sizeof(struct usb_bus));
      memset(fakebus, 0, sizeof(struct usb_bus));

      /* Make sure to modify the linked list correctly. If we fuck up,
         this works fine until Fiesta where they started to iterate
         everything in a different manner (-> crash) */
      if (it == NULL) {
        usb_busses = fakebus;
        /* Bus empty, no prev bus */
        fakebus->prev = NULL;
      } else {
        /* Append at end of bus chain */
        it->next = fakebus;
        /* Keep doubly linked list intact */
        fakebus->prev = it->next;
      }

      fakebus_dev_tail = NULL;

      pthread_mutex_lock(&_cnh_usb_emu_virtdevs_lock);

      for (size_t i = 0; i < _cnh_usb_emu_nvirtdevs; i++) {
        struct cnh_usb_emu_virtdev *virtdev = &_cnh_usb_emu_virtdevs[i];

        virtdev->enumerated = virtdev->ep->enumerate(virtdev->real_dev_avail);

        if (virtdev->enumerated) {
          if (virtdev->real_dev_avail) {
            /* Suppress real device and detour everything to
               our hook */
            log_debug(
                "Suppressing real device %04X:%04X",
                virtdev->ep->vid,
                virtdev->ep->pid);
          } else {
            /* Create a fakedev and add it to the bus */
            struct usb_device *fakedev;

            {
              /* Print this once. Pump Pro calls this
                 function every second */
              static int counter = 0;

              if (counter++ == 0) {
                log_info(
                    "Injecting fake usb device %04X:%04X into bus",
                    virtdev->ep->vid,
                    virtdev->ep->pid);
              } else {
                counter = 1;
              }
            }

            fakedev =
                _cnh_usb_emu_create_fakedev(virtdev->ep->vid, virtdev->ep->pid);

            /* Append to linked list */
            if (!fakebus->devices) {
              /* Set root on bus */
              fakebus->devices = fakedev;
            }

            /* Cur element */
            fakedev->prev = fakebus_dev_tail;
            fakedev->next = NULL;

            /* Update prev element's next */
            if (fakebus_dev_tail) {
              fakebus_dev_tail->next = fakedev;
            }

            /* tail update */
            fakebus_dev_tail = fakedev;

            fakedev_count++;
          }
        } else {
          log_debug(
              "Reject enumerate hook for %04X:%04X",
              virtdev->ep->vid,
              virtdev->ep->pid);
        }
      }

      pthread_mutex_unlock(&_cnh_usb_emu_virtdevs_lock);

      break;
    }
    /* End of if block which handles fakedevs and hooking */

    it = it->next;
  } while (it != NULL);

  {
    /* Print this once. Pump Pro calls this
        function every second */
    static int counter = 0;

    if (counter++ == 0) {
      log_info("Added %d fakedevs to usb bus", fakedev_count);
    } else {
      counter = 1;
    }
  }

  /* include our fakedev count as well */
  return result + fakedev_count;
}

static enum cnh_result _cnh_usb_emu_usbhook_open(struct cnh_usbhook_irp *irp)
{
  struct cnh_usb_emu_virtdev *virtdev;
  enum cnh_result result;

  virtdev = _cnh_usb_emu_get_virtdev(
      (uint16_t) irp->open_usb_dev->descriptor.idVendor,
      (uint16_t) irp->open_usb_dev->descriptor.idProduct);

  if (virtdev == NULL) {
    return cnh_usbhook_invoke_next(irp);
  }

  if (!virtdev->enumerated) {
    log_debug(
        "Virtdev %04X:%04X not enumerated, skipping open",
        virtdev->ep->vid,
        virtdev->ep->pid);
    return cnh_usbhook_invoke_next(irp);
  }

  result = virtdev->ep->open();

  if (result == CNH_RESULT_SUCCESS) {
    /* Return handle, use virtual device as fake handle */
    irp->handle = &virtdev->usb_dev_handle;
  }

  return result;
}

static enum cnh_result _cnh_usb_emu_usbhook_close(struct cnh_usbhook_irp *irp)
{
  struct cnh_usb_emu_virtdev *virtdev;

  assert(irp->handle);

  virtdev = _cnh_usb_emu_get_virtdev_by_handle(irp->handle);

  /* Not a virtual device handle, probably real handle from libusb */
  if (virtdev == NULL) {
    return cnh_usbhook_invoke_next(irp);
  }

  virtdev->ep->close();

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usb_emu_usbhook_reset(struct cnh_usbhook_irp *irp)
{
  struct cnh_usb_emu_virtdev *virtdev;

  assert(irp->handle);

  virtdev = _cnh_usb_emu_get_virtdev_by_handle(irp->handle);

  /* Not a virtual device handle, probably real handle from libusb */
  if (virtdev == NULL) {
    return cnh_usbhook_invoke_next(irp);
  }

  return virtdev->ep->reset();
}

static enum cnh_result
_cnh_usb_emu_usbhook_ctrl_msg(struct cnh_usbhook_irp *irp)
{
  struct cnh_usb_emu_virtdev *virtdev;

  assert(irp->handle);

  virtdev = _cnh_usb_emu_get_virtdev_by_handle(irp->handle);

  /* Not a virtual device handle, probably real handle from libusb */
  if (virtdev == NULL) {
    return cnh_usbhook_invoke_next(irp);
  }

  return virtdev->ep->control_msg(
      irp->ctrl_req_type,
      irp->ctrl_req,
      irp->ctrl_value,
      irp->ctrl_index,
      &irp->ctrl_buffer,
      irp->ctrl_timeout);
}

/* ------------------------------------------------------------------------------------------------------------------
 */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------
 */

void _cnh_usb_emu_init(void)
{
  int expected;

  if (atomic_load(&_cnh_usb_emu_initted) > 0) {
    return;
  }

  expected = 0;

  if (!atomic_compare_exchange_strong(
          &_cnh_usb_emu_init_in_progress, &expected, 1)) {
    while (atomic_load(&_cnh_usb_emu_init_in_progress) > 1) {
      util_time_sleep_us(100);
    }

    return;
  }

  /* Check again to ensure the current thread yielded between the init'd check
   * and setting init in progress */
  if (atomic_load(&_cnh_usb_emu_initted) > 0) {
    /* Revert init in progress, because nothing to do anymore */
    atomic_store(&_cnh_usb_emu_init_in_progress, 0);
    return;
  }

  pthread_mutex_init(&_cnh_usb_emu_virtdevs_lock, NULL);

  atomic_store(&_cnh_usb_emu_initted, 1);
  atomic_store(&_cnh_usb_emu_init_in_progress, 0);
}

static struct cnh_usb_emu_virtdev *
_cnh_usb_emu_get_virtdev(uint16_t vid, uint16_t pid)
{
  struct cnh_usb_emu_virtdev *virtdev;
  struct cnh_usb_emu_virtdev *tmp;

  virtdev = NULL;

  pthread_mutex_lock(&_cnh_usb_emu_virtdevs_lock);

  for (size_t i = 0; i < _cnh_usb_emu_nvirtdevs; i++) {
    tmp = &_cnh_usb_emu_virtdevs[i];

    if (tmp->ep->vid == vid && tmp->ep->pid == pid) {
      virtdev = tmp;
      break;
    }
  }

  pthread_mutex_unlock(&_cnh_usb_emu_virtdevs_lock);

  return virtdev;
}

static struct cnh_usb_emu_virtdev *
_cnh_usb_emu_get_virtdev_by_handle(usb_dev_handle *handle)
{
  struct cnh_usb_emu_virtdev *virtdev;
  struct cnh_usb_emu_virtdev *tmp;

  virtdev = NULL;

  pthread_mutex_lock(&_cnh_usb_emu_virtdevs_lock);

  for (size_t i = 0; i < _cnh_usb_emu_nvirtdevs; i++) {
    tmp = &_cnh_usb_emu_virtdevs[i];

    if (&tmp->usb_dev_handle == handle) {
      virtdev = tmp;
      break;
    }
  }

  pthread_mutex_unlock(&_cnh_usb_emu_virtdevs_lock);

  return virtdev;
}

static struct usb_device *
_cnh_usb_emu_create_fakedev(uint16_t vid, uint16_t pid)
{
  struct usb_device *fakedev =
      (struct usb_device *) util_xmalloc(sizeof(struct usb_device));
  memset(fakedev, 0, sizeof(struct usb_device));

  /* Populate fakedev */
  fakedev->descriptor.idVendor = vid;
  fakedev->descriptor.idProduct = pid;

  /* Mark as fakedev */
  fakedev->descriptor.bcdDevice = 0xBEEF;

  fakedev->config = (struct usb_config_descriptor *) util_xmalloc(
      sizeof(struct usb_config_descriptor));
  memset(fakedev->config, 0, sizeof(struct usb_config_descriptor));

  fakedev->config->interface =
      (struct usb_interface *) util_xmalloc(sizeof(struct usb_interface));
  memset(fakedev->config->interface, 0, sizeof(struct usb_interface));

  fakedev->config->interface->altsetting =
      (struct usb_interface_descriptor *) util_xmalloc(
          sizeof(struct usb_interface_descriptor));
  memset(
      fakedev->config->interface->altsetting,
      0,
      sizeof(struct usb_interface_descriptor));

  return fakedev;
}