/**
 * Copyright (c) 2014, AppliedMicro Corp. All rights reserved.
 * Copyright (c) 2016, Hisilicon Limited. All rights reserved.
 * Copyright (c) 2016, Linaro Limited. All rights reserved.
 *
 * This program and the accompanying materials
 * are licensed and made available under the terms and conditions of the BSD License
 * which accompanies this distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 **/

#include <Uefi.h>
#include <Protocol/EmbeddedGpio.h>
#include <Guid/EventGroup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/OemMiscLib.h>

#include "PciHostBridge.h"

UINTN RootBridgeNumber[PCIE_MAX_HOSTBRIDGE] = { PCIE_MAX_ROOTBRIDGE,PCIE_MAX_ROOTBRIDGE };

UINT64 RootBridgeAttribute[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {
    { //Host Bridge0
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
    },
    { //Host Bridge1
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
            EFI_PCI_HOST_BRIDGE_MEM64_DECODE,
    }
    };

EFI_PCI_ROOT_BRIDGE_DEVICE_PATH mEfiPciRootBridgeDevicePath[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {
  { //Host Bridge0
  /* Port 0 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A03),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 1 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A04),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 2 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A05),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 3 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A06),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 4 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A07),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 5 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A08),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 6 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A09),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 7 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0A),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    }
},
{ // Host Bridge1
  /* Port 0 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0B),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 1 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0C),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 2 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0D),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
  /* Port 3 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0E),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
   /* Port 4 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A0F),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 5 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A10),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 6 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A11),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    },
    /* Port 7 */
    {
      {
        {
          ACPI_DEVICE_PATH,
          ACPI_DP,
          {
            (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
            (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)
          }
        },
        EISA_PNP_ID(0x0A12),
        0
      },

      {
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        {
          END_DEVICE_PATH_LENGTH,
          0
        }
      }
    }
  }
};

EFI_HANDLE mDriverImageHandle;

PCI_HOST_BRIDGE_INSTANCE mPciHostBridgeInstanceTemplate = {
  PCI_HOST_BRIDGE_SIGNATURE,  // Signature
  NULL,                       // HostBridgeHandle
  0,                          // RootBridgeNumber
  {NULL, NULL},               // Head
  FALSE,                      // ResourceSubiteed
  TRUE,                       // CanRestarted
  {
    NotifyPhase,
    GetNextRootBridge,
    GetAttributes,
    StartBusEnumeration,
    SetBusNumbers,
    SubmitResources,
    GetProposedResources,
    PreprocessController
  }
};

/**
  Entry point of this driver

  @param ImageHandle     Handle of driver image
  @param SystemTable     Point to EFI_SYSTEM_TABLE

  @retval EFI_OUT_OF_RESOURCES  Can not allocate memory resource
  @retval EFI_DEVICE_ERROR      Can not install the protocol instance
  @retval EFI_SUCCESS           Success to initialize the Pci host bridge.
**/
EFI_STATUS
EFIAPI
InitializePciHostBridge (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status;
  UINTN                       Loop1;
  UINTN                       Loop2;
  PCI_HOST_BRIDGE_INSTANCE    *HostBridge = NULL;
  PCI_ROOT_BRIDGE_INSTANCE    *PrivateData;
  UINT32       PcieRootBridgeMask;

  if (!OemIsMpBoot())
  {
    PcieRootBridgeMask = PcdGet32(PcdPcieRootBridgeMask);
  }
  else
  {
    PcieRootBridgeMask = PcdGet32(PcdPcieRootBridgeMask2P);
  }

  mDriverImageHandle = ImageHandle;
  //
  // Create Host Bridge Device Handle
  //
  //Each Host Bridge have 8 Root Bridges max, every bits of 0xFF(8 bit) stands for the according PCIe Port
  //is enable or not
  for (Loop1 = 0; Loop1 < PCIE_MAX_HOSTBRIDGE; Loop1++) {
    if (((PcieRootBridgeMask >> (PCIE_MAX_ROOTBRIDGE * Loop1)) & 0xFF ) == 0) {
      continue;
    }


    HostBridge = AllocateCopyPool (sizeof(PCI_HOST_BRIDGE_INSTANCE), &mPciHostBridgeInstanceTemplate);
    if (HostBridge == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    HostBridge->RootBridgeNumber = RootBridgeNumber[Loop1];
    InitializeListHead (&HostBridge->Head);

    Status = gBS->InstallMultipleProtocolInterfaces (
                  &HostBridge->HostBridgeHandle,
                  &gEfiPciHostBridgeResourceAllocationProtocolGuid, &HostBridge->ResAlloc,
                  NULL
                  );
    if (EFI_ERROR (Status)) {
      FreePool (HostBridge);
      return EFI_DEVICE_ERROR;
    }

    //
    // Create Root Bridge Device Handle in this Host Bridge
    //
    for (Loop2 = 0; Loop2 < HostBridge->RootBridgeNumber; Loop2++) {
      if (!(((PcieRootBridgeMask >> (PCIE_MAX_ROOTBRIDGE * Loop1)) >> Loop2 ) & 0x01)) {
        continue;
      }

      PrivateData = AllocateZeroPool (sizeof(PCI_ROOT_BRIDGE_INSTANCE));
      if (PrivateData == NULL) {
        FreePool (HostBridge);
        return EFI_OUT_OF_RESOURCES;
      }
      PrivateData->Port = Loop2;
      PrivateData->SocType = PcdGet32(Pcdsoctype);
      PrivateData->Signature = PCI_ROOT_BRIDGE_SIGNATURE;
      PrivateData->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&mEfiPciRootBridgeDevicePath[Loop1][Loop2];

      (VOID)RootBridgeConstructor (
           &PrivateData->Io,
           HostBridge->HostBridgeHandle,
           RootBridgeAttribute[Loop1][Loop2],
           &mResAppeture[Loop1][Loop2],
           Loop1
      );

      Status = gBS->InstallMultipleProtocolInterfaces(
                      &PrivateData->Handle,
                      &gEfiDevicePathProtocolGuid,      PrivateData->DevicePath,
                      &gEfiPciRootBridgeIoProtocolGuid, &PrivateData->Io,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        (VOID)gBS->UninstallMultipleProtocolInterfaces (
                HostBridge->HostBridgeHandle,
                &gEfiPciHostBridgeResourceAllocationProtocolGuid, &HostBridge->ResAlloc,
                NULL
                );
        FreePool(PrivateData);
        FreePool (HostBridge);
        return EFI_DEVICE_ERROR;
      }
      // PCI  Memory Space
      Status = gDS->AddMemorySpace (
             EfiGcdMemoryTypeMemoryMappedIo,
             mResAppeture[Loop1][Loop2] .MemBase,
             mResAppeture[Loop1][Loop2] .MemLimit -mResAppeture[Loop1][Loop2] .MemBase + 1,
             0
      );
      if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR,"PCIE AddMemorySpace Error\n"));
      }
      InsertTailList (&HostBridge->Head, &PrivateData->Link);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
NotifyAllocateMemResources(
 IN PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance,
 IN PCI_RESOURCE_TYPE                     Index,
 IN OUT UINT64                            *AllocatedLenMem
)
{
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  EFI_STATUS                            ReturnStatus;
  UINT64                                AddrLen;
  UINTN                                 BitsOfAlignment;

  AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
  PCIE_DEBUG("Addrlen:%llx\n", AddrLen);
  // Get the number of '1' in Alignment.
  BitsOfAlignment = (UINTN) (HighBitSet64 (RootBridgeInstance->ResAllocNode[Index].Alignment) + 1);

  BaseAddress = (RootBridgeInstance->MemBase + *AllocatedLenMem +
                       RootBridgeInstance->ResAllocNode[Index].Alignment)
                        & ~(RootBridgeInstance->ResAllocNode[Index].Alignment);
  if ((BaseAddress + AddrLen - 1) > RootBridgeInstance->MemLimit) {
          ReturnStatus = EFI_OUT_OF_RESOURCES;
          RootBridgeInstance->ResAllocNode[Index].Length = 0;
          return ReturnStatus;
  }

  PCIE_DEBUG("(P)Mem32/64 request memory at:%llx\n", BaseAddress);
  ReturnStatus = gDS->AllocateMemorySpace (
                     EfiGcdAllocateAddress,
                     EfiGcdMemoryTypeMemoryMappedIo,
                     BitsOfAlignment,
                     AddrLen,
                     &BaseAddress,
                     mDriverImageHandle,
                     NULL
                 );

  if (!EFI_ERROR (ReturnStatus)) {
     // We were able to allocate the PCI memory
     RootBridgeInstance->ResAllocNode[Index].Base   = (UINTN)BaseAddress;
     RootBridgeInstance->ResAllocNode[Index].Status = ResAllocated;
     *AllocatedLenMem += AddrLen;
     PCIE_DEBUG("(P)Mem32/64 resource allocated:%llx\n", BaseAddress);

  } else {
      // Not able to allocate enough PCI memory
      if (ReturnStatus != EFI_OUT_OF_RESOURCES) {
        RootBridgeInstance->ResAllocNode[Index].Length = 0;
      }
    }
  return ReturnStatus;
}

EFI_STATUS
EFIAPI
NotifyAllocateResources(
 IN PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance
)
{
  EFI_STATUS                            ReturnStatus;
  LIST_ENTRY                            *List;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  PCI_RESOURCE_TYPE                     Index;

  ReturnStatus = EFI_SUCCESS;
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);

    UINT64 AllocatedLenMem = 0;
    for (Index = TypeIo; Index < TypeBus; Index++) {
      if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
        if(Index == TypeIo) {
          PCIE_DEBUG("NOT SUPPOER IO RESOURCES ON THIS PLATFORM\n");
        } else if ((Index >= TypeMem32) && (Index <= TypePMem64)) {
          ReturnStatus = NotifyAllocateMemResources(RootBridgeInstance,Index,&AllocatedLenMem);
        } else {
               ASSERT (FALSE);
        }
      }
    }

    List = List->ForwardLink;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
NotifyFreeResources(
  IN PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance
)
{
  EFI_STATUS                            ReturnStatus;
  LIST_ENTRY                            *List;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  PCI_RESOURCE_TYPE                     Index;
  UINT64                                AddrLen;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;

  ReturnStatus = EFI_SUCCESS;
  List = HostBridgeInstance->Head.ForwardLink;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    for (Index = TypeIo; Index < TypeBus; Index++) {
      if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
        AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        BaseAddress = RootBridgeInstance->ResAllocNode[Index].Base;

        if(Index <= TypePMem64){
          ReturnStatus = gDS->FreeMemorySpace (BaseAddress, AddrLen);
        }else{
          ASSERT (FALSE);
        }

        RootBridgeInstance->ResAllocNode[Index].Type      = Index;
        RootBridgeInstance->ResAllocNode[Index].Base      = 0;
        RootBridgeInstance->ResAllocNode[Index].Length    = 0;
        RootBridgeInstance->ResAllocNode[Index].Status    = ResNone;
      }
    }

    List = List->ForwardLink;
  }

  HostBridgeInstance->ResourceSubmited = FALSE;
  HostBridgeInstance->CanRestarted     = TRUE;
  return ReturnStatus;

}

VOID
EFIAPI
NotifyBeginEnumeration(
  IN PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance
)
{
  LIST_ENTRY                            *List;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  PCI_RESOURCE_TYPE                     Index;

  //
  // Reset the Each Root Bridge
  //
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    for (Index = TypeIo; Index < TypeMax; Index++) {
      RootBridgeInstance->ResAllocNode[Index].Type      = Index;
      RootBridgeInstance->ResAllocNode[Index].Base      = 0;
      RootBridgeInstance->ResAllocNode[Index].Length    = 0;
      RootBridgeInstance->ResAllocNode[Index].Status    = ResNone;
    }

    List = List->ForwardLink;
  }

  HostBridgeInstance->ResourceSubmited = FALSE;
  HostBridgeInstance->CanRestarted     = TRUE;
}

