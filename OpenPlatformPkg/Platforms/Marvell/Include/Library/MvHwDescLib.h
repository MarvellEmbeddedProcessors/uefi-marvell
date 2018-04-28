/********************************************************************************
Copyright (C) 2017 Marvell International Ltd.

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

#ifndef __MVHWDESCLIB_H__
#define __MVHWDESCLIB_H__

#include <Library/MvComPhyLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>

//
// Helper macros
//

// Check if device is enabled - it expects PCD to be read to '<type>DeviceTable' array
#define MVHW_DEV_ENABLED(type, index) (type ## DeviceTable[index])

//
// CommonPhy devices description template definition
//
#define MVHW_MAX_COMPHY_DEVS       4

typedef struct {
  UINT8 ComPhyDevCount;
  UINTN ComPhyBaseAddresses[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyHpipe3BaseAddresses[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyLaneCount[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyMuxBitCount[MVHW_MAX_COMPHY_DEVS];
  MV_COMPHY_CHIP_TYPE ComPhyChipType[MVHW_MAX_COMPHY_DEVS];
} MVHW_COMPHY_DESC;

//
// GPIO devices description template definition
//
#define MVHW_MAX_GPIO_DEVS       5

typedef struct {
  UINT8 GpioDevCount;
  UINTN GpioBaseAddresses[MVHW_MAX_GPIO_DEVS];
  UINTN GpioPinCount[MVHW_MAX_GPIO_DEVS];
} MVHW_GPIO_DESC;

//
// I2C devices description template definition
//
#define MVHW_MAX_I2C_DEVS         4

typedef struct {
  UINT8 I2cDevCount;
  UINTN I2cBaseAddresses[MVHW_MAX_I2C_DEVS];
} MVHW_I2C_DESC;

//
// MDIO devices description template definition
//
#define MVHW_MAX_MDIO_DEVS         2

typedef struct {
  UINT8 MdioDevCount;
  UINTN MdioBaseAddresses[MVHW_MAX_MDIO_DEVS];
} MVHW_MDIO_DESC;

//
// Fuse devices description template definition
//
#define MVHW_MAX_FUSE_DEVS         7

typedef struct {
  UINT8 EfuseDevCount;
  UINT8 BankId[MVHW_MAX_FUSE_DEVS];
  UINT8 BankType[MVHW_MAX_FUSE_DEVS];
  UINT8 BankOPMode[MVHW_MAX_FUSE_DEVS];
  UINTN BankRegBase[MVHW_MAX_FUSE_DEVS];
  UINTN OTPMemBase[MVHW_MAX_FUSE_DEVS];
  UINTN RowCount[MVHW_MAX_FUSE_DEVS];
  UINTN RowLength[MVHW_MAX_FUSE_DEVS];
  UINTN RowStep[MVHW_MAX_FUSE_DEVS];
  CHAR16 BankName[MVHW_MAX_FUSE_DEVS][10];
  UINT8 PriBitOffset[MVHW_MAX_FUSE_DEVS];
} MVHW_EFUSE_DESC;

//
// NonDiscoverable devices description template definition
//
#define MVHW_MAX_XHCI_DEVS         4
#define MVHW_MAX_AHCI_DEVS         4
#define MVHW_MAX_SDHCI_DEVS        4

typedef struct {
  // XHCI
  UINT8 XhciDevCount;
  UINTN XhciBaseAddresses[MVHW_MAX_XHCI_DEVS];
  UINTN XhciMemSize[MVHW_MAX_XHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE XhciDmaType[MVHW_MAX_XHCI_DEVS];
  // AHCI
  UINT8 AhciDevCount;
  UINTN AhciBaseAddresses[MVHW_MAX_AHCI_DEVS];
  UINTN AhciMemSize[MVHW_MAX_AHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE AhciDmaType[MVHW_MAX_AHCI_DEVS];
  // SDHCI
  UINT8 SdhciDevCount;
  UINTN SdhciBaseAddresses[MVHW_MAX_SDHCI_DEVS];
  UINTN SdhciMemSize[MVHW_MAX_SDHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE SdhciDmaType[MVHW_MAX_SDHCI_DEVS];
} MVHW_NONDISCOVERABLE_DESC;

//
// PP2 NIC devices description template definition
//
#define MVHW_MAX_PP2_DEVS         4

typedef struct {
  UINT8 Pp2DevCount;
  UINTN Pp2BaseAddresses[MVHW_MAX_PP2_DEVS];
  UINTN Pp2ClockFrequency[MVHW_MAX_PP2_DEVS];
} MVHW_PP2_DESC;

//
// RealTimeClock devices description template definition
//
#define MVHW_MAX_RTC_DEVS         2

typedef struct {
  UINT8 RtcDevCount;
  UINTN RtcBaseAddresses[MVHW_MAX_RTC_DEVS];
  UINTN RtcMemSize[MVHW_MAX_RTC_DEVS];
} MVHW_RTC_DESC;

//
// UTMI PHY's description template definition
//

typedef struct {
  UINT8 UtmiDevCount;
  UINT32 UtmiPhyId[MVHW_MAX_XHCI_DEVS];
  UINTN UtmiBaseAddresses[MVHW_MAX_XHCI_DEVS];
  UINTN UtmiConfigAddresses[MVHW_MAX_XHCI_DEVS];
  UINTN UtmiUsbConfigAddresses[MVHW_MAX_XHCI_DEVS];
  UINTN UtmiMuxBitCount[MVHW_MAX_XHCI_DEVS];
} MVHW_UTMI_DESC;

//
// Platform description of CommonPhy devices
//
#define MVHW_CP0_COMPHY_BASE       0xF2441000
#define MVHW_CP0_HPIPE3_BASE       0xF2120000
#define MVHW_CP0_COMPHY_LANES      6
#define MVHW_CP0_COMPHY_MUX_BITS   4
#define MVHW_CP1_COMPHY_BASE       0xF4441000
#define MVHW_CP1_HPIPE3_BASE       0xF4120000
#define MVHW_CP1_COMPHY_LANES      6
#define MVHW_CP1_COMPHY_MUX_BITS   4

#define DECLARE_A7K8K_COMPHY_TEMPLATE \
STATIC \
MVHW_COMPHY_DESC mA7k8kComPhyDescTemplate = {\
  2,\
  { MVHW_CP0_COMPHY_BASE, MVHW_CP1_COMPHY_BASE },\
  { MVHW_CP0_HPIPE3_BASE, MVHW_CP1_HPIPE3_BASE },\
  { MVHW_CP0_COMPHY_LANES, MVHW_CP1_COMPHY_LANES },\
  { MVHW_CP0_COMPHY_MUX_BITS, MVHW_CP1_COMPHY_MUX_BITS },\
  { MvComPhyTypeCp110, MvComPhyTypeCp110 }\
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

#define DECLARE_A7K8K_GPIO_TEMPLATE \
STATIC \
MVHW_GPIO_DESC mA7k8kGpioDescTemplate = {\
  5,\
  { MVHW_AP_GPIO0_BASE, MVHW_CP0_GPIO0_BASE, MVHW_CP0_GPIO1_BASE, \
    MVHW_CP1_GPIO0_BASE, MVHW_CP1_GPIO1_BASE},\
  { MVHW_AP_GPIO0_PIN_COUNT, MVHW_CP0_GPIO0_PIN_COUNT, \
    MVHW_CP0_GPIO1_PIN_COUNT, MVHW_CP1_GPIO0_PIN_COUNT, \
    MVHW_CP1_GPIO1_PIN_COUNT},\
}

//
// Platform description of I2C devices
//
#define MVHW_CP0_I2C0_BASE       0xF2701000
#define MVHW_CP0_I2C1_BASE       0xF2701100
#define MVHW_CP1_I2C0_BASE       0xF4701000
#define MVHW_CP1_I2C1_BASE       0xF4701100

#define DECLARE_A7K8K_I2C_TEMPLATE \
STATIC \
MVHW_I2C_DESC mA7k8kI2cDescTemplate = {\
  4,\
  { MVHW_CP0_I2C0_BASE, MVHW_CP0_I2C1_BASE, MVHW_CP1_I2C0_BASE, MVHW_CP1_I2C1_BASE }\
}

//
// Platform description of MDIO devices
//
#define MVHW_CP0_MDIO_BASE       0xF212A200
#define MVHW_CP1_MDIO_BASE       0xF412A200

#define DECLARE_A7K8K_MDIO_TEMPLATE \
STATIC \
MVHW_MDIO_DESC mA7k8kMdioDescTemplate = {\
  2,\
  { MVHW_CP0_MDIO_BASE, MVHW_CP1_MDIO_BASE }\
}

//
// Platform description of efuse
//
#define MVHW_AP_EFUSE_CTRL_BASE             0xF06F8008
#define MVHW_AP_EFUSE_HD_MEN_BASE           0xF06F9000
#define MVHW_AP_EFUSE_LD_MEN_BASE           0xF06F8F00
#define MVHW_CP0_EFUSE_CTRL_BASE            0xF2400008
#define MVHW_CP0_EFUSE_LD_MEN_BASE          0xF2400F00
#define MVHW_CP1_EFUSE_CTRL_BASE            0xF4400008
#define MVHW_CP1_EFUSE_LD_MEN_BASE          0xF4400F00

#define MVHW_EFUSE_COUNT_PER_CP            2
#define MVHW_EFUSE_LD_ROW_LEN              256
#define MVHW_EFUSE_LD_ROW_CNT              1
#define MVHW_EFUSE_HD_ROW_LEN              65
#define MVHW_EFUSE_HD_ROW_CNT              64
#define MVHW_EFUSE_HD_ROW_STEP             16

#define MVHW_AP_EFUSE_HD0                 0
#define MVHW_AP_EFUSE_LD0                1
#define MVHW_AP_EFUSE_LD1                2
#define MVHW_CP0_EFUSE_LD0               3
#define MVHW_CP0_EFUSE_LD1               4
#define MVHW_CP1_EFUSE_LD0               5
#define MVHW_CP1_EFUSE_LD1               6

typedef enum {
  HighDensity = 0,
  LowDensity = 1
} FuseBankType;

typedef enum {
  BankRW = 0,
  BankRO = 1,
  BankWO = 2
} FuseBankOPMode;

#define DECLARE_A7K8K_EFUSE_TEMPLATE \
STATIC \
MVHW_EFUSE_DESC mA7k8kEfuseDescTemplate = {\
  7,\
  { MVHW_AP_EFUSE_HD0, MVHW_AP_EFUSE_LD0, MVHW_AP_EFUSE_LD1, MVHW_CP0_EFUSE_LD0,\
    MVHW_CP0_EFUSE_LD1, MVHW_CP1_EFUSE_LD0, MVHW_CP1_EFUSE_LD1 },\
  { HighDensity, LowDensity, LowDensity, LowDensity, LowDensity, LowDensity, LowDensity },\
  { BankRW, BankRO, BankRW, BankRO, BankRW, BankRO, BankRW },\
  { MVHW_AP_EFUSE_CTRL_BASE, MVHW_AP_EFUSE_CTRL_BASE, MVHW_AP_EFUSE_CTRL_BASE, MVHW_CP0_EFUSE_CTRL_BASE,\
    MVHW_CP0_EFUSE_CTRL_BASE, MVHW_CP1_EFUSE_CTRL_BASE, MVHW_CP1_EFUSE_CTRL_BASE },\
  { MVHW_AP_EFUSE_HD_MEN_BASE, MVHW_AP_EFUSE_LD_MEN_BASE, MVHW_AP_EFUSE_LD_MEN_BASE, MVHW_CP0_EFUSE_LD_MEN_BASE,\
    MVHW_CP0_EFUSE_LD_MEN_BASE, MVHW_CP1_EFUSE_LD_MEN_BASE, MVHW_CP1_EFUSE_LD_MEN_BASE },\
  { MVHW_EFUSE_HD_ROW_CNT, MVHW_EFUSE_LD_ROW_CNT, MVHW_EFUSE_LD_ROW_CNT, MVHW_EFUSE_LD_ROW_CNT,\
    MVHW_EFUSE_LD_ROW_CNT, MVHW_EFUSE_LD_ROW_CNT, MVHW_EFUSE_LD_ROW_CNT },\
  { MVHW_EFUSE_HD_ROW_LEN, MVHW_EFUSE_LD_ROW_LEN, MVHW_EFUSE_LD_ROW_LEN, MVHW_EFUSE_LD_ROW_LEN,\
    MVHW_EFUSE_LD_ROW_LEN, MVHW_EFUSE_LD_ROW_LEN, MVHW_EFUSE_LD_ROW_LEN },\
  { MVHW_EFUSE_HD_ROW_STEP, 0, 0, 0, 0, 0, 0 },\
  { L"AP0-HD0", L"AP0-LD0", L"AP0-LD1", L"CP0-LD0", L"CP0-LD1", L"CP1-LD0", L"CP1-LD1" },\
  { 0, 0, 0, 0, 0, 0, 0 }\
}

//
// Platform description of NonDiscoverable devices
//
#define MVHW_CP0_XHCI0_BASE        0xF2500000
#define MVHW_CP0_XHCI1_BASE        0xF2510000
#define MVHW_CP1_XHCI0_BASE        0xF4500000
#define MVHW_CP1_XHCI1_BASE        0xF4510000

#define MVHW_CP0_AHCI0_BASE        0xF2540000
#define MVHW_CP0_AHCI0_ID          0
#define MVHW_CP1_AHCI0_BASE        0xF4540000
#define MVHW_CP1_AHCI0_ID          1

#define MVHW_AP0_SDHCI0_BASE       0xF06E0000
#define MVHW_CP0_SDHCI0_BASE       0xF2780000

#define DECLARE_A7K8K_NONDISCOVERABLE_TEMPLATE   \
STATIC \
MVHW_NONDISCOVERABLE_DESC mA7k8kNonDiscoverableDescTemplate = {\
  4, /* XHCI */\
  { MVHW_CP0_XHCI0_BASE, MVHW_CP0_XHCI1_BASE, MVHW_CP1_XHCI0_BASE, MVHW_CP1_XHCI1_BASE },\
  { SIZE_16KB, SIZE_16KB, SIZE_16KB, SIZE_16KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent,\
    NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent },\
  2, /* AHCI */\
  { MVHW_CP0_AHCI0_BASE, MVHW_CP1_AHCI0_BASE },\
  { SIZE_8KB, SIZE_8KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent },\
  2, /* SDHCI */\
  { MVHW_AP0_SDHCI0_BASE, MVHW_CP0_SDHCI0_BASE },\
  { SIZE_1KB, SIZE_1KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent }\
}

