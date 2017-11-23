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

#ifndef _SNP_H_
#define _SNP_H_


#include <Uefi.h>

#include <Protocol/SimpleNetwork.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>

#include <Guid/EventGroup.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/NetLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#include <IndustryStandard/Pci.h>

#define YUKON_DRIVER_SIGNATURE  SIGNATURE_32 ('m', 'y', 'u', 'k')

typedef struct {
  UINT32                      Signature;
  EFI_LOCK                    Lock;

  EFI_HANDLE                  Controller;
  UINTN                       Port;
  EFI_EVENT                   ExitBootEvent;

  EFI_SIMPLE_NETWORK_PROTOCOL Snp;
  EFI_SIMPLE_NETWORK_MODE     SnpMode;

  EFI_HANDLE                  DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL*   DevicePath;
  EFI_PCI_IO_PROTOCOL*        PciIo;

} YUKON_DRIVER;

#define YUKON_DEV_FROM_THIS_SNP(a) CR (a, YUKON_DRIVER, Snp, YUKON_DRIVER_SIGNATURE)

#define SNP_MEM_PAGES(x)  (((x) - 1) / 4096 + 1)

typedef struct {
  UINT32                      Signature;
  LIST_ENTRY                  Link;
  EFI_HANDLE                  Controller;
  struct msk_softc            *Data;
} MSK_LINKED_DRV_BUF;

#define MSK_DRV_SIGNATURE  SIGNATURE_32 ('m', 's', 'k', 'c')

#define MSK_DRV_INFO_FROM_THIS(a) \
  CR (a, \
      MSK_LINKED_DRV_BUF, \
      Link, \
      MSK_DRV_SIGNATURE \
      );

//
// Global Variables
//
extern EFI_COMPONENT_NAME_PROTOCOL    gSimpleNetworkComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL   gSimpleNetworkComponentName2;

//
// The SNP driver control functions
//

EFI_STATUS
InitializeSNPProtocol (
    IN OUT  YUKON_DRIVER *YukonDriver
    );

/**
  Changes the state of a network interface from "stopped" to "started".

  This function starts a network interface. If the network interface successfully
  starts, then EFI_SUCCESS will be returned.

   <at> param  This                   A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.

   <at> retval EFI_SUCCESS            The network interface was started.
   <at> retval EFI_ALREADY_STARTED    The network interface is already in the started state.
   <at> retval EFI_INVALID_PARAMETER  This parameter was NULL or did not point to a valid
                                      EFI_SIMPLE_NETWORK_PROTOCOL structure.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network interface.

**/
EFI_STATUS
EFIAPI
SnpStart (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This
    );

/**
  Changes the state of a network interface from "started" to "stopped".

  This function stops a network interface. This call is only valid if the network
  interface is in the started state. If the network interface was successfully
  stopped, then EFI_SUCCESS will be returned.

   <at> param  This                   A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.


   <at> retval EFI_SUCCESS            The network interface was stopped.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  This parameter was NULL or did not point to a valid
                                      EFI_SIMPLE_NETWORK_PROTOCOL structure.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network interface.

**/
EFI_STATUS
EFIAPI
SnpStop (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This
    );

//
// The SNP protocol functions
//


/**
  Resets a network adapter and allocates the transmit and receive buffers
  required by the network interface; optionally, also requests allocation of
  additional transmit and receive buffers.

  This function allocates the transmit and receive buffers required by the network
  interface. If this allocation fails, then EFI_OUT_OF_RESOURCES is returned.
  If the allocation succeeds and the network interface is successfully initialized,
  then EFI_SUCCESS will be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.

   <at> param ExtraRxBufferSize       The size, in bytes, of the extra receive buffer space
                                      that the driver should allocate for the network interface.
                                      Some network interfaces will not be able to use the
                                      extra buffer, and the caller will not know if it is
                                      actually being used.
   <at> param ExtraTxBufferSize       The size, in bytes, of the extra transmit buffer space
                                      that the driver should allocate for the network interface.
                                      Some network interfaces will not be able to use the
                                      extra buffer, and the caller will not know if it is
                                      actually being used.

   <at> retval EFI_SUCCESS            The network interface was initialized.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_OUT_OF_RESOURCES   There was not enough memory for the transmit and
                                      receive buffers.
   <at> retval EFI_INVALID_PARAMETER  This parameter was NULL or did not point to a valid
                                      EFI_SIMPLE_NETWORK_PROTOCOL structure.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.
   <at> retval EFI_UNSUPPORTED        The increased buffer size feature is not supported.

**/
EFI_STATUS
EFIAPI
SnpInitialize (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN UINTN                       ExtraRxBufferSize OPTIONAL,
    IN UINTN                       ExtraTxBufferSize OPTIONAL
    );

