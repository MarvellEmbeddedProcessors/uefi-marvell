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
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MvFuseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MvHwDescLib.h>

DECLARE_A7K8K_EFUSE_TEMPLATE;

MV_SOC_FUSE_DESC *mGolbalFuseDesc;
UINTN mGolbalFuseCnt;

#define RoundUp(n, d)    (((n) + (d) - 1) / (d))
#define GetLen(width)    RoundUp(width, 32)


EFI_STATUS
EFIAPI
ArmadaSoCDescFuseGet (
  IN OUT MV_SOC_FUSE_DESC  **FuseDesc,
  IN OUT UINT32             *DescCount
  )
{
  MV_SOC_FUSE_DESC *Desc;
  MVHW_EFUSE_DESC *TmpDesc;
  UINT32 Index;

  TmpDesc = &mA7k8kEfuseDescTemplate;
  *DescCount = PcdGet32 (PcdFuseDevCount);

  Desc = AllocateZeroPool (*DescCount * sizeof (MV_SOC_FUSE_DESC));

  for (Index = 0; Index < *DescCount; Index++) {
    Desc[Index].BankId = TmpDesc->BankId[Index];
    Desc[Index].BankType = TmpDesc->BankType[Index];
    CopyMem (Desc[Index].BankName, TmpDesc->BankName[Index], 16);
    Desc[Index].BankOPMode = TmpDesc->BankOPMode[Index];
    Desc[Index].BankRegBase = TmpDesc->BankRegBase[Index];
    Desc[Index].OTPMemBase = TmpDesc->OTPMemBase[Index];
    Desc[Index].PriBitOffset = TmpDesc->PriBitOffset[Index];
    Desc[Index].RowCount = TmpDesc->RowCount[Index];
    Desc[Index].RowLength = TmpDesc->RowLength[Index];
    Desc[Index].RowStep = TmpDesc->RowStep[Index];
  }

  *FuseDesc = Desc;

  return EFI_SUCCESS;
}

CHAR16*
EFIAPI
ArmadaFuseGetName (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].BankName;
}

UINTN
EFIAPI
ArmadaFuseGetRowLength (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].RowLength;
}

UINTN
EFIAPI
ArmadaFuseGetOPMode (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].BankOPMode;
}

UINTN
EFIAPI
ArmadaFuseGetBankType (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].BankType;
}

UINTN
EFIAPI
ArmadaFuseGetRowCount (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].RowCount;
}

UINTN
EFIAPI
ArmadaFuseGetRowStep (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].RowStep;
}

UINTN
EFIAPI
ArmadaFuseGetRegBase (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].BankRegBase;
}

UINTN
EFIAPI
ArmadaFuseGetMemBase (
  IN UINT32 FuseNum
  )
{
  return mGolbalFuseDesc[FuseNum].OTPMemBase;
}

