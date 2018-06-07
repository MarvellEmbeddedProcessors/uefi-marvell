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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/BoardDesc.h>

//
// Common macros
//
#define MV_SOC_CP_BASE(Cp)               (0x8100000000 + (Cp) * 0x700000000)
#define MV_SOC_AP_BASE(Ap)               (0xe8000000 - (Ap) * 0x4000000)

//
// Platform description of ComPhy controllers
//
#define MV_SOC_COMPHY_BASE(Cp)           (MV_SOC_CP_BASE ((Cp)) + 0x441000)
#define MV_SOC_HPIPE3_BASE(Cp)           (MV_SOC_CP_BASE ((Cp)) + 0x120000)
#define MV_SOC_COMPHY_LANE_COUNT         6
#define MV_SOC_COMPHY_MUX_BITS           4

EFI_STATUS
EFIAPI
ArmadaSoCDescApBaseGet (
  IN OUT UINT64  *ApBase,
  IN UINT8        ApNr
  )
{
  if (ApNr >= 4) {
    DEBUG ((DEBUG_ERROR, "%a: Max 4 AP in A8K-p SoC\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  *ApBase = MV_SOC_AP_BASE(ApNr);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ArmadaSoCDescComPhyGet (
  IN OUT MV_SOC_COMPHY_DESC  **ComPhyDesc,
  IN OUT UINT8                *DescCount
  )
{
  MV_SOC_COMPHY_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 CpIndex;

  Desc = AllocateZeroPool (CpCount * sizeof (MV_SOC_COMPHY_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    Desc[CpIndex].ComPhyBaseAddress = MV_SOC_COMPHY_BASE (CpIndex);
    Desc[CpIndex].ComPhyHpipe3BaseAddress = MV_SOC_HPIPE3_BASE (CpIndex);
    Desc[CpIndex].ComPhyLaneCount = MV_SOC_COMPHY_LANE_COUNT;
    Desc[CpIndex].ComPhyMuxBitCount = MV_SOC_COMPHY_MUX_BITS;
    Desc[CpIndex].ComPhyChipType = MvComPhyTypeCp110;
    Desc[CpIndex].ComPhyId = CpIndex;
  }

  *ComPhyDesc = Desc;
  *DescCount = CpCount;

  return EFI_SUCCESS;
}

//
// Platform description of GPIO
//
#define MVHW_AP_GPIO0_BASE             MV_SOC_AP_BASE (0) + 0x6F5040
#define MVHW_AP_GPIO0_PIN_COUNT        20
#define MVHW_CP0_GPIO0_BASE            MV_SOC_CP_BASE (0) + 0x440100
#define MVHW_CP0_GPIO0_PIN_COUNT       32
#define MVHW_CP0_GPIO1_BASE            MV_SOC_CP_BASE (0) + 0x440140
#define MVHW_CP0_GPIO1_PIN_COUNT       31
#define MVHW_CP1_GPIO0_BASE            MV_SOC_CP_BASE (1) + 0x440100
#define MVHW_CP1_GPIO0_PIN_COUNT       32
#define MVHW_CP1_GPIO1_BASE            MV_SOC_CP_BASE (1) + 0x440140
#define MVHW_CP1_GPIO1_PIN_COUNT       31

STATIC
MVHW_GPIO_DESC mA7k8kGpioDescTemplate = {
  5,
  { MVHW_AP_GPIO0_BASE, MVHW_CP0_GPIO0_BASE, MVHW_CP0_GPIO1_BASE,
    MVHW_CP1_GPIO0_BASE, MVHW_CP1_GPIO1_BASE},
  { MVHW_AP_GPIO0_PIN_COUNT, MVHW_CP0_GPIO0_PIN_COUNT,
    MVHW_CP0_GPIO1_PIN_COUNT, MVHW_CP1_GPIO0_PIN_COUNT,
    MVHW_CP1_GPIO1_PIN_COUNT},
};

EFI_STATUS
EFIAPI
ArmadaSoCDescGpioGet (
  IN OUT MVHW_GPIO_DESC **GpioDesc
  )
{
  MVHW_GPIO_DESC *Desc;

  Desc = AllocateCopyPool (sizeof (mA7k8kGpioDescTemplate),
           &mA7k8kGpioDescTemplate);
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  *GpioDesc = Desc;

  return EFI_SUCCESS;
}

//
// Platform description of I2C controllers
//
#define MV_SOC_I2C_PER_CP_COUNT         2
#define MV_SOC_I2C_BASE(I2c)         (0x701000 + (I2c) * 0x100)

EFI_STATUS
EFIAPI
ArmadaSoCDescI2cGet (
  IN OUT MV_SOC_I2C_DESC  **I2cDesc,
  IN OUT UINT8             *DescCount
  )
{
  MV_SOC_I2C_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 Index, CpIndex, I2cIndex = 0;

  Desc = AllocateZeroPool (CpCount * MV_SOC_I2C_PER_CP_COUNT *
                           sizeof (MV_SOC_I2C_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (Index = 0; Index < MV_SOC_I2C_PER_CP_COUNT; Index++) {
      Desc[I2cIndex].I2cBaseAddress =
                         MV_SOC_CP_BASE (CpIndex) + MV_SOC_I2C_BASE (Index);
      I2cIndex++;
    }
  }

  *I2cDesc = Desc;
  *DescCount = I2cIndex;

  return EFI_SUCCESS;
}

//
// Platform description of MDIO controllers
//
#define MV_SOC_MDIO_BASE(Cp)            MV_SOC_CP_BASE ((Cp)) + 0x12A200
#define MV_SOC_MDIO_ID(Cp)              ((Cp))

EFI_STATUS
EFIAPI
ArmadaSoCDescMdioGet (
  IN OUT MV_SOC_MDIO_DESC  **MdioDesc,
  IN OUT UINT8             *DescCount
  )
{
  MV_SOC_MDIO_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 CpIndex;

  Desc = AllocateZeroPool (CpCount * sizeof (MV_SOC_MDIO_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    Desc[CpIndex].MdioId = MV_SOC_MDIO_ID (CpIndex);
    Desc[CpIndex].MdioBaseAddress = MV_SOC_MDIO_BASE (CpIndex);
  }

  *MdioDesc = Desc;
  *DescCount = CpCount;

  return EFI_SUCCESS;
}

//
// Platform description of NonDiscoverableDevices
//

//
// Platform description of AHCI controllers
//
#define MV_SOC_AHCI_BASE(Cp)            MV_SOC_CP_BASE ((Cp)) + 0x540000
#define MV_SOC_AHCI_ID(Cp)              ((Cp) % 2)

EFI_STATUS
EFIAPI
ArmadaSoCDescAhciGet (
  IN OUT MV_SOC_AHCI_DESC  **AhciDesc,
  IN OUT UINT8             *DescCount
  )
{
  MV_SOC_AHCI_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 CpIndex;

  Desc = AllocateZeroPool (CpCount * sizeof (MV_SOC_AHCI_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    Desc[CpIndex].AhciId = MV_SOC_AHCI_ID (CpIndex);
    Desc[CpIndex].AhciBaseAddress = MV_SOC_AHCI_BASE (CpIndex);
    Desc[CpIndex].AhciMemSize = SIZE_8KB;
    Desc[CpIndex].AhciDmaType = NonDiscoverableDeviceDmaTypeCoherent;
  }

  *AhciDesc = Desc;
  *DescCount = CpCount;

  return EFI_SUCCESS;
}

//
// Platform description of SDMMC controllers
//
#define MV_SOC_MAX_SDMMC_COUNT     2
#define MV_SOC_SDMMC_CP_BASE(Cp)   MV_SOC_CP_BASE ((Cp)) + 0x780000
#define MV_SOC_SDMMC_AP_BASE(Ap)   MV_SOC_AP_BASE ((Ap)) + 0x6E0000
#define MV_SOC_SDMMC_BASE(Index)   ((Index) == 0 ? \
                                    MV_SOC_AP_BASE (0) : MV_SOC_CP_BASE (0))

EFI_STATUS
EFIAPI
ArmadaSoCDescSdMmcGet (
  IN OUT MV_SOC_SDMMC_DESC  **SdMmcDesc,
  IN OUT UINT8               *DescCount
  )
{
  MV_SOC_SDMMC_DESC *Desc;
  UINT8 Index;

  Desc = AllocateZeroPool (MV_SOC_MAX_SDMMC_COUNT * sizeof (MV_SOC_SDMMC_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < MV_SOC_MAX_SDMMC_COUNT; Index++) {
    Desc[Index].SdMmcBaseAddress = MV_SOC_SDMMC_BASE (Index);
    Desc[Index].SdMmcMemSize = SIZE_1KB;
    Desc[Index].SdMmcDmaType = NonDiscoverableDeviceDmaTypeCoherent;
  }

  *SdMmcDesc = Desc;
  *DescCount = MV_SOC_MAX_SDMMC_COUNT;

  return EFI_SUCCESS;
}

//
// Platform description of XHCI controllers
//
#define MV_SOC_XHCI_PER_CP_COUNT         2
#define MV_SOC_XHCI_BASE(Xhci)           (0x500000 + (Xhci) * 0x10000)

EFI_STATUS
EFIAPI
ArmadaSoCDescXhciGet (
  IN OUT MV_SOC_XHCI_DESC  **XhciDesc,
  IN OUT UINT8              *DescCount
  )
{
  MV_SOC_XHCI_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 Index, CpIndex, XhciIndex = 0;

  Desc = AllocateZeroPool (CpCount * MV_SOC_XHCI_PER_CP_COUNT *
                           sizeof (MV_SOC_XHCI_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (Index = 0; Index < MV_SOC_XHCI_PER_CP_COUNT; Index++) {
      Desc[XhciIndex].XhciBaseAddress =
                         MV_SOC_CP_BASE (CpIndex) + MV_SOC_XHCI_BASE (Index);
      Desc[XhciIndex].XhciMemSize = SIZE_16KB;
      Desc[XhciIndex].XhciDmaType = NonDiscoverableDeviceDmaTypeCoherent;
      XhciIndex++;
    }
  }

  *XhciDesc = Desc;
  *DescCount = XhciIndex;

  return EFI_SUCCESS;
}

//
// Platform description of PP2 NIC
//
#define MV_SOC_PP2_BASE(Cp)             MV_SOC_CP_BASE ((Cp))
#define MV_SOC_PP2_CLK_FREQ             333333333

EFI_STATUS
EFIAPI
ArmadaSoCDescPp2Get (
  IN OUT MV_SOC_PP2_DESC  **Pp2Desc,
  IN OUT UINT8             *DescCount
  )
{
  MV_SOC_PP2_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 CpIndex;

  Desc = AllocateZeroPool (CpCount * sizeof (MV_SOC_PP2_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    Desc[CpIndex].Pp2BaseAddress = MV_SOC_PP2_BASE (CpIndex);
    Desc[CpIndex].Pp2ClockFrequency = MV_SOC_PP2_CLK_FREQ;
  }

  *Pp2Desc = Desc;
  *DescCount = CpCount;

  return EFI_SUCCESS;
}

//
// Platform description of UTMI PHY's
//
#define MV_SOC_UTMI_PER_CP_COUNT         2
#define MV_SOC_UTMI_ID(Utmi)             (Utmi)
#define MV_SOC_UTMI_BASE(Utmi)           (0x580000 + (Utmi) * 0x1000)
#define MV_SOC_UTMI_CFG_BASE             0x440440
#define MV_SOC_UTMI_USB_CFG_BASE         0x440420

EFI_STATUS
EFIAPI
ArmadaSoCDescUtmiGet (
  IN OUT MV_SOC_UTMI_DESC  **UtmiDesc,
  IN OUT UINT8              *DescCount
  )
{
  MV_SOC_UTMI_DESC *Desc;
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 Index, CpIndex, UtmiIndex = 0;

  Desc = AllocateZeroPool (CpCount * MV_SOC_UTMI_PER_CP_COUNT *
                           sizeof (MV_SOC_UTMI_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (Index = 0; Index < MV_SOC_UTMI_PER_CP_COUNT; Index++) {
      Desc[UtmiIndex].UtmiPhyId = MV_SOC_UTMI_ID (UtmiIndex);
      Desc[UtmiIndex].UtmiBaseAddress =
                         MV_SOC_CP_BASE (CpIndex) + MV_SOC_UTMI_BASE (Index);
      Desc[UtmiIndex].UtmiConfigAddress =
                                 MV_SOC_CP_BASE (CpIndex) + MV_SOC_UTMI_CFG_BASE;
      Desc[UtmiIndex].UsbConfigAddress =
                             MV_SOC_CP_BASE (CpIndex) + MV_SOC_UTMI_USB_CFG_BASE;
      UtmiIndex++;
    }
  }

  *UtmiDesc = Desc;
  *DescCount = UtmiIndex;

  return EFI_SUCCESS;
}

//
// Platform description of PCIe
//
#define MV_SOC_PCIE_PER_CP_COUNT         3
#define MV_SOC_PCIE_BASE(Pcie)           (0x600000 + (Pcie) * 0x20000)

EFI_STATUS
EFIAPI
ArmadaSoCDescPcieGet (
  IN OUT UINT8    *DevCount,
  IN OUT UINTN   **PcieRegBase
  )
{
  UINT8 CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINT8 Index, CpIndex, PcieIndex = 0;
  UINTN *RegBase;

  RegBase = AllocateZeroPool (CpCount * MV_SOC_PCIE_PER_CP_COUNT *
                              sizeof (UINTN));
  if (RegBase == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (Index = 0; Index < MV_SOC_PCIE_PER_CP_COUNT; Index++) {
      RegBase[PcieIndex] = MV_SOC_CP_BASE (CpIndex) + MV_SOC_PCIE_BASE (Index);
      PcieIndex++;
    }
  }

  *PcieRegBase = RegBase;
  *DevCount = PcieIndex;

  return EFI_SUCCESS;
}
