/** @file

  ACPI Memory mapped configuration space base address Description Table (MCFG).
  Implementation based on PCI Firmware Specification Revision 3.0 final draft,
  downloadable at http://www.pcisig.com/home

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.

  This program and the accompanying materials are licensed and
  made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the
  license may be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>

//
// CSRT for ARM_CCN504 (L3 CACHE)
//
#define AMD_ACPI_ARM_CCN504_CSRT_REVISION 0
#define AMD_ACPI_ARM_CCN504_VENDOR_ID     SIGNATURE_32('A','R','M','H')
#define AMD_ACPI_ARM_CCN504_DEVICE_ID     0x510
#define AMD_ACPI_ARM_CCN504_RESOURCE_TYPE 0x04
#define AMD_ACPI_ARM_CCN504_DESC_VERSION  1
#define AMD_ACPI_ARM_CCN504_HNF_COUNT     8
#define AMD_ACPI_ARM_CCN504_BASE_ADDR     0xE8000000ULL
#define AMD_ACPI_ARM_CCN504_CACHE_SIZE    0x00800000ULL

//
// Ensure proper (byte-packed) structure formats
//
#pragma pack(push, 1)

typedef struct {
  UINT32  Version;
  UINT8   HnfRegionCount;
  UINT8   Reserved[3];
  UINT64  BaseAddress;
  UINT64  CacheSize;
} AMD_ACPI_ARM_CCN504_CSRT_DEVICE_DESCRIPTOR;

typedef struct {
  UINT32  Length;
  UINT16  ResourceType;
  UINT16  ResourceSubtype;
  UINT32  UID;
  AMD_ACPI_ARM_CCN504_CSRT_DEVICE_DESCRIPTOR Ccn504Desc;
} AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_DESCRIPTOR;

typedef struct {
  UINT32  Length;
  UINT32  VendorId;
  UINT32  SubvendorId;
  UINT16  DeviceId;
  UINT16  SubdeviceId;
  UINT16  Revision;
  UINT8   Reserved[2];
  UINT32  SharedInfoLength;
  AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_DESCRIPTOR RsrcDesc;
} AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_GROUP;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_GROUP RsrcGroup;
} AMD_ACPI_ARM_CCN504_CORE_SYSTEM_RESOURCE_TABLE;


AMD_ACPI_ARM_CCN504_CORE_SYSTEM_RESOURCE_TABLE AcpiCsrt = {
   AMD_ACPI_HEADER (EFI_ACPI_5_1_CORE_SYSTEM_RESOURCE_TABLE_SIGNATURE,
                    AMD_ACPI_ARM_CCN504_CORE_SYSTEM_RESOURCE_TABLE,
                    AMD_ACPI_ARM_CCN504_CSRT_REVISION),
  { sizeof (AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_GROUP), // UINT32  RsrcGroup.Length
    AMD_ACPI_ARM_CCN504_VENDOR_ID,                    // UINT32  RsrcGroup.VendorId
    0,                                                // UINT32  RsrcGroup.SubvendorId
    AMD_ACPI_ARM_CCN504_DEVICE_ID,                    // UINT16  RsrcGroup.DeviceId
    0,                                                // UINT16  RsrcGroup.SubdeviceId
    0,                                                // UINT16  RsrcGroup.Revision
    { 0 },                                            // UINT8   RsrcGroup.Reserved[]
    0,                                                // UINT32  RsrcGroup.SharedInfoLength
    { sizeof (AMD_ACPI_ARM_CCN504_CSRT_RESOURCE_DESCRIPTOR),  // UINT32  RsrcDesc.Length
      AMD_ACPI_ARM_CCN504_RESOURCE_TYPE,                      // UINT16  RsrcDesc.ResourceType
      0,                                                      // UINT16  RsrcDesc.ResourceSubtype
      0,                                                      // UINT32  RsrcDesc.UID
      { AMD_ACPI_ARM_CCN504_DESC_VERSION,                     // UINT32  Ccn504Desc.Version
        AMD_ACPI_ARM_CCN504_HNF_COUNT,                        // UINT8   Ccn504Desc.HnfRegionCount
        { 0 },                                                // UINT8   Ccn504Desc.Reserved[]
        AMD_ACPI_ARM_CCN504_BASE_ADDR,                        // UINT64  Ccn504Desc.BaseAddress
        AMD_ACPI_ARM_CCN504_CACHE_SIZE,                       // UINT64  Ccn504Desc.CacheSize
      },
    },
  },
};

#pragma pack(pop)


EFI_ACPI_DESCRIPTION_HEADER *
CsrtHeader (
  VOID
  )
{
  return &AcpiCsrt.Header;
}
