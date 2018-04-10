/*******************************************************************************
Copyright (C) 2018 Marvell International Ltd.

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
#include <ShellBase.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FileHandleLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Library/MvFuseLib.h>

#define CMD_NAME_STRING       L"fuse"

STATIC CONST CHAR16 gShellFuseFileName[] = L"ShellCommands";
STATIC EFI_HANDLE gShellFuseHiiHandle = NULL;

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"read", TypeFlag},
  {L"prog", TypeFlag},
  {L"list", TypeFlag},
  {NULL , TypeMax}
  };

/**
  Return the file name of the help text file if not using HII.

  @return The string pointer to the file name.
**/
STATIC
CONST CHAR16*
EFIAPI
ShellCommandGetManFileNameFuse (
  VOID
  )
{
  return gShellFuseFileName;
}

STATIC
SHELL_STATUS
EFIAPI
ShellCommandRunFuse (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  CHAR16                *ProblemParam;
  LIST_ENTRY            *CheckPackage;
  EFI_STATUS            Status;
  UINT8                 *FuseOperationEnabled;

  /* Check Fuse Command Enabled or not*/
  FuseOperationEnabled = PcdGetPtr (PcdFuseOperationEnabled);
  if (*FuseOperationEnabled == 0) {
    Print (L"%s: Not Support\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  //Parse command line
  Status = ShellInitialize ();
  if (EFI_ERROR (Status)) {
    Print (L"%s: Error while initializing Shell\n", CMD_NAME_STRING);
    ASSERT_EFI_ERROR (Status);
    return SHELL_ABORTED;
  }

  Status = ShellCommandLineParse (ParamList, &CheckPackage, &ProblemParam, TRUE);
  if (EFI_ERROR (Status)) {
    Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  if (ShellCommandLineGetFlag (CheckPackage, L"read")) {
    CONST CHAR16* GlbaLineIdStr;
    CONST CHAR16* PosStr;
    CONST CHAR16* CountStr;
    UINT32 i, Cnt, GlbaLineId, Pos, Count, Val;

    /* Parameters vaild check */
    Cnt = ShellCommandLineGetCount (CheckPackage);
    if (Cnt < 3) {
      Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
      return SHELL_ABORTED;
    } else if (Cnt == 3) {
      Count = 1;
    } else if (Cnt == 4) {
      CountStr = ShellCommandLineGetRawValue (CheckPackage, 3);
      Count = ShellStrToUintn (CountStr);
    } else {
      Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
      return SHELL_ABORTED;
    }

    GlbaLineIdStr = ShellCommandLineGetRawValue (CheckPackage, 1);
    GlbaLineId = ShellStrToUintn (GlbaLineIdStr);
    PosStr = ShellCommandLineGetRawValue (CheckPackage, 2);
    Pos = ShellStrToUintn (PosStr);

    Print (L"Reading bank %u:\n", GlbaLineId);
    for (i = 0; i < Count; i++, Pos++) {
      if (!(i % 4))
        Print (L"\nWord 0x%.8x:", Pos);

      Status = ArmadaFuseRead (GlbaLineId, Pos, &Val);
      if (Status) {
        Print (L"%s: Read Failure\n", CMD_NAME_STRING);
        return SHELL_ABORTED;
      }

      Print (L" %.8x", Val);
    }
    Print (L"\n");
  } else if (ShellCommandLineGetFlag (CheckPackage, L"prog")) {
    CONST CHAR16* GlbaLineIdStr;
    CONST CHAR16* PosStr;
    CONST CHAR16* ValueStr;
    UINT32 i, Cnt, GlbaLineId, Pos, Val;

    /* Parameters vaild check */
    Cnt = ShellCommandLineGetCount (CheckPackage);
    if (Cnt < 4) {
      Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
      return SHELL_ABORTED;
    }

    GlbaLineIdStr = ShellCommandLineGetRawValue (CheckPackage, 1);
    GlbaLineId = ShellStrToUintn (GlbaLineIdStr);
    PosStr = ShellCommandLineGetRawValue (CheckPackage, 2);
    Pos = ShellStrToUintn (PosStr);

    for (i = 3; i < Cnt; i++, Pos++) {
      ValueStr = ShellCommandLineGetRawValue (CheckPackage, i);
      Val = ShellStrToUintn(ValueStr);

      Print (L"Programming bank %u word 0x%.8x to 0x%.8x...\n", GlbaLineId, Pos, Val);
      Status = ArmadaFuseProg (GlbaLineId, Pos, Val);
      if (Status) {
        Print (L"%s: Prog Failure\n", CMD_NAME_STRING);
        return SHELL_ABORTED;
      }
    }
  } else if (ShellCommandLineGetFlag (CheckPackage, L"list")) {
    Status = ArmadaFuseList();
    if (Status) {
      Print (L"%s: Prog Failure\n", CMD_NAME_STRING);
      return SHELL_ABORTED;
    }
  } else {
    Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
ShellFuseCommandConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  gShellFuseHiiHandle = NULL;

  gShellFuseHiiHandle = HiiAddPackages (
                          &gShellFuseHiiGuid,
                          gImageHandle,
                          UefiShellFuseCommandLibStrings,
                          NULL
                          );

  if (gShellFuseHiiHandle == NULL) {
    Print (L"%s: Cannot add Hii package\n", CMD_NAME_STRING);
    return EFI_DEVICE_ERROR;
  }

  Status = ShellCommandRegisterCommandName (
             CMD_NAME_STRING,
             ShellCommandRunFuse,
             ShellCommandGetManFileNameFuse,
             0,
             CMD_NAME_STRING,
             TRUE,
             gShellFuseHiiHandle,
             STRING_TOKEN (STR_GET_HELP_FUSE)
             );

  if (EFI_ERROR(Status)) {
    Print (L"%s: Error while registering command\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ShellFuseCommandDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (gShellFuseHiiHandle != NULL) {
    HiiRemovePackages (gShellFuseHiiHandle);
  }

  return EFI_SUCCESS;
}
