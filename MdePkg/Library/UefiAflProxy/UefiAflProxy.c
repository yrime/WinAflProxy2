#include <Library/UefiAflProxy.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/AflProxy.h>

EFI_STATUS afl_log2( IN UINT32 data) {
    EFI_STATUS Status = 0;

    EFI_AFL_PROXY_PROTOCOL* AflProxy;
    Status = gBS->LocateProtocol(
        &gEfiAflProxyProtocolGuid,
        NULL,
        (VOID**)&AflProxy
    );

    if (EFI_ERROR(Status)) {
        //  Print(L"s\n");
        return Status;
    }
    AflProxy->afl_maybe_log(AflProxy, data);
    return Status;
}