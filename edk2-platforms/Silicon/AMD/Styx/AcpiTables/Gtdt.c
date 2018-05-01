/** @file

  Generic Timer Description Table (GTDT)

  Copyright (c) 2012 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/**

  Derived from:
   ArmPlatformPkg/ArmJunoPkg/AcpiTables/Gtdt.aslc

**/

#include <AmdStyxAcpiLib.h>

#pragma pack(push, 1)

#define CNT_CONTROL_BASE_ADDRESS      FixedPcdGet64(PcdCntControlBase)
#define CNT_READ_BASE_ADDRESS         FixedPcdGet64(PcdCntReadBase)
#define CNT_CTL_BASE_ADDRESS          FixedPcdGet64(PcdCntCTLBase)
#define CNT_BASE0_ADDRESS             FixedPcdGet64(PcdCntBase0)
#define CNT_EL0_BASE0_ADDRESS         FixedPcdGet64(PcdCntEL0Base0)
#define SBSA_WATCHDOG_REFRESH_BASE    FixedPcdGet64(PcdSbsaWatchDogRefreshBase)
#define SBSA_WATCHDOG_CONTROL_BASE    FixedPcdGet64(PcdSbsaWatchDogControlBase)
#define SBSA_WAKEUP_GSIV              FixedPcdGet64(PcdSbsaWakeUpGSIV)
#define SBSA_WATCHDOG_GSIV            FixedPcdGet64(PcdSbsaWatchDogGSIV)


/*
 * Section 8.2.3 of Cortex-A15 r2p1 TRM
 */
#define CP15_TIMER_SEC_INTR    29
#define CP15_TIMER_NS_INTR     30
#define CP15_TIMER_VIRT_INTR   27
#define CP15_TIMER_NSHYP_INTR  26

/* SBSA Timers */
  #define PLATFORM_TIMER_COUNT          2
  #define PLATFORM_TIMER_OFFSET         sizeof (EFI_ACPI_5_1_GENERIC_TIMER_DESCRIPTION_TABLE)

/*
// GTDT Table timer flags.

Bit 0: Timer interrupt Mode
  This bit indicates the mode of the timer interrupt
    1: Interrupt is Edge triggered
    0: Interrupt is Level triggered
Timer Interrupt polarity
  This bit indicates the polarity of the timer interrupt
    1: Interrupt is Active low
    0: Interrupt is Active high
Reserved 2 30 Reserved, must be zero.

From A15 TRM:
   9.2 Generic Timer functional description
    ...
    Each timer provides an active-LOW interrupt output that is an external pin to the SoC and is
    sent to the GIC as a Private Peripheral Interrupt (PPI). See Interrupt sources on page 8-4 for
    the ID and PPI allocation of the Timer interrupts.
      PPI6 Virtual Maintenance Interrupt.
      PPI5 Hypervisor timer event.
      PPI4 Virtual timer event.
      PPI3 nIRQ.
      PPI2 Non-secure physical timer event.
      PPI1 Secure physical timer event.
      PPI0-5 Active-LOW level-sensitive.
      PPI6 Active-HIGH level-sensitive.*/

#define GTDT_TIMER_EDGE_TRIGGERED   EFI_ACPI_5_1_GTDT_TIMER_FLAG_TIMER_INTERRUPT_MODE
#define GTDT_TIMER_LEVEL_TRIGGERED  0
#define GTDT_TIMER_ACTIVE_LOW       EFI_ACPI_5_1_GTDT_TIMER_FLAG_TIMER_INTERRUPT_POLARITY
#define GTDT_TIMER_ACTIVE_HIGH      0
#define GTDT_TIMER_SECURE           EFI_ACPI_5_1_GTDT_TIMER_FLAG_ALWAYS_ON_CAPABILITY
#define GTDT_TIMER_NON_SECURE       0
#define GTDT_GTIMER_FLAGS           (GTDT_TIMER_NON_SECURE | GTDT_TIMER_ACTIVE_HIGH | GTDT_TIMER_LEVEL_TRIGGERED)

