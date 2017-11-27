/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmLib.h>
#include <Library/PcdLib.h>

#include <Guid/Fdt.h>

#include "LinuxLoader.h"

#define ALIGN32_BELOW(addr)   ALIGN_POINTER(addr - 32,32)

#define IS_ADDRESS_IN_REGION(RegionStart, RegionSize, Address) \
    (((UINTN)(RegionStart) <= (UINTN)(Address)) && ((UINTN)(Address) <= ((UINTN)(RegionStart) + (UINTN)(RegionSize))))

EFI_STATUS
PrepareAtagList (
  IN  EFI_PHYSICAL_ADDRESS  SystemMemoryBase,
  IN  CONST CHAR8*          CommandLineString,
  IN  EFI_PHYSICAL_ADDRESS  InitrdImage,
  IN  UINTN                 InitrdImageSize,
  OUT EFI_PHYSICAL_ADDRESS  *AtagBase,
  OUT UINT32                *AtagSize
  );

STATIC
VOID
PreparePlatformHardware (
  VOID
  )
{
  //Note: Interrupts will be disabled by the GIC driver when ExitBootServices() will be called.

  // Clean before Disable else the Stack gets corrupted with old data.
  ArmCleanDataCache ();
  ArmDisableDataCache ();
  // Invalidate all the entries that might have snuck in.
  ArmInvalidateDataCache ();

  // Invalidate and disable the Instruction cache
  ArmDisableInstructionCache ();
  ArmInvalidateInstructionCache ();

  // Turn off MMU
  ArmDisableMmu ();
}

STATIC
EFI_STATUS
StartLinux (
  IN  EFI_PHYSICAL_ADDRESS  SystemMemoryBase,
  IN  EFI_PHYSICAL_ADDRESS  LinuxImage,
  IN  UINTN                 LinuxImageSize,
  IN  EFI_PHYSICAL_ADDRESS  KernelParamsAddress,
  IN  UINTN                 KernelParamsSize,
  IN  UINT32                MachineType
  )
{
  EFI_STATUS            Status;
  LINUX_KERNEL          LinuxKernel;

  // Shut down UEFI boot services. ExitBootServices() will notify every driver that created an event on
  // ExitBootServices event. Example the Interrupt DXE driver will disable the interrupts on this event.
  Status = ShutdownUefiBootServices ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "ERROR: Can not shutdown UEFI boot services. Status=0x%X\n", Status));
    return Status;
  }

  // Move the kernel parameters to any address inside the first 1MB.
  // This is necessary because the ARM Linux kernel requires
  // the FTD / ATAG List to reside entirely inside the first 1MB of
  // physical memory.
  //Note: There is no requirement on the alignment
  if (MachineType != ARM_FDT_MACHINE_TYPE) {
    if (((UINTN)KernelParamsAddress > LINUX_ATAG_MAX_OFFSET) && (KernelParamsSize < PcdGet32 (PcdArmLinuxAtagMaxOffset))) {
      KernelParamsAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)CopyMem (ALIGN32_BELOW (LINUX_ATAG_MAX_OFFSET - KernelParamsSize), (VOID*)(UINTN)KernelParamsAddress, KernelParamsSize);
    }
  } else {
    if (((UINTN)KernelParamsAddress > LINUX_FDT_MAX_OFFSET) && (KernelParamsSize < PcdGet32 (PcdArmLinuxFdtMaxOffset))) {
      KernelParamsAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)CopyMem (ALIGN32_BELOW (LINUX_FDT_MAX_OFFSET - KernelParamsSize), (VOID*)(UINTN)KernelParamsAddress, KernelParamsSize);
    }
  }

  if ((UINTN)LinuxImage > LINUX_KERNEL_MAX_OFFSET) {
    //Note: There is no requirement on the alignment
    LinuxKernel = (LINUX_KERNEL)CopyMem (ALIGN32_BELOW (LINUX_KERNEL_MAX_OFFSET - LinuxImageSize), (VOID*)(UINTN)LinuxImage, LinuxImageSize);
  } else {
    LinuxKernel = (LINUX_KERNEL)(UINTN)LinuxImage;
  }

  // Check if the Linux Image is a uImage
  if (*(UINT32*)LinuxKernel == LINUX_UIMAGE_SIGNATURE) {
    // Assume the Image Entry Point is just after the uImage header (64-byte size)
    LinuxKernel = (LINUX_KERNEL)((UINTN)LinuxKernel + 64);
    LinuxImageSize -= 64;
  }

  // Check there is no overlapping between kernel and its parameters
  // We can only assert because it is too late to fallback to UEFI (ExitBootServices has been called).
  ASSERT (!IS_ADDRESS_IN_REGION (LinuxKernel, LinuxImageSize, KernelParamsAddress) &&
          !IS_ADDRESS_IN_REGION (LinuxKernel, LinuxImageSize, KernelParamsAddress + KernelParamsSize));

  //
  // Switch off interrupts, caches, mmu, etc
  //
  PreparePlatformHardware ();

  // Register and print out performance information
  PERF_END (NULL, "BDS", NULL, 0);
  if (PerformanceMeasurementEnabled ()) {
    PrintPerformance ();
  }

  //
  // Start the Linux Kernel
  //

  // Outside BootServices, so can't use Print();
  DEBUG ((EFI_D_ERROR, "\nStarting the kernel:\n\n"));

  // Jump to kernel with register set
  LinuxKernel ((UINTN)0, MachineType, (UINTN)KernelParamsAddress);

  // Kernel should never exit
  // After Life services are not provided
  ASSERT (FALSE);
  // We cannot recover the execution at this stage
  while (1);
}