/**
  Resets a network adapter and reinitializes it with the parameters that were
  provided in the previous call to Initialize().

  This function resets a network adapter and reinitializes it with the parameters
  that were provided in the previous call to Initialize(). The transmit and
  receive queues are emptied and all pending interrupts are cleared.
  Receive filters, the station address, the statistics, and the multicast-IP-to-HW
  MAC addresses are not reset by this call. If the network interface was
  successfully reset, then EFI_SUCCESS will be returned. If the driver has not
  been initialized, EFI_DEVICE_ERROR will be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param ExtendedVerification    Indicates that the driver may perform a more
                                      exhaustive verification operation of the device
                                      during reset.

   <at> retval EFI_SUCCESS            The network interface was reset.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  One or more of the parameters has an unsupported value.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network interface.

**/
EFI_STATUS
EFIAPI
SnpReset (
    IN EFI_SIMPLE_NETWORK_PROTOCOL  *This,
    IN BOOLEAN                      ExtendedVerification
    );

/**
  Resets a network adapter and leaves it in a state that is safe for another
  driver to initialize.

  This function releases the memory buffers assigned in the Initialize() call.
  Pending transmits and receives are lost, and interrupts are cleared and disabled.
  After this call, only the Initialize() and Stop() calls may be used. If the
  network interface was successfully shutdown, then EFI_SUCCESS will be returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.

   <at> param                         This  A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.

   <at> retval EFI_SUCCESS            The network interface was shutdown.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  This parameter was NULL or did not point to a valid
                                      EFI_SIMPLE_NETWORK_PROTOCOL structure.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.

**/
EFI_STATUS
EFIAPI
SnpShutdown (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This
    );

