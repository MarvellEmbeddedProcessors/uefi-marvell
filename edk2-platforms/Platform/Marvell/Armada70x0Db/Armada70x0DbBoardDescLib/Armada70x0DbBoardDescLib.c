/**
*
*  Copyright (C) 2018, Marvell International Ltd. and its affiliates.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>

#include <Library/ArmadaBoardDescLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

//
// Platform description of PCIe
//
#define MV_70X0DB_CP0_PCIE0_BASE            0xE0000000
#define MV_70X0DB_CP0_IO_TRANSLATION        0xEFF00000
#define MV_70X0DB_CP0_IO_BASE               0
#define MV_70X0DB_CP0_IO_SIZE               0x10000
#define MV_70X0DB_CP0_MMIO32_BASE           0xC0000000
#define MV_70X0DB_CP0_MMIO32_SIZE           0x20000000
#define MV_70X0DB_CP0_MMIO64_BASE           0x800000000
#define MV_70X0DB_CP0_MMIO64_SIZE           0x100000000

STATIC
MV_BOARD_PCIE_DEV_DESC m70x0DbPcieDevDescTemplate[] = {
  {
    2, /* CP0 controller 2 */
    0, /* RegBase would be filled with the information from ArmadaSoCDescLib */
    0,
    0xfe,
    MV_70X0DB_CP0_PCIE0_BASE,
    MV_70X0DB_CP0_IO_TRANSLATION,
    MV_70X0DB_CP0_IO_BASE,
    MV_70X0DB_CP0_IO_SIZE,
    0,
    MV_70X0DB_CP0_MMIO32_BASE,
    MV_70X0DB_CP0_MMIO32_SIZE,
    0,
    MV_70X0DB_CP0_MMIO64_BASE,
    MV_70X0DB_CP0_MMIO64_SIZE,
    FALSE,
    { 0 }
   }
};

EFI_STATUS
EFIAPI
ArmadaBoardDescPcieGet (
  IN OUT UINT8                    *PcieDevCount,
  IN OUT MV_BOARD_PCIE_DEV_DESC  **PcieDesc
  )
{
  *PcieDevCount = ARRAY_SIZE (m70x0DbPcieDevDescTemplate);

  *PcieDesc = m70x0DbPcieDevDescTemplate;
  if (*PcieDesc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
  }

  return EFI_SUCCESS;
}