/**
  Start a Linux kernel from a Device Path

  @param  SystemMemoryBase      Base of the system memory
  @param  LinuxKernel           Device Path to the Linux Kernel
  @param  Parameters            Linux kernel arguments
  @param  Fdt                   Device Path to the Flat Device Tree
  @param  MachineType           ARM machine type value

  @retval EFI_SUCCESS           All drivers have been connected
  @retval EFI_NOT_FOUND         The Linux kernel Device Path has not been found
  @retval EFI_OUT_OF_RESOURCES  There is not enough resource memory to store the matching results.
  @retval RETURN_UNSUPPORTED    ATAG is not support by this architecture

**/
EFI_STATUS
BootLinuxAtag (
  IN  EFI_PHYSICAL_ADDRESS      SystemMemoryBase,
  IN  EFI_DEVICE_PATH_PROTOCOL* LinuxKernelDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL* InitrdDevicePath,
  IN  CONST CHAR8*              CommandLineArguments,
  IN  UINTN                     MachineType
  )
{
  EFI_STATUS            Status;
  UINT32                LinuxImageSize;
  UINT32                InitrdImageBaseSize = 0;
  UINT32                InitrdImageSize = 0;
  UINT32                AtagSize;
  EFI_PHYSICAL_ADDRESS  AtagBase;
  EFI_PHYSICAL_ADDRESS  LinuxImage;
  EFI_PHYSICAL_ADDRESS  InitrdImageBase = 0;
  EFI_PHYSICAL_ADDRESS  InitrdImage = 0;

  PERF_START (NULL, "BDS", NULL, 0);

  // Load the Linux kernel from a device path
  LinuxImage = LINUX_KERNEL_MAX_OFFSET;
  Status = BdsLoadImage (LinuxKernelDevicePath, AllocateMaxAddress, &LinuxImage, &LinuxImageSize);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Did not find Linux kernel.\n");
    return Status;
  }

  if (InitrdDevicePath) {
    // Load the initrd near to the Linux kernel
    InitrdImageBase = LINUX_KERNEL_MAX_OFFSET;
    Status = BdsLoadImage (InitrdDevicePath, AllocateMaxAddress, &InitrdImageBase, &InitrdImageBaseSize);
    if (Status == EFI_OUT_OF_RESOURCES) {
      Status = BdsLoadImage (InitrdDevicePath, AllocateAnyPages, &InitrdImageBase, &InitrdImageBaseSize);
    }
    if (EFI_ERROR (Status)) {
      Print (L"ERROR: Did not find initrd image.\n");
      goto EXIT_FREE_LINUX;
    }

    // Check if the initrd is a uInitrd
    if (*(UINT32*)((UINTN)InitrdImageBase) == LINUX_UIMAGE_SIGNATURE) {
      // Skip the 64-byte image header
      InitrdImage = (EFI_PHYSICAL_ADDRESS)((UINTN)InitrdImageBase + 64);
      InitrdImageSize = InitrdImageBaseSize - 64;
    } else {
      InitrdImage = InitrdImageBase;
      InitrdImageSize = InitrdImageBaseSize;
    }
  }

  //
  // Setup the Linux Kernel Parameters
  //

  // By setting address=0 we leave the memory allocation to the function
  Status = PrepareAtagList (SystemMemoryBase, CommandLineArguments, InitrdImage, InitrdImageSize, &AtagBase, &AtagSize);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Can not prepare ATAG list. Status=0x%X\n", Status);
    goto EXIT_FREE_INITRD;
  }

  return StartLinux (SystemMemoryBase, LinuxImage, LinuxImageSize, AtagBase, AtagSize, MachineType);

EXIT_FREE_INITRD:
  if (InitrdDevicePath) {
    gBS->FreePages (InitrdImageBase, EFI_SIZE_TO_PAGES (InitrdImageBaseSize));
  }

EXIT_FREE_LINUX:
  gBS->FreePages (LinuxImage, EFI_SIZE_TO_PAGES (LinuxImageSize));

  return Status;
}

