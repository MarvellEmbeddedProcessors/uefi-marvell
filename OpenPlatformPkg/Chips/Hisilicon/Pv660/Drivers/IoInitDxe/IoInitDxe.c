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

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/PlatformSysCtrlLib.h>
#include <Library/SerdesLib.h>

#include "Smmu.h"

SMMU_DEVICE mSpecialSmmu[] = {
  {FixedPcdGet64 (PcdM3SmmuBaseAddress), 0},
  {FixedPcdGet64 (PcdPcieSmmuBaseAddress), 0},
};

VOID
SpecialSmmuConfig (VOID)
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mSpecialSmmu) / sizeof (mSpecialSmmu[0]); Index++) {
    (VOID) SmmuConfigSwitch (&mSpecialSmmu[Index]);
  }
}

VOID
SpecialSmmuEnable (VOID)
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mSpecialSmmu) / sizeof (mSpecialSmmu[0]); Index++) {
    (VOID) SmmuEnableTable (&mSpecialSmmu[Index]);
  }
}

VOID
EFIAPI
ExitBootServicesEventSmmu (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  SmmuConfigForOS ();
  SpecialSmmuEnable ();
  DEBUG((EFI_D_ERROR,"SMMU ExitBootServicesEvent\n"));
}


EFI_STATUS
EFIAPI
IoInitDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable)
{
  EFI_STATUS Status;
  EFI_EVENT  Event = NULL;

  (VOID) EfiSerdesInitWrap ();

  SmmuConfigForBios ();

  SpecialSmmuConfig ();

  Status = gBS->CreateEvent (
      EVT_SIGNAL_EXIT_BOOT_SERVICES,
      TPL_CALLBACK,
      ExitBootServicesEventSmmu,
      NULL,
      &Event
      );

  if (EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_ERROR, "[%a:%d] - CreateEvent failed: %r\n", __FUNCTION__,
        __LINE__, Status));
  }

  return Status;
}

