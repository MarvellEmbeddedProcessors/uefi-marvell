/** @file  MvFvbDxe.h

  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2017 Marvell International Ltd.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FVB_FLASH_DXE_H__
#define __FVB_FLASH_DXE_H__

#include <Protocol/BlockIo.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/Spi.h>
#include <Protocol/SpiFlash.h>

#define GET_DATA_OFFSET(BaseAddr, Lba, LbaSize) ((BaseAddr) + (UINTN)((Lba) * (LbaSize)))

#define FVB_FLASH_SIGNATURE                       SIGNATURE_32('S', 'n', 'o', 'r')
#define INSTANCE_FROM_FVB_THIS(a)                 CR(a, FVB_DEVICE, FvbProtocol, FVB_FLASH_SIGNATURE)

typedef struct {
  VENDOR_DEVICE_PATH                  Vendor;
  EFI_DEVICE_PATH_PROTOCOL            End;
} FVB_DEVICE_PATH;

typedef struct {
  SPI_DEVICE                          SpiDevice;

  MARVELL_SPI_FLASH_PROTOCOL          *SpiFlashProtocol;
  MARVELL_SPI_MASTER_PROTOCOL         *SpiMasterProtocol;

  EFI_HANDLE                          Handle;

  UINT32                              Signature;

  UINTN                               DeviceBaseAddress;
  UINTN                               RegionBaseAddress;
  UINTN                               Size;
  UINTN                               FvbOffset;
  UINTN                               FvbSize;
  EFI_LBA                             StartLba;

  EFI_BLOCK_IO_MEDIA                  Media;
  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL FvbProtocol;

  FVB_DEVICE_PATH               DevicePath;
} FVB_DEVICE;

EFI_STATUS
EFIAPI
MvFvbGetAttributes(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    OUT       EFI_FVB_ATTRIBUTES_2*                    Attributes
);

EFI_STATUS
EFIAPI
MvFvbSetAttributes(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    IN OUT    EFI_FVB_ATTRIBUTES_2*                    Attributes
);

EFI_STATUS
EFIAPI
MvFvbGetPhysicalAddress(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    OUT       EFI_PHYSICAL_ADDRESS*                    Address
);

EFI_STATUS
EFIAPI
MvFvbGetBlockSize(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    IN        EFI_LBA                                  Lba,
    OUT       UINTN*                                   BlockSize,
    OUT       UINTN*                                   NumberOfBlocks
);

EFI_STATUS
EFIAPI
MvFvbRead(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    IN        EFI_LBA                                  Lba,
    IN        UINTN                                   Offset,
    IN OUT    UINTN*                                   NumBytes,
    IN OUT    UINT8*                                   Buffer
);

EFI_STATUS
EFIAPI
MvFvbWrite(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    IN        EFI_LBA                                  Lba,
    IN        UINTN                                    Offset,
    IN OUT    UINTN*                                   NumBytes,
    IN        UINT8*                                   Buffer
);

EFI_STATUS
EFIAPI
MvFvbEraseBlocks(
    IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL*     This,
    ...
);

#endif /* __FVB_FLASH_DXE_H__ */
