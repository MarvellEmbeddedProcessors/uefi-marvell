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
#include "MvSpiDxe.h"

SPI_MASTER *mSpiMasterInstance;

STATIC
EFI_STATUS
SpiSetBaudRate (
  IN UINT32 CpuClock,
  IN UINT32 MaxFreq
  )
{
  UINT32 Spr, BestSpr, Sppr, BestSppr, ClockDivider, Match, Reg, MinBaudDiff;
  UINTN SpiRegBase = PcdGet32 (PcdSpiRegBase);

  MinBaudDiff = 0xFFFFFFFF;
  BestSppr = 0;

  //Spr is in range 1-15 and Sppr in range 0-8
  for (Spr = 1; Spr <= 15; Spr++) {
    for (Sppr = 0; Sppr <= 7; Sppr++) {
      ClockDivider = Spr * (1 << Sppr);

      if ((CpuClock / ClockDivider) > MaxFreq) {
        continue;
      }

      if ((CpuClock / ClockDivider) == MaxFreq) {
        BestSpr = Spr;
        BestSppr = Sppr;
        Match = 1;
        break;
        }

      if ((MaxFreq - (CpuClock / ClockDivider)) < MinBaudDiff) {
        MinBaudDiff = (MaxFreq - (CpuClock / ClockDivider));
        BestSpr = Spr;
        BestSppr = Sppr;
      }
    }

    if (Match == 1) {
      break;
    }
  }

  if (BestSpr == 0) {
    return (EFI_INVALID_PARAMETER);
  }

  Reg = MmioRead32 (SpiRegBase + SPI_CONF_REG);
  Reg &= ~(SPI_SPR_MASK | SPI_SPPR_0_MASK | SPI_SPPR_HI_MASK);
  Reg |= (BestSpr << SPI_SPR_OFFSET) |
         ((BestSppr & 0x1) << SPI_SPPR_0_OFFSET) |
         ((BestSppr >> 1) << SPI_SPPR_HI_OFFSET);
  MmioWrite32 (SpiRegBase + SPI_CONF_REG, Reg);

  return EFI_SUCCESS;
}

STATIC
VOID
SpiSetCs (
  UINT8 CsId
  )
{
  UINT32 Reg, SpiRegBase = PcdGet32 (PcdSpiRegBase);

  Reg = MmioRead32 (SpiRegBase + SPI_CTRL_REG);
  Reg &= ~SPI_CS_NUM_MASK;
  Reg |= (CsId << SPI_CS_NUM_OFFSET);
  MmioWrite32 (SpiRegBase + SPI_CTRL_REG, Reg);
}

STATIC
VOID
SpiActivateCs (
  UINT8 IN CsId
  )
{
  UINT32  Reg, SpiRegBase = PcdGet32 (PcdSpiRegBase);

  SpiSetCs(CsId);
  Reg = MmioRead32 (SpiRegBase + SPI_CTRL_REG);
  Reg |= SPI_CS_EN_MASK;
  MmioWrite32(SpiRegBase + SPI_CTRL_REG, Reg);
}

STATIC
VOID
SpiDeactivateCs (
  VOID
  )
{
  UINT32  Reg, SpiRegBase = PcdGet32 (PcdSpiRegBase);

  Reg = MmioRead32 (SpiRegBase + SPI_CTRL_REG);
  Reg &= ~SPI_CS_EN_MASK;
  MmioWrite32(SpiRegBase + SPI_CTRL_REG, Reg);
}

STATIC
VOID
SpiSetupTransfer (
  IN MARVELL_SPI_MASTER_PROTOCOL *This,
  IN SPI_DEVICE *Slave
  )
{
  SPI_MASTER *SpiMaster;
  UINT32 Reg, SpiRegBase, CoreClock, SpiMaxFreq;

  SpiMaster = SPI_MASTER_FROM_SPI_MASTER_PROTOCOL (This);

  // Initialize values from PCDs
  SpiRegBase  = PcdGet32 (PcdSpiRegBase);
  CoreClock   = PcdGet32 (PcdSpiClockFrequency);
  SpiMaxFreq  = PcdGet32 (PcdSpiMaxFrequency);

  EfiAcquireLock (&SpiMaster->Lock);

  Reg = MmioRead32 (SpiRegBase + SPI_CONF_REG);
  Reg |= SPI_BYTE_LENGTH;
  MmioWrite32 (SpiRegBase + SPI_CONF_REG, Reg);

  SpiSetCs(Slave->Cs);

  SpiSetBaudRate (CoreClock, SpiMaxFreq);

  Reg = MmioRead32 (SpiRegBase + SPI_CONF_REG);
  Reg &= ~(SPI_CPOL_MASK | SPI_CPHA_MASK | SPI_TXLSBF_MASK | SPI_RXLSBF_MASK);

  switch (Slave->Mode) {
  case SPI_MODE0:
    break;
  case SPI_MODE1:
    Reg |= SPI_CPHA_MASK;
    break;
  case SPI_MODE2:
    Reg |= SPI_CPOL_MASK;
    break;
  case SPI_MODE3:
    Reg |= SPI_CPOL_MASK;
    Reg |= SPI_CPHA_MASK;
    break;
  }

  MmioWrite32 (SpiRegBase + SPI_CONF_REG, Reg);

  EfiReleaseLock (&SpiMaster->Lock);
}