/**
  Manages the multicast receive filters of a network interface.

  This function is used enable and disable the hardware and software receive
  filters for the underlying network device.
  The receive filter change is broken down into three steps:
  * The filter mask bits that are set (ON) in the Enable parameter are added to
    the current receive filter settings.
  * The filter mask bits that are set (ON) in the Disable parameter are subtracted
    from the updated receive filter settings.
  * If the resulting receive filter setting is not supported by the hardware a
    more liberal setting is selected.
  If the same bits are set in the Enable and Disable parameters, then the bits
  in the Disable parameter takes precedence.
  If the ResetMCastFilter parameter is TRUE, then the multicast address list
  filter is disabled (irregardless of what other multicast bits are set in the
  Enable and Disable parameters). The SNP->SnpMode->MCastFilterCount field is set
  to zero. The Snp->SnpMode->MCastFilter contents are undefined.
  After enabling or disabling receive filter settings, software should verify
  the new settings by checking the Snp->SnpMode->ReceiveFilterSettings,
  Snp->SnpMode->MCastFilterCount and Snp->SnpMode->MCastFilter fields.
  Note: Some network drivers and/or devices will automatically promote receive
    filter settings if the requested setting can not be honored. For example, if
    a request for four multicast addresses is made and the underlying hardware
    only supports two multicast addresses the driver might set the promiscuous
    or promiscuous multicast receive filters instead. The receiving software is
    responsible for discarding any extra packets that get through the hardware
    receive filters.
    Note: Note: To disable all receive filter hardware, the network driver must
      be Shutdown() and Stopped(). Calling ReceiveFilters() with Disable set to
      Snp->SnpMode->ReceiveFilterSettings will make it so no more packets are
      returned by the Receive() function, but the receive hardware may still be
      moving packets into system memory before inspecting and discarding them.
      Unexpected system errors, reboots and hangs can occur if an OS is loaded
      and the network devices are not Shutdown() and Stopped().
  If ResetMCastFilter is TRUE, then the multicast receive filter list on the
  network interface will be reset to the default multicast receive filter list.
  If ResetMCastFilter is FALSE, and this network interface allows the multicast
  receive filter list to be modified, then the MCastFilterCnt and MCastFilter
  are used to update the current multicast receive filter list. The modified
  receive filter list settings can be found in the MCastFilter field of
  EFI_SIMPLE_NETWORK_MODE. If the network interface does not allow the multicast
  receive filter list to be modified, then EFI_INVALID_PARAMETER will be returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.
  If the receive filter mask and multicast receive filter list have been
  successfully updated on the network interface, EFI_SUCCESS will be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param Enable                  A bit mask of receive filters to enable on the network
                                      interface.
   <at> param Disable                 A bit mask of receive filters to disable on the network
                                      interface. For backward compatibility with EFI 1.1
                                      platforms, the EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST bit
                                      must be set when the ResetMCastFilter parameter is TRUE.
   <at> param ResetMCastFilter        Set to TRUE to reset the contents of the multicast
                                      receive filters on the network interface to their
                                      default values.
   <at> param MCastFilterCnt          Number of multicast HW MAC addresses in the new MCastFilter
                                      list. This value must be less than or equal to the
                                      MCastFilterCnt field of EFI_SIMPLE_NETWORK_MODE.
                                      This field is optional if ResetMCastFilter is TRUE.
   <at> param MCastFilter             A pointer to a list of new multicast receive filter HW
                                      MAC addresses. This list will replace any existing
                                      multicast HW MAC address list. This field is optional
                                      if ResetMCastFilter is TRUE.

   <at> retval EFI_SUCCESS            The multicast receive filter list was updated.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  One or more of the following conditions is TRUE:
                                      * This is NULL
                                      * There are bits set in Enable that are not set
                                        in Snp->SnpMode->ReceiveFilterMask
                                      * There are bits set in Disable that are not set
                                        in Snp->SnpMode->ReceiveFilterMask
                                      * Multicast is being enabled (the
                                        EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST bit is
                                        set in Enable, it is not set in Disable, and
                                        ResetMCastFilter is FALSE) and MCastFilterCount
                                        is zero
                                      * Multicast is being enabled and MCastFilterCount
                                        is greater than Snp->SnpMode->MaxMCastFilterCount
                                      * Multicast is being enabled and MCastFilter is NULL
                                      * Multicast is being enabled and one or more of
                                        the addresses in the MCastFilter list are not
                                        valid multicast MAC addresses
   <at> retval EFI_DEVICE_ERROR       One or more of the following conditions is TRUE:
                                      * The network interface has been started but has
                                        not been initialized
                                      * An unexpected error was returned by the
                                        underlying network driver or device
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network
                                      interface.

**/
EFI_STATUS
EFIAPI
SnpReceiveFilters (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN UINT32                      Enable,
    IN UINT32                      Disable,
    IN BOOLEAN                     ResetMCastFilter,
    IN UINTN                       MCastFilterCnt,  OPTIONAL
    IN EFI_MAC_ADDRESS             *MCastFilter     OPTIONAL
    );

/**
  Modifies or resets the current station address, if supported.

  This function modifies or resets the current station address of a network
  interface, if supported. If Reset is TRUE, then the current station address is
  set to the network interface's permanent address. If Reset is FALSE, and the
  network interface allows its station address to be modified, then the current
  station address is changed to the address specified by New. If the network
  interface does not allow its station address to be modified, then
  EFI_INVALID_PARAMETER will be returned. If the station address is successfully
  updated on the network interface, EFI_SUCCESS will be returned. If the driver
  has not been initialized, EFI_DEVICE_ERROR will be returned.

   <at> param This  A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param Reset Flag used to reset the station address to the network interface's
               permanent address.
   <at> param New   New station address to be used for the network interface.


   <at> retval EFI_SUCCESS            The network interface's station address was updated.
   <at> retval EFI_NOT_STARTED        The Simple Network Protocol interface has not been
                                      started by calling Start().
   <at> retval EFI_INVALID_PARAMETER  The New station address was not accepted by the NIC.
   <at> retval EFI_INVALID_PARAMETER  Reset is FALSE and New is NULL.
   <at> retval EFI_DEVICE_ERROR       The Simple Network Protocol interface has not
                                      been initialized by calling Initialize().
   <at> retval EFI_DEVICE_ERROR       An error occurred attempting to set the new
                                      station address.
   <at> retval EFI_UNSUPPORTED        The NIC does not support changing the network
                                      interface's station address.

**/
EFI_STATUS
EFIAPI
SnpStationAddress (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN BOOLEAN                     Reset,
    IN EFI_MAC_ADDRESS             *New  OPTIONAL
    );

