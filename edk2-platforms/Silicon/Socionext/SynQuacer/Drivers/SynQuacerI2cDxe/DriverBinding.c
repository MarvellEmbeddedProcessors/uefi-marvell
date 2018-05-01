/** @file

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD
  License which accompanies this distribution. The full text of the license may
  be found at  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SynQuacerI2cDxe.h"

/**
  Tests to see if this driver supports a given controller.

  @param  This[in]                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL
                                   instance.
  @param  ControllerHandle[in]     The handle of the controller to test.
  @param  RemainingDevicePath[in]  The remaining device path.
                                   (Ignored - this is not a bus driver.)

  @retval EFI_SUCCESS              The driver supports this controller.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle is
                                   already being managed by the driver specified
                                   by This.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle is
                                   not supported by the driver specified by This.

**/
EFI_STATUS
EFIAPI
SynQuacerI2cDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  NON_DISCOVERABLE_DEVICE    *Dev;
  EFI_STATUS                 Status;

  //
  //  Connect to the non-discoverable device
  //
  Status = gBS->OpenProtocol (ControllerHandle,
                              &gEdkiiNonDiscoverableDeviceProtocolGuid,
                              (VOID **) &Dev,
                              This->DriverBindingHandle,
                              ControllerHandle,
                              EFI_OPEN_PROTOCOL_BY_DRIVER);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CompareGuid (Dev->Type, &gSynQuacerNonDiscoverableI2cMasterGuid) ||
      CompareGuid (Dev->Type, &gSynQuacerNonDiscoverableRuntimeI2cMasterGuid)) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  }

  //
  // Clean up.
  //
  gBS->CloseProtocol (ControllerHandle,
                      &gEdkiiNonDiscoverableDeviceProtocolGuid,
                      This->DriverBindingHandle,
                      ControllerHandle);

  return Status;
}


/**
  Starts a device controller or a bus controller.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL
                                   instance.
  @param[in]  ControllerHandle     The handle of the device to start. This
                                   handle must support a protocol interface that
                                   supplies an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  The remaining portion of the device path.
                                   (Ignored - this is not a bus driver.)

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a
                                   device error.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a
                                   lack of resources.

**/
EFI_STATUS
EFIAPI
SynQuacerI2cDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  return SynQuacerI2cInit (This->DriverBindingHandle, ControllerHandle);
}


/**
  Stops a device controller or a bus controller.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL
                                instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle
                                must support a bus specific I/O protocol for the
                                driver to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in
                                ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be
                                NULL if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device
                                error.

**/
EFI_STATUS
EFIAPI
SynQuacerI2cDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  UINTN                       NumberOfChildren,
  IN  EFI_HANDLE                  *ChildHandleBuffer OPTIONAL
  )
{
  return SynQuacerI2cRelease (This->DriverBindingHandle, ControllerHandle);
}


STATIC EFI_DRIVER_BINDING_PROTOCOL  gSynQuacerI2cDriverBinding = {
  SynQuacerI2cDriverBindingSupported,
  SynQuacerI2cDriverBindingStart,
  SynQuacerI2cDriverBindingStop,
  0xa,
  NULL,
  NULL
};


/**
  The entry point of I2c UEFI Driver.

  @param  ImageHandle                The image handle of the UEFI Driver.
  @param  SystemTable                A pointer to the EFI System Table.

  @retval  EFI_SUCCESS               The Driver or UEFI Driver exited normally.
  @retval  EFI_INCOMPATIBLE_VERSION  _gUefiDriverRevision is greater than
                                     SystemTable->Hdr.Revision.

**/
EFI_STATUS
EFIAPI
SynQuacerI2cDxeEntryPoint (
  IN  EFI_HANDLE          ImageHandle,
  IN  EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS    Status;

  //
  //  Add the driver to the list of drivers
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle, SystemTable, &gSynQuacerI2cDriverBinding, ImageHandle,
             NULL, &gSynQuacerI2cDriverComponentName2);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "*** Installed SynQuacer I2C UEFI driver! ***\n"));

  return EFI_SUCCESS;
}


/**
  Unload function for the I2c UEFI Driver.

  @param  ImageHandle[in]        The allocated handle for the EFI image

  @retval EFI_SUCCESS            The driver was unloaded successfully
  @retval EFI_INVALID_PARAMETER  ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
SynQuacerI2cDxeUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  UINTN       HandleCount;
  UINTN       Index;

  //
  // Retrieve all USB I/O handles in the handle database
  //
  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEdkiiNonDiscoverableDeviceProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Disconnect the driver from the handles in the handle database
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (HandleBuffer[Index],
                                        gImageHandle,
                                        NULL);
  }

  //
  // Free the handle array
  //
  gBS->FreePool (HandleBuffer);

  //
  // Uninstall protocols installed by the driver in its entrypoint
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (ImageHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gSynQuacerI2cDriverBinding,
                  NULL
                  );

  return EFI_SUCCESS;
}