EFI_STATUS
EFIAPI
MvSpiTransfer (
  IN MARVELL_SPI_MASTER_PROTOCOL *This,
  IN SPI_DEVICE *Slave,
  IN UINTN DataByteCount,
  IN VOID *DataOut,
  IN VOID *DataIn,
  IN UINTN Flag
  )
{
  SPI_MASTER *SpiMaster;
  UINT64  Length;
  UINT32  Iterator, Reg, SpiRegBase;
  UINT8   *DataOutPtr = (UINT8 *)DataOut;
  UINT8   *DataInPtr  = (UINT8 *)DataIn;
  UINT8   DataToSend  = 0;

  SpiMaster = SPI_MASTER_FROM_SPI_MASTER_PROTOCOL (This);

  SpiRegBase = PcdGet32 (PcdSpiRegBase);

  Length = 8 * DataByteCount;

  EfiAcquireLock (&SpiMaster->Lock);

  if (Flag & SPI_TRANSFER_BEGIN) {
    SpiActivateCs (Slave->Cs);
  }

  // Set 8-bit mode
  Reg = MmioRead32 (SpiRegBase + SPI_CONF_REG);
  Reg &= ~SPI_BYTE_LENGTH;
  MmioWrite32 (SpiRegBase + SPI_CONF_REG, Reg);

  while (Length > 0) {
    if (DataOut != NULL) {
      DataToSend = *DataOutPtr & 0xFF;
    }
    // Transmit Data
    MmioWrite32 (SpiRegBase + SPI_INT_CAUSE_REG, 0x0);
    MmioWrite32 (SpiRegBase + SPI_DATA_OUT_REG, DataToSend);
    // Wait for memory ready
    for (Iterator = 0; Iterator < SPI_TIMEOUT; Iterator++) {
      if (MmioRead32 (SpiRegBase + SPI_INT_CAUSE_REG)) {
        if (DataInPtr != NULL) {
          *DataInPtr = MmioRead32 (SpiRegBase + SPI_DATA_IN_REG);
          DataInPtr++;
        }
        if (DataOutPtr != NULL) {
          DataOutPtr++;
        }
        Length -= 8;
        break;
      }
    }

    if (Iterator >= SPI_TIMEOUT) {
      DEBUG ((DEBUG_ERROR, "EfiSpiTransfer: Timeout\n"));
      return EFI_TIMEOUT;
    }
  }

  if (Flag & SPI_TRANSFER_END) {
    SpiDeactivateCs ();
  }

  EfiReleaseLock (&SpiMaster->Lock);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MvSpiReadWrite (
  IN  MARVELL_SPI_MASTER_PROTOCOL *This,
  IN  SPI_DEVICE *Slave,
  IN  UINT8 *Cmd,
  IN  UINTN CmdSize,
  IN  UINT8 *DataOut,
  OUT UINT8 *DataIn,
  IN  UINTN DataSize
  )
{
  EFI_STATUS Status;

  Status = MvSpiTransfer (This, Slave, CmdSize, Cmd, NULL, SPI_TRANSFER_BEGIN);
  if (EFI_ERROR (Status)) {
    Print (L"Spi Transfer Error\n");
    return EFI_DEVICE_ERROR;
  }

  Status = MvSpiTransfer (This, Slave, DataSize, DataOut, DataIn, SPI_TRANSFER_END);
  if (EFI_ERROR (Status)) {
    Print (L"Spi Transfer Error\n");
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MvSpiInit (
  IN MARVELL_SPI_MASTER_PROTOCOL * This
  )
{

  return EFI_SUCCESS;
}

SPI_DEVICE *
EFIAPI
MvSpiSetupSlave (
  IN MARVELL_SPI_MASTER_PROTOCOL *This,
  IN SPI_DEVICE *Slave,
  IN UINTN Cs,
  IN SPI_MODE Mode
  )
{
  if (!Slave) {
    Slave = AllocateZeroPool (sizeof(SPI_DEVICE));
    if (Slave == NULL) {
      DEBUG((DEBUG_ERROR, "Cannot allocate memory\n"));
      return NULL;
    }

    Slave->Cs   = Cs;
    Slave->Mode = Mode;
  }

  SpiSetupTransfer (This, Slave);

  return Slave;
}

EFI_STATUS
EFIAPI
MvSpiFreeSlave (
  IN SPI_DEVICE *Slave
  )
{
  FreePool (Slave);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SpiMasterInitProtocol (
  IN MARVELL_SPI_MASTER_PROTOCOL *SpiMasterProtocol
  )
{

  SpiMasterProtocol->Init        = MvSpiInit;
  SpiMasterProtocol->SetupDevice = MvSpiSetupSlave;
  SpiMasterProtocol->FreeDevice  = MvSpiFreeSlave;
  SpiMasterProtocol->Transfer    = MvSpiTransfer;
  SpiMasterProtocol->ReadWrite   = MvSpiReadWrite;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SpiMasterEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;

  mSpiMasterInstance = AllocateZeroPool (sizeof (SPI_MASTER));

  if (mSpiMasterInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  EfiInitializeLock (&mSpiMasterInstance->Lock, TPL_NOTIFY);

  SpiMasterInitProtocol (&mSpiMasterInstance->SpiMasterProtocol);

  mSpiMasterInstance->Signature = SPI_MASTER_SIGNATURE;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(mSpiMasterInstance->Handle),
                  &gMarvellSpiMasterProtocolGuid,
                  &(mSpiMasterInstance->SpiMasterProtocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    FreePool (mSpiMasterInstance);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
