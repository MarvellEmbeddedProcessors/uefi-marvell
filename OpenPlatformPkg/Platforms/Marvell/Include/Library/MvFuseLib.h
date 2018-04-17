/********************************************************************************
Copyright (C) 2018 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must Retain the above copyright notice,
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

#ifndef __MV_FUSE_LIB_H__
#define __MV_FUSE_LIB_H__

#include <Protocol/BoardDesc.h>

CHAR16*
EFIAPI
ArmadaFuseGetName (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetRowLength (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetOPMode (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetBankType (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetRowCount (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetRowStep (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetRegBase (
  IN UINT32 FuseNum
  );

UINTN
EFIAPI
ArmadaFuseGetMemBase (
  IN UINT32 FuseNum
  );

EFI_STATUS
EFIAPI
ArmadaFuseProg (
  IN UINT32            GlobalLineId,
  IN UINT32            Position,
  IN UINT32            Value
  );

EFI_STATUS
EFIAPI
ArmadaFuseRead (
  IN UINT32            GlobalLineId,
  IN UINT32            Position,
  IN OUT UINT32       *Value
  );

EFI_STATUS
EFIAPI
ArmadaFuseList (VOID);

#endif /* __MV_FUSE_LIB_H__ */
