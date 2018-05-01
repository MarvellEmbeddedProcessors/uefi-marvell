/** @file

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/ArmLib.h>
#include <Guid/ArmMpCoreInfo.h>

#include <Ppi/IscpPpi.h>
#include <Iscp.h>

/*----------------------------------------------------------------------------------------
 *                                  G L O B A L S
 *----------------------------------------------------------------------------------------
 */
//
// CoreInfo table
//
STATIC ARM_CORE_INFO mAmdMpCoreInfoTable[] = {
  {
    // Cluster 0, Core 0
    0x0, 0x0,
  },
  {
    // Cluster 0, Core 1
    0x0, 0x1,
  },
  {
    // Cluster 1, Core 0
    0x1, 0x0,
  },
  {
    // Cluster 1, Core 1
    0x1, 0x1,
  },
  {
    // Cluster 2, Core 0
    0x2, 0x0,
  },
  {
    // Cluster 2, Core 1
    0x2, 0x1,
  },
  {
    // Cluster 3, Core 0
    0x3, 0x0,
  },
  {
    // Cluster 3, Core 1
    0x3, 0x1,
  }
};

//
// Core count
//
STATIC UINTN mAmdCoreCount = sizeof (mAmdMpCoreInfoTable) / sizeof (ARM_CORE_INFO);


/*----------------------------------------------------------------------------------------
 *                                   P P I   L I S T
 *----------------------------------------------------------------------------------------
 */
STATIC EFI_PEI_ISCP_PPI            *PeiIscpPpi;


/*----------------------------------------------------------------------------------------
 *                            P P I   D E S C R I P T O R
 *----------------------------------------------------------------------------------------
 */
STATIC EFI_PEI_PPI_DESCRIPTOR mPlatInitPpiDescriptor =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdStyxPlatInitPpiGuid,
  NULL
};


