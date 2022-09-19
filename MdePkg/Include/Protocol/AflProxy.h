

#ifndef __AFL_PROXY_H__
#define __AFL_PROXY_H__

#define EFI_AFL_PROXY_PROTOCOL_GUID \
  { \
    0x69e4b522, 0x4695, 0x112d, {0xe8, 0x93, 0x0, 0x0a, 0x9c, 0x96, 0x27, 0xb3 } \
  }

typedef struct _EFI_AFL_PROXY_PROTOCOL  EFI_AFL_PROXY_PROTOCOL;


#define AFL_PROXY_PROTOCOL       EFI_AFL_PROXY_PROTOCOL_GUID

typedef 
EFI_STATUS
(EFIAPI* EFI_AFL_MAYBE_LOG) (
    IN EFI_AFL_PROXY_PROTOCOL* This,
    IN UINT32 date
    );

struct _EFI_AFL_PROXY_PROTOCOL {
    EFI_AFL_MAYBE_LOG afl_maybe_log;
};


extern EFI_GUID gEfiAflProxyProtocolGuid;

#endif
