/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
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

#ifndef __OEM_NIC_CONFIG_H__
#define __OEM_NIC_CONFIG_H__

#define I2C_SLAVEADDR_EEPROM     (0x52)

#define I2C_OFFSET_EEPROM_ETH0   (0xc00)
#define I2C_OFFSET_EEPROM_ETH1   (I2C_OFFSET_EEPROM_ETH0 + 6)
#define I2C_OFFSET_EEPROM_ETH2   (I2C_OFFSET_EEPROM_ETH1 + 6)
#define I2C_OFFSET_EEPROM_ETH3   (I2C_OFFSET_EEPROM_ETH2 + 6)
#define I2C_OFFSET_EEPROM_ETH4   (I2C_OFFSET_EEPROM_ETH3 + 6)
#define I2C_OFFSET_EEPROM_ETH5   (I2C_OFFSET_EEPROM_ETH4 + 6)
#define I2C_OFFSET_EEPROM_ETH6   (I2C_OFFSET_EEPROM_ETH5 + 6)
#define I2C_OFFSET_EEPROM_ETH7   (I2C_OFFSET_EEPROM_ETH6 + 6)


#endif
