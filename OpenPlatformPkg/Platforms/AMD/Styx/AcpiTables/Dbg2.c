/** @file

  Microsoft Debug Port Table 2 (DBG2)
  © 2012 Microsoft. All rights reserved.<BR>
  http://go.microsoft.com/fwlink/p/?linkid=403551

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <IndustryStandard/DebugPort2Table.h>

#pragma pack(push, 1)

#define EFI_ACPI_DBG2_REVISION                     0
#define DBG2_NUM_DEBUG_PORTS                       1
#define DBG2_NUMBER_OF_GENERIC_ADDRESS_REGISTERS   1
#define DBG2_NAMESPACESTRING_FIELD_SIZE            8
#define DBG2_OEM_DATA_FIELD_SIZE                   0
#define DBG2_OEM_DATA_FIELD_OFFSET                 0

#define DBG2_DEBUG_PORT_SUBTYPE_PL011              0x0003        // Sub type for Pl011
#define DBG2_DEBUG_PORT_SUBTYPE_UEFI               0x0007        // Sub type for UEFI Debug Port
#define PL011_UART_LENGTH                          0x1000

#define NAME_STR_UART1     {'C', 'O', 'M', '1', '\0', '\0', '\0', '\0'}
#define NAME_STR_UEFI      {'U', 'E', 'F', 'I', '\0', '\0', '\0', '\0'}


typedef struct {
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT     Dbg2Device;
  EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE            BaseAddressRegister;
  UINT32                                            AddressSize;
  UINT8                                             NameSpaceString[DBG2_NAMESPACESTRING_FIELD_SIZE];
} DBG2_DEBUG_DEVICE_INFORMATION;

typedef struct {
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE           Description;
  DBG2_DEBUG_DEVICE_INFORMATION                     Dbg2DeviceInfo[DBG2_NUM_DEBUG_PORTS];
} DBG2_TABLE;


#define DBG2_DEBUG_PORT_DDI(NumReg, SubType, UartBase, UartAddrLen, UartNameStr) {                                           \
    {                                                                                                                        \
      EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT_REVISION,              /* UINT8     Revision; */                         \
      sizeof (DBG2_DEBUG_DEVICE_INFORMATION),                              /* UINT16    Length; */                           \
      NumReg,                                                              /* UINT8     NumberofGenericAddressRegisters; */  \
      DBG2_NAMESPACESTRING_FIELD_SIZE,                                     /* UINT16    NameSpaceStringLength; */            \
      OFFSET_OF(DBG2_DEBUG_DEVICE_INFORMATION, NameSpaceString),           /* UINT16    NameSpaceStringOffset; */            \
      DBG2_OEM_DATA_FIELD_SIZE,                                            /* UINT16    OemDataLength; */                    \
      DBG2_OEM_DATA_FIELD_OFFSET,                                          /* UINT16    OemDataOffset; */                    \
      EFI_ACPI_DBG2_PORT_TYPE_SERIAL,                                      /* UINT16    Port Type; */                        \
      SubType,                                                             /* UINT16    Port Subtype; */                     \
      {EFI_ACPI_RESERVED_BYTE,  EFI_ACPI_RESERVED_BYTE},                   /* UINT8     Reserved[2]; */                      \
      OFFSET_OF(DBG2_DEBUG_DEVICE_INFORMATION, BaseAddressRegister),       /* UINT16    BaseAddressRegister Offset; */       \
      OFFSET_OF(DBG2_DEBUG_DEVICE_INFORMATION, AddressSize)                /* UINT16    AddressSize Offset; */               \
    },                                                                                                                       \
    AMD_GASN (UartBase),                              /* EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE BaseAddressRegister */       \
    UartAddrLen,                                      /* UINT32  AddressSize */                                              \
    UartNameStr                                       /* UINT8   NameSpaceString[MAX_DBG2_NAME_LEN] */                       \
  }


STATIC DBG2_TABLE AcpiDbg2 = {
  {
    AMD_ACPI_HEADER (EFI_ACPI_5_0_DEBUG_PORT_2_TABLE_SIGNATURE,
                     DBG2_TABLE,
                     EFI_ACPI_DBG2_REVISION),
    OFFSET_OF(DBG2_TABLE, Dbg2DeviceInfo),
    DBG2_NUM_DEBUG_PORTS                                                    // UINT32  NumberDbgDeviceInfo
  },
  {
    /*
     * Kernel Debug Port
     */
#if (DBG2_NUM_DEBUG_PORTS > 0)
    DBG2_DEBUG_PORT_DDI(DBG2_NUMBER_OF_GENERIC_ADDRESS_REGISTERS,
                    DBG2_DEBUG_PORT_SUBTYPE_PL011,
                    FixedPcdGet64(PcdSerialDbgRegisterBase),
                    PL011_UART_LENGTH,
                    NAME_STR_UART1),
#endif
    /*
     * UEFI Debug Port
    */
#if (DBG2_NUM_DEBUG_PORTS > 1)
    DBG2_DEBUG_PORT_DDI(0,
                    DBG2_DEBUG_PORT_SUBTYPE_UEFI,
                    0,
                    0,
                    NAME_STR_UEFI),
#endif
  }
};

#pragma pack(pop)

EFI_ACPI_DESCRIPTION_HEADER *
Dbg2Header (
  VOID
  )
{
  return &AcpiDbg2.Description.Header;
}

