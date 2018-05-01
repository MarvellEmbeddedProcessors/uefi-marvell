/** @file

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <IndustryStandard/IoRemappingTable.h>

#define FIELD_OFFSET(type, name)            __builtin_offsetof(type, name)

#define STYX_PCIE_SMMU_BASE                 0xE0A00000
#define STYX_PCIE_SMMU_SIZE                 0x10000
#define STYX_PCIE_SMMU_INTERRUPT            0x16d

#define STYX_ETH0_SMMU_BASE                 0xE0600000
#define STYX_ETH0_SMMU_SIZE                 0x10000
#define STYX_ETH0_SMMU_INTERRUPT            0x170

#define STYX_ETH1_SMMU_BASE                 0xE0800000
#define STYX_ETH1_SMMU_SIZE                 0x10000
#define STYX_ETH1_SMMU_INTERRUPT            0x16f

#define STYX_SATA0_SMMU_BASE                0xE0200000
#define STYX_SATA0_SMMU_SIZE                0x10000
#define STYX_SATA0_SMMU_INTERRUPT           0x16c

#define STYX_SATA1_SMMU_BASE                0xE0C00000
#define STYX_SATA1_SMMU_SIZE                0x10000
#define STYX_SATA1_SMMU_INTERRUPT           0x16b

#pragma pack(1)
typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_SMMU_NODE       Node;
  EFI_ACPI_6_0_IO_REMAPPING_SMMU_INT        Context[1];
} STYX_SMMU_NODE;

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_RC_NODE         Node;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE        RcIdMapping[1];
} STYX_RC_NODE;

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_NAMED_COMP_NODE Node;
  CONST CHAR8                               Name[11];
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE        RcIdMapping[32];
} STYX_NC_NODE;

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_TABLE           Iort;
  STYX_SMMU_NODE                            PciSmmuNode;
  STYX_RC_NODE                              PciRcNode;

#if DO_XGBE
  STYX_SMMU_NODE                            Eth0SmmuNode;
  STYX_NC_NODE                              Eth0NamedNode;
  STYX_SMMU_NODE                            Eth1SmmuNode;
  STYX_NC_NODE                              Eth1NamedNode;
#endif

  STYX_SMMU_NODE                            Sata0SmmuNode;
  STYX_NC_NODE                              Sata0NamedNode;
  STYX_SMMU_NODE                            Sata1SmmuNode;
  STYX_NC_NODE                              Sata1NamedNode;
} STYX_IO_REMAPPING_STRUCTURE;

#define __STYX_SMMU_NODE(Base, Size, Irq)                   \
  {                                                         \
    {                                                       \
      EFI_ACPI_IORT_TYPE_SMMUv1v2,                          \
      sizeof(STYX_SMMU_NODE),                               \
      0x0,                                                  \
      0x0,                                                  \
      0x0,                                                  \
      0x0,                                                  \
    },                                                      \
    Base,                                                   \
    Size,                                                   \
    EFI_ACPI_IORT_SMMUv1v2_MODEL_MMU401,                    \
    EFI_ACPI_IORT_SMMUv1v2_FLAG_COH_WALK,                   \
    FIELD_OFFSET(EFI_ACPI_6_0_IO_REMAPPING_SMMU_NODE,       \
                 SMMU_NSgIrpt),                             \
    0x1,                                                    \
    sizeof(EFI_ACPI_6_0_IO_REMAPPING_SMMU_NODE),            \
    0x0,                                                    \
    0x0,                                                    \
    Irq,                                                    \
    EFI_ACPI_IORT_SMMUv1v2_INT_FLAG_LEVEL,                  \
    0x0,                                                    \
    0x0,                                                    \
  }, {                                                      \
    {                                                       \
      Irq,                                                  \
      EFI_ACPI_IORT_SMMUv1v2_INT_FLAG_LEVEL,                \
    },                                                      \
  }

#define __STYX_ID_MAPPING(In, Num, Out, Ref, Flags)         \
  {                                                         \
    In,                                                     \
    Num,                                                    \
    Out,                                                    \
    FIELD_OFFSET(STYX_IO_REMAPPING_STRUCTURE, Ref),         \
    Flags                                                   \
  }

#define __STYX_ID_MAPPING_SINGLE(Out, Ref)                  \
  {                                                         \
    0x0,                                                    \
    0x0,                                                    \
    Out,                                                    \
    FIELD_OFFSET(STYX_IO_REMAPPING_STRUCTURE, Ref),         \
    EFI_ACPI_IORT_ID_MAPPING_FLAGS_SINGLE                   \
  }

#define __STYX_NAMED_COMPONENT_NODE(Name)                   \
    {                                                       \
      {                                                     \
        EFI_ACPI_IORT_TYPE_NAMED_COMP,                      \
        sizeof(STYX_NC_NODE),                               \
        0x0,                                                \
        0x0,                                                \
        0x20,                                               \
        FIELD_OFFSET(STYX_NC_NODE, RcIdMapping),            \
      },                                                    \
      0x0,                                                  \
      EFI_ACPI_IORT_MEM_ACCESS_PROP_CCA,                    \
      0x0,                                                  \
      0x0,                                                  \
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_CPM |                  \
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_DACS,                  \
      40,                                                   \
    },                                                      \
    Name

STATIC STYX_IO_REMAPPING_STRUCTURE AcpiIort = {
  {
    AMD_ACPI_HEADER(EFI_ACPI_6_0_IO_REMAPPING_TABLE_SIGNATURE,
                    STYX_IO_REMAPPING_STRUCTURE,
                    EFI_ACPI_IO_REMAPPING_TABLE_REVISION),
#if DO_XGBE
    10,                                             // NumNodes
#else
    6,                                              // NumNodes
#endif
    sizeof(EFI_ACPI_6_0_IO_REMAPPING_TABLE),        // NodeOffset
    0                                               // Reserved
  }, {
    // PciSmmuNode
    __STYX_SMMU_NODE(STYX_PCIE_SMMU_BASE,
                     STYX_PCIE_SMMU_SIZE,
                     STYX_PCIE_SMMU_INTERRUPT)
  }, {
    // PciRcNode
    {
      {
        EFI_ACPI_IORT_TYPE_ROOT_COMPLEX,                    // Type
        sizeof(STYX_RC_NODE),                               // Length
        0x0,                                                // Revision
        0x0,                                                // Reserved
        0x1,                                                // NumIdMappings
        FIELD_OFFSET(STYX_RC_NODE, RcIdMapping),            // IdReference
      },
      EFI_ACPI_IORT_MEM_ACCESS_PROP_CCA,                    // CacheCoherent
      0x0,                                                  // AllocationHints
      0x0,                                                  // Reserved
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_CPM |
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_DACS,                  // MemoryAccessFlags
      EFI_ACPI_IORT_ROOT_COMPLEX_ATS_UNSUPPORTED,           // AtsAttribute
      0x0,                                                  // PciSegmentNumber
    }, {
      __STYX_ID_MAPPING(0x0, 0xffff, 0x0, PciSmmuNode, 0x0),
    }
#if DO_XGBE
  }, {
    // Eth0SmmuNode
    __STYX_SMMU_NODE(STYX_ETH0_SMMU_BASE,
                     STYX_ETH0_SMMU_SIZE,
                     STYX_ETH0_SMMU_INTERRUPT)
  }, {
    // Eth0NamedNode
    __STYX_NAMED_COMPONENT_NODE("\\_SB_.ETH0"),
    {
      __STYX_ID_MAPPING_SINGLE(0x00, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x01, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x02, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x03, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x04, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x05, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x06, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x07, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x08, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x09, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0A, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0B, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0C, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0D, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0E, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0F, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x10, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x11, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x12, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x13, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x14, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x15, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x16, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x17, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x18, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x19, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1A, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1B, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1C, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1D, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1E, Eth0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1F, Eth0SmmuNode),
    }
  }, {
    // Eth1SmmuNode
    __STYX_SMMU_NODE(STYX_ETH1_SMMU_BASE,
                     STYX_ETH1_SMMU_SIZE,
                     STYX_ETH1_SMMU_INTERRUPT)
  }, {
    // Eth1NamedNode
    __STYX_NAMED_COMPONENT_NODE("\\_SB_.ETH1"),
    {
      __STYX_ID_MAPPING_SINGLE(0x00, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x01, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x02, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x03, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x04, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x05, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x06, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x07, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x08, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x09, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0A, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0B, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0C, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0D, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0E, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0F, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x10, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x11, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x12, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x13, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x14, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x15, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x16, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x17, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x18, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x19, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1A, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1B, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1C, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1D, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1E, Eth1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1F, Eth1SmmuNode),
    }
#endif
  }, {
    // Sata0SmmuNode
    __STYX_SMMU_NODE(STYX_SATA0_SMMU_BASE,
                     STYX_SATA0_SMMU_SIZE,
                     STYX_SATA0_SMMU_INTERRUPT)
  }, {
    // Sata0NamedNode
    __STYX_NAMED_COMPONENT_NODE("\\_SB_.AHC0"),
    {
      __STYX_ID_MAPPING_SINGLE(0x00, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x01, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x02, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x03, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x04, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x05, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x06, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x07, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x08, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x09, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0A, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0B, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0C, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0D, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0E, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0F, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x10, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x11, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x12, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x13, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x14, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x15, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x16, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x17, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x18, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x19, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1A, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1B, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1C, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1D, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1E, Sata0SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1F, Sata0SmmuNode),
    }
  }, {
    // Sata1SmmuNode
    __STYX_SMMU_NODE(STYX_SATA1_SMMU_BASE,
                     STYX_SATA1_SMMU_SIZE,
                     STYX_SATA1_SMMU_INTERRUPT)
  }, {
    // Sata1NamedNode
    __STYX_NAMED_COMPONENT_NODE("\\_SB_.AHC1"),
    {
      __STYX_ID_MAPPING_SINGLE(0x00, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x01, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x02, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x03, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x04, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x05, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x06, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x07, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x08, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x09, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0A, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0B, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0C, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0D, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0E, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x0F, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x10, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x11, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x12, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x13, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x14, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x15, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x16, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x17, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x18, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x19, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1A, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1B, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1C, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1D, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1E, Sata1SmmuNode),
      __STYX_ID_MAPPING_SINGLE(0x1F, Sata1SmmuNode),
    }
  }
};

#pragma pack()

#define STYX_SOC_VERSION_MASK    0xFFF
#define STYX_SOC_VERSION_A0      0x000
#define STYX_SOC_VERSION_B0      0x010
#define STYX_SOC_VERSION_B1      0x011

EFI_ACPI_DESCRIPTION_HEADER *
IortHeader (
  VOID
  )
{
  if ((PcdGet32 (PcdSocCpuId) & STYX_SOC_VERSION_MASK) < STYX_SOC_VERSION_B1) {
    //
    // Silicon revisions prior to B1 have only one SATA port,
    // so omit the nodes of the second port in this case.
    //
    AcpiIort.Iort.NumNodes -= 2;
  }
  return (EFI_ACPI_DESCRIPTION_HEADER *)&AcpiIort.Iort.Header;
}
