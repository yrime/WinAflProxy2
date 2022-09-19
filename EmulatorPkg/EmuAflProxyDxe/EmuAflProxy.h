
#ifndef _EMU_AFL_PROXY_H_
#define _EMU_AFL_PROXY_H_

#include "PiDxe.h"

#include <Protocol/EmuIoThunk.h>
#include <Protocol/AflProxy.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

extern EFI_DRIVER_BINDING_PROTOCOL   gEmuAflProxyDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL   gEmuAflProxyComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL  gEmuAflProxyComponentName2;

#define EMU_AFL_PROXY_PRIVATE_SIGNATURE  SIGNATURE_32 ('E', 'A', 'F', 'l')

typedef struct {
    UINTN                               Signature;
    EMU_IO_THUNK_PROTOCOL               *IoThunk;
    EFI_AFL_PROXY_PROTOCOL              AflProxy;
    EFI_AFL_PROXY_PROTOCOL              *Io;
    EFI_UNICODE_STRING_TABLE            *ControllerNameTable;
} EMU_AFL_PROXY_PRIVATE;

#define EMU_AFL_PROXY_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      EMU_AFL_PROXY_PRIVATE, \
      AflProxy, \
      EMU_AFL_PROXY_PRIVATE_SIGNATURE \
      )

#endif
