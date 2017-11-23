/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#ifndef __MV_SPI_FLASH_H__
#define __MV_SPI_FLASH_H__

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Spi.h>
#include <Protocol/SpiFlash.h>

#define SPI_FLASH_SIGNATURE             SIGNATURE_32 ('F', 'S', 'P', 'I')

#define CMD_READ_ID                     0x9f
#define READ_STATUS_REG_CMD             0x0b
#define CMD_WRITE_ENABLE                0x06
#define CMD_READ_STATUS                 0x05
#define CMD_FLAG_STATUS                 0x70
#define CMD_WRITE_STATUS_REG            0x01
#define CMD_READ_ARRAY_FAST             0x0b
#define CMD_PAGE_PROGRAM                0x02
#define CMD_BANK_WRITE                  0xc5
#define CMD_ERASE_4K                    0x20
#define CMD_ERASE_32K                   0x52
#define CMD_ERASE_64K                   0xd8
#define CMD_4B_ADDR_ENABLE              0xb7

#define STATUS_REG_POLL_WIP             (1 << 0)
#define STATUS_REG_POLL_PEC             (1 << 7)

#define SPI_TRANSFER_BEGIN              0x01  // Assert CS before transfer
#define SPI_TRANSFER_END                0x02  // Deassert CS after transfers

#define SPI_ERASE_SIZE_4K               4096
#define SPI_ERASE_SIZE_32K              32768
#define SPI_ERASE_SIZE_64K              65536

#define SPI_FLASH_16MB_BOUN             0x1000000

typedef enum {
  SPI_FLASH_READ_ID,
  SPI_FLASH_READ, // Read from SPI flash with address
  SPI_FLASH_WRITE, // Write to SPI flash with address
  SPI_FLASH_ERASE,
  SPI_FLASH_UPDATE,
  SPI_COMMAND_MAX
} SPI_COMMAND;

typedef struct {
  MARVELL_SPI_FLASH_PROTOCOL  SpiFlashProtocol;
  UINTN                   Signature;
  EFI_HANDLE              Handle;
} SPI_FLASH_INSTANCE;

EFI_STATUS
EFIAPI
SpiFlashReadId (
  IN     SPI_DEVICE *SpiDev,
  IN     UINT32     DataByteCount,
  IN OUT UINT8      *Buffer
  );

EFI_STATUS
SpiFlashRead (
  IN SPI_DEVICE   *Slave,
  IN UINT32       Offset,
  IN UINTN        Length,
  IN VOID         *Buf
  );

EFI_STATUS
SpiFlashWrite (
  IN SPI_DEVICE *Slave,
  IN UINT32     Offset,
  IN UINTN      Length,
  IN VOID       *Buf
  );

EFI_STATUS
SpiFlashUpdate (
  IN SPI_DEVICE *Slave,
  IN UINT32 Offset,
  IN UINTN ByteCount,
  IN UINT8 *Buf
  );

EFI_STATUS
SpiFlashErase (
  IN SPI_DEVICE *SpiDev,
  IN UINTN      Offset,
  IN UINTN      Length
  );

EFI_STATUS
EFIAPI
EfiSpiFlashInit (
  IN MARVELL_SPI_FLASH_PROTOCOL *This,
  IN SPI_DEVICE *Slave
  );

#endif // __MV_SPI_FLASH_H__
