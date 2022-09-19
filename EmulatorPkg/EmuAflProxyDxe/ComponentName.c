
#include "EmuAflProxy.h"

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
EmuAflProxyComponentNameGetDriverName(
    IN  EFI_COMPONENT_NAME_PROTOCOL* This,
    IN  CHAR8* Language,
    OUT CHAR16** DriverName
);

EFI_STATUS
EFIAPI
EmuAflProxyComponentNameGetControllerName(
    IN  EFI_COMPONENT_NAME_PROTOCOL* This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
    IN  CHAR8* Language,
    OUT CHAR16** ControllerName
);

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gEmuAflProxyComponentName = {
  EmuAflProxyComponentNameGetDriverName,
  EmuAflProxyComponentNameGetControllerName,
  "eng"
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL  gEmuAflProxyComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)EmuAflProxyComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)EmuAflProxyComponentNameGetControllerName,
  "en"
};

EFI_UNICODE_STRING_TABLE  mEmuAflProxyDriverNameTable[] = {
  {
    "eng;en",
    L"Emu Afl Proxy Driver"
  },
  {
    NULL,
    NULL
  }
};


EFI_STATUS
EFIAPI
EmuAflProxyComponentNameGetDriverName(
    IN  EFI_COMPONENT_NAME_PROTOCOL* This,
    IN  CHAR8* Language,
    OUT CHAR16** DriverName
)
{
    return LookupUnicodeString2(
        Language,
        This->SupportedLanguages,
        mEmuAflProxyDriverNameTable,
        DriverName,
        (BOOLEAN)(This == &gEmuAflProxyComponentName)
    );
}


EFI_STATUS
EFIAPI
EmuAflProxyComponentNameGetControllerName(
    IN  EFI_COMPONENT_NAME_PROTOCOL     *This,
    IN  EFI_HANDLE                      ControllerHandle,
    IN  EFI_HANDLE                      ChildHandle        OPTIONAL,
    IN  CHAR8                           *Language,
    OUT CHAR16                          **ControllerName
)
{
    EFI_STATUS                      Status;
    EFI_AFL_PROXY_PROTOCOL          *AflProxy;
    EMU_AFL_PROXY_PRIVATE           *Private;

    //
    // This is a device driver, so ChildHandle must be NULL.
    //
    if (ChildHandle != NULL) {
        return EFI_UNSUPPORTED;
    }

    //
    // Make sure this driver is currently managing ControllerHandle
    //
    Status = EfiTestManagedDevice(
        ControllerHandle,
        gEmuAflProxyDriverBinding.DriverBindingHandle,
        &gEmuIoThunkProtocolGuid
    );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    //
    // Get our context back
    //
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEfiAflProxyProtocolGuid,
        (VOID**)&AflProxy,
        gEmuAflProxyDriverBinding.DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    Private = EMU_AFL_PROXY_PRIVATE_DATA_FROM_THIS(AflProxy);

    return LookupUnicodeString2(
        Language,
        This->SupportedLanguages,
        Private->ControllerNameTable,
        ControllerName,
        (BOOLEAN)(This == &gEmuAflProxyComponentName)
    );
}
