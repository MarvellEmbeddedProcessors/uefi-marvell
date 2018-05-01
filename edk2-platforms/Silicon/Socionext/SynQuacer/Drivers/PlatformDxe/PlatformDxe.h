/** @file
  SynQuacer DXE platform driver.

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __PLATFORM_DXE_H__
#define __PLATFORM_DXE_H__

#include <PiDxe.h>
#include <Guid/SynQuacerPlatformFormSet.h>
#include <IndustryStandard/Pci.h>
#include <Library/ArmGenericTimerCounterLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DtPlatformDtbLoaderLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/HiiLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Platform/MemoryMap.h>
#include <Platform/Pcie.h>
#include <Platform/VarStore.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/NonDiscoverableDevice.h>
#include <Protocol/PciIo.h>
#include <Protocol/SdMmcOverride.h>

#define EMMC_TABLE_ID     SIGNATURE_64('S','y','n','Q','e','M','M','C')

extern UINT8                             PlatformDxeHiiBin[];
extern UINT8                             PlatformDxeStrings[];

extern UINT64                            mHiiSettingsVal;
extern SYNQUACER_PLATFORM_VARSTORE_DATA  *mHiiSettings;

EFI_STATUS
EFIAPI
RegisterPcieNotifier (
  VOID
  );

EFI_STATUS
EFIAPI
RegisterEmmc (
  VOID
  );

#endif
