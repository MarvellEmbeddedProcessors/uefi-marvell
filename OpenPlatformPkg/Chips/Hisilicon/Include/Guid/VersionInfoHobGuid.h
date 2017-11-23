/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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

#ifndef _VERSION_INFO_HOB_GUID_H_
#define _VERSION_INFO_HOB_GUID_H_

// {0E13A14C-859C-4f22-82BD-180EE14212BF}
#define VERSION_INFO_HOB_GUID \
  {0xe13a14c, 0x859c, 0x4f22, {0x82, 0xbd, 0x18, 0xe, 0xe1, 0x42, 0x12, 0xbf}}

extern GUID gVersionInfoHobGuid;

#pragma pack(1)

typedef struct {
  EFI_TIME BuildTime;
  CHAR16   String[1];
} VERSION_INFO;

#pragma pack()

#endif

