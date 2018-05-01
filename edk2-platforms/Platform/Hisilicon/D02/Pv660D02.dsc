#
#  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
#  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
#  Copyright (c) 2015, Linaro Limited. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Pv660D02
  PLATFORM_GUID                  = E1AB8AC3-3EF1-4c6f-8D9F-ABE3EC67188E
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Hisilicon/D02/$(PLATFORM_NAME).fdf

!include Silicon/Hisilicon/Hisilicon.dsc.inc

[LibraryClasses.common]
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmPlatformLib|Silicon/Hisilicon/Library/ArmPlatformLibHisilicon/ArmPlatformLib.inf

  I2CLib|Silicon/Hisilicon/Library/I2CLib/I2CLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  IpmiCmdLib|Silicon/Hisilicon/Hi1610/Library/IpmiCmdLib/IpmiCmdLib.inf

  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf

!ifdef $(FDT_ENABLE)
  #FDTUpdateLib
  FdtUpdateLib|Platform/Hisilicon/D02/FdtUpdateLibD02/FdtUpdateLib.inf
!endif #$(FDT_ENABLE)

  CpldIoLib|Silicon/Hisilicon/Library/CpldIoLib/CpldIoLib.inf

  SerdesLib|Silicon/Hisilicon/Pv660/Library/Pv660Serdes/Pv660SerdesLib.inf

  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf
  RealTimeClockLib|Silicon/Hisilicon/Library/DS3231RealTimeClockLib/DS3231RealTimeClockLib.inf
  OemMiscLib|Platform/Hisilicon/D02/Library/OemMiscLibD02/OemMiscLibD02.inf
  OemAddressMapLib|Platform/Hisilicon/D02/Library/AddressMapPv660D02/OemAddressMapPv660D02.inf
  PlatformSysCtrlLib|Silicon/Hisilicon/Pv660/Library/PlatformSysCtrlLibPv660/PlatformSysCtrlLibPv660.inf

  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  PlatformBdsLib|Silicon/Hisilicon/Library/PlatformIntelBdsLib/PlatformIntelBdsLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf

## GIC on D02/D03 is not fully ARM GIC compatible: IRQ cannot be cancelled when
## input signal is de-asserted, except for virtual timer interrupt IRQ #27.
## So we choose to use virtual timer instead of physical one as a workaround.
## This library instance is to override the original define in LibraryClasses.AARCH64 in Hisilicon.dsc.inc.
[LibraryClasses.AARCH64]
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerVirtCounterLib/ArmGenericTimerVirtCounterLib.inf

[LibraryClasses.common.SEC]
  ArmPlatformLib|Silicon/Hisilicon/Library/ArmPlatformLibHisilicon/ArmPlatformLibSec.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  I2CLib|Silicon/Hisilicon/Library/I2CLib/I2CLibRuntime.inf
  SerialPortLib|Silicon/Hisilicon/Library/Dw8250SerialPortRuntimeLib/Dw8250SerialPortRuntimeLib.inf

[BuildOptions]
  GCC:*_*_AARCH64_PLATFORM_FLAGS == -I$(WORKSPACE)/Silicon/Hisilicon/Pv660/Include


################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|FALSE

