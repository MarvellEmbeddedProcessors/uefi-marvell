/** @file
  UART Serial Port library functions

  Copyright (c) 2006 - 2009, Intel Corporation
  Copyright (c) 2015 - 2016, Hisilicon Limited. All rights reserved.
  Copyright (c) 2015 - 2016, Linaro Limited. All rights reserved.
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/Library/PL011SerialPortLib/
**/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/IoLib.h>
#include <Protocol/SerialIo.h>

#include "Dw8250SerialPortLib.h"


/**
  Initialize the serial device hardware.

  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfuly initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.

  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serail device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
    UINT32 ulUartClkFreq;

    MmioWrite8 (UART_LCR_REG, UART_LCR_DLS8);
    MmioWrite8 (UART_FCR_REG, UART_FCR_EN | UART_FCR_RXCLR | UART_FCR_TXCLR);
    MmioWrite8 (UART_LCR_REG, UART_LCR_DLAB | UART_LCR_DLS8);

    ulUartClkFreq = PcdGet32(PcdUartClkInHz);

    MmioWrite8 (UART_DLL_REG, (ulUartClkFreq / (16 * (UINT32)BAUDRATE) ) & 0xff);
    MmioWrite8 (UART_DLH_REG, ((ulUartClkFreq/ (16 * (UINT32)BAUDRATE) ) >> 8 ) & 0xff);
    MmioWrite8 (UART_LCR_REG, UART_LCR_DLS8);
    MmioWrite8 (UART_IEL_REG, 0x00);

    return RETURN_SUCCESS;
}


/**
  Write data from buffer to serial device.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
)
{
  UINTN  Result;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  while (NumberOfBytes--) {

    SerialPortWriteChar(*Buffer);
    Buffer++;
  }

  return Result;
}


/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
  UINTN  Result;

  if (NULL == Buffer) {
    return 0;
  }

  Result = 0;

  while (NumberOfBytes--) {
    //
    // Wait for the serail port to be ready.
    //
    *Buffer=SerialPortReadChar();
    Buffer++ ;
    Result++;
  }

  return Result;
}

/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{

  return (BOOLEAN) ((MmioRead8 (UART_LSR_REG) & UART_LSR_DR) == UART_LSR_DR);

}


VOID SerialPortWriteChar(UINT8 scShowChar)
{
    UINT32 ulLoop = 0;

    while(ulLoop < (UINT32)UART_SEND_DELAY)
    {

        if ((MmioRead8 (UART_USR_REG) & 0x02) == 0x02)
        {
            break;
        }

        ulLoop++;
    }
    MmioWrite8 (UART_THR_REG, (UINT8)scShowChar);

    ulLoop = 0;
    while(ulLoop < (UINT32)UART_SEND_DELAY)
    {
        if ((MmioRead8 (UART_USR_REG) & 0x04) == 0x04)
        {
            break;
        }
        ulLoop++;
    }

    return;
}


UINT8 SerialPortReadChar(VOID)
{
    UINT8 recvchar = 0;

    while(1)
    {
        if ((MmioRead8 (UART_LSR_REG) & UART_LSR_DR) == UART_LSR_DR)
        {
            break;
        }
    }

    recvchar = MmioRead8 (UART_RBR_REG);

    return recvchar;
}

/**
  Sets the baud rate, receive FIFO depth, transmit/receice time out, parity,
  data bits, and stop bits on a serial device.

  @param BaudRate           The requested baud rate. A BaudRate value of 0 will use the
                            device's default interface speed.
                            On output, the value actually set.
  @param ReveiveFifoDepth   The requested depth of the FIFO on the receive side of the
                            serial interface. A ReceiveFifoDepth value of 0 will use
                            the device's default FIFO depth.
                            On output, the value actually set.
  @param Timeout            The requested time out for a single character in microseconds.
                            This timeout applies to both the transmit and receive side of the
                            interface. A Timeout value of 0 will use the device's default time
                            out value.
                            On output, the value actually set.
  @param Parity             The type of parity to use on this serial device. A Parity value of
                            DefaultParity will use the device's default parity value.
                            On output, the value actually set.
  @param DataBits           The number of data bits to use on the serial device. A DataBits
                            vaule of 0 will use the device's default data bit setting.
                            On output, the value actually set.
  @param StopBits           The number of stop bits to use on this serial device. A StopBits
                            value of DefaultStopBits will use the device's default number of
                            stop bits.
                            On output, the value actually set.

  @retval RETURN_SUCCESS            The new attributes were set on the serial device.
  @retval RETURN_UNSUPPORTED        The serial device does not support this operation.
  @retval RETURN_INVALID_PARAMETER  One or more of the attributes has an unsupported value.
  @retval RETURN_DEVICE_ERROR       The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetAttributes (
  IN OUT UINT64             *BaudRate,
  IN OUT UINT32             *ReceiveFifoDepth,
  IN OUT UINT32             *Timeout,
  IN OUT EFI_PARITY_TYPE    *Parity,
  IN OUT UINT8              *DataBits,
  IN OUT EFI_STOP_BITS_TYPE *StopBits
  )
{
  return RETURN_UNSUPPORTED;
}

/**
  Set the serial device control bits.

  @param  Control                 Control bits which are to be set on the serial device.

  @retval EFI_SUCCESS             The new control bits were set on the serial device.
  @retval EFI_UNSUPPORTED         The serial device does not support this operation.
  @retval EFI_DEVICE_ERROR        The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetControl (
  IN UINT32                  Control
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get the serial device control bits.

  @param  Control                 Control signals read from the serial device.

  @retval EFI_SUCCESS             The control bits were read from the serial device.
  @retval EFI_DEVICE_ERROR        The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortGetControl (
  OUT UINT32                  *Control
  )
{

    if (SerialPortPoll ()) {
        // If a character is pending don't set EFI_SERIAL_INPUT_BUFFER_EMPTY
        *Control = EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
    } else {
        *Control = EFI_SERIAL_INPUT_BUFFER_EMPTY | EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
    }
    return EFI_SUCCESS;
}