/**
  Start a Linux kernel from a Device Path

  @param  LinuxKernelDevicePath  Device Path to the Linux Kernel
  @param  InitrdDevicePath       Device Path to the Initrd
  @param  CommandLineArguments   Linux command line

  @retval EFI_SUCCESS           All drivers have been connected
  @retval EFI_NOT_FOUND         The Linux kernel Device Path has not been found
  @retval EFI_OUT_OF_RESOURCES  There is not enough resource memory to store the matching results.

**/
EFI_STATUS
BootLinuxFdt (
  IN  EFI_PHYSICAL_ADDRESS      SystemMemoryBase,
  IN  EFI_DEVICE_PATH_PROTOCOL* LinuxKernelDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL* InitrdDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL* FdtDevicePath,
  IN  CONST CHAR8*              CommandLineArguments
  )
{
  EFI_STATUS               Status;
  UINT32                   LinuxImageSize;
  UINT32                   InitrdImageBaseSize = 0;
  UINT32                   InitrdImageSize = 0;
  VOID                     *InstalledFdtBase;
  UINT32                   FdtBlobSize;
  EFI_PHYSICAL_ADDRESS     FdtBlobBase;
  EFI_PHYSICAL_ADDRESS     LinuxImage;
  EFI_PHYSICAL_ADDRESS     InitrdImageBase = 0;
  EFI_PHYSICAL_ADDRESS     InitrdImage = 0;

  PERF_START (NULL, "BDS", NULL, 0);

  // Load the Linux kernel from a device path
  LinuxImage = LINUX_KERNEL_MAX_OFFSET;
  Status = BdsLoadImage (LinuxKernelDevicePath, AllocateMaxAddress, &LinuxImage, &LinuxImageSize);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Did not find Linux kernel.\n");
    return Status;
  }

  if (InitrdDevicePath) {
    InitrdImageBase = LINUX_KERNEL_MAX_OFFSET;
    Status = BdsLoadImage (InitrdDevicePath, AllocateMaxAddress, &InitrdImageBase, &InitrdImageBaseSize);
    if (Status == EFI_OUT_OF_RESOURCES) {
      Status = BdsLoadImage (InitrdDevicePath, AllocateAnyPages, &InitrdImageBase, &InitrdImageBaseSize);
    }
    if (EFI_ERROR (Status)) {
      Print (L"ERROR: Did not find initrd image.\n");
      goto EXIT_FREE_LINUX;
    }

    // Check if the initrd is a uInitrd
    if (*(UINT32*)((UINTN)InitrdImageBase) == LINUX_UIMAGE_SIGNATURE) {
      // Skip the 64-byte image header
      InitrdImage = (EFI_PHYSICAL_ADDRESS)((UINTN)InitrdImageBase + 64);
      InitrdImageSize = InitrdImageBaseSize - 64;
    } else {
      InitrdImage = InitrdImageBase;
      InitrdImageSize = InitrdImageBaseSize;
    }
  }

  if (FdtDevicePath == NULL) {
    //
    // Get the FDT from the Configuration Table.
    // The FDT will be reloaded in PrepareFdt() to a more appropriate
    // location for the Linux Kernel.
    //
    Status = EfiGetSystemConfigurationTable (&gFdtTableGuid, &InstalledFdtBase);
    if (EFI_ERROR (Status)) {
      Print (L"ERROR: Did not get the Device Tree blob (%r).\n", Status);
      goto EXIT_FREE_INITRD;
    }
    FdtBlobBase = (EFI_PHYSICAL_ADDRESS)(UINTN)InstalledFdtBase;
    FdtBlobSize = fdt_totalsize (InstalledFdtBase);
  } else {
    //
    // FDT device path explicitly defined. The FDT is relocated later to a
    // more appropriate location for the Linux kernel.
    //
    FdtBlobBase = LINUX_KERNEL_MAX_OFFSET;
    Status = BdsLoadImage (FdtDevicePath, AllocateMaxAddress, &FdtBlobBase, &FdtBlobSize);
    if (EFI_ERROR (Status)) {
      Print (L"ERROR: Did not find Device Tree blob (%r).\n", Status);
      goto EXIT_FREE_INITRD;
    }
  }

  // Update the Fdt with the Initrd information. The FDT will increase in size.
  // By setting address=0 we leave the memory allocation to the function
  Status = PrepareFdt (SystemMemoryBase, CommandLineArguments, InitrdImage, InitrdImageSize, &FdtBlobBase, &FdtBlobSize);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Can not load kernel with FDT. Status=%r\n", Status);
    goto EXIT_FREE_FDT;
  }

  return StartLinux (SystemMemoryBase, LinuxImage, LinuxImageSize, FdtBlobBase, FdtBlobSize, ARM_FDT_MACHINE_TYPE);

EXIT_FREE_FDT:
  gBS->FreePages (FdtBlobBase, EFI_SIZE_TO_PAGES (FdtBlobSize));

EXIT_FREE_INITRD:
  if (InitrdDevicePath) {
    gBS->FreePages (InitrdImageBase, EFI_SIZE_TO_PAGES (InitrdImageBaseSize));
  }

EXIT_FREE_LINUX:
  gBS->FreePages (LinuxImage, EFI_SIZE_TO_PAGES (LinuxImageSize));

  return Status;
}
