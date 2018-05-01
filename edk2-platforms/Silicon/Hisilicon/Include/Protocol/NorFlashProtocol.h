/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/


#ifndef _NOR_FLASH_PROTOCOL_H_
#define _NOR_FLASH_PROTOCOL_H_

#define UNI_NOR_FLASH_PROTOCOL_GUID   \
    {0x86F305EA, 0xDFAC, 0x4A6B, {0x92, 0x77, 0x47, 0x31, 0x2E, 0xCE, 0x42, 0xA}}

typedef struct _UNI_NOR_FLASH_PROTOCOL UNI_NOR_FLASH_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *UNI_FLASH_ERASE_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN UINT32                  Length
    );
typedef
EFI_STATUS
(EFIAPI *UNI_FLASH_WRITE_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN  UINT32                 Offset,
    IN  UINT8                 *Buffer,
    UINT32                   ulLength
    );

typedef
EFI_STATUS
(EFIAPI *UNI_FLASH_READ_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN OUT UINT8              *Buffer,
    IN UINT32                   ulLen
    );


struct _UNI_NOR_FLASH_PROTOCOL {
    UNI_FLASH_ERASE_INTERFACE             Erase;
    UNI_FLASH_WRITE_INTERFACE             Write;
    UNI_FLASH_READ_INTERFACE              Read;
};

extern EFI_GUID gUniNorFlashProtocolGuid;

#endif
