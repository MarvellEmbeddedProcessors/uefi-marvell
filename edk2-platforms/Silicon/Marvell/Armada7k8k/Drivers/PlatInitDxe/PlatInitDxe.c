/** @file
  Copyright (c) 2017, Linaro Limited. All rights reserved.
  Copyright (c) 2017, Marvell International Ltd. and its affiliates

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MppLib.h>
#include <Library/MvBoardDescLib.h>
#include <Library/MvComPhyLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UtmiPhyLib.h>

#define SOC_CONFIG_AMB_REG(Cp)          (0xf2000000 + 0x2000000 * (Cp) + 0x441910)
#define SOC_CONFIG_FORCE_CBE_ATTR_MASK  0x1

STATIC
EFI_STATUS
ArmadaPlatInitBoardSelect (
  )
{
  UINT8    BoardId;

  MVBOARD_ID_GET (BoardId);

  switch (BoardId) {
  case MVBOARD_ID_ARMADA7040_DB:
    DEBUG ((DEBUG_ERROR, "\nArmada 7040 DB Platform Init\n\n"));
    /* Ensure proper access to memory mapped SPI */
    MmioAnd32 (SOC_CONFIG_AMB_REG (0), ~SOC_CONFIG_FORCE_CBE_ATTR_MASK);
    return EFI_SUCCESS;
  case MVBOARD_ID_ARMADA8040_DB:
    DEBUG ((DEBUG_ERROR, "\nArmada 8040 DB Platform Init\n\n"));
    /* Ensure proper access to memory mapped SPI */
    MmioAnd32 (SOC_CONFIG_AMB_REG (1), ~SOC_CONFIG_FORCE_CBE_ATTR_MASK);
    return EFI_SUCCESS;
  case MVBOARD_ID_ARMADA8040_MCBIN:
    DEBUG ((DEBUG_ERROR, "\nArmada 8040 MachiatoBin Platform Init\n\n"));
    /* Ensure proper access to memory mapped SPI */
    MmioAnd32 (SOC_CONFIG_AMB_REG (1), ~SOC_CONFIG_FORCE_CBE_ATTR_MASK);
    return EFI_SUCCESS;
  case MVBOARD_ID_ARMADA8082_DB:
    DEBUG ((DEBUG_ERROR, "\nArmada 8082 DB Platform Init\n\n"));
    return EFI_SUCCESS;
  default:
    DEBUG ((DEBUG_ERROR, "\nInvalid Board Id 0x%x\n", BoardId));
    return EFI_INVALID_PARAMETER;
  }
}

EFI_STATUS
EFIAPI
ArmadaPlatInitDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS    Status;

  Status = ArmadaPlatInitBoardSelect ();
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallProtocolInterface (&ImageHandle,
                  &gMarvellPlatformInitCompleteProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL);
  ASSERT_EFI_ERROR (Status);

  MvComPhyInit ();
  UtmiPhyInit ();
  MppInitialize ();

  return EFI_SUCCESS;
}
