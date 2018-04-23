/********************************************************************************
Copyright (C) 2018 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must Retain the above copyright notice,
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

#include <Uefi.h>

#include <Library/ArmadaSoCFuseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#define RoundUp(n, d)                   (((n) + (d) - 1) / (d))
#define GetLen(width)                   RoundUp(width, 32)
#define BankReadOnly                    1
#define HighDensity                     0
#define LowDensity                      1

/* Fuse Contrl register BIT map */
#define FUSE_SRV_CTRL_LD_SEL_USER       BIT(6)
#define FUSE_CTRL_LD_SEC_EN_MASK        BIT(7)
#define FUSE_CTRL_PROGRAM_ENABLE        BIT(31)
#define ECC_BITS_MASK                   0xFE000000

STATIC UINT32 ValidProgWords;
STATIC UINT32 ProgVal[10];

EFI_STATUS
EFIAPI
ArmadaSoCHDFuseRead (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN OUT UINT32        **Value
  )
{
  UINTN OtpMem;
  UINT32 *FuseData;
  UINT32 WordsPerRow;
  UINT32 Index;
  UINT8  *FuseDryRunEnabled;

  /* Check Fuse DryRun Enabled or not*/
  FuseDryRunEnabled = PcdGetPtr (PcdFuseDryRunEnabled);
  if (*FuseDryRunEnabled == 1) {
    FuseDesc->OTPMemBase = 0;
  }

  WordsPerRow = GetLen(FuseDesc->RowLength);
  FuseData = AllocateZeroPool (WordsPerRow * sizeof (UINT32));
  OtpMem = FuseDesc->OTPMemBase + RowId * FuseDesc->RowStep;

  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseData[Index] = MmioRead32 (OtpMem + 4 * Index);
  }

  *Value = FuseData;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaSoCLDFuseRead (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN OUT UINT32        **Value
  )
{
  UINTN OtpMem, CtrlReg;
  UINT32 *FuseData;
  UINT32 WordsPerRow;
  UINT32 Index;
  UINT8  *FuseDryRunEnabled;

  /* Check Fuse DryRun Enabled or not*/
  FuseDryRunEnabled = PcdGetPtr (PcdFuseDryRunEnabled);
  if (*FuseDryRunEnabled == 1) {
    FuseDesc->OTPMemBase = 0;
  }

  WordsPerRow = GetLen(FuseDesc->RowLength);
  FuseData = AllocateZeroPool (WordsPerRow * sizeof (UINT32));
  OtpMem = FuseDesc->OTPMemBase + RowId * FuseDesc->RowStep;
  CtrlReg = FuseDesc->BankRegBase;

  /* When ReadOnly flag is set, which means LD0 */
  if (FuseDesc->BankOPMode == BankReadOnly) {
    MmioAnd32 (CtrlReg, ~FUSE_SRV_CTRL_LD_SEL_USER);
  } else {
    MmioOr32 (CtrlReg, FUSE_SRV_CTRL_LD_SEL_USER);
  }

  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseData[Index] = MmioRead32 (OtpMem + 4 * Index);
  }

  *Value = FuseData;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DoSoCHDFuseWrite (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32             *NewValue
  )
{
  UINTN OtpMem;
  UINTN CtrlReg = FuseDesc->BankRegBase;
  UINT32 Index;
  UINT32 WordsPerRow = GetLen(FuseDesc->RowLength);
  UINT8  *FuseDryRunEnabled;
  UINT32 FuseReadValue[WordsPerRow];

  /* Only write a fuse line with Lock Bit */
  if (!(*(NewValue + 2) & 0x1)) {
    DEBUG ((DEBUG_ERROR, "DoSoCHDFuseWrite: Error Without Lock Bit\n"));
    return EFI_ABORTED;
  }
  /* According to specs ECC protection bits must be 0 on write */
  if (*(NewValue + 1) & ECC_BITS_MASK) {
    DEBUG ((DEBUG_ERROR, "DoSoCHDFuseWrite: ECC protection bits Error\n"));
    return EFI_ABORTED;
  }
  /* Enable Fuse Prog */
  FuseDryRunEnabled = PcdGetPtr (PcdFuseDryRunEnabled);
  if (*FuseDryRunEnabled == 1) {
    FuseDesc->OTPMemBase = 0;
  } else if (*FuseDryRunEnabled == 0) {
    MmioOr32 (CtrlReg, FUSE_CTRL_PROGRAM_ENABLE);
  }

  OtpMem = FuseDesc->OTPMemBase + RowId * FuseDesc->RowStep;

  /* Read Fuse Row Value before burn Fuse */
  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseReadValue[Index] = MmioRead32 (OtpMem + 4 * Index);
  }
  /* Fuse Row Value Burn */
  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseReadValue[Index] |= *(NewValue + Index);
    MmioWrite32 (OtpMem + 4 * Index, FuseReadValue[Index]);
  }

  /* Wait 1 ms for burn Fuse */
  MicroSecondDelay(1);

  /* Disable Fuse Write */
  if (*FuseDryRunEnabled == 0) {
    MmioAnd32 (CtrlReg, ~FUSE_CTRL_PROGRAM_ENABLE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaSoCHDFuseWrite (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN UINT32              NewValue
  )
{
  EFI_STATUS Status;
  UINT32 WordsPerRow = GetLen(FuseDesc->RowLength);

  if (Position < WordsPerRow - 1) {
    ProgVal[Position] = NewValue;
    ValidProgWords |= (1 << Position);
  } else if (Position == WordsPerRow - 1) {
    if ((ValidProgWords & 3) == 0 && NewValue) {
      ProgVal[0] = 0;
      ProgVal[1] = 0;
      /* Lock Bit is set to 1 */
      ProgVal[2] = 1;
      Status = DoSoCHDFuseWrite (FuseDesc, RowId, ProgVal);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "ArmadaSoCHDFuseWrite: Error\n"));
        return EFI_ABORTED;
      }
      ValidProgWords = 0;
    } else if ((ValidProgWords & 3) == 3 && NewValue) {
      ProgVal[WordsPerRow - 1] = NewValue;
      Status = DoSoCHDFuseWrite (FuseDesc, RowId, ProgVal);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "ArmadaSoCHDFuseWrite: Error\n"));
        return EFI_ABORTED;
      }
      ValidProgWords = 0;
    } else {
      Status = EFI_ABORTED;
      ValidProgWords = 0;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "ArmadaSoCHDFuseWrite: Error Out of Max Length!\n"));
    Status = EFI_ABORTED;
    ValidProgWords = 0;
  }

  return Status;
}

