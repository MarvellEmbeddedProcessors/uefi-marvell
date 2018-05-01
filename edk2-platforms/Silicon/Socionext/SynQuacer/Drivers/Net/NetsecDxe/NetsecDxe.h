/** @file

  Copyright (c) 2016 Socionext Inc. All rights reserved.<BR>
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __NETSEC_DXE_H_
#define __NETSEC_DXE_H_

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DmaLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NetLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/NonDiscoverableDevice.h>

#include "netsec_for_uefi/netsec_sdk/include/ogma_api.h"
#include "netsec_for_uefi/pfdep.h"

extern EFI_COMPONENT_NAME2_PROTOCOL gNetsecDriverComponentName2;

/*--------------- Simple Network Driver entry point functions ----------------*/

// Refer to the Simple Network Protocol section (21.1)
// in the UEFI 2.3.1 Specification for documentation.

#define ReturnUnlock(s)   do { Status = (s); goto ExitUnlock; } while (0)

/*------------------------------------------------------------------------------
  NETSEC Information Structure
------------------------------------------------------------------------------*/

#pragma pack(1)
typedef struct {
  MAC_ADDR_DEVICE_PATH              Netsec;
  EFI_DEVICE_PATH_PROTOCOL          End;
} NETSEC_DEVICE_PATH;
#pragma pack()

typedef struct {
  // Driver signature
  UINT32                            Signature;
  EFI_HANDLE                        ControllerHandle;

  // EFI SNP protocol instances
  EFI_SIMPLE_NETWORK_PROTOCOL       Snp;
  EFI_SIMPLE_NETWORK_MODE           SnpMode;

  // EFI Snp statistics instance
  EFI_NETWORK_STATISTICS            Stats;

  // ogma handle
  ogma_handle_t                     Handle;

  // List of submitted TX buffers
  LIST_ENTRY                        TxBufferList;

  EFI_EVENT                         ExitBootEvent;

  NON_DISCOVERABLE_DEVICE           *Dev;

  NETSEC_DEVICE_PATH                DevicePath;

  UINTN                             PhyAddress;
} NETSEC_DRIVER;

#define NETSEC_SIGNATURE            SIGNATURE_32('n', 't', 's', 'c')
#define INSTANCE_FROM_SNP_THIS(a)   CR((a), NETSEC_DRIVER, Snp, NETSEC_SIGNATURE)

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/

EFI_STATUS
NetsecInit (
  IN  EFI_HANDLE        DriverBindingHandle,
  IN  EFI_HANDLE        ControllerHandle
  );

EFI_STATUS
NetsecRelease (
  IN  EFI_HANDLE        DriverBindingHandle,
  IN  EFI_HANDLE        ControllerHandle
  );

#define MAC_ADDRESS                 0x00

#define HM_ME_ADDRESS_H             0x08
#define HM_ME_ADDRESS_L             0x0C
#define HM_ME_SIZE                  0x10

#define MH_ME_ADDRESS_H             0x14
#define MH_ME_ADDRESS_L             0x18
#define MH_ME_SIZE                  0x1C

#define PACKET_ME_ADDRESS           0x20
#define PACKET_ME_SIZE              0x24

#define SCAT_NUM                    1

#define RXINT_TMR_CNT_US            0
#define RXINT_PKTCNT                1

#endif