/**
   These are the notifications from the PCI bus driver that it is about to enter a certain
   phase of the PCI enumeration process.

   This member function can be used to notify the host bridge driver to perform specific actions,
   including any chipset-specific initialization, so that the chipset is ready to enter the next phase.
   Eight notification points are defined at this time. See belows:
   EfiPciHostBridgeBeginEnumeration       Resets the host bridge PCI apertures and internal data
                                          structures. The PCI enumerator should issue this notification
                                          before starting a fresh enumeration process. Enumeration cannot
                                          be restarted after sending any other notification such as
                                          EfiPciHostBridgeBeginBusAllocation.
   EfiPciHostBridgeBeginBusAllocation     The bus allocation phase is about to begin. No specific action is
                                          required here. This notification can be used to perform any
                                          chipset-specific programming.
   EfiPciHostBridgeEndBusAllocation       The bus allocation and bus programming phase is complete. No
                                          specific action is required here. This notification can be used to
                                          perform any chipset-specific programming.
   EfiPciHostBridgeBeginResourceAllocation
                                          The resource allocation phase is about to begin. No specific
                                          action is required here. This notification can be used to perform
                                          any chipset-specific programming.
   EfiPciHostBridgeAllocateResources      Allocates resources per previously submitted requests for all the PCI
                                          root bridges. These resource settings are returned on the next call to
                                          GetProposedResources(). Before calling NotifyPhase() with a Phase of
                                          EfiPciHostBridgeAllocateResource, the PCI bus enumerator is responsible
                                          for gathering I/O and memory requests for
                                          all the PCI root bridges and submitting these requests using
                                          SubmitResources(). This function pads the resource amount
                                          to suit the root bridge hardware, takes care of dependencies between
                                          the PCI root bridges, and calls the Global Coherency Domain (GCD)
                                          with the allocation request. In the case of padding, the allocated range
                                          could be bigger than what was requested.
   EfiPciHostBridgeSetResources           Programs the host bridge hardware to decode previously allocated
                                          resources (proposed resources) for all the PCI root bridges. After the
                                          hardware is programmed, reassigning resources will not be supported.
                                          The bus settings are not affected.
   EfiPciHostBridgeFreeResources          Deallocates resources that were previously allocated for all the PCI
                                          root bridges and resets the I/O and memory apertures to their initial
                                          state. The bus settings are not affected. If the request to allocate
                                          resources fails, the PCI enumerator can use this notification to
                                          deallocate previous resources, adjust the requests, and retry
                                          allocation.
   EfiPciHostBridgeEndResourceAllocation  The resource allocation phase is completed. No specific action is
                                          required here. This notification can be used to perform any chipsetspecific
                                          programming.

   @param[in] This                The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in] Phase               The phase during enumeration

   @retval EFI_NOT_READY          This phase cannot be entered at this time. For example, this error
                                  is valid for a Phase of EfiPciHostBridgeAllocateResources if
                                  SubmitResources() has not been called for one or more
                                  PCI root bridges before this call
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error. This error is valid
                                  for a Phase of EfiPciHostBridgeSetResources.
   @retval EFI_INVALID_PARAMETER  Invalid phase parameter
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
                                  This error is valid for a Phase of EfiPciHostBridgeAllocateResources if the
                                  previously submitted resource requests cannot be fulfilled or
                                  were only partially fulfilled.
   @retval EFI_SUCCESS            The notification was accepted without any errors.

**/
EFI_STATUS
EFIAPI
NotifyPhase(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  EFI_STATUS                            ReturnStatus;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  ReturnStatus = EFI_SUCCESS;

  switch (Phase) {

  case EfiPciHostBridgeBeginEnumeration:
  PCIE_DEBUG("Case EfiPciHostBridgeBeginEnumeration\n");
    if (HostBridgeInstance->CanRestarted) {
      NotifyBeginEnumeration(HostBridgeInstance);
    } else {
      //
      // Can not restart
      //
      return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeEndEnumeration:
    PCIE_DEBUG("Case EfiPciHostBridgeEndEnumeration\n");
    EnlargeAtuConfig0 (This);
    break;

  case EfiPciHostBridgeBeginBusAllocation:
    PCIE_DEBUG("Case EfiPciHostBridgeBeginBusAllocation\n");
    //
    // No specific action is required here, can perform any chipset specific programing
    //

    HostBridgeInstance->CanRestarted = FALSE;
    break;

  case EfiPciHostBridgeEndBusAllocation:
    PCIE_DEBUG("Case EfiPciHostBridgeEndBusAllocation\n");
    //
    // No specific action is required here, can perform any chipset specific programing
    //
    break;

  case EfiPciHostBridgeBeginResourceAllocation:
    PCIE_DEBUG("Case EfiPciHostBridgeBeginResourceAllocation\n");
    //
    // No specific action is required here, can perform any chipset specific programing
    //
    break;

  case EfiPciHostBridgeAllocateResources:
    PCIE_DEBUG("Case EfiPciHostBridgeAllocateResources\n");

    if (HostBridgeInstance->ResourceSubmited) {
      //
      // Take care of the resource dependencies between the root bridges
      //
     ReturnStatus = NotifyAllocateResources(HostBridgeInstance);
    } else {
      return EFI_NOT_READY;
    }
    //break;

  case EfiPciHostBridgeSetResources:
    PCIE_DEBUG("Case EfiPciHostBridgeSetResources\n");
    break;

  case EfiPciHostBridgeFreeResources:
    PCIE_DEBUG("Case EfiPciHostBridgeFreeResources\n");

    ReturnStatus = NotifyFreeResources(HostBridgeInstance);
    break;

  case EfiPciHostBridgeEndResourceAllocation:
    PCIE_DEBUG("Case EfiPciHostBridgeEndResourceAllocation\n");
    HostBridgeInstance->CanRestarted = FALSE;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  return ReturnStatus;
}

/**
   Return the device handle of the next PCI root bridge that is associated with this Host Bridge.

   This function is called multiple times to retrieve the device handles of all the PCI root bridges that
   are associated with this PCI host bridge. Each PCI host bridge is associated with one or more PCI
   root bridges. On each call, the handle that was returned by the previous call is passed into the
   interface, and on output the interface returns the device handle of the next PCI root bridge. The
   caller can use the handle to obtain the instance of the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
   for that root bridge. When there are no more PCI root bridges to report, the interface returns
   EFI_NOT_FOUND. A PCI enumerator must enumerate the PCI root bridges in the order that they
   are returned by this function.
   For D945 implementation, there is only one root bridge in PCI host bridge.

   @param[in]       This              The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in, out]  RootBridgeHandle  Returns the device handle of the next PCI root bridge.

   @retval EFI_SUCCESS            If parameter RootBridgeHandle = NULL, then return the first Rootbridge handle of the
                                  specific Host bridge and return EFI_SUCCESS.
   @retval EFI_NOT_FOUND          Can not find the any more root bridge in specific host bridge.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not an EFI_HANDLE that was
                                  returned on a previous call to GetNextRootBridge().
**/
EFI_STATUS
EFIAPI
GetNextRootBridge(
  IN       EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT   EFI_HANDLE                                       *RootBridgeHandle
  )
{
  BOOLEAN                               NoRootBridge;
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;

  NoRootBridge = TRUE;
  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;


  while (List != &HostBridgeInstance->Head) {
    NoRootBridge = FALSE;
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (*RootBridgeHandle == NULL) {
      //
      // Return the first Root Bridge Handle of the Host Bridge
      //
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    } else {
      if (*RootBridgeHandle == RootBridgeInstance->Handle) {
        //
        // Get next if have
        //
        List = List->ForwardLink;
        if (List!=&HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          *RootBridgeHandle = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_FOUND;
        }
      }
    }

    List = List->ForwardLink;
  } //end while

  if (NoRootBridge) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

/**
   Returns the allocation attributes of a PCI root bridge.

   The function returns the allocation attributes of a specific PCI root bridge. The attributes can vary
   from one PCI root bridge to another. These attributes are different from the decode-related
   attributes that are returned by the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.GetAttributes() member function. The
   RootBridgeHandle parameter is used to specify the instance of the PCI root bridge. The device
   handles of all the root bridges that are associated with this host bridge must be obtained by calling
   GetNextRootBridge(). The attributes are static in the sense that they do not change during or
   after the enumeration process. The hardware may provide mechanisms to change the attributes on
   the fly, but such changes must be completed before EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL is
   installed. The permitted values of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ATTRIBUTES are defined in
   "Related Definitions" below. The caller uses these attributes to combine multiple resource requests.
   For example, if the flag EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM is set, the PCI bus enumerator needs to
   include requests for the prefetchable memory in the nonprefetchable memory pool and not request any
   prefetchable memory.
      Attribute                                 Description
   ------------------------------------         ----------------------------------------------------------------------
   EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM         If this bit is set, then the PCI root bridge does not support separate
                                                windows for nonprefetchable and prefetchable memory. A PCI bus
                                                driver needs to include requests for prefetchable memory in the
                                                nonprefetchable memory pool.

   EFI_PCI_HOST_BRIDGE_MEM64_DECODE             If this bit is set, then the PCI root bridge supports 64-bit memory
                                                windows. If this bit is not set, the PCI bus driver needs to include
                                                requests for a 64-bit memory address in the corresponding 32-bit
                                                memory pool.

   @param[in]   This               The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in]   RootBridgeHandle   The device handle of the PCI root bridge in which the caller is interested. Type
                                   EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param[out]  Attributes         The pointer to attribte of root bridge, it is output parameter

   @retval EFI_INVALID_PARAMETER   Attribute pointer is NULL
   @retval EFI_INVALID_PARAMETER   RootBridgehandle is invalid.
   @retval EFI_SUCCESS             Success to get attribute of interested root bridge.

**/
EFI_STATUS
EFIAPI
GetAttributes(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                           *Attributes
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;

  if (Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      *Attributes = RootBridgeInstance->RootBridgeAttrib;
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  //
  // RootBridgeHandle is not an EFI_HANDLE
  // that was returned on a previous call to GetNextRootBridge()
  //
  return EFI_INVALID_PARAMETER;
}

/**
   Sets up the specified PCI root bridge for the bus enumeration process.

   This member function sets up the root bridge for bus enumeration and returns the PCI bus range
   over which the search should be performed in ACPI 2.0 resource descriptor format.

   @param[in]   This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
   @param[in]   RootBridgeHandle  The PCI Root Bridge to be set up.
   @param[out]  Configuration     Pointer to the pointer to the PCI bus resource descriptor.

   @retval EFI_INVALID_PARAMETER Invalid Root bridge's handle
   @retval EFI_OUT_OF_RESOURCES  Fail to allocate ACPI resource descriptor tag.
   @retval EFI_SUCCESS           Sucess to allocate ACPI resource descriptor.

**/
EFI_STATUS
EFIAPI
StartBusEnumeration(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                             **Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  VOID                                  *Buffer;
  UINT8                                 *Temp;
  UINT64                                BusStart;
  UINT64                                BusEnd;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      //
      // Set up the Root Bridge for Bus Enumeration
      //
      BusStart = RootBridgeInstance->BusBase;
      BusEnd   = RootBridgeInstance->BusLimit;
      //
      // Program the Hardware(if needed) if error return EFI_DEVICE_ERROR
      //

      Buffer = AllocatePool (sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Temp = (UINT8 *)Buffer;

      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Desc = 0x8A;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Len  = 0x2B;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->ResType = 2;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->GenFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->SpecificFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrSpaceGranularity = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMin = BusStart;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMax = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrLen = BusEnd - BusStart + 1;

      Temp = Temp + sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;

      *Configuration = Buffer;
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
   Programs the PCI root bridge hardware so that it decodes the specified PCI bus range.

   This member function programs the specified PCI root bridge to decode the bus range that is
   specified by the input parameter Configuration.
   The bus range information is specified in terms of the ACPI 2.0 resource descriptor format.

   @param[in] This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
   @param[in] RootBridgeHandle  The PCI Root Bridge whose bus range is to be programmed
   @param[in] Configuration     The pointer to the PCI bus resource descriptor

   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER  Configuration is NULL.
   @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration does not include a valid ACPI 2.0 bus resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration includes valid ACPI 2.0 resource descriptors other than
                                  bus descriptors.
   @retval EFI_INVALID_PARAMETER  Configuration contains one or more invalid ACPI resource descriptors.
   @retval EFI_INVALID_PARAMETER  "Address Range Minimum" is invalid for this root bridge.
   @retval EFI_INVALID_PARAMETER  "Address Range Length" is invalid for this root bridge.
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
   @retval EFI_SUCCESS            The bus range for the PCI root bridge was programmed.

**/
EFI_STATUS
EFIAPI
SetBusNumbers(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINT8                                 *Ptr;
  UINTN                                 BusStart;
  UINTN                                 BusEnd;
  UINTN                                 BusLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = Configuration;

  //
  // Check the Configuration is valid
  //
  if(*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->ResType != 2) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr += sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  if (*Ptr != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Ptr = Configuration;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      BusStart = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMin;
      BusLen = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrLen;
      BusEnd = BusStart + BusLen - 1;

      if (BusStart > BusEnd) {
        return EFI_INVALID_PARAMETER;
      }

      if ((BusStart < RootBridgeInstance->BusBase) || (BusEnd > RootBridgeInstance->BusLimit)) {
        return EFI_INVALID_PARAMETER;
      }

      //
      // Update the Bus Range
      //
      RootBridgeInstance->ResAllocNode[TypeBus].Base   = BusStart;
      RootBridgeInstance->ResAllocNode[TypeBus].Length = BusLen;
      RootBridgeInstance->ResAllocNode[TypeBus].Status = ResAllocated;

      //
      // Program the Root Bridge Hardware
      //

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

VOID
EFIAPI
SubmitGetResourceType(
 IN EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *Ptr,
 OUT   UINT64*                            Index
)
{
  switch (Ptr->ResType) {
    case 0:
      if (Ptr->AddrSpaceGranularity == 32) {
        if (Ptr->SpecificFlag == 0x06)
          *Index = TypePMem32;
        else
          *Index = TypeMem32;
      }

      if (Ptr->AddrSpaceGranularity == 64) {
        if (Ptr->SpecificFlag == 0x06)
          *Index = TypePMem64;
        else
          *Index = TypeMem64;
      }
      break;

    case 1:
      *Index = TypeIo;
      break;

    default:
      break;
  };

}

/**
   Submits the I/O and memory resource requirements for the specified PCI root bridge.

   This function is used to submit all the I/O and memory resources that are required by the specified
   PCI root bridge. The input parameter Configuration is used to specify the following:
   - The various types of resources that are required
   - The associated lengths in terms of ACPI 2.0 resource descriptor format

   @param[in] This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param[in] RootBridgeHandle  The PCI root bridge whose I/O and memory resource requirements are being submitted.
   @param[in] Configuration     The pointer to the PCI I/O and PCI memory resource descriptor.

   @retval EFI_SUCCESS            The I/O and memory resource requests for a PCI root bridge were accepted.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER  Configuration is NULL.
   @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration includes requests for one or more resource types that are
                                  not supported by this PCI root bridge. This error will happen if the caller
                                  did not combine resources according to Attributes that were returned by
                                  GetAllocAttributes().
   @retval EFI_INVALID_PARAMETER  Address Range Maximum" is invalid.
   @retval EFI_INVALID_PARAMETER  "Address Range Length" is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  "Address Space Granularity" is invalid for this PCI root bridge.

**/
EFI_STATUS
EFIAPI
SubmitResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *Ptr;
  UINT64                                AddrLen;
  UINT64                                Alignment;
  UINTN                                 Index;

  PCIE_DEBUG("In SubmitResources\n");
  //
  // Check the input parameter: Configuration
  //
  if (Configuration == NULL)
    return EFI_INVALID_PARAMETER;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Temp = (UINT8 *)Configuration;
  while ( *Temp == 0x8A)
    Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;

  if (*Temp != 0x79)
    return EFI_INVALID_PARAMETER;

  Temp = (UINT8 *)Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      while ( *Temp == 0x8A) {
        Ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;
        PCIE_DEBUG("Ptr->ResType:%d\n", Ptr->ResType);
        PCIE_DEBUG("Ptr->Addrlen:%llx\n", Ptr->AddrLen);
        PCIE_DEBUG("Ptr->AddrRangeMax:%llx\n", Ptr->AddrRangeMax);
        PCIE_DEBUG("Ptr->AddrRangeMin:%llx\n", Ptr->AddrRangeMin);
        PCIE_DEBUG("Ptr->SpecificFlag:%llx\n", Ptr->SpecificFlag);
        PCIE_DEBUG("Ptr->AddrSpaceGranularity:%d\n", Ptr->AddrSpaceGranularity);
        PCIE_DEBUG("RootBridgeInstance->RootBridgeAttrib:%llx\n", RootBridgeInstance->RootBridgeAttrib);
        //
        // Check address range alignment
        //
        if (Ptr->AddrRangeMax != (GetPowerOfTwo64 (Ptr->AddrRangeMax + 1) - 1)) {
          return EFI_INVALID_PARAMETER;
        }
        Index = 0;
        SubmitGetResourceType(Ptr,&Index);
        AddrLen = (UINTN) Ptr->AddrLen;
        Alignment = (UINTN) Ptr->AddrRangeMax;
        RootBridgeInstance->ResAllocNode[Index].Length  = AddrLen;
        RootBridgeInstance->ResAllocNode[Index].Alignment = Alignment;
        RootBridgeInstance->ResAllocNode[Index].Status  = ResRequested;
        HostBridgeInstance->ResourceSubmited = TRUE;

        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }
  return EFI_INVALID_PARAMETER;
}

/**
   Returns the proposed resource settings for the specified PCI root bridge.

   This member function returns the proposed resource settings for the specified PCI root bridge. The
   proposed resource settings are prepared when NotifyPhase() is called with a Phase of
   EfiPciHostBridgeAllocateResources. The output parameter Configuration
   specifies the following:
   - The various types of resources, excluding bus resources, that are allocated
   - The associated lengths in terms of ACPI 2.0 resource descriptor format

   @param[in]  This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param[in]  RootBridgeHandle  The PCI root bridge handle. Type EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param[out] Configuration     The pointer to the pointer to the PCI I/O and memory resource descriptor.

   @retval EFI_SUCCESS            The requested parameters were returned.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
SetResource(
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance,
  VOID                                  *Buffer

)
{
  UINTN                                 Index;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *Ptr;
  UINT64                                ResStatus;

  Temp = Buffer;

  for (Index = 0; Index < TypeBus; Index ++)
  {
    if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
      Ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;
      ResStatus = RootBridgeInstance->ResAllocNode[Index].Status;

      switch (Index) {

      case TypeIo:
        //
        // Io
        //
        Ptr->Desc = 0x8A;
        Ptr->Len  = 0x2B;
        Ptr->ResType = 1;
        Ptr->GenFlag = 0;
        Ptr->SpecificFlag = 0;
        /* PCIE Device Iobar address should be based on IoBase */
        Ptr->AddrRangeMin = RootBridgeInstance->IoBase;
        Ptr->AddrRangeMax = 0;
        Ptr->AddrTranslationOffset = \
             (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
        Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        break;

      case TypeMem32:
        //
        // Memory 32
        //
        Ptr->Desc = 0x8A;
        Ptr->Len  = 0x2B;
        Ptr->ResType = 0;
        Ptr->GenFlag = 0;
        Ptr->SpecificFlag = 0;
        Ptr->AddrSpaceGranularity = 32;
        /* PCIE device Bar should be based on PciRegionBase */
        if (RootBridgeInstance->PciRegionBase > MAX_UINT32) {
          DEBUG((DEBUG_ERROR, "PCIE Res(TypeMem32) unsupported.\n"));
          return EFI_UNSUPPORTED;
        }
        Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base - RootBridgeInstance->MemBase +
                            RootBridgeInstance->PciRegionBase;
        Ptr->AddrRangeMax = 0;
        Ptr->AddrTranslationOffset = \
             (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
        Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        break;

      case TypePMem32:
        //
        // Prefetch memory 32
        //
        Ptr->Desc = 0x8A;
        Ptr->Len  = 0x2B;
        Ptr->ResType = 0;
        Ptr->GenFlag = 0;
        Ptr->SpecificFlag = 6;
        Ptr->AddrSpaceGranularity = 32;
        /* PCIE device Bar should be based on PciRegionBase */
        if (RootBridgeInstance->PciRegionBase > MAX_UINT32) {
          DEBUG((DEBUG_ERROR, "PCIE Res(TypePMem32) unsupported.\n"));
          return EFI_UNSUPPORTED;
        }
        Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base - RootBridgeInstance->MemBase +
                            RootBridgeInstance->PciRegionBase;
        Ptr->AddrRangeMax = 0;
        Ptr->AddrTranslationOffset = \
             (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
        Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        break;

      case TypeMem64:
        //
        // Memory 64
        //
        Ptr->Desc = 0x8A;
        Ptr->Len  = 0x2B;
        Ptr->ResType = 0;
        Ptr->GenFlag = 0;
        Ptr->SpecificFlag = 0;
        Ptr->AddrSpaceGranularity = 64;
        /* PCIE device Bar should be based on PciRegionBase */
        Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base - RootBridgeInstance->MemBase +
                            RootBridgeInstance->PciRegionBase;
        Ptr->AddrRangeMax = 0;
        Ptr->AddrTranslationOffset = \
             (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
        Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        break;

      case TypePMem64:
        //
        // Prefetch memory 64
        //
        Ptr->Desc = 0x8A;
        Ptr->Len  = 0x2B;
        Ptr->ResType = 0;
        Ptr->GenFlag = 0;
        Ptr->SpecificFlag = 6;
        Ptr->AddrSpaceGranularity = 64;
        /* PCIE device Bar should be based on PciRegionBase */
        Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base - RootBridgeInstance->MemBase +
                            RootBridgeInstance->PciRegionBase;
        Ptr->AddrRangeMax = 0;
        Ptr->AddrTranslationOffset = \
             (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
        Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
        break;
      };
      PCIE_DEBUG("Ptr->ResType:%d\n", Ptr->ResType);
      PCIE_DEBUG("Ptr->Addrlen:%llx\n", Ptr->AddrLen);
      PCIE_DEBUG("Ptr->AddrRangeMax:%llx\n", Ptr->AddrRangeMax);
      PCIE_DEBUG("Ptr->AddrRangeMin:%llx\n", Ptr->AddrRangeMin);
      PCIE_DEBUG("Ptr->SpecificFlag:%llx\n", Ptr->SpecificFlag);
      PCIE_DEBUG("Ptr->AddrTranslationOffset:%d\n", Ptr->AddrTranslationOffset);
      PCIE_DEBUG("Ptr->AddrSpaceGranularity:%d\n", Ptr->AddrSpaceGranularity);

      Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
    }
  }
  ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;

  return EFI_SUCCESS;
}
/**
   Returns the proposed resource settings for the specified PCI root bridge.

   This member function returns the proposed resource settings for the specified PCI root bridge. The
   proposed resource settings are prepared when NotifyPhase() is called with a Phase of
   EfiPciHostBridgeAllocateResources. The output parameter Configuration
   specifies the following:
   - The various types of resources, excluding bus resources, that are allocated
   - The associated lengths in terms of ACPI 2.0 resource descriptor format

   @param[in]  This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param[in]  RootBridgeHandle  The PCI root bridge handle. Type EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param[out] Configuration     The pointer to the pointer to the PCI I/O and memory resource descriptor.

   @retval EFI_SUCCESS            The requested parameters were returned.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
GetProposedResources(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                             **Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINTN                                 Index;
  UINTN                                 Number;
  VOID                                  *Buffer;

  Buffer = NULL;
  Number = 0;

  PCIE_DEBUG("In GetProposedResources\n");
  //
  // Get the Host Bridge Instance from the resource allocation protocol
  //
  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  //
  // Enumerate the root bridges in this host bridge
  //
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      for (Index = 0; Index < TypeBus; Index ++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
          Number ++;
        }
      }

      Buffer = AllocateZeroPool (Number * sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      (VOID)SetResource(RootBridgeInstance,Buffer);

      *Configuration = Buffer;

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
   Provides the hooks from the PCI bus driver to every PCI controller (device/function) at various
   stages of the PCI enumeration process that allow the host bridge driver to preinitialize individual
   PCI controllers before enumeration.

   This function is called during the PCI enumeration process. No specific action is expected from this
   member function. It allows the host bridge driver to preinitialize individual PCI controllers before
   enumeration.

   @param This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param RootBridgeHandle  The associated PCI root bridge handle. Type EFI_HANDLE is defined in
                            InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param PciAddress        The address of the PCI device on the PCI bus. This address can be passed to the
                            EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL member functions to access the PCI
                            configuration space of the device. See Table 12-1 in the UEFI 2.0 Specification for
                            the definition of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS.
   @param Phase             The phase of the PCI device enumeration.

   @retval EFI_SUCCESS              The requested parameters were returned.
   @retval EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER    Phase is not a valid phase that is defined in
                                    EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE.
   @retval EFI_DEVICE_ERROR         Programming failed due to a hardware error. The PCI enumerator should
                                    not enumerate this device, including its child devices if it is a PCI-to-PCI
                                    bridge.

**/
EFI_STATUS
EFIAPI
PreprocessController (
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS               PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE              Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  LIST_ENTRY                            *List;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  //
  // Enumerate the root bridges in this host bridge
  //
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      break;
    }
    List = List->ForwardLink;
  }
  if (List == &HostBridgeInstance->Head) {
    return EFI_INVALID_PARAMETER;
  }

  if ((UINT32)Phase > EfiPciBeforeResourceCollection) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
