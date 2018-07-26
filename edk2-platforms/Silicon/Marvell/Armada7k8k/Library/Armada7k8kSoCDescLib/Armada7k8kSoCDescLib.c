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
#define MV_SOC_CP_BASE(Cp)               (0xF2000000 + (Cp) * 0x2000000)
#define MV_SOC_AP_BASE                   0xF0000000

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
  if (ApNr != 0) {
    DEBUG ((DEBUG_ERROR, "%a: Only one AP in A7K/A8K SoC\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  *ApBase = MV_SOC_AP_BASE;

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

UINTN
EFIAPI
ArmadaSoCDescCpBaseGet (
  IN UINTN        CpIndex
  )
{
  if (CpIndex >= FixedPcdGet8 (PcdMaxCpCount)) {
    DEBUG ((DEBUG_ERROR, "%a: Wrong CP index (%d)\n", __FUNCTION__, CpIndex));
    return 0;
  }

  return MV_SOC_CP_BASE (CpIndex);
}

//
// Platform description of Fuse controllers
//
#define MV_AP_FUSE_COUNT                3
#define MV_AP_FUSE_CTRL_BASE            (MV_SOC_AP_BASE + 0x6F8008)
#define MV_AP_FUSE_HD_MEN_BASE          (MV_SOC_AP_BASE + 0x6F9000)
#define MV_AP_FUSE_LD_MEN_BASE          (MV_SOC_AP_BASE + 0x6F8F00)
#define MV_CP_FUSE_CTRL_BASE(Cp)        (MV_SOC_CP_BASE(Cp) + 0x400008)
#define MV_CP_FUSE_LD_MEN_BASE(Cp)      (MV_SOC_CP_BASE(Cp) + 0x400F00)

#define MV_FUSE_COUNT_PER_CP            2
#define MV_FUSE_LD_ROW_LEN              256
#define MV_FUSE_LD_ROW_CNT              1
#define MV_FUSE_HD_ROW_LEN              65
#define MV_FUSE_HD_ROW_CNT              64

STATIC CONST CHAR16 *Cp0BankNameArray[] = {
L"CP0-LD0",
L"CP0-LD1"
};

STATIC CONST CHAR16 *Cp1BankNameArray[] = {
L"CP1-LD0",
L"CP1-LD1"
};


typedef enum {
  HighDensity = 0,
  LowDensity = 1
} FuseBankType;

typedef enum {
  BankRW = 0,
  BankRO = 1,
  BankWO = 2
} FuseBankOPMode;

STATIC MV_SOC_FUSE_DESC MvApFuseDesc[MV_AP_FUSE_COUNT] = {
  { 0, HighDensity, BankRW, MV_AP_FUSE_CTRL_BASE, MV_AP_FUSE_HD_MEN_BASE,
    MV_FUSE_HD_ROW_CNT, MV_FUSE_HD_ROW_LEN, 16, L"AP0-HD0", 0 },
  { 1, LowDensity, BankRO, MV_AP_FUSE_CTRL_BASE, MV_AP_FUSE_LD_MEN_BASE,
    MV_FUSE_LD_ROW_CNT, MV_FUSE_LD_ROW_LEN, 0, L"AP0-LD0", 0 },
  { 2, LowDensity, BankRW, MV_AP_FUSE_CTRL_BASE, MV_AP_FUSE_LD_MEN_BASE,
    MV_FUSE_LD_ROW_CNT, MV_FUSE_LD_ROW_LEN, 0, L"AP0-LD1", 0 }
};

EFI_STATUS
EFIAPI
ArmadaSoCDescFuseGet (
  IN OUT MV_SOC_FUSE_DESC  **FuseDesc,
  IN OUT UINTN              *DescCount
  )
{
  MV_SOC_FUSE_DESC *Desc;
  UINTN CpCount = FixedPcdGet8 (PcdMaxCpCount);
  UINTN CpIndex, FuseIndex = 0, LdIndex = 0;

  Desc = AllocateZeroPool ((CpCount * MV_FUSE_COUNT_PER_CP + MV_AP_FUSE_COUNT) *
                           sizeof (MV_SOC_FUSE_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  /* AP Fuse Description */
  CopyMem (Desc, MvApFuseDesc, MV_AP_FUSE_COUNT * sizeof (MV_SOC_FUSE_DESC));

  /* CP Fuse Description */
  FuseIndex = MV_AP_FUSE_COUNT;
  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (LdIndex = 0; LdIndex < MV_FUSE_COUNT_PER_CP; LdIndex++, FuseIndex++) {
      Desc[FuseIndex].BankId        = FuseIndex;
      Desc[FuseIndex].BankType      = LowDensity;
      if (LdIndex == 0) {
        Desc[FuseIndex].BankOPMode  = BankRO;
      } else {
        Desc[FuseIndex].BankOPMode  = BankRW;
      }
      Desc[FuseIndex].BankRegBase   = MV_CP_FUSE_CTRL_BASE (CpIndex);
      Desc[FuseIndex].OTPMemBase    = MV_CP_FUSE_LD_MEN_BASE (CpIndex);
      Desc[FuseIndex].RowCount      = MV_FUSE_LD_ROW_CNT;
      Desc[FuseIndex].RowLength     = MV_FUSE_LD_ROW_LEN;
      if (CpIndex == 0) {
        CopyMem (Desc[FuseIndex].BankName, Cp0BankNameArray[LdIndex], 16);
      } else if (CpIndex == 1) {
        CopyMem (Desc[FuseIndex].BankName, Cp1BankNameArray[LdIndex], 16);
      }
    }
  }

  *FuseDesc = Desc;
  *DescCount = FuseIndex;

  return EFI_SUCCESS;
}

//
// Platform description of GPIO
//
#define MVHW_AP_GPIO0_BASE             0xF06F5040
#define MVHW_AP_GPIO0_PIN_COUNT        20
#define MVHW_CP0_GPIO0_BASE            0xF2440100
#define MVHW_CP0_GPIO0_PIN_COUNT       32
#define MVHW_CP0_GPIO1_BASE            0xF2440140
#define MVHW_CP0_GPIO1_PIN_COUNT       31
#define MVHW_CP1_GPIO0_BASE            0xF4440100
#define MVHW_CP1_GPIO0_PIN_COUNT       32
#define MVHW_CP1_GPIO1_BASE            0xF4440140
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
#define MV_SOC_I2C_PER_AP_COUNT   1
#define MV_SOC_I2C_AP_BASE        (MV_SOC_AP_BASE + 0x511000)
#define MV_SOC_I2C_PER_CP_COUNT   3
#define MV_SOC_I2C_CP_BASE(I2c)   ((I2c < 2) ? (0x701000 + (I2c) * 0x100) : 0x211000)

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

  *DescCount = CpCount * MV_SOC_I2C_PER_CP_COUNT + MV_SOC_I2C_PER_AP_COUNT;
  Desc = AllocateZeroPool (*DescCount * sizeof (MV_SOC_I2C_DESC));
  if (Desc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  Desc[I2cIndex].I2cBaseAddress = MV_SOC_I2C_AP_BASE;
  I2cIndex++;

  for (CpIndex = 0; CpIndex < CpCount; CpIndex++) {
    for (Index = 0; Index < MV_SOC_I2C_PER_CP_COUNT; Index++) {
      Desc[I2cIndex].I2cBaseAddress =
                         MV_SOC_CP_BASE (CpIndex) + MV_SOC_I2C_CP_BASE (Index);
      I2cIndex++;
    }
  }

  *I2cDesc = Desc;

  return EFI_SUCCESS;
}

//
// Allocate the MSI address per interrupt Group,
// unsupported Groups get NULL address.
//
STATIC
MV_SOC_ICU_DESC mA7k8kIcuDescTemplate = {
  0,                                           /* GIC SPI mapping offset */
  {
    {ICU_GROUP_NSR,  0xf03f0040, 0xf03f0048},  /* Non secure interrupts*/
    {ICU_GROUP_SR,   0x0,        0x0},         /* Secure interrupts */
    {ICU_GROUP_LPI,  0x0,        0x0},         /* LPI interrupts */
    {ICU_GROUP_VLPI, 0x0,        0x0},         /* Virtual LPI interrupts */
    {ICU_GROUP_SEI,  0xf03f0230, 0xf03f0230},  /* System error interrupts */
    {ICU_GROUP_REI,  0xf03f0270, 0xf03f0270},  /* RAM error interrupts */
  }
};

EFI_STATUS
EFIAPI
ArmadaSoCDescIcuGet (
  IN OUT MV_SOC_ICU_DESC  **IcuDesc
  )
{
  *IcuDesc = AllocateCopyPool (sizeof (mA7k8kIcuDescTemplate),
               &mA7k8kIcuDescTemplate);
  if (*IcuDesc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

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
#define MV_SOC_SDMMC_BASE(Index)   ((Index) == 0 ? 0xF06E0000 : 0xF2780000)

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
