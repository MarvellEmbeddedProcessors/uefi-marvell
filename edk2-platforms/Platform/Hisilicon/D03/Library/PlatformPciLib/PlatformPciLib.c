/** @file

  Copyright (c) 2016, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2016, Linaro Limited. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/PcdLib.h>
#include <Library/PlatformPciLib.h>
UINT64 pcie_subctrl_base_1610[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {{0xa0000000, 0xa0000000,0xa0000000,0xa0000000,0xa0000000,0xa0000000,0xa0000000,0xa0000000},
                                        {0xb0000000,0xb0000000,0xb0000000,0xb0000000, 0xb0000000,0xb0000000,0xb0000000,0xb0000000}};
UINT64 PCIE_APB_SLAVE_BASE_1610[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {{0xa0090000, 0xa0200000, 0xa00a0000, 0xa00b0000},
                                      {0xb0090000, 0xb0200000, 0xb00a0000, 0xb00b0000}};
UINT64 PCIE_PHY_BASE_1610[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {{0xa00c0000, 0xa00d0000, 0xa00e0000, 0xa00f0000},
                                  {0xb00c0000,0xb00d0000, 0xb00e0000, 0xb00f0000}};
UINT64 PCIE_ITS_1610[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {{0xc6010040, 0xc6010040, 0xc6010040, 0xc6010040},
                                  {0xc6010040, 0xc6010040, 0xc6010040, 0xc6010040}};

PCI_ROOT_BRIDGE_RESOURCE_APPETURE  mResAppeture[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {
 {// HostBridge 0
  /* Port 0 */
  {
      PCI_HB0RB0_ECAM_BASE, //ecam
      0,  //BusBase
      31, //BusLimit
      PCI_HB0RB0_PCIREGION_BASE, //Membase
      PCI_HB0RB0_PCIREGION_BASE + PCI_HB0RB0_PCIREGION_SIZE - 1, //Memlimit
      PCI_HB0RB0_IO_BASE,  //IoBase
      (PCI_HB0RB0_CPUIOREGIONBASE + PCI_HB0RB0_IO_SIZE - 1), //IoLimit
      PCI_HB0RB0_CPUMEMREGIONBASE, //CpuMemRegionBase
      PCI_HB0RB0_CPUIOREGIONBASE,  //CpuIoRegionBase
      (PCI_HB0RB0_PCI_BASE),  //RbPciBar
      PCI_HB0RB0_PCIREGION_BASE, //PciRegionBase
      PCI_HB0RB0_PCIREGION_BASE + PCI_HB0RB0_PCIREGION_SIZE - 1, //PciRegionLimit

  },
  /* Port 1 */
  {
      PCI_HB0RB1_ECAM_BASE,//ecam
      224,  //BusBase
      254, //BusLimit
      PCI_HB0RB1_PCIREGION_BASE, //Membase
      PCI_HB0RB1_PCIREGION_BASE + PCI_HB0RB1_PCIREGION_SIZE - 1, //MemLimit
      (PCI_HB0RB1_IO_BASE),  //IoBase
      (PCI_HB0RB1_CPUIOREGIONBASE + PCI_HB0RB1_IO_SIZE - 1), //IoLimit
      PCI_HB0RB1_CPUMEMREGIONBASE, //CpuMemRegionBase
      PCI_HB0RB1_CPUIOREGIONBASE,  //CpuIoRegionBase
      (PCI_HB0RB1_PCI_BASE),  //RbPciBar
      PCI_HB0RB1_PCIREGION_BASE, //PciRegionBase
      PCI_HB0RB1_PCIREGION_BASE + PCI_HB0RB1_PCIREGION_SIZE - 1, //PciRegionLimit
  },
  /* Port 2 */
  {
      PCI_HB0RB2_ECAM_BASE,
      128,  //BusBase
      159, //BusLimit
      PCI_HB0RB2_PCIREGION_BASE ,//MemBase
      PCI_HB0RB2_PCIREGION_BASE + PCI_HB0RB2_PCIREGION_SIZE - 1, //MemLimit
      (PCI_HB0RB2_IO_BASE),  //IOBase
      (PCI_HB0RB2_CPUIOREGIONBASE + PCI_HB0RB2_IO_SIZE - 1), //IoLimit
      PCI_HB0RB2_CPUMEMREGIONBASE, //CpuMemRegionBase
      PCI_HB0RB2_CPUIOREGIONBASE,  //CpuIoRegionBase
      (PCI_HB0RB2_PCI_BASE),  //RbPciBar
      PCI_HB0RB2_PCIREGION_BASE, //PciRegionBase
      PCI_HB0RB2_PCIREGION_BASE + PCI_HB0RB2_PCIREGION_SIZE - 1, //PciRegionLimit
  },

  /* Port 3 */
  {
      PCI_HB0RB3_ECAM_BASE,
      96,  //BusBase
      127, //BusLimit
      (PCI_HB0RB3_ECAM_BASE),  //MemBase
      (PCI_HB0RB3_ECAM_BASE + PCI_HB0RB3_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB0RB3_PCI_BASE),  //RbPciBar
      0,
      0
  }
 },
{// HostBridge 1
  /* Port 0 */
  {
      PCI_HB1RB0_ECAM_BASE,
      128,  //BusBase
      159, //BusLimit
      (PCI_HB1RB0_ECAM_BASE),  //MemBase
      (PCI_HB1RB0_ECAM_BASE + PCI_HB1RB0_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB0_PCI_BASE),  //RbPciBar
      0,
      0
  },
  /* Port 1 */
  {
      PCI_HB1RB1_ECAM_BASE,
      160,  //BusBase
      191, //BusLimit
      (PCI_HB1RB1_ECAM_BASE),  //MemBase
      (PCI_HB1RB1_ECAM_BASE + PCI_HB1RB1_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB1_PCI_BASE),  //RbPciBar
      0,
      0
  },
  /* Port 2 */
  {
      PCI_HB1RB2_ECAM_BASE,
      192,  //BusBase
      223, //BusLimit
      (PCI_HB1RB2_ECAM_BASE),  //MemBase
      (PCI_HB1RB2_ECAM_BASE + PCI_HB1RB2_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB2_PCI_BASE),  //RbPciBar
      0,
      0
  },

  /* Port 3 */
  {
      PCI_HB1RB3_ECAM_BASE,
      224,  //BusBase
      255, //BusLimit
      (PCI_HB1RB3_ECAM_BASE),  //MemBase
      (PCI_HB1RB3_ECAM_BASE + PCI_HB1RB3_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB3_PCI_BASE),  //RbPciBar
      0,
      0
  }
 }
};