/**
 *---------------------------------------------------------------------------------------
 *  PlatInitPeiEntryPoint
 *
 *  Description:
 *    Entry point of the PlatInit PEI module.
 *
 *  Control flow:
 *    Query platform parameters via ISCP.
 *
 *  Parameters:
 *    @param[in]      FfsHeader            EFI_PEI_FILE_HANDLE
 *    @param[in]      **PeiServices        Pointer to the PEI Services Table.
 *
 *    @return         EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
PlatInitPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE      FfsHeader,
  IN       CONST EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  AMD_MEMORY_RANGE_DESCRIPTOR IscpMemDescriptor = {0};
  ISCP_FUSE_INFO              IscpFuseInfo = {0};
  ISCP_CPU_RESET_INFO         CpuResetInfo = {0};
#if DO_XGBE == 1
  ISCP_MAC_INFO               MacAddrInfo = {0};
  UINT64                      MacAddr0, MacAddr1;
#endif
  UINTN                       CpuCoreCount, CpuMap, CpuMapSize;
  UINTN                       Index, CoreNum;
  UINT32                      *CpuIdReg = (UINT32 *)FixedPcdGet32 (PcdCpuIdRegister);

  DEBUG ((EFI_D_ERROR, "PlatInit PEIM Loaded\n"));

  // CPUID
  PcdSet32 (PcdSocCpuId, *CpuIdReg);
  DEBUG ((EFI_D_ERROR, "SocCpuId = 0x%X\n", PcdGet32 (PcdSocCpuId)));

  // Update core count based on PCD option
  if (mAmdCoreCount > PcdGet32 (PcdSocCoreCount)) {
    mAmdCoreCount = PcdGet32 (PcdSocCoreCount);
  }

  if (FixedPcdGetBool (PcdIscpSupport)) {
    Status = PeiServicesLocatePpi (&gPeiIscpPpiGuid, 0, NULL, (VOID**)&PeiIscpPpi);
    ASSERT_EFI_ERROR (Status);

    // Get fuse information from ISCP
    Status = PeiIscpPpi->ExecuteFuseTransaction (PeiServices, &IscpFuseInfo);
    ASSERT_EFI_ERROR (Status);

    CpuMap = IscpFuseInfo.SocConfiguration.CpuMap;
    CpuCoreCount = IscpFuseInfo.SocConfiguration.CpuCoreCount;
    CpuMapSize = sizeof (IscpFuseInfo.SocConfiguration.CpuMap) * 8;

    ASSERT (CpuMap != 0);
    ASSERT (CpuCoreCount != 0);
    ASSERT (CpuCoreCount <= CpuMapSize);

    // Update core count based on fusing
    if (mAmdCoreCount > CpuCoreCount) {
      mAmdCoreCount = CpuCoreCount;
    }
  }

  //
  // Update per-core information from ISCP
  //
  if (!FixedPcdGetBool (PcdIscpSupport)) {
    DEBUG ((EFI_D_ERROR, "Warning: Could not get CPU info via ISCP, using default values.\n"));
  } else {
    //
    // Walk CPU map to enumerate active cores
    //
    for (CoreNum = 0, Index = 0; CoreNum < CpuMapSize && Index < mAmdCoreCount; ++CoreNum) {
      if (CpuMap & 1) {
        CpuResetInfo.CoreNum = CoreNum;
        Status = PeiIscpPpi->ExecuteCpuRetrieveIdTransaction (
                   PeiServices, &CpuResetInfo );
        ASSERT_EFI_ERROR (Status);
        ASSERT (CpuResetInfo.CoreStatus.Status != CPU_CORE_DISABLED);
        ASSERT (CpuResetInfo.CoreStatus.Status != CPU_CORE_UNDEFINED);

        mAmdMpCoreInfoTable[Index].ClusterId = CpuResetInfo.CoreStatus.ClusterId;
        mAmdMpCoreInfoTable[Index].CoreId = CpuResetInfo.CoreStatus.CoreId;

        DEBUG ((EFI_D_ERROR, "Core[%d]: ClusterId = %d   CoreId = %d\n",
          Index, mAmdMpCoreInfoTable[Index].ClusterId,
          mAmdMpCoreInfoTable[Index].CoreId));

        // Next core in Table
        ++Index;
      }
      // Next core in Map
      CpuMap >>= 1;
    }

    // Update core count based on CPU map
    if (mAmdCoreCount > Index) {
      mAmdCoreCount = Index;
    }
  }

  // Update SocCoreCount on Dynamic PCD
  if (PcdGet32 (PcdSocCoreCount) != mAmdCoreCount) {
    PcdSet32 (PcdSocCoreCount, mAmdCoreCount);
  }

  DEBUG ((EFI_D_ERROR, "SocCoreCount = %d\n", PcdGet32 (PcdSocCoreCount)));

  // Build AmdMpCoreInfo HOB
  BuildGuidDataHob (&gAmdStyxMpCoreInfoGuid, mAmdMpCoreInfoTable, sizeof (ARM_CORE_INFO) * mAmdCoreCount);

  // Get SystemMemorySize from ISCP
  IscpMemDescriptor.Size0 = 0;
  if (FixedPcdGetBool (PcdIscpSupport)) {
    Status = PeiIscpPpi->ExecuteMemoryTransaction (PeiServices, &IscpMemDescriptor);
    ASSERT_EFI_ERROR (Status);

    // Update SystemMemorySize on Dynamic PCD
    if (IscpMemDescriptor.Size0) {
      PcdSet64 (PcdSystemMemorySize, IscpMemDescriptor.Size0);
    }
  }
  if (IscpMemDescriptor.Size0 == 0) {
    DEBUG ((EFI_D_ERROR, "Warning: Could not get SystemMemorySize via ISCP, using default value.\n"));
  }

  DEBUG ((EFI_D_ERROR, "SystemMemorySize = %ld\n", PcdGet64 (PcdSystemMemorySize)));

#if DO_XGBE == 1
  // Get MAC Address from ISCP
  if (FixedPcdGetBool (PcdIscpSupport)) {
    Status = PeiIscpPpi->ExecuteGetMacAddressTransaction (
               PeiServices, &MacAddrInfo );
    ASSERT_EFI_ERROR (Status);

    MacAddr0 = MacAddr1 = 0;
    for (Index = 0; Index < 6; ++Index) {
      MacAddr0 |= (UINT64)MacAddrInfo.MacAddress0[Index] << (Index * 8);
      MacAddr1 |= (UINT64)MacAddrInfo.MacAddress1[Index] << (Index * 8);
    }
    PcdSet64 (PcdEthMacA, MacAddr0);
    PcdSet64 (PcdEthMacB, MacAddr1);
  }

  DEBUG ((EFI_D_ERROR, "EthMacA = 0x%lX\n", PcdGet64 (PcdEthMacA)));
  DEBUG ((EFI_D_ERROR, "EthMacB = 0x%lX\n", PcdGet64 (PcdEthMacB)));
#endif

  // Let other PEI modules know we're done!
  Status = (*PeiServices)->InstallPpi (PeiServices, &mPlatInitPpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