[PcdsFixedAtBuild.common]
  gArmPlatformTokenSpaceGuid.PcdCoreCount|8

  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000

  # Stacks for MPCores in Normal World
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0xE1000000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0xFF00

  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x3FC00000

  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x10000000

  ## SP805 Watchdog - Motherboard Watchdog
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogBase|0x801e0000

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x80300000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gHisiTokenSpaceGuid.PcdUartClkInHz|200000000

  gHisiTokenSpaceGuid.PcdSerialPortSendDelay|10000000

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1

  gHisiTokenSpaceGuid.PcdM3SmmuBaseAddress|0xa0040000
  gHisiTokenSpaceGuid.PcdPcieSmmuBaseAddress|0xb0040000
  gHisiTokenSpaceGuid.PcdDsaSmmuBaseAddress|0xc0040000
  gHisiTokenSpaceGuid.PcdAlgSmmuBaseAddress|0xd0040000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"Hisilicon D02 UEFI 16.08 RC1"

  gHisiTokenSpaceGuid.PcdSystemProductName|L"D02"
  gHisiTokenSpaceGuid.PcdSystemVersion|L"Estuary"
  gHisiTokenSpaceGuid.PcdBaseBoardProductName|L"D02"
  gHisiTokenSpaceGuid.PcdBaseBoardVersion|L"Estuary"

  gHisiTokenSpaceGuid.PcdSlotPerChannelNum|0x2

  #
  # ARM PL390 General Interrupt Controller
  #
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x8D000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0xFE000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x8D100000

  ## DTB address at spi flash
  gHisiTokenSpaceGuid.FdtFileAddress|0xA4B00000

  gHisiTokenSpaceGuid.PcdNORFlashBase|0x90000000
  gHisiTokenSpaceGuid.PcdNORFlashCachableSize|0x8000000

  #
  # ARM Architectual Timer Frequency
  #
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|50000000

  gHisiTokenSpaceGuid.PcdSysControlBaseAddress|0x80010000
  gHisiTokenSpaceGuid.PcdMailBoxAddress|0x0000FFF8
  gHisiTokenSpaceGuid.PcdCpldBaseAddress|0x98000000
  gHisiTokenSpaceGuid.PcdSFCCFGBaseAddress|0xA6000000
  gHisiTokenSpaceGuid.PcdSFCMEM0BaseAddress|0xA4000000

  gHisiTokenSpaceGuid.PcdPeriSubctrlAddress|0x80000000

  gHisiTokenSpaceGuid.PcdMdioSubctrlAddress|0x80000000

  ## 1 SCCL + 1 SICL
  gHisiTokenSpaceGuid.PcdPlatformDefaultPackageType|0x0
  gHisiTokenSpaceGuid.PcdArmPrimaryCoreTemp|0x80020000
  gHisiTokenSpaceGuid.PcdTopOfLowMemory|0x80000000
  gHisiTokenSpaceGuid.PcdBottomOfHighMemory|0x1000000000
  gHisiTokenSpaceGuid.PcdTrustedFirmwareEnable|0x1

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gHisiTokenSpaceGuid.PcdPcieRootBridgeMask|0x6 # bit0:HB0RB0,bit1:HB0RB1,bit2:HB0RB2,bit3:HB0RB3,bit4:HB1RB0,bit5:HB1RB1,bit6:HB1RB2,bit7:HB1RB3
  gHisiTokenSpaceGuid.PcdHb1BaseAddress|0x400000000000 # 4T

  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceBaseAddress|0x30000000000
  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceSize|0x10000000000

  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceBaseAddress|0x22000000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceBaseAddress|0x24000000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceBaseAddress|0x26000000000
  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PciHb0Rb0Base|0xb0070000
  gHisiTokenSpaceGuid.PciHb0Rb1Base|0xb0080000
  gHisiTokenSpaceGuid.PciHb0Rb2Base|0xb0090000
  gHisiTokenSpaceGuid.PciHb0Rb3Base|0xb00a0000

  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionBaseAddress|0xb0000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionSize|0x7feffff

  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionBaseAddress|0xc0000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionSize|0x3feffff

  gHisiTokenSpaceGuid.PcdHb0Rb1CpuMemRegionBase|0x22008000000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuMemRegionBase|0x2400c000000

  gHisiTokenSpaceGuid.PcdHb0Rb1CpuIoRegionBase|0x2200fff0000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuIoRegionBase|0x2400fff0000

  gHisiTokenSpaceGuid.PcdHb0Rb1IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb1IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb2IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb2IoSize|0xffff #64K

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # PEI Phase modules
  #
  ArmPlatformPkg/PrePeiCore/PrePeiCoreMPCore.inf
  MdeModulePkg/Core/Pei/PeiMain.inf
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  ArmPlatformPkg/PlatformPei/PlatformPeim.inf
  Platform/Hisilicon/D02/MemoryInitPei/MemoryInitPeim.inf
  ArmPkg/Drivers/CpuPei/CpuPei.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/StatusCodePei.inf
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf
  Platform/Hisilicon/D02/EarlyConfigPeim/EarlyConfigPeim.inf
  Silicon/Hisilicon/Drivers/VersionInfoPeim/VersionInfoPeim.inf

  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }

  #
  # DXE
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
  }
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  Silicon/Hisilicon/Pv660/Drivers/IoInitDxe/IoInitDxe.inf

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

  Silicon/Hisilicon/Drivers/NorFlashDxe/NorFlashDxe.inf


  Platform/Hisilicon/D02/OemNicConfigD02/OemNicConfigD02.inf

  Platform/Hisilicon/D02/Drivers/SFC/SfcDxeDriver.inf

  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
  Silicon/Hisilicon/Drivers/FlashFvbDxe/FlashFvbDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  }
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf

  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  # Simple TextIn/TextOut for UEFI Terminal
  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf

  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf

  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  ArmPlatformPkg/Drivers/SP805WatchdogDxe/SP805WatchdogDxe.inf

  IntelFrameworkModulePkg/Universal/StatusCode/RuntimeDxe/StatusCodeRuntimeDxe.inf
  #
  #ACPI
  #
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  Silicon/Hisilicon/Pv660/Pv660AcpiTables/AcpiTables.inf
  Silicon/Hisilicon/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Silicon/Hisilicon/Pv660/Drivers/UnInstallAcpiTableDxe/UnInstallAcpiTable.inf

  #Pci Express
  Silicon/Hisilicon/Pv660/Drivers/PcieInitDxe/PcieInitDxe.inf
  Silicon/Hisilicon/Drivers/PciHostBridgeDxe/PciHostBridgeDxe.inf {
     <LibraryClasses>
       DmaLib|EmbeddedPkg/Library/NonCoherentDmaLib/NonCoherentDmaLib.inf
       NULL|Platform/Hisilicon/D02/Library/PlatformPciLib/PlatformPciLib.inf
  }
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

  #
  #network
  #
  Platform/Hisilicon/D02/Drivers/Net/SnpPV600Dxe/SnpPV600Dxe.inf
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  Silicon/Hisilicon/Drivers/SasV1Dxe/SasV1Dxe.inf {
    <LibraryClasses>
      DmaLib|EmbeddedPkg/Library/NonCoherentDmaLib/NonCoherentDmaLib.inf
  }
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  Silicon/Hisilicon/Pv660/Drivers/SasInitDxe/SasV1Init.inf

  MdeModulePkg/Application/HelloWorld/HelloWorld.inf
  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

  Platform/Hisilicon/D02/Drivers/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf

  #
  # USB Support
  #
  Silicon/Hisilicon/Drivers/VirtualEhciPciIo/VirtualEhciPciIo.inf
  MdeModulePkg/Bus/Pci/NonDiscoverablePciDeviceDxe/NonDiscoverablePciDeviceDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf


  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  Silicon/Hisilicon/Drivers/Smbios/SmbiosMiscDxe/SmbiosMiscDxe.inf
  Silicon/Hisilicon/Drivers/Smbios/MemorySubClassDxe/MemorySubClassDxe.inf
  Silicon/Hisilicon/Drivers/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf

!ifdef $(FDT_ENABLE)
  Silicon/Hisilicon/Drivers/UpdateFdtDxe/UpdateFdtDxe.inf {
    <LibraryClasses>
      BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  }
!endif #$(FDT_ENABLE)

  #
  # Memory test
  #
  MdeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf

  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  IntelFrameworkModulePkg/Universal/BdsDxe/BdsDxe.inf

  #
  # UEFI application (Shell Embedded Boot Loader)
  #
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
!ifdef $(INCLUDE_DP)
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf
!endif #$(INCLUDE_DP)

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|8000
  }
!ifdef $(INCLUDE_TFTP_COMMAND)
  ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf
!endif #$(INCLUDE_TFTP_COMMAND)
