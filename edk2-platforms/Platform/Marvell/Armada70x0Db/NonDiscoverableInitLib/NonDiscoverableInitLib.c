/**
*
*  Copyright (c) 2017, Linaro Ltd. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/Gpio.h>
#include <Protocol/NonDiscoverableDevice.h>

#define ARMADA_70x0_DB_IO_EXPANDER0       0
#define ARMADA_70x0_DB_VBUS0_PIN          0
#define ARMADA_70x0_DB_VBUS0_LIMIT_PIN    4
#define ARMADA_70x0_DB_VBUS1_PIN          1
#define ARMADA_70x0_DB_VBUS1_LIMIT_PIN    5

STATIC CONST GPIO_PIN_DESC mArmada70x0DbVbusEn[] = {
  {
    ARMADA_70x0_DB_IO_EXPANDER0,
    ARMADA_70x0_DB_VBUS0_PIN,
    TRUE,
  },
  {
    ARMADA_70x0_DB_IO_EXPANDER0,
    ARMADA_70x0_DB_VBUS0_LIMIT_PIN,
    TRUE,
  },
  {
    ARMADA_70x0_DB_IO_EXPANDER0,
    ARMADA_70x0_DB_VBUS1_PIN,
    TRUE,
  },
  {
    ARMADA_70x0_DB_IO_EXPANDER0,
    ARMADA_70x0_DB_VBUS1_LIMIT_PIN,
    TRUE,
  },
};

STATIC
EFI_STATUS
EFIAPI
Armada70x0DbInitXhciVbus (
  IN  NON_DISCOVERABLE_DEVICE       *This
  )
{
  CONST GPIO_PIN_DESC     *VbusEnPinDesc;
  EFI_STATUS              Status = EFI_SUCCESS;
  MARVELL_GPIO_PROTOCOL   *GpioProtocol;
  EFI_HANDLE              *ProtHandle = NULL;
  UINTN                    Index;

  Status = MarvellGpioGetHandle (GPIO_DRIVER_TYPE_IO_EXPANDER, &ProtHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to find GPIO for IO-Expander protocol, Status: 0x%x\n", Status));
    return Status;
  }

  Status = gBS->OpenProtocol (ProtHandle,
                  &gMarvellGpioProtocolGuid,
                  (void **)&GpioProtocol,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if(EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to open GPIO for IO-Expander protocol, Status: 0x%x\n", Status));
    return Status;
  }

  VbusEnPinDesc = mArmada70x0DbVbusEn;
  for (Index = 0; Index < ARRAY_SIZE (mArmada70x0DbVbusEn); Index++) {
    GpioProtocol->DirectionOutput (GpioProtocol,
                    VbusEnPinDesc->ControllerId,
                    VbusEnPinDesc->PinNumber,
                    VbusEnPinDesc->ActiveHigh);
    VbusEnPinDesc++;
  }

  Status = gBS->CloseProtocol (ProtHandle,
                  &gMarvellGpioProtocolGuid,
                  gImageHandle,
                  NULL);

  return Status;
}

NON_DISCOVERABLE_DEVICE_INIT
EFIAPI
GetInitializerForType (
  IN  NON_DISCOVERABLE_DEVICE_TYPE  Type,
  IN  UINTN                         Index
  )
{
  if (Type == NonDiscoverableDeviceTypeXhci) {
        return Armada70x0DbInitXhciVbus;
  }

  return NULL;
}