/**
  Resets or collects the statistics on a network interface.

  This function resets or collects the statistics on a network interface. If the
  size of the statistics table specified by StatisticsSize is not big enough for
  all the statistics that are collected by the network interface, then a partial
  buffer of statistics is returned in StatisticsTable, StatisticsSize is set to
  the size required to collect all the available statistics, and
  EFI_BUFFER_TOO_SMALL is returned.
  If StatisticsSize is big enough for all the statistics, then StatisticsTable
  will be filled, StatisticsSize will be set to the size of the returned
  StatisticsTable structure, and EFI_SUCCESS is returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.
  If Reset is FALSE, and both StatisticsSize and StatisticsTable are NULL, then
  no operations will be performed, and EFI_SUCCESS will be returned.
  If Reset is TRUE, then all of the supported statistics counters on this network
  interface will be reset to zero.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param Reset                   Set to TRUE to reset the statistics for the network interface.
   <at> param StatisticsSize          On input the size, in bytes, of StatisticsTable. On output
                                      the size, in bytes, of the resulting table of statistics.
   <at> param StatisticsTable         A pointer to the EFI_NETWORK_STATISTICS structure that
                                      contains the statistics. Type EFI_NETWORK_STATISTICS is
                                      defined in "Related Definitions" below.

   <at> retval EFI_SUCCESS            The requested operation succeeded.
   <at> retval EFI_NOT_STARTED        The Simple Network Protocol interface has not been
                                      started by calling Start().
   <at> retval EFI_BUFFER_TOO_SMALL   StatisticsSize is not NULL and StatisticsTable is
                                      NULL. The current buffer size that is needed to
                                      hold all the statistics is returned in StatisticsSize.
   <at> retval EFI_BUFFER_TOO_SMALL   StatisticsSize is not NULL and StatisticsTable is
                                      not NULL. The current buffer size that is needed
                                      to hold all the statistics is returned in
                                      StatisticsSize. A partial set of statistics is
                                      returned in StatisticsTable.
   <at> retval EFI_INVALID_PARAMETER  StatisticsSize is NULL and StatisticsTable is not
                                      NULL.
   <at> retval EFI_DEVICE_ERROR       The Simple Network Protocol interface has not
                                      been initialized by calling Initialize().
   <at> retval EFI_DEVICE_ERROR       An error was encountered collecting statistics
                                      from the NIC.
   <at> retval EFI_UNSUPPORTED        The NIC does not support collecting statistics
                                      from the network interface.

**/
EFI_STATUS
EFIAPI
SnpStatistics (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN BOOLEAN                     Reset,
    IN OUT UINTN                   *StatisticsSize,  OPTIONAL
    IN OUT EFI_NETWORK_STATISTICS  *StatisticsTable  OPTIONAL
    );

/**
  Converts a multicast IP address to a multicast HW MAC address.

  This function converts a multicast IP address to a multicast HW MAC address
  for all packet transactions. If the mapping is accepted, then EFI_SUCCESS will
  be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param IPv6                    Set to TRUE if the multicast IP address is IPv6 [RFC 2460].
                                      Set to FALSE if the multicast IP address is IPv4 [RFC 791].
   <at> param IP                      The multicast IP address that is to be converted to a multicast
                                      HW MAC address.
   <at> param MAC                     The multicast HW MAC address that is to be generated from IP.

   <at> retval EFI_SUCCESS            The multicast IP address was mapped to the
                                      multicast HW MAC address.
   <at> retval EFI_NOT_STARTED        The Simple Network Protocol interface has not
                                      been started by calling Start().
   <at> retval EFI_INVALID_PARAMETER  IP is NULL.
   <at> retval EFI_INVALID_PARAMETER  MAC is NULL.
   <at> retval EFI_INVALID_PARAMETER  IP does not point to a valid IPv4 or IPv6
                                      multicast address.
   <at> retval EFI_DEVICE_ERROR       The Simple Network Protocol interface has not
                                      been initialized by calling Initialize().
   <at> retval EFI_UNSUPPORTED        IPv6 is TRUE and the implementation does not
                                      support IPv6 multicast to MAC address conversion.

**/
EFI_STATUS
EFIAPI
SnpMcastIpToMac (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN BOOLEAN                     IPv6,
    IN EFI_IP_ADDRESS              *IP,
    OUT EFI_MAC_ADDRESS            *MAC
    );