#define GTX_TIMER_EDGE_TRIGGERED    EFI_ACPI_5_1_GTDT_GT_BLOCK_TIMER_FLAG_TIMER_INTERRUPT_MODE
#define GTX_TIMER_LEVEL_TRIGGERED   0
#define GTX_TIMER_ACTIVE_LOW        EFI_ACPI_5_1_GTDT_GT_BLOCK_TIMER_FLAG_TIMER_INTERRUPT_POLARITY
#define GTX_TIMER_ACTIVE_HIGH       0
#define GTX_TIMER_FLAGS             (GTX_TIMER_ACTIVE_HIGH | GTX_TIMER_LEVEL_TRIGGERED)

#define GTX_TIMER_SECURE            EFI_ACPI_5_1_GTDT_GT_BLOCK_COMMON_FLAG_SECURE_TIMER
#define GTX_TIMER_NON_SECURE        0
#define GTX_TIMER_SAVE_CONTEXT      EFI_ACPI_5_1_GTDT_GT_BLOCK_COMMON_FLAG_ALWAYS_ON_CAPABILITY
#define GTX_TIMER_LOSE_CONTEXT      0
#define GTX_COMMON_FLAGS            (GTX_TIMER_SAVE_CONTEXT | GTX_TIMER_NON_SECURE)

#define SBSA_WATCHDOG_EDGE_TRIGGERED   EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG_FLAG_TIMER_INTERRUPT_MODE
#define SBSA_WATCHDOG_LEVEL_TRIGGERED  0
#define SBSA_WATCHDOG_ACTIVE_LOW       EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG_FLAG_TIMER_INTERRUPT_POLARITY
#define SBSA_WATCHDOG_ACTIVE_HIGH      0
#define SBSA_WATCHDOG_SECURE           EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG_FLAG_SECURE_TIMER
#define SBSA_WATCHDOG_NON_SECURE       0
#define SBSA_WATCHDOG_FLAGS            (SBSA_WATCHDOG_NON_SECURE | SBSA_WATCHDOG_ACTIVE_HIGH | SBSA_WATCHDOG_LEVEL_TRIGGERED)


#define AMD_SBSA_GTX {                                                                     \
   EFI_ACPI_5_1_GTDT_GT_BLOCK,                           /* UINT8  Type */                 \
   sizeof (EFI_ACPI_5_1_GTDT_GT_BLOCK_STRUCTURE) +                                         \
   sizeof (EFI_ACPI_5_1_GTDT_GT_BLOCK_TIMER_STRUCTURE),  /* UINT16 Length */               \
   EFI_ACPI_RESERVED_BYTE,                               /* UINT8  Reserved */             \
   CNT_CTL_BASE_ADDRESS,                                 /* UINT64 CntCtlBase */           \
   1,                                                    /* UINT32 GTBlockTimerCount */    \
   sizeof (EFI_ACPI_5_1_GTDT_GT_BLOCK_STRUCTURE)         /* UINT32 GTBlockTimerOffset */   \
  }

#define AMD_SBSA_GTX_TIMER {                                                                  \
   0,                                                    /* UINT8  GTFrameNumber */           \
   {0, 0, 0},                                            /* UINT8  Reserved[3] */             \
   CNT_BASE0_ADDRESS,                                    /* UINT64 CntBaseX */                \
   CNT_EL0_BASE0_ADDRESS,                                /* UINT64 CntEL0BaseX */             \
   SBSA_WAKEUP_GSIV,                                     /* UINT32 GTxPhysicalTimerGSIV */    \
   GTX_TIMER_FLAGS,                                      /* UINT32 GTxPhysicalTimerFlags */   \
   0,                                                    /* UINT32 GTxVirtualTimerGSIV */     \
   GTX_TIMER_FLAGS,                                      /* UINT32 GTxVirtualTimerFlags */    \
   GTX_COMMON_FLAGS                                      /* UINT32 GTxCommonFlags */          \
  }

