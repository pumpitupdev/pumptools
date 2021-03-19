#include <assert.h>

#include "capnhook/hooklib/usb-init-fix.h"

#include "util/log.h"

static enum cnh_result _cnh_usb_init_fix_default(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usb_init_fix_init(struct cnh_usbhook_irp *irp);
static enum cnh_result _cnh_usbio_emu_find_busses(struct cnh_usbhook_irp *irp);

static const cnh_usbhook_fn_t _hook_usb_init_fix_handlers[10] = {
    [CNH_USBHOOK_IRP_OP_INIT] = _cnh_usb_init_fix_init,
    [CNH_USBHOOK_IRP_OP_FIND_BUSSES] = _cnh_usbio_emu_find_busses,
    [CNH_USBHOOK_IRP_OP_FIND_DEVICES] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_OPEN] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_CLOSE] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_RESET] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_SET_ALTINTERFACE] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_SET_CONFIGURATION] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_CLAIM_INTERFACE] = _cnh_usb_init_fix_default,
    [CNH_USBHOOK_IRP_OP_CTRL_MSG] = _cnh_usb_init_fix_default,
};

static int _cnh_usb_init_fix_init_count = 0;

enum cnh_result cnh_usb_init_fix(struct cnh_usbhook_irp *irp)
{
  cnh_usbhook_fn_t handler;

  handler = _hook_usb_init_fix_handlers[irp->op];

  assert(handler != NULL);

  return handler(irp);
}

static enum cnh_result _cnh_usb_init_fix_default(struct cnh_usbhook_irp *irp)
{
  return cnh_usbhook_invoke_next(irp);
}

static enum cnh_result _cnh_usb_init_fix_init(struct cnh_usbhook_irp *irp)
{
  /* On NXA, one programmer/multiple programmers had the bright idea to call
     usb_init twice for reasons...
     This breaks stuff in the usb-0.1 backend resulting in usb_find_busses
     not even finding native devices anymore.
     Make sure we call the real usb_init ONCE ONLY to fix that fuck up */
  _cnh_usb_init_fix_init_count++;

  if (_cnh_usb_init_fix_init_count <= 1) {
    return cnh_usbhook_invoke_next(irp);
  } else {
    /* Pump Pro is even worse and calls usb_init approx. every second...
       avoid overflow of counter if we really leave this running forever.
       Furthermore, this avoids printing the log message below with ever
       init call */
    if (_cnh_usb_init_fix_init_count > 2) {
      _cnh_usb_init_fix_init_count = 2;
    } else {
      log_info("usb_init called multiple times, blocking further calls...");
    }

    return CNH_RESULT_SUCCESS;
  }
}

static enum cnh_result _cnh_usbio_emu_find_busses(struct cnh_usbhook_irp *irp)
{
  enum cnh_result result;

  /* Fixes quirks with usb_init called multiple times on NXA */
  if (_cnh_usb_init_fix_init_count <= 1) {
    result = cnh_usbhook_invoke_next(irp);
  } else {
    result = CNH_RESULT_SUCCESS;
  }

  return result;
}