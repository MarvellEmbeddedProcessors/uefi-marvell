/*******************************************************************************
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

#include <Base.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include "Armada37x0LibMem.h"

// The total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS 16

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

STATIC ARM_MEMORY_REGION_DESCRIPTOR mVirtualMemoryTable[MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS];

// Obtain DRAM size basing on register values filled by early firmware.
STATIC
UINT64
GetDramSize (
  IN OUT UINT64 *MemSize
  )
{
  UINT64 BaseAddr;
  UINT8 RegionCode;
  UINT8 Cs;

  *MemSize = 0;

  for (Cs = 0; Cs < DRAM_MAX_CS_NUM; Cs++) {

    /* Exit loop on first disabled DRAM CS */
    if (!DRAM_CS_ENABLED (Cs)) {
      break;
    }

    /*
     * Sanity check for base address of next DRAM block.
     * Only continuous space will be used.
     */
    BaseAddr = GET_DRAM_REGION_BASE (Cs);
    if (BaseAddr != *MemSize) {
      DEBUG ((DEBUG_ERROR,
        "%a: DRAM blocks are not contiguous, limit size to 0x%llx\n",
        __FUNCTION__,
        *MemSize));
      return EFI_SUCCESS;
    }

    /* Decode area length for current CS from register value */
    RegionCode = GET_DRAM_REGION_SIZE_CODE (Cs);

    if (DRAM_REGION_SIZE_EVEN (RegionCode)) {
      *MemSize += GET_DRAM_REGION_SIZE_EVEN (RegionCode);
    } else if (DRAM_REGION_SIZE_ODD (RegionCode)) {
      *MemSize += GET_DRAM_REGION_SIZE_ODD (RegionCode);
    } else {
      DEBUG ((DEBUG_ERROR,
        "%a: Invalid memory region code (0x%x) for CS#%d\n",
        __FUNCTION__,
        RegionCode,
        Cs));
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  UINTN                         Index = 0;
  UINT64                        MemSize;
  UINT64                        ConfigSpaceBaseAddr;
  EFI_RESOURCE_ATTRIBUTE_TYPE   ResourceAttributes;
  EFI_STATUS                    Status;

  ASSERT (VirtualMemoryMap != NULL);

  ConfigSpaceBaseAddr = FixedPcdGet64 (PcdConfigSpaceBaseAddress);

  // Obtain total memory size from the hardware.
  Status = GetDramSize (&MemSize);
  if (EFI_ERROR (Status)) {
    MemSize = FixedPcdGet64 (PcdSystemMemorySize);
    DEBUG ((DEBUG_ERROR, "Limit total memory size to %d MB\n", MemSize / 1024 / 1024));
  }

  ResourceAttributes = (
      EFI_RESOURCE_ATTRIBUTE_PRESENT |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    ResourceAttributes,
    FixedPcdGet64 (PcdSystemMemoryBase),
    MemSize
    );

  // DDR
  mVirtualMemoryTable[Index].PhysicalBase    = FixedPcdGet64 (PcdSystemMemoryBase);
  mVirtualMemoryTable[Index].VirtualBase     = FixedPcdGet64 (PcdSystemMemoryBase);
  mVirtualMemoryTable[Index].Length          = MemSize;
  mVirtualMemoryTable[Index].Attributes      = DDR_ATTRIBUTES_CACHED;

  // Configuration space 0xD000_0000 - 0xD200_0000
  mVirtualMemoryTable[++Index].PhysicalBase  = ConfigSpaceBaseAddr;
  mVirtualMemoryTable[Index].VirtualBase     = ConfigSpaceBaseAddr;
  mVirtualMemoryTable[Index].Length          = 0x2000000;
  mVirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // End of Table
  mVirtualMemoryTable[++Index].PhysicalBase  = 0;
  mVirtualMemoryTable[Index].VirtualBase     = 0;
  mVirtualMemoryTable[Index].Length          = 0;
  mVirtualMemoryTable[Index].Attributes      = 0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = mVirtualMemoryTable;
}