#define AMD_SBSA_WATCHDOG {                                                                                        \
   EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG,                     /* UINT8  Type */                                  \
   sizeof (EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG_STRUCTURE),  /* UINT16 Length */                                \
   EFI_ACPI_RESERVED_BYTE,                                      /* UINT8  Reserved */                              \
   SBSA_WATCHDOG_REFRESH_BASE,                                  /* UINT64 RefreshFramePhysicalAddress */           \
   SBSA_WATCHDOG_CONTROL_BASE,                                  /* UINT64 WatchdogControlFramePhysicalAddress */   \
   SBSA_WATCHDOG_GSIV,                                          /* UINT32 WatchdogTimerGSIV */                     \
   SBSA_WATCHDOG_FLAGS                                          /* UINT32 WatchdogTimerFlags */                    \
  }

typedef struct {
  EFI_ACPI_5_1_GENERIC_TIMER_DESCRIPTION_TABLE      Gtdt;
  EFI_ACPI_5_1_GTDT_GT_BLOCK_STRUCTURE              GTxBlock;
  EFI_ACPI_5_1_GTDT_GT_BLOCK_TIMER_STRUCTURE        GTxTimer;
  EFI_ACPI_5_1_GTDT_SBSA_GENERIC_WATCHDOG_STRUCTURE WatchDog;
} AMD_ACPI_5_1_ARM_GTDT_STRUCTURE;

STATIC AMD_ACPI_5_1_ARM_GTDT_STRUCTURE AcpiGtdt = {
  {
    AMD_ACPI_HEADER(EFI_ACPI_5_1_GENERIC_TIMER_DESCRIPTION_TABLE_SIGNATURE,
                    AMD_ACPI_5_1_ARM_GTDT_STRUCTURE,
                    EFI_ACPI_5_1_GENERIC_TIMER_DESCRIPTION_TABLE_REVISION),
    CNT_CONTROL_BASE_ADDRESS,     // UINT64  PhysicalAddress
    0,                            // UINT32  Reserved
    CP15_TIMER_SEC_INTR,          // UINT32  SecureEL1TimerGSIV
    GTDT_GTIMER_FLAGS,            // UINT32  SecureEL1TimerFlags
    CP15_TIMER_NS_INTR,           // UINT32  NonSecureEL1TimerGSIV
    GTDT_GTIMER_FLAGS,            // UINT32  NonSecureEL1TimerFlags
    CP15_TIMER_VIRT_INTR,         // UINT32  VirtualTimerGSIV
    GTDT_GTIMER_FLAGS,            // UINT32  VirtualTimerFlags
    CP15_TIMER_NSHYP_INTR,        // UINT32  NonSecureEL2TimerGSIV
    GTDT_GTIMER_FLAGS,            // UINT32  NonSecureEL2TimerFlags
    CNT_READ_BASE_ADDRESS,        // UINT64  CntReadBaseAddress
    PLATFORM_TIMER_COUNT,         // UINT32  PlatformTimerCount
    PLATFORM_TIMER_OFFSET         // UINT32  PlatformTimerOffset
  },
  AMD_SBSA_GTX,
  AMD_SBSA_GTX_TIMER,
  AMD_SBSA_WATCHDOG,
};

#pragma pack(pop)


EFI_ACPI_DESCRIPTION_HEADER *
GtdtHeader (
  VOID
  )
{
  UINT32 CpuId = PcdGet32 (PcdSocCpuId);

  // Check BaseModel and Stepping: Styx-B0 or prior?
  if (((CpuId & 0xFF0) == 0) || ((CpuId & 0x00F) == 0)) {
    AcpiGtdt.Gtdt.Header.Length = sizeof (EFI_ACPI_5_1_GENERIC_TIMER_DESCRIPTION_TABLE);
    AcpiGtdt.Gtdt.PlatformTimerCount = 0;
    AcpiGtdt.Gtdt.PlatformTimerOffset = 0;
  }

  return (EFI_ACPI_DESCRIPTION_HEADER *) &AcpiGtdt.Gtdt.Header;
}
