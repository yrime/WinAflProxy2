

#include "EmuAflProxy.h"


EFI_STATUS
EFIAPI
EmuAflMaybeLog(
    IN EFI_AFL_PROXY_PROTOCOL   *This,
    IN UINT32                   date
)
{
    EFI_STATUS                      Status;
    EMU_AFL_PROXY_PRIVATE           *Private;

    Status = EFI_UNSUPPORTED;

    Private = EMU_AFL_PROXY_PRIVATE_DATA_FROM_THIS(This);

    Status = Private->Io->afl_maybe_log(Private->Io, date);
    if (EFI_ERROR(Status)) {
        goto Done;
    }
/*
    AddUnicodeString2(
        "eng",
        gEmuAflProxyComponentName.SupportedLanguages,
        &Private->ControllerNameTable,
        Private->IoThunk->ConfigString,
        TRUE
    );

    AddUnicodeString2(
        "en",
        gEmuAflProxyComponentName.SupportedLanguages,
        &Private->ControllerNameTable,
        Private->IoThunk->ConfigString,
        FALSE
    );
*/
Done:
    return Status;
}


EFI_STATUS
EFIAPI
EmuAflProxyDriverBindingSupported(
    IN  EFI_DRIVER_BINDING_PROTOCOL* This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_DEVICE_PATH_PROTOCOL* RemainingDevicePath
)
{
    EFI_STATUS             Status;
    EMU_IO_THUNK_PROTOCOL* EmuIoThunk;

    //
    // Open the IO Abstraction(s) needed to perform the supported test
    //
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEmuIoThunkProtocolGuid,
        (VOID**)&EmuIoThunk,
        This->DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_BY_DRIVER
    );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Make sure GUID is for a File System handle.
    //
    Status = EFI_UNSUPPORTED;
    if (CompareGuid(EmuIoThunk->Protocol, &gEfiAflProxyProtocolGuid)) {
        Status = EFI_SUCCESS;
    }

    //
    // Close the I/O Abstraction(s) used to perform the supported test
    //
    gBS->CloseProtocol(
        ControllerHandle,
        &gEmuIoThunkProtocolGuid,
        This->DriverBindingHandle,
        ControllerHandle
    );

    return Status;
}

EFI_STATUS
EFIAPI
EmuAflProxyDriverBindingStart(
    IN  EFI_DRIVER_BINDING_PROTOCOL* This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_DEVICE_PATH_PROTOCOL* RemainingDevicePath
)
{
    EFI_STATUS                      Status;
    EMU_IO_THUNK_PROTOCOL           *EmuIoThunk;
    EMU_AFL_PROXY_PRIVATE           *Private;

    Private = NULL;

    //
    // Open the IO Abstraction(s) needed
    //
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEmuIoThunkProtocolGuid,
        (VOID**)&EmuIoThunk,
        This->DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_BY_DRIVER
    );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Validate GUID
    //
    if (!CompareGuid(EmuIoThunk->Protocol, &gEfiAflProxyProtocolGuid)) {
        Status = EFI_UNSUPPORTED;
        goto Done;
    }

    Private = AllocateZeroPool(sizeof(EMU_AFL_PROXY_PRIVATE));
    if (Private == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
    }

    Status = EmuIoThunk->Open(EmuIoThunk);
    if (EFI_ERROR(Status)) {
        goto Done;
    }

    Private->Signature = EMU_AFL_PROXY_PRIVATE_SIGNATURE;
    Private->IoThunk = EmuIoThunk;
    Private->Io = EmuIoThunk->Interface;

    Private->AflProxy.afl_maybe_log = EmuAflMaybeLog;

    Private->ControllerNameTable = NULL;

    AddUnicodeString2(
        "eng",
        gEmuAflProxyComponentName.SupportedLanguages,
        &Private->ControllerNameTable,
        EmuIoThunk->ConfigString,
        TRUE
    );

    AddUnicodeString2(
        "en",
        gEmuAflProxyComponentName2.SupportedLanguages,
        &Private->ControllerNameTable,
        EmuIoThunk->ConfigString,
        FALSE
    );

    Status = gBS->InstallMultipleProtocolInterfaces(
        &ControllerHandle,
        &gEfiAflProxyProtocolGuid,
        &Private->AflProxy,
        NULL
    );

Done:
    if (EFI_ERROR(Status)) {
        if (Private != NULL) {
            if (Private->ControllerNameTable != NULL) {
                FreeUnicodeStringTable(Private->ControllerNameTable);
            }

            gBS->FreePool(Private);
        }

        gBS->CloseProtocol(
            ControllerHandle,
            &gEmuIoThunkProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle
        );
    }

    return Status;
}

EFI_STATUS
EFIAPI
EmuAflProxyDriverBindingStop(
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      ControllerHandle,
    IN  UINTN                           NumberOfChildren,
    IN  EFI_HANDLE                      *ChildHandleBuffer
)
{
    EFI_STATUS                      Status;
    EFI_AFL_PROXY_PROTOCOL          *AflProxy;
    EMU_AFL_PROXY_PRIVATE* Private;

    //
    // Get our context back
    //
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEfiAflProxyProtocolGuid,
        (VOID**)&AflProxy,
        This->DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    Private = EMU_AFL_PROXY_PRIVATE_DATA_FROM_THIS(AflProxy);

    Status = gBS->UninstallMultipleProtocolInterfaces(
        ControllerHandle,
        &gEfiAflProxyProtocolGuid,
        &Private->AflProxy,
        NULL
    );
    if (!EFI_ERROR(Status)) {
        Status = gBS->CloseProtocol(
            ControllerHandle,
            &gEmuIoThunkProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle
        );
        ASSERT_EFI_ERROR(Status);
        //
        // Destroy the IO interface.
        //
        Status = Private->IoThunk->Close(Private->IoThunk);
        ASSERT_EFI_ERROR(Status);
        //
        // Free our instance data
        //
        FreeUnicodeStringTable(Private->ControllerNameTable);
        gBS->FreePool(Private);
    }

    return Status;
}

EFI_DRIVER_BINDING_PROTOCOL  gEmuAflProxyDriverBinding = {
  EmuAflProxyDriverBindingSupported,
  EmuAflProxyDriverBindingStart,
  EmuAflProxyDriverBindingStop,
  0xa,
  NULL,
  NULL
};


EFI_STATUS
EFIAPI
InitializeEmuAflProxy(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
)
{
    EFI_STATUS  Status;

    Status = EfiLibInstallDriverBindingComponentName2(
        ImageHandle,
        SystemTable,
        &gEmuAflProxyDriverBinding,
        ImageHandle,
        &gEmuAflProxyComponentName,
        &gEmuAflProxyComponentName2
    );
    ASSERT_EFI_ERROR(Status);

    return Status;
}
