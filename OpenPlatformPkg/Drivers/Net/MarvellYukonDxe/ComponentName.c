/**
*
*  Copyright (c) 2011-2016, ARM Limited. All rights reserved.
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

#include "MarvellYukon.h"

//
// EFI Component Name Functions
//
/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

   <at> param  This[in]               A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                      EFI_COMPONENT_NAME_PROTOCOL instance.

   <at> param  Language[in]           A pointer to a Null-terminated ASCII string
                                      array indicating the language. This is the
                                      language of the driver name that the caller is
                                      requesting, and it must match one of the
                                      languages specified in SupportedLanguages. The
                                      number of languages supported by a driver is up
                                      to the driver writer. Language is specified
                                      in RFC 4646 or ISO 639-2 language code format.

   <at> param  DriverName[out]        A pointer to the Unicode string to return.
                                      This Unicode string is the name of the
                                      driver specified by This in the language
                                      specified by Language.

   <at> retval EFI_SUCCESS            The Unicode string for the Driver specified by
                                      This and the language specified by Language was
                                      returned in DriverName.

   <at> retval EFI_INVALID_PARAMETER  Language is NULL.

   <at> retval EFI_INVALID_PARAMETER  DriverName is NULL.

   <at> retval EFI_UNSUPPORTED        The driver specified by This does not support
                                      the language specified by Language.

**/
EFI_STATUS
EFIAPI
SimpleNetworkComponentNameGetDriverName (
    IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
    );


/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

   <at> param  This[in]               A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                      EFI_COMPONENT_NAME_PROTOCOL instance.

   <at> param  ControllerHandle[in]   The handle of a controller that the driver
                                      specified by This is managing.  This handle
                                      specifies the controller whose name is to be
                                      returned.

   <at> param  ChildHandle[in]        The handle of the child controller to retrieve
                                      the name of.  This is an optional parameter that
                                      may be NULL.  It will be NULL for device
                                      drivers.  It will also be NULL for a bus drivers
                                      that wish to retrieve the name of the bus
                                      controller.  It will not be NULL for a bus
                                      driver that wishes to retrieve the name of a
                                      child controller.

   <at> param  Language[in]           A pointer to a Null-terminated ASCII string
                                      array indicating the language.  This is the
                                      language of the driver name that the caller is
                                      requesting, and it must match one of the
                                      languages specified in SupportedLanguages. The
                                      number of languages supported by a driver is up
                                      to the driver writer. Language is specified in
                                      RFC 4646 or ISO 639-2 language code format.

   <at> param  ControllerName[out]    A pointer to the Unicode string to return.
                                      This Unicode string is the name of the
                                      controller specified by ControllerHandle and
                                      ChildHandle in the language specified by
                                      Language from the point of view of the driver
                                      specified by This.

   <at> retval EFI_SUCCESS            The Unicode string for the user readable name in
                                      the language specified by Language for the
                                      driver specified by This was returned in
                                      DriverName.

   <at> retval EFI_INVALID_PARAMETER  ControllerHandle is not a valid EFI_HANDLE.

   <at> retval EFI_INVALID_PARAMETER  ChildHandle is not NULL and it is not a valid
                                      EFI_HANDLE.

   <at> retval EFI_INVALID_PARAMETER  Language is NULL.

   <at> retval EFI_INVALID_PARAMETER  ControllerName is NULL.

   <at> retval EFI_UNSUPPORTED        The driver specified by This is not currently
                                      managing the controller specified by
                                      ControllerHandle and ChildHandle.

   <at> retval EFI_UNSUPPORTED        The driver specified by This does not support
                                      the language specified by Language.

**/
EFI_STATUS
EFIAPI
SimpleNetworkComponentNameGetControllerName (
    IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
    IN  EFI_HANDLE                                      ControllerHandle,
    IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
    IN  CHAR8                                           *Language,
    OUT CHAR16                                          **ControllerName
    );


//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gSimpleNetworkComponentName = {
  SimpleNetworkComponentNameGetDriverName,
  SimpleNetworkComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gSimpleNetworkComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) SimpleNetworkComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) SimpleNetworkComponentNameGetControllerName,
  "en"
};


GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mSimpleNetworkDriverNameTable[] = {
  {
    "eng;en",
    L"Marvell Yukon Simple Network Protocol Driver"
  },
  {
    NULL,
    NULL
  }
};

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

   <at> param  This[in]               A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                      EFI_COMPONENT_NAME_PROTOCOL instance.

   <at> param  Language[in]           A pointer to a Null-terminated ASCII string
                                      array indicating the language. This is the
                                      language of the driver name that the caller is
                                      requesting, and it must match one of the
                                      languages specified in SupportedLanguages. The
                                      number of languages supported by a driver is up
                                      to the driver writer. Language is specified
                                      in RFC 4646 or ISO 639-2 language code format.

   <at> param  DriverName[out]        A pointer to the Unicode string to return.
                                      This Unicode string is the name of the
                                      driver specified by This in the language
                                      specified by Language.

   <at> retval EFI_SUCCESS            The Unicode string for the Driver specified by
                                      This and the language specified by Language was
                                      returned in DriverName.

   <at> retval EFI_INVALID_PARAMETER  Language is NULL.

   <at> retval EFI_INVALID_PARAMETER  DriverName is NULL.

   <at> retval EFI_UNSUPPORTED        The driver specified by This does not support
                                      the language specified by Language.

**/
EFI_STATUS
EFIAPI
SimpleNetworkComponentNameGetDriverName (
    IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
    )
{
  return LookupUnicodeString2 (
        Language,
        This->SupportedLanguages,
        mSimpleNetworkDriverNameTable,
        DriverName,
        (BOOLEAN)(This == &gSimpleNetworkComponentName)
        );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.
  Currently not implemented.

   <at> param  This[in]               A pointer to the EFI_COMPONENT_NAME2_PROTOCOL
                                      or EFI_COMPONENT_NAME_PROTOCOL instance.

   <at> param  ControllerHandle[in]   The handle of a controller that the driver
                                      specified by This is managing.  This handle
                                      specifies the controller whose name is to be
                                      returned.

   <at> param  ChildHandle[in]        The handle of the child controller to retrieve
                                      the name of.  This is an optional parameter that
                                      may be NULL.  It will be NULL for device
                                      drivers.  It will also be NULL for a bus drivers
                                      that wish to retrieve the name of the bus
                                      controller.  It will not be NULL for a bus
                                      driver that wishes to retrieve the name of a
                                      child controller.

   <at> param  Language[in]           A pointer to a Null-terminated ASCII string
                                      array indicating the language.  This is the
                                      language of the driver name that the caller is
                                      requesting, and it must match one of the
                                      languages specified in SupportedLanguages. The
                                      number of languages supported by a driver is up
                                      to the driver writer. Language is specified in
                                      RFC 4646 or ISO 639-2 language code format.

   <at> param  ControllerName[out]    A pointer to the Unicode string to return.
                                      This Unicode string is the name of the
                                      controller specified by ControllerHandle and
                                      ChildHandle in the language specified by
                                      Language from the point of view of the driver
                                      specified by This.

   <at> retval EFI_SUCCESS            The Unicode string for the user readable name in
                                      the language specified by Language for the
                                      driver specified by This was returned in
                                      DriverName.

   <at> retval EFI_INVALID_PARAMETER  ControllerHandle is not a valid EFI_HANDLE.

   <at> retval EFI_INVALID_PARAMETER  ChildHandle is not NULL and it is not a valid
                                      EFI_HANDLE.

   <at> retval EFI_INVALID_PARAMETER  Language is NULL.

   <at> retval EFI_INVALID_PARAMETER  ControllerName is NULL.

   <at> retval EFI_UNSUPPORTED        The driver specified by This is not currently
                                      managing the controller specified by
                                      ControllerHandle and ChildHandle.

   <at> retval EFI_UNSUPPORTED        The driver specified by This does not support
                                      the language specified by Language.

**/
EFI_STATUS
EFIAPI
SimpleNetworkComponentNameGetControllerName (
    IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
    IN  EFI_HANDLE                                      ControllerHandle,
    IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
    IN  CHAR8                                           *Language,
    OUT CHAR16                                          **ControllerName
    )
{
  return EFI_UNSUPPORTED;
}
