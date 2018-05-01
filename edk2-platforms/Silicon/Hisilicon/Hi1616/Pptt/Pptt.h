/** @file
*
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Based on the files under Platform/ARM/JunoPkg/AcpiTables/
*
**/

#ifndef _PPTT_H_
#define _PPTT_H_

#include <IndustryStandard/Acpi.h>
#include <Library/ArmLib/ArmLibPrivate.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#include "../D05AcpiTables/Hi1616Platform.h"

#define PPTT_VENDOR_ID             SIGNATURE_32('H', 'I', 'S', 'I')

#define EFI_ACPI_MAX_NUM_TABLES    20

#define PPTT_TABLE_MAX_LEN         0x6000
#define PPTT_SOCKET_NO             0x2
#define PPTT_SCCL_NO               0x2
#define PPTT_CLUSTER_NO            0x4
#define PPTT_CORE_NO               0x4
#define PPTT_SOCKET_COMPONENT_NO   0x1
#define PPTT_CACHE_NO              0x4

typedef union {
  struct {
    UINT32    InD           :1;
    UINT32    Level         :3;
    UINT32    Reserved      :28;
  } Bits;
  UINT32 Data;
} CSSELR_DATA;

typedef union {
  struct {
    UINT32    LineSize           :3;
    UINT32    Associativity      :10;
    UINT32    NumSets            :15;
    UINT32    Wa                 :1;
    UINT32    Ra                 :1;
    UINT32    Wb                 :1;
    UINT32    Wt                 :1;
  } Bits;
  UINT32 Data;
} CCSIDR_DATA;

#endif    // _PPTT_H_