/**
  Performs read and write operations on the NVRAM device attached to a network
  interface.

  This function performs read and write operations on the NVRAM device attached
  to a network interface. If ReadWrite is TRUE, a read operation is performed.
  If ReadWrite is FALSE, a write operation is performed. Offset specifies the
  byte offset at which to start either operation. Offset must be a multiple of
  NvRamAccessSize , and it must have a value between zero and NvRamSize.
  BufferSize specifies the length of the read or write operation. BufferSize must
  also be a multiple of NvRamAccessSize, and Offset + BufferSize must not exceed
  NvRamSize.
  If any of the above conditions is not met, then EFI_INVALID_PARAMETER will be
  returned.
  If all the conditions are met and the operation is "read," the NVRAM device
  attached to the network interface will be read into Buffer and EFI_SUCCESS
  will be returned. If this is a write operation, the contents of Buffer will be
  used to update the contents of the NVRAM device attached to the network
  interface and EFI_SUCCESS will be returned.

  It does the basic checking on the input parameters and retrieves snp structure
  and then calls the read_nvdata() call which does the actual reading

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param ReadWrite               TRUE for read operations, FALSE for write operations.
   <at> param Offset                  Byte offset in the NVRAM device at which to start the read or
                                      write operation. This must be a multiple of NvRamAccessSize
                                      and less than NvRamSize. (See EFI_SIMPLE_NETWORK_MODE)
   <at> param BufferSize              The number of bytes to read or write from the NVRAM device.
                                      This must also be a multiple of NvramAccessSize.
   <at> param Buffer                  A pointer to the data buffer.

   <at> retval EFI_SUCCESS            The NVRAM access was performed.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  One or more of the following conditions is TRUE:
                                      * The This parameter is NULL
                                      * The This parameter does not point to a valid
                                        EFI_SIMPLE_NETWORK_PROTOCOL  structure
                                      * The Offset parameter is not a multiple of
                                        EFI_SIMPLE_NETWORK_MODE.NvRamAccessSize
                                      * The Offset parameter is not less than
                                        EFI_SIMPLE_NETWORK_MODE.NvRamSize
                                      * The BufferSize parameter is not a multiple of
                                        EFI_SIMPLE_NETWORK_MODE.NvRamAccessSize
                                      * The Buffer parameter is NULL
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network
                                      interface.
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network
                                      interface.

**/
EFI_STATUS
EFIAPI
SnpNvData (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN BOOLEAN                     ReadWrite,
    IN UINTN                       Offset,
    IN UINTN                       BufferSize,
    IN OUT VOID                    *Buffer
    );

/**
  Reads the current interrupt status and recycled transmit buffer status from a
  network interface.

  This function gets the current interrupt and recycled transmit buffer status
  from the network interface. The interrupt status is returned as a bit mask in
  InterruptStatus. If InterruptStatus is NULL, the interrupt status will not be
  read. If TxBuf is not NULL, a recycled transmit buffer address will be retrieved.
  If a recycled transmit buffer address is returned in TxBuf, then the buffer has
  been successfully transmitted, and the status for that buffer is cleared. If
  the status of the network interface is successfully collected, EFI_SUCCESS
  will be returned. If the driver has not been initialized, EFI_DEVICE_ERROR will
  be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param InterruptStatus         A pointer to the bit mask of the currently active
                                      interrupts (see "Related Definitions"). If this is NULL,
                                      the interrupt status will not be read from the device.
                                      If this is not NULL, the interrupt status will be read
                                      from the device. When the interrupt status is read, it
                                      will also be cleared. Clearing the transmit interrupt does
                                      not empty the recycled transmit buffer array.
   <at> param TxBuf                   Recycled transmit buffer address. The network interface
                                      will not transmit if its internal recycled transmit
                                      buffer array is full. Reading the transmit buffer does
                                      not clear the transmit interrupt. If this is NULL, then
                                      the transmit buffer status will not be read. If there
                                      are no transmit buffers to recycle and TxBuf is not NULL,
                                      TxBuf will be set to NULL.

   <at> retval EFI_SUCCESS            The status of the network interface was retrieved.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_INVALID_PARAMETER  This parameter was NULL or did not point to a valid
                                      EFI_SIMPLE_NETWORK_PROTOCOL structure.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network
                                      interface.

**/
EFI_STATUS
EFIAPI
SnpGetStatus (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    OUT UINT32                     *InterruptStatus, OPTIONAL
    OUT VOID                       **TxBuf           OPTIONAL
    );

