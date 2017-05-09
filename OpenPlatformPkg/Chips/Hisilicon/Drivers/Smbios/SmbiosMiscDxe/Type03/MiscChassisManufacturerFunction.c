/** @file

Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
Copyright (c) 2015, Hisilicon Limited. All rights reserved.<BR>
Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscChassisManufacturerFunction.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to smbios.

Based on files under Nt32Pkg/MiscSubClassPlatformDxe/
**/
/* Modify list
DATA        AUTHOR            REASON
*/

#include "SmbiosMisc.h"

UINT8
GetChassisType (
)
{
    EFI_STATUS                      Status;
    UINT8                           ChassisType;
    Status = IpmiGetChassisType(&ChassisType);
    if (EFI_ERROR(Status))
    {
       return 0;
    }

    return ChassisType;
}

/**
  This function makes boot time changes to the contents of the
  MiscChassisManufacturer (Type 3).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscChassisManufacturer)
{
    CHAR8                           *OptionalStrStart;
    UINTN                           ManuStrLen;
    UINTN                           VerStrLen;
    UINTN                           AssertTagStrLen;
    UINTN                           SerialNumStrLen;
    UINTN                           ChaNumStrLen;
    EFI_STRING                      Manufacturer;
    EFI_STRING                      Version;
    EFI_STRING                      SerialNumber;
    EFI_STRING                      AssertTag;
    EFI_STRING                      ChassisSkuNumber;
    STRING_REF                      TokenToGet;
    EFI_SMBIOS_HANDLE               SmbiosHandle;
    SMBIOS_TABLE_TYPE3              *SmbiosRecord;
    SMBIOS_TABLE_TYPE3              *InputData;
    EFI_STATUS                      Status;

    UINT8                           ContainedElementCount;
    CONTAINED_ELEMENT               ContainedElements = {0};
    UINT8                           ExtendLength = 0;

    UINT8                           ChassisType;

    //
    // First check for invalid parameters.
    //
    if (RecordData == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    InputData = (SMBIOS_TABLE_TYPE3 *)RecordData;

    UpdateSmbiosInfo(mHiiHandle, STRING_TOKEN (STR_MISC_CHASSIS_ASSET_TAG), AssetTagType03);
    UpdateSmbiosInfo(mHiiHandle, STRING_TOKEN (STR_MISC_CHASSIS_SERIAL_NUMBER), SrNumType03);
    UpdateSmbiosInfo(mHiiHandle, STRING_TOKEN (STR_MISC_CHASSIS_VERSION), VersionType03);
    UpdateSmbiosInfo(mHiiHandle, STRING_TOKEN (STR_MISC_CHASSIS_MANUFACTURER), ManufacturerType03);

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_MANUFACTURER);
    Manufacturer = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ManuStrLen = StrLen(Manufacturer);

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_VERSION);
    Version = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VerStrLen = StrLen(Version);

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_SERIAL_NUMBER);
    SerialNumber = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    SerialNumStrLen = StrLen(SerialNumber);

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_ASSET_TAG);
    AssertTag = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    AssertTagStrLen = StrLen(AssertTag);

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_SKU_NUMBER);
    ChassisSkuNumber = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ChaNumStrLen = StrLen(ChassisSkuNumber);

    ContainedElementCount = InputData->ContainedElementCount;

    if (ContainedElementCount > 1)
    {
        ExtendLength = (ContainedElementCount - 1) * sizeof (CONTAINED_ELEMENT);
    }

    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE3) + ExtendLength    + 1
                                                                + ManuStrLen      + 1
                                                                + VerStrLen       + 1
                                                                + SerialNumStrLen + 1
                                                                + AssertTagStrLen + 1
                                                                + ChaNumStrLen    + 1 + 1);
    if (NULL == SmbiosRecord)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
    }

    (VOID)CopyMem(SmbiosRecord, InputData, sizeof (SMBIOS_TABLE_TYPE3));

    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE3) + ExtendLength + 1;

    ChassisType = GetChassisType ();
    if (ChassisType != 0)
    {
        SmbiosRecord->Type  = ChassisType;
    }

    //ContainedElements
    (VOID)CopyMem(SmbiosRecord + 1, &ContainedElements, ExtendLength);

    //ChassisSkuNumber
    *((UINT8 *)SmbiosRecord + sizeof (SMBIOS_TABLE_TYPE3) + ExtendLength) = 5;

    OptionalStrStart = (CHAR8 *)((UINT8 *)SmbiosRecord + sizeof (SMBIOS_TABLE_TYPE3) + ExtendLength + 1);
    UnicodeStrToAsciiStr(Manufacturer,     OptionalStrStart);
    UnicodeStrToAsciiStr(Version,          OptionalStrStart + ManuStrLen + 1);
    UnicodeStrToAsciiStr(SerialNumber,     OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1);
    UnicodeStrToAsciiStr(AssertTag,        OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1);
    UnicodeStrToAsciiStr(ChassisSkuNumber, OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1 + SerialNumStrLen +1 + AssertTagStrLen + 1);
    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = LogSmbiosData( (UINT8*)SmbiosRecord, &SmbiosHandle);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type03 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));
    }

    FreePool(SmbiosRecord);

Exit:
    if(Manufacturer != NULL)
    {
        FreePool(Manufacturer);
    }

    if(Version != NULL)
    {
        FreePool(Version);
    }

    if(SerialNumber != NULL)
    {
        FreePool(SerialNumber);
    }

    if(AssertTag != NULL)
    {
        FreePool(AssertTag);
    }

    if(ChassisSkuNumber != NULL)
    {
        FreePool(ChassisSkuNumber);
    }

    return Status;
}
