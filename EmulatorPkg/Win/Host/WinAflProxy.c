#include "WinHost.h"

#define WIN_AFL_PROXY_PRIVATE_SIGNATURE SIGNATURE_32 ('A', 'F', 'L', 'P')



typedef struct {
    UINTN                           Signature;
    EMU_IO_THUNK_PROTOCOL*          Thunk;
    EFI_AFL_PROXY_PROTOCOL          AflProxy;
} WIN_AFL_PROXY_PRIVATE;

#define WIN_AFL_PROXY_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      WIN_AFL_PROXY_PRIVATE, \
      AflProxy, \
      WIN_AFL_PROXY_PRIVATE_SIGNATURE \
      )

extern EFI_AFL_PROXY_PROTOCOL gWinAflProxyProtocol;

EFI_STATUS
WinAflMaybeLog(
    IN EFI_AFL_PROXY_PROTOCOL   *This,
    IN UINT32               date
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    printf("\n%i \n", date);
    return Status;
}

EFI_STATUS
WinAflProxyThunkOpen(
    IN  EMU_IO_THUNK_PROTOCOL* This
)
{
    WIN_AFL_PROXY_PRIVATE* Private;

    Private = AllocateZeroPool(sizeof(*Private));
    if (Private == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }
    Private->Signature = WIN_AFL_PROXY_PRIVATE_SIGNATURE;
    Private->Thunk = This;
    CopyMem(&Private->AflProxy, &gWinAflProxyProtocol, sizeof(Private->AflProxy));
    This->Interface = &Private->AflProxy;

    This->Private = Private;
    return EFI_SUCCESS;
}


EFI_STATUS
WinAflProxyThunkClose(
    IN  EMU_IO_THUNK_PROTOCOL* This
)
{
    WIN_AFL_PROXY_PRIVATE* Private;

    Private = This->Private;
    ASSERT(Private != NULL);

    FreePool(Private);
    return EFI_SUCCESS;
}

EFI_AFL_PROXY_PROTOCOL gWinAflProxyProtocol = {
    WinAflMaybeLog
};


EMU_IO_THUNK_PROTOCOL mWinAflProxyThunkIo = {       // EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
    &gEfiAflProxyProtocolGuid,				        // Status = gBS->LocateProtocol(
    NULL,											// 		&gEfiSimpleFileSystemProtocolGuid,
    NULL,											// 		NULL,
    0,												// 		(VOID**)&SimpleFileSystem
    WinAflProxyThunkOpen,							// );
    WinAflProxyThunkClose,						    // Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);
    NULL
};