/**
  Places a packet in the transmit queue of a network interface.

  This function places the packet specified by Header and Buffer on the transmit
  queue. If HeaderSize is nonzero and HeaderSize is not equal to
  This->SnpMode->MediaHeaderSize, then EFI_INVALID_PARAMETER will be returned. If
  BufferSize is less than This->SnpMode->MediaHeaderSize, then EFI_BUFFER_TOO_SMALL
  will be returned. If Buffer is NULL, then EFI_INVALID_PARAMETER will be
  returned. If HeaderSize is nonzero and DestAddr or Protocol is NULL, then
  EFI_INVALID_PARAMETER will be returned. If the transmit engine of the network
  interface is busy, then EFI_NOT_READY will be returned. If this packet can be
  accepted by the transmit engine of the network interface, the packet contents
  specified by Buffer will be placed on the transmit queue of the network
  interface, and EFI_SUCCESS will be returned. GetStatus() can be used to
  determine when the packet has actually been transmitted. The contents of the
  Buffer must not be modified until the packet has actually been transmitted.
  The Transmit() function performs nonblocking I/O. A caller who wants to perform
  blocking I/O, should call Transmit(), and then GetStatus() until the
  transmitted buffer shows up in the recycled transmit buffer.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param HeaderSize              The size, in bytes, of the media header to be filled in by the
                                      Transmit() function. If HeaderSize is nonzero, then it must
                                      be equal to This->SnpMode->MediaHeaderSize and the DestAddr and
                                      Protocol parameters must not be NULL.
   <at> param BufferSize              The size, in bytes, of the entire packet (media header and
                                      data) to be transmitted through the network interface.
   <at> param Buffer                  A pointer to the packet (media header followed by data) to be
                                      transmitted. This parameter cannot be NULL. If HeaderSize is
                                      zero, then the media header in Buffer must already be filled
                                      in by the caller. If HeaderSize is nonzero, then the media
                                      header will be filled in by the Transmit() function.
   <at> param SrcAddr                 The source HW MAC address. If HeaderSize is zero, then this
                                      parameter is ignored. If HeaderSize is nonzero and SrcAddr
                                      is NULL, then This->SnpMode->CurrentAddress is used for the
                                      source HW MAC address.
   <at> param DestAddr                The destination HW MAC address. If HeaderSize is zero, then
                                      this parameter is ignored.
   <at> param Protocol                The type of header to build. If HeaderSize is zero, then this
                                      parameter is ignored. See RFC 1700, section "Ether Types,"
                                      for examples.

   <at> retval EFI_SUCCESS            The packet was placed on the transmit queue.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_NOT_READY          The network interface is too busy to accept this
                                      transmit request.
   <at> retval EFI_BUFFER_TOO_SMALL   The BufferSize parameter is too small.
   <at> retval EFI_INVALID_PARAMETER  One or more of the parameters has an unsupported
                                      value.
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.
   <at> retval EFI_UNSUPPORTED        This function is not supported by the network interface.

**/
EFI_STATUS
EFIAPI
SnpTransmit (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    IN UINTN                       HeaderSize,
    IN UINTN                       BufferSize,
    IN VOID                        *Buffer,
    IN EFI_MAC_ADDRESS             *SrcAddr,  OPTIONAL
    IN EFI_MAC_ADDRESS             *DestAddr, OPTIONAL
    IN UINT16                      *Protocol  OPTIONAL
    );

