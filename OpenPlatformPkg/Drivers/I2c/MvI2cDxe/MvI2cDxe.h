/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

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

#ifndef __MV_I2C_H__
#define __MV_I2C_H__

#include <Uefi.h>

#define I2C_BASE_ADDRESS    0xf0511000

#define I2C_SLAVE_ADDR    0x00
#define I2C_EXT_SLAVE_ADDR  0x10
#define I2C_DATA    0x04

#define I2C_CONTROL    0x08
#define I2C_CONTROL_ACK  (1 << 2)
#define I2C_CONTROL_IFLG  (1 << 3)
#define I2C_CONTROL_STOP  (1 << 4)
#define I2C_CONTROL_START  (1 << 5)
#define I2C_CONTROL_I2CEN  (1 << 6)
#define I2C_CONTROL_INTEN  (1 << 7)

#define I2C_STATUS      0x0c
#define I2C_STATUS_START    0x08
#define I2C_STATUS_RPTD_START    0x10
#define I2C_STATUS_ADDR_W_ACK    0x18
#define I2C_STATUS_DATA_WR_ACK    0x28
#define I2C_STATUS_ADDR_R_ACK    0x40
#define I2C_STATUS_DATA_RD_ACK    0x50
#define I2C_STATUS_DATA_RD_NOACK  0x58

#define I2C_BAUD_RATE    0x0c
#define I2C_BAUD_RATE_PARAM(M,N)  ((((M) << 3) | ((N) & 0x7)) & 0x7f)
#define I2C_BAUD_RATE_RAW(C,M,N)  ((C)/((10*(M+1))<<(N+1)))
#define I2C_M_FROM_BAUD(baud)     (((baud) >> 3) & 0xf)
#define I2C_N_FROM_BAUD(baud)     ((baud) & 0x7)

#define I2C_SOFT_RESET    0x1c
#define I2C_TRANSFER_TIMEOUT 10000
#define I2C_OPERATION_TIMEOUT 100

#define I2C_UNKNOWN        0x0
#define I2C_SLOW           0x1
#define I2C_FAST           0x2
#define I2C_FASTEST        0x3

/*
 * I2C_FLAG_NORESTART is not part of PI spec, it allows to continue
 * transmission without repeated start operation.
 * FIXME: This flag is also defined in
 * Platforms/Marvell/Include/Protocol/Eeprom.h and it's important to have both
 * version synced. This solution is temporary and shared flag should be used by
 * both files.
 * Situation is analogous with I2C_GUID, which also should be common, but is
 * for now defined same way in two header files.
 */
#define I2C_FLAG_NORESTART 0x00000002
#define I2C_GUID \
  { \
  0xadc1901b, 0xb83c, 0x4831, { 0x8f, 0x59, 0x70, 0x89, 0x8f, 0x26, 0x57, 0x1e } \
  }

#define I2C_MASTER_SIGNATURE          SIGNATURE_32 ('I', '2', 'C', 'M')

typedef struct {
  UINT32      Signature;
  EFI_HANDLE  Controller;
  EFI_LOCK    Lock;
  UINTN       TclkFrequency;
  UINTN       BaseAddress;
  INTN        Bus;
  EFI_I2C_MASTER_PROTOCOL I2cMaster;
  EFI_I2C_ENUMERATE_PROTOCOL I2cEnumerate;
  EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL I2cBusConf;
} I2C_MASTER_CONTEXT;

#define I2C_SC_FROM_MASTER(a) CR (a, I2C_MASTER_CONTEXT, I2cMaster, I2C_MASTER_SIGNATURE)
#define I2C_SC_FROM_ENUMERATE(a) CR (a, I2C_MASTER_CONTEXT, I2cEnumerate, I2C_MASTER_SIGNATURE)
#define I2C_SC_FROM_BUSCONF(a) CR (a, I2C_MASTER_CONTEXT, I2cBusConf, I2C_MASTER_SIGNATURE)

typedef struct {
  UINT32  raw;
  UINTN    param;
  UINTN    m;
  UINTN    n;
} MV_I2C_BAUD_RATE;

typedef struct {
  VENDOR_DEVICE_PATH            Guid;
  EFI_DEVICE_PATH_PROTOCOL      End;
} MV_I2C_DEVICE_PATH;

STATIC
UINT32
I2C_READ(
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINTN off
  );

STATIC
EFI_STATUS
I2C_WRITE (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINTN off,
  IN UINT32 val
  );

EFI_STATUS
EFIAPI
MvI2cInitialise (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

STATIC
VOID
MvI2cControlClear (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINT32 mask
  );

STATIC
VOID
MvI2cControlSet (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINT32 mask
  );

STATIC
VOID
MvI2cClearIflg (
 IN I2C_MASTER_CONTEXT *I2cMasterContext
 );
STATIC
UINTN
MvI2cPollCtrl (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINTN timeout,
  IN UINT32 mask
  );

STATIC
EFI_STATUS
MvI2cLockedStart (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN INT32 mask,
  IN UINT8 slave,
  IN UINTN timeout
  );

STATIC
VOID
MvI2cCalBaudRate (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN CONST UINT32 target,
  IN OUT MV_I2C_BAUD_RATE *rate,
  UINT32 clk
  );

EFI_STATUS
EFIAPI
MvI2cReset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  );

STATIC
EFI_STATUS
MvI2cRepeatedStart (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINT8 slave,
  IN UINTN timeout
  );

STATIC
EFI_STATUS
MvI2cStart (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN UINT8 slave,
  IN UINTN timeout
  );

STATIC
EFI_STATUS
MvI2cStop (
  IN I2C_MASTER_CONTEXT *I2cMasterContext
  );

STATIC
EFI_STATUS
MvI2cRead (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN OUT UINT8 *buf,
  IN UINTN len,
  IN OUT UINTN *read,
  IN UINTN last,
  IN UINTN delay
  );

STATIC
EFI_STATUS
MvI2cWrite (
  IN I2C_MASTER_CONTEXT *I2cMasterContext,
  IN OUT CONST UINT8 *buf,
  IN UINTN len,
  IN OUT UINTN *sent,
  IN UINTN timeout
  );

STATIC
EFI_STATUS
EFIAPI
MvI2cStartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
MvI2cEnumerate (
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL *This,
  IN OUT CONST EFI_I2C_DEVICE         **Device
  );

STATIC
EFI_STATUS
EFIAPI
MvI2cEnableConf (
  IN CONST EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL *This,
  IN UINTN                                               I2cBusConfiguration,
  IN EFI_EVENT                                           Event      OPTIONAL,
  IN EFI_STATUS                                          *I2cStatus OPTIONAL
  );

#endif // __MV_I2C_H__