EFI_STATUS
EFIAPI
GlobalLineIdToInternalRowId (
  IN UINT32            GlobalLineId,
  OUT UINT32          *RowId,
  OUT UINT32          *BankId
  )
{
  if (GlobalLineId < 0) {
    DEBUG ((DEBUG_ERROR, "INVALID PARAMETER\n"));
    return EFI_INVALID_PARAMETER;
  } else if (GlobalLineId < 64) {
    *RowId = GlobalLineId;
    *BankId = 0;
  } else if (GlobalLineId < 65) {
    *RowId = 0;
    *BankId = 1;
  } else if (GlobalLineId < 66) {
    *RowId = 0;
    *BankId = 2;
  } else if (GlobalLineId < 67) {
    *RowId = 0;
    *BankId = 3;
  } else if (GlobalLineId < 68) {
    *RowId = 0;
    *BankId = 4;
  } else if (GlobalLineId < 69) {
    *RowId = 0;
    *BankId = 5;
  } else if (GlobalLineId < 70) {
    *RowId = 0;
    *BankId = 6;
  } else {
    DEBUG ((DEBUG_ERROR, "INVALID PARAMETER\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (*BankId > mGolbalFuseCnt - 1) {
    DEBUG ((DEBUG_ERROR, "INVALID PARAMETER\n"));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ArmadaFuseProg (
  IN UINT32             GlobalLineId,
  IN UINT32             Position,
  IN UINT32             Value
  )
{

  UINT32 RowId;
  UINT32 BankId;
  EFI_STATUS Status;

  Status = GlobalLineIdToInternalRowId (GlobalLineId, &RowId, &BankId);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ArmadaFuseProg: Error\n"));
    return EFI_ABORTED;
  }

  Status = ArmadaSoCFuseWrite (&mGolbalFuseDesc[BankId], RowId, Position, Value);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ArmadaFuseProg: Error\n"));
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ArmadaFuseRead (
  IN UINT32            GlobalLineId,
  IN UINT32            Position,
  IN OUT UINT32        *Value
  )
{

  UINT32 RowId;
  UINT32 BankId;
  UINT32 *FuseData = NULL;
  EFI_STATUS Status;

  Status = GlobalLineIdToInternalRowId (GlobalLineId, &RowId, &BankId);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ArmadaFuseRead: Error\n"));
    return EFI_ABORTED;
  }

  Status = ArmadaSoCFuseRead (&mGolbalFuseDesc[BankId], RowId, Position, &FuseData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ArmadaFuseRead: Error\n"));
    return EFI_ABORTED;
  }

  *Value = *(FuseData + Position);

  FreePool (FuseData);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaFuseList (
  VOID
  )
{
  UINTN GolbalId = 0, InternalId = 0, FuseCount;
  CHAR16 *BankOpMode;

  for (FuseCount = 0; FuseCount < mGolbalFuseCnt; FuseCount++) {
    if (ArmadaFuseGetOPMode (FuseCount) == 0)
      BankOpMode = L"Read/Write";
    else if (ArmadaFuseGetOPMode (FuseCount) == 1)
      BankOpMode = L"ReadOnly";
    else if (ArmadaFuseGetOPMode (FuseCount) == 2)
      BankOpMode = L"WriteOnly";
    else
      BankOpMode = L"Unknow";
    DEBUG ((DEBUG_ERROR, "=======================================================\n"));
    DEBUG ((DEBUG_ERROR, "Bank Name:         %s\n", ArmadaFuseGetName(FuseCount)));
    DEBUG ((DEBUG_ERROR, "Row Length:        %d\n", ArmadaFuseGetRowLength(FuseCount)));
    DEBUG ((DEBUG_ERROR, "Row Number:        %d\n", ArmadaFuseGetRowCount(FuseCount)));
    DEBUG ((DEBUG_ERROR, "Row Step:          %d\n", ArmadaFuseGetRowStep(FuseCount)));
    DEBUG ((DEBUG_ERROR, "Bank OP Mode:      %s\n", BankOpMode));
    DEBUG ((DEBUG_ERROR, "Bank Control Reg:  0x%08x\n", ArmadaFuseGetRegBase(FuseCount)));
    DEBUG ((DEBUG_ERROR, "Memory Base:       0x%08x\n", ArmadaFuseGetMemBase(FuseCount)));
    DEBUG ((DEBUG_ERROR, "                   Golbal Row ID     Internal Line ID\n"));
    for (InternalId = 0; InternalId < ArmadaFuseGetRowCount(FuseCount); InternalId++, GolbalId++)
      DEBUG ((DEBUG_ERROR, "                        %d                  %d\n", GolbalId, InternalId));
    }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaFuseLibConstructor (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  MV_SOC_FUSE_DESC *FuseDesc;
  EFI_STATUS Status;
  UINT32 FuseCount;

  /* Get SoC data about all Fuse */
  Status = ArmadaSoCDescFuseGet (&FuseDesc, &FuseCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mGolbalFuseDesc = FuseDesc;
  mGolbalFuseCnt = FuseCount;

  return EFI_SUCCESS;
}