/**
  Receives a packet from a network interface.

  This function retrieves one packet from the receive queue of a network interface.
  If there are no packets on the receive queue, then EFI_NOT_READY will be
  returned. If there is a packet on the receive queue, and the size of the packet
  is smaller than BufferSize, then the contents of the packet will be placed in
  Buffer, and BufferSize will be updated with the actual size of the packet.
  In addition, if SrcAddr, DestAddr, and Protocol are not NULL, then these values
  will be extracted from the media header and returned. EFI_SUCCESS will be
  returned if a packet was successfully received.
  If BufferSize is smaller than the received packet, then the size of the receive
  packet will be placed in BufferSize and EFI_BUFFER_TOO_SMALL will be returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.

   <at> param This                    A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
   <at> param HeaderSize              The size, in bytes, of the media header received on the network
                                      interface. If this parameter is NULL, then the media header size
                                      will not be returned.
   <at> param BufferSize              On entry, the size, in bytes, of Buffer. On exit, the size, in
                                      bytes, of the packet that was received on the network interface.
   <at> param Buffer                  A pointer to the data buffer to receive both the media
                                      header and the data.
   <at> param SrcAddr                 The source HW MAC address. If this parameter is NULL, the HW
                                      MAC source address will not be extracted from the media header.
   <at> param DestAddr                The destination HW MAC address. If this parameter is NULL,
                                      the HW MAC destination address will not be extracted from
                                      the media header.
   <at> param Protocol                The media header type. If this parameter is NULL, then the
                                      protocol will not be extracted from the media header. See
                                      RFC 1700 section "Ether Types" for examples.

   <at> retval EFI_SUCCESS            The received data was stored in Buffer, and
                                      BufferSize has been updated to the number of
                                      bytes received.
   <at> retval EFI_NOT_STARTED        The network interface has not been started.
   <at> retval EFI_NOT_READY          No packets have been received on the network interface.
   <at> retval EFI_BUFFER_TOO_SMALL   BufferSize is too small for the received packets.
                                      BufferSize has been updated to the required size.
   <at> retval EFI_INVALID_PARAMETER  One or more of the following conditions is TRUE:
                                      * The This parameter is NULL
                                      * The This parameter does not point to a valid
                                        EFI_SIMPLE_NETWORK_PROTOCOL structure.
                                      * The BufferSize parameter is NULL
                                      * The Buffer parameter is NULL
   <at> retval EFI_DEVICE_ERROR       The command could not be sent to the network interface.

**/
EFI_STATUS
EFIAPI
SnpReceive (
    IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
    OUT UINTN                      *HeaderSize OPTIONAL,
    IN OUT UINTN                   *BufferSize,
    OUT VOID                       *Buffer,
    OUT EFI_MAC_ADDRESS            *SrcAddr OPTIONAL,
    OUT EFI_MAC_ADDRESS            *DestAddr OPTIONAL,
    OUT UINT16                     *Protocol OPTIONAL
    );

#endif

/**
  Process exit boot event.

  @param [in] Event                   Event id.
  @param [in] Context                 Driver context.

**/
VOID
EFIAPI
MarvellYukonNotifyExitBoot (
  IN EFI_EVENT Event,
  IN VOID *Context
  );

/**
  Get driver's data structure associated with controller

  @param [in] Controller           Controller Id.
  @param [out] Data                Driver's data.

**/
EFI_STATUS
EFIAPI
MarvellYukonGetControllerData (
  IN EFI_HANDLE Controller,
  OUT struct msk_softc **Data
  );

/**
  Add driver's data structure associated with controller

  @param [in] Controller           Controller Id.
  @param [in] Data                 Driver's data.

**/
EFI_STATUS
EFIAPI
MarvellYukonAddControllerData (
  IN EFI_HANDLE Controller,
  IN struct msk_softc *
  );

/**
  Delete driver's data structure associated with controller

  @param [in] Controller           Controller Id.

**/
EFI_STATUS
EFIAPI
MarvellYukonDelControllerData (
  IN EFI_HANDLE Controller
  );

/**
  Find node associated with controller

  @param [in] Controller           Controller Id.
  @param [out] DrvLinkedBuff       Controller's node.

**/
EFI_STATUS
EFIAPI
MarvellYukonFindControllerNode (
  IN EFI_HANDLE Controller,
  OUT MSK_LINKED_DRV_BUF **DrvLinkedBuff
  );

/*  _SNP_H_  */