//
// Platform description of Pp2 NIC devices
//
#define MVHW_CP0_PP2_BASE       0xF2000000
#define MVHW_CP1_PP2_BASE       0xF4000000
#define MVHW_PP2_CLK_FREQ       333333333

#define DECLARE_A7K8K_PP2_TEMPLATE \
STATIC \
MVHW_PP2_DESC mA7k8kPp2DescTemplate = {\
  2,\
  { MVHW_CP0_PP2_BASE, MVHW_CP1_PP2_BASE },\
  { MVHW_PP2_CLK_FREQ, MVHW_PP2_CLK_FREQ } \
}

//
// Platform description of RealTimeClock devices
//
#define MVHW_CP0_RTC0_BASE       0xF2284000
#define MVHW_CP1_RTC0_BASE       0xF4284000

#define DECLARE_A7K8K_RTC_TEMPLATE \
STATIC \
MVHW_RTC_DESC mA7k8kRtcDescTemplate = {\
  2,\
  { MVHW_CP0_RTC0_BASE, MVHW_CP1_RTC0_BASE },\
  { SIZE_4KB, SIZE_4KB }\
}

//
// Platform description of UTMI PHY's
//
#define MVHW_CP0_UTMI0_BASE            0xF2580000
#define MVHW_CP0_UTMI0_CFG_BASE        0xF2440440
#define MVHW_CP0_UTMI0_USB_CFG_BASE    0xF2440420
#define MVHW_CP0_UTMI0_ID              0x0
#define MVHW_CP0_UTMI1_BASE            0xF2581000
#define MVHW_CP0_UTMI1_CFG_BASE        0xF2440444
#define MVHW_CP0_UTMI1_USB_CFG_BASE    0xF2440420
#define MVHW_CP0_UTMI1_ID              0x1
#define MVHW_CP1_UTMI0_BASE            0xF4580000
#define MVHW_CP1_UTMI0_CFG_BASE        0xF4440440
#define MVHW_CP1_UTMI0_USB_CFG_BASE    0xF4440420
#define MVHW_CP1_UTMI0_ID              0x0
#define MVHW_CP1_UTMI1_BASE            0xF4581000
#define MVHW_CP1_UTMI1_CFG_BASE        0xF4440444
#define MVHW_CP1_UTMI1_USB_CFG_BASE    0xF4440420
#define MVHW_CP1_UTMI1_ID              0x1

#define DECLARE_A7K8K_UTMI_TEMPLATE \
STATIC \
MVHW_UTMI_DESC mA7k8kUtmiDescTemplate = {\
  4,\
  { MVHW_CP0_UTMI0_ID, MVHW_CP0_UTMI1_ID,\
    MVHW_CP1_UTMI0_ID, MVHW_CP1_UTMI1_ID },\
  { MVHW_CP0_UTMI0_BASE, MVHW_CP0_UTMI1_BASE,\
    MVHW_CP1_UTMI0_BASE, MVHW_CP1_UTMI1_BASE },\
  { MVHW_CP0_UTMI0_CFG_BASE, MVHW_CP0_UTMI1_CFG_BASE,\
    MVHW_CP1_UTMI0_CFG_BASE, MVHW_CP1_UTMI1_CFG_BASE },\
  { MVHW_CP0_UTMI0_USB_CFG_BASE, MVHW_CP0_UTMI1_USB_CFG_BASE,\
    MVHW_CP1_UTMI0_USB_CFG_BASE, MVHW_CP1_UTMI1_USB_CFG_BASE }\
}

#endif /* __MVHWDESCLIB_H__ */