EFI_STATUS
EFIAPI
DoSoCLDFuseWrite (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32             *NewValue
  )
{
  UINTN OtpMem;
  UINTN CtrlReg = FuseDesc->BankRegBase;
  UINT32 Index;
  UINT32 WordsPerRow = GetLen(FuseDesc->RowLength);
  UINT8  *FuseDryRunEnabled;
  UINT32 FuseReadValue[WordsPerRow];

  /* Check the Fuse is ReadOnly or not */
  if (FuseDesc->BankOPMode == BankReadOnly) {
    DEBUG ((DEBUG_ERROR, "DoSoCLDFuseWrite: This Row is ReadOnly\n"));
    return EFI_ABORTED;
  }

  /* Select LD1 for Fuse Burn */
  MmioOr32 (CtrlReg, FUSE_SRV_CTRL_LD_SEL_USER);

  /* Enable Fuse Prog */
  FuseDryRunEnabled = PcdGetPtr (PcdFuseDryRunEnabled);
  if (*FuseDryRunEnabled == 1) {
    FuseDesc->OTPMemBase = 0;
  } else if (*FuseDryRunEnabled == 0) {
    MmioOr32 (CtrlReg, FUSE_CTRL_PROGRAM_ENABLE);

    /* Enable security bit to lock LD efuse row for further programming */
    MmioOr32 (CtrlReg, FUSE_CTRL_LD_SEC_EN_MASK);
  }

  OtpMem = FuseDesc->OTPMemBase + RowId * FuseDesc->RowStep;

  /* Read Fuse Row Value before burn Fuse */
  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseReadValue[Index] = MmioRead32 (OtpMem + 4 * Index);
  }
  /* Fuse Row Value Burn */
  for (Index = 0; Index < WordsPerRow; Index++) {
    FuseReadValue[Index] |= *(NewValue + Index);
    MmioWrite32 (OtpMem + 4 * Index, FuseReadValue[Index]);
  }

  /* write all 0 for LD eFuse to burn efuse  */
  MmioWrite32 (OtpMem + 0x20, 0x0);

  /* Wait 1 ms for burn Fuse */
  MicroSecondDelay(1);

  /* Disable Fuse Write */
  if (*FuseDryRunEnabled == 0) {
    MmioAnd32 (CtrlReg, ~FUSE_CTRL_PROGRAM_ENABLE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaSoCLDFuseWrite (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN UINT32              NewValue
  )
{
  EFI_STATUS Status;
  UINT32 WordsPerRow = GetLen(FuseDesc->RowLength);

  if (Position < WordsPerRow - 1) {
    ProgVal[Position] = NewValue;
    ValidProgWords |= (1 << Position);
  } else if ((ValidProgWords & 0x7F) != 0x7F) {
    Status = EFI_ABORTED;
  } else {
    ProgVal[WordsPerRow - 1] = NewValue;
    Status = DoSoCLDFuseWrite (FuseDesc, RowId, ProgVal);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ArmadaSoCLDFuseWrite: Error\n"));
      return EFI_ABORTED;
    }
    ValidProgWords = 0;
  }

  return Status;
}

EFI_STATUS
EFIAPI
ArmadaSoCFuseRead (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN OUT UINT32        **Value
  )
{
  EFI_STATUS Status;

  if (FuseDesc->BankType == HighDensity) {
    Status = ArmadaSoCHDFuseRead (FuseDesc, RowId, Position, Value);
  } else if (FuseDesc->BankType == LowDensity) {
    Status = ArmadaSoCLDFuseRead (FuseDesc, RowId, Position, Value);
  }

  return Status;
}

EFI_STATUS
EFIAPI
ArmadaSoCFuseWrite (
  IN MV_SOC_FUSE_DESC   *FuseDesc,
  IN UINT32              RowId,
  IN UINT32              Position,
  IN UINT32              Value
  )
{
  EFI_STATUS Status;

  if (FuseDesc->BankType == HighDensity) {
    Status = ArmadaSoCHDFuseWrite (FuseDesc, RowId, Position, Value);
  } else if (FuseDesc->BankType == LowDensity) {
    Status = ArmadaSoCLDFuseWrite (FuseDesc, RowId, Position, Value);
  }

  return Status;
}
