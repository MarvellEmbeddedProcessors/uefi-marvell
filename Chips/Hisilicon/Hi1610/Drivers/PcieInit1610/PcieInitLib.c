/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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

#include "PcieInitLib.h"
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

static PCIE_INIT_CFG mPcieIntCfg;
UINT64 pcie_subctrl_base[2] = {0xb0000000, BASE_4TB + 0xb0000000};
UINT64 pcie_subctrl_base_1610[2] = {0xa0000000, 0xb0000000};
UINT64 io_sub0_base = 0xa0000000;
UINT64 PCIE_APB_SLVAE_BASE[2] = {0xb0070000, BASE_4TB + 0xb0070000};
#define PCIE_REG_BASE(HostBridgeNum,port)              (PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(port * 0x10000))
UINT64 PCIE_APB_SLAVE_BASE_1610[2][4] = {{0xa0090000, 0xa0200000, 0xa00a0000, 0xa00b0000},
                                         {0xb0090000, 0xb0200000, 0xb00a0000, 0xb00b0000}};
UINT64 PCIE_PHY_BASE_1610[2][4] = {{0xa00c0000, 0xa00d0000, 0xa00e0000, 0xa00f0000},
                                    {0xb00c0000,0xb00d0000, 0xb00e0000, 0xb00f0000}};
UINT32 loop_test_flag[4] = {0,0,0,0};
UINT64 pcie_dma_des_base = PCIE_ADDR_BASE_HOST_ADDR;
#define PcieMaxLanNum       8


extern PCIE_DRIVER_CFG gastr_pcie_driver_cfg;
extern PCIE_IATU gastr_pcie_iatu_cfg;
extern PCIE_IATU_VA mPcieIatuTable;

VOID PcieRegWrite(UINT32 Port, UINTN Offset, UINT32 Value)
{
    RegWrite((UINT64)mPcieIntCfg.RegResource[Port] + Offset, Value);

}

UINT32 PcieRegRead(UINT32 Port, UINTN Offset)
{
    UINT32 Value = 0;

    RegRead((UINT64)mPcieIntCfg.RegResource[Port] + Offset, Value);
    return Value;
}

VOID PcieMmioWrite(UINT32 Port, UINTN Offset0, UINTN Offset1, UINT32 Value)
{
    RegWrite((UINT64)mPcieIntCfg.CfgResource[Port] + Offset0 + Offset1, Value);
}

UINT32 PcieMmioRead(UINT32 Port, UINTN Offset0, UINTN Offset1)
{
    UINT32 Value = 0;
    RegRead((UINT64)mPcieIntCfg.CfgResource[Port] + Offset0 + Offset1, Value);
    return Value;
}

VOID PcieChangeRwMode(UINT32 HostBridgeNum, UINT32 Port, PCIE_RW_MODE Mode)
{
    u_sc_pcie0_clkreq pcie0;
    u_sc_pcie1_clkreq pcie1;
    u_sc_pcie2_clkreq pcie2;
    u_sc_pcie3_clkreq pcie3;

    switch(Port)
    {
        case 0:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_CLKREQ_REG, pcie0.UInt32);
            pcie0.Bits.pcie0_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_CLKREQ_REG, pcie0.UInt32);
            break;
        case 1:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE1_CLKREQ_REG, pcie1.UInt32);
            pcie1.Bits.pcie1_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE1_CLKREQ_REG, pcie1.UInt32);
            break;
        case 2:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE2_CLKREQ_REG, pcie2.UInt32);
            pcie2.Bits.pcie2_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE2_CLKREQ_REG, pcie2.UInt32);
            break;
        case 3:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_CLKREQ_REG, pcie3.UInt32);
            pcie3.Bits.pcie3_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_CLKREQ_REG, pcie3.UInt32);
            break;
        default:
            break;
    }
}

VOID PcieRxValidCtrl(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, BOOLEAN On)
{
    UINT32 i;
    UINT32 Lanenum;
    UINT32 Value;
    UINT32 Laneid;
    UINT32 Loopcnt;
    UINT32 Lockedcnt[PcieMaxLanNum] = {0};

    Lanenum = 8;
    if (0x1610 == soctype)
    {
        if (On) {
            /*
            * to valid the RX, firstly, we should check and make
            * sure the RX lanes have been steadily locked.
            */
            for (Loopcnt = 500 * Lanenum; Loopcnt > 0; Loopcnt--) {
                Laneid = Loopcnt % Lanenum;
                RegRead(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0xf4 + Laneid * 0x4, Value);
                if (((Value >> 21) & 0x7) >= 4)
                    Lockedcnt[Laneid]++;
                else
                    Lockedcnt[Laneid] = 0;
                /*
                * If we get a locked status above 8 times incessantly
                * on anyone of the lanes, we get a stable lock.
                */
                if (Lockedcnt[Laneid] >= 8)
                    break;
                if (Laneid == (Lanenum - 1))
                    MicroSecondDelay(500);
                }
            if (Loopcnt == 0)
                DEBUG((EFI_D_ERROR, "pcs locked timeout!\n"));
            for (i = 0; i < Lanenum; i++) {
                RegRead(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i * 0x4, Value);
                Value &= (~BIT14);
                RegWrite(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i*0x4, Value);
                }
            } else {
            for (i = 0; i < Lanenum; i++) {
                RegRead(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i * 0x4, Value);
                Value |= BIT14;
                Value &= (~BIT15);
                RegWrite(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i*0x4, Value);
           }
        }
    }
}

EFI_STATUS PcieEnableItssm(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    PCIE_CTRL_7_U pcie_ctrl7;
    UINT32 Value = 0;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if (0x1610 == soctype)
    {
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1114, Value);
        Value |= BIT11|BIT30|BIT31;
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1114, Value);
        (VOID)PcieRxValidCtrl(soctype, HostBridgeNum, Port, 1);
        return EFI_SUCCESS;
    }
    else
    {
        PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

        pcie_ctrl7.UInt32 = PcieRegRead(Port, PCIE_CTRL_7_REG);
        pcie_ctrl7.Bits.pcie_linkdown_auto_rstn_enable = 0x1;
        pcie_ctrl7.Bits.pcie2_app_ltssm_enable = 0x1;
        PcieRegWrite(Port, PCIE_CTRL_7_REG, pcie_ctrl7.UInt32);

        PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

        return EFI_SUCCESS;
    }

}

EFI_STATUS PcieDisableItssm(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    PCIE_CTRL_7_U pcie_ctrl7;
    UINT32 Value = 0;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return PCIE_ERR_PARAM_INVALID;
    }

    if (0x1610 == soctype)
    {
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1114, Value);
        Value &= ~(BIT11);
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1114, Value);
        PcieRxValidCtrl(soctype, HostBridgeNum, Port, 1);
        return EFI_SUCCESS;
    }
    else
    {
        PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

        pcie_ctrl7.UInt32 = PcieRegRead(Port, PCIE_CTRL_7_REG);
        pcie_ctrl7.Bits.pcie2_app_ltssm_enable = 0x0;
        PcieRegWrite(Port,PCIE_CTRL_7_REG, pcie_ctrl7.UInt32);

        PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

        return EFI_SUCCESS;
    }

}


EFI_STATUS PcieLinkSpeedSet(UINT32 Port,PCIE_PORT_GEN Speed)
{
    PCIE_EP_PCIE_CAP12_U pcie_cap12;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    pcie_cap12.UInt32 = PcieRegRead(Port, PCIE_EP_PCIE_CAP12_REG);
    pcie_cap12.Bits.targetlinkspeed = Speed;
    PcieRegWrite(Port, PCIE_EP_PCIE_CAP12_REG, pcie_cap12.UInt32);

    if(mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_NTB ||
        mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_RP)
    {
        pcie_cap12.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PCIE_CAP12_REG);
        pcie_cap12.Bits.targetlinkspeed = Speed;
        PcieMmioWrite(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PCIE_CAP12_REG, pcie_cap12.UInt32);
    }
    return EFI_SUCCESS;
}

EFI_STATUS PcieLinkWidthSet(UINT32 Port, PCIE_PORT_WIDTH Width)
{
    PCIE_EP_PORT_LOGIC4_U pcie_logic4;
    PCIE_EP_PORT_LOGIC22_U logic22;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return PCIE_ERR_PARAM_INVALID;
    }

    pcie_logic4.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC4_REG);
    pcie_logic4.Bits.linkmodeenable = Width;
    pcie_logic4.Bits.crosslinkenable = 0;
    pcie_logic4.Bits.fastlinkmode = 1;
    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC4_REG, pcie_logic4.UInt32);

    logic22.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC22_REG);
    logic22.Bits.n_fts = 0xff;
    if(Width == PCIE_WITDH_X1)
    {
        logic22.Bits.pre_determ_num_of_lane = 1;
    }
    else if(Width == PCIE_WITDH_X2)
    {
        logic22.Bits.pre_determ_num_of_lane = 2;
    }
    else
    {
        logic22.Bits.pre_determ_num_of_lane = 3;
    }
    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);

    if(mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_NTB ||
        mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_RP)
    {
        pcie_logic4.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC4_REG);
        pcie_logic4.Bits.linkmodeenable = Width;
        pcie_logic4.Bits.crosslinkenable = 0;
        pcie_logic4.Bits.fastlinkmode = 1;
        PcieMmioWrite(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC4_REG, pcie_logic4.UInt32);

        logic22.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC22_REG);
        logic22.Bits.n_fts = 0xff;
        if(Width == PCIE_WITDH_X1)
        {
            logic22.Bits.pre_determ_num_of_lane = 1;
        }
        else if(Width == PCIE_WITDH_X2)
        {
            logic22.Bits.pre_determ_num_of_lane = 2;
        }
        else
        {
            logic22.Bits.pre_determ_num_of_lane = 3;
        }
        PcieMmioWrite(Port,PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);
    }
    return EFI_SUCCESS;
}

EFI_STATUS PcieSetupRC(UINT32 Port, PCIE_PORT_WIDTH Width)
{
    PCIE_EP_PORT_LOGIC22_U logic22;
    PCIE_EEP_PCI_CFG_HDR15_U hdr15;
    UINT32 Value = 0;
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    Value = PcieRegRead(Port, PCIE_EP_PORT_LOGIC4_REG);
    Value &= ~(0x3f<<16);

    if(Width == PCIE_WITDH_X1)
    {
        Value |= (0x1 << 16);
    }
    else if(Width == PCIE_WITDH_X2)
    {
        Value |= (0x3 << 16);
    }
    else if(Width == PCIE_WITDH_X4)
    {
        Value |= (0x7 << 16);
    }
    else if(Width == PCIE_WITDH_X8)
    {
        Value |= (0xf << 16);
    }
    else
    {
        DEBUG((EFI_D_ERROR,"Width is not valid\n"));
    }

    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC4_REG, Value);

    logic22.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC22_REG);
    if(Width == PCIE_WITDH_X1)
    {
        logic22.Bits.pre_determ_num_of_lane = 1;
    }
    else if(Width == PCIE_WITDH_X2)
    {
        logic22.Bits.pre_determ_num_of_lane = 2;
    }
    else if(Width == PCIE_WITDH_X4)
    {
        logic22.Bits.pre_determ_num_of_lane = 4;
    }
    else if(Width == PCIE_WITDH_X8)
    {
        logic22.Bits.pre_determ_num_of_lane = 8;
    }
    else
    {
        DEBUG((EFI_D_ERROR,"Width is not valid\n"));
    }

    logic22.UInt32 |= (0x100<<8);
    logic22.UInt32 |= (0x1<<17);
    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);

    /* setup RC BARs */
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR4_REG, 0x00000004);
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR5_REG, 0x00000000);

    /* setup interrupt pins */
    hdr15.UInt32 = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR15_REG);
    hdr15.UInt32 &= 0xffff00ff;
    hdr15.UInt32 |= 0x00000100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR15_REG, hdr15.UInt32);

    /* setup bus numbers */
    Value = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR6_REG);
    Value &= 0xff000000;
    Value |= 0x00010100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR6_REG, Value);

    /* setup command register */
    Value = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR1_REG);
    Value &= 0xffff0000;
    Value |= 0x1|0x2|0x4|0x100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR1_REG, Value);

    return EFI_SUCCESS;
}


EFI_STATUS PcieModeSet(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, PCIE_PORT_TYPE PcieType)
{
    PCIE_CTRL_0_U str_pcie_ctrl_0;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if (0x1610 == soctype)
    {
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1000 + 0xf8, 0x4 << 28);
    }
    else
    {
        PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

        str_pcie_ctrl_0.UInt32 = PcieRegRead(Port, PCIE_CTRL_0_REG);
        if(PcieType == PCIE_END_POINT)
        {
            str_pcie_ctrl_0.Bits.pcie2_slv_device_type = PCIE_EP_DEVICE;
        }
        else
        {
            str_pcie_ctrl_0.Bits.pcie2_slv_device_type = RP_OF_PCIE_RC;
        }
        PcieRegWrite(Port, PCIE_CTRL_0_REG, str_pcie_ctrl_0.UInt32);

        PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);
    }
    return EFI_SUCCESS;
}

VOID PciePcsInit(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT8 i = 0;
    UINT32 Value = 0;
    if (0x1610 == soctype)
    {
        RegRead(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i*0x4, Value);
        Value |= (1 << 20);
        RegWrite(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x204 + i*0x4, Value);
        PcieRxValidCtrl(soctype, HostBridgeNum, Port, 0);
        RegWrite(PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x264, 0x3D090);
    }
    else
    {
        if(Port<=2)
        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8020, 0x2026044);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8060, 0x2126044);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80c4, 0x2126044);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80e4, 0x2026044);

            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80a0, 0x4018);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80a4, 0x804018);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80c0, 0x11201100);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x15c, 0x3);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x158, 0);
        }
        else

        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x74, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x78, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x7c, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x84, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x88, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8c, 0x46e000);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x90, 0x46e000);

            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x34, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x38, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x3c, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x40, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x44, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x48, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x4c, 0x1001);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x50, 0x1001);

            RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0xe4, 0xffff);
        }
    }
    return;
}

VOID PcieEqualization(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32  Value;

    if (0x1610 == soctype)
    {
       PcieRegWrite(Port, 0x890, 0x1c00);
    }
    else
    PcieRegWrite(Port, 0x890, 0x1400);
    PcieRegWrite(Port, 0x894, 0xfd7);

    PcieRegWrite(Port, 0x89c, 0x0);
    PcieRegWrite(Port, 0x898, 0xfc00);
    PcieRegWrite(Port, 0x89c, 0x1);
    PcieRegWrite(Port, 0x898, 0xbd00);
    PcieRegWrite(Port, 0x89c, 0x2);
    PcieRegWrite(Port, 0x898, 0xccc0);
    PcieRegWrite(Port, 0x89c, 0x3);
    PcieRegWrite(Port, 0x898, 0x8dc0);
    PcieRegWrite(Port, 0x89c, 0x4);
    PcieRegWrite(Port, 0x898, 0xfc0);
    PcieRegWrite(Port, 0x89c, 0x5);
    PcieRegWrite(Port, 0x898, 0xe46);
    PcieRegWrite(Port, 0x89c, 0x6);
    PcieRegWrite(Port, 0x898, 0xdc8);
    PcieRegWrite(Port, 0x89c, 0x7);
    PcieRegWrite(Port, 0x898, 0xcb46);
    PcieRegWrite(Port, 0x89c, 0x8);
    PcieRegWrite(Port, 0x898, 0x8c07);
    PcieRegWrite(Port, 0x89c, 0x9);
    PcieRegWrite(Port, 0x898, 0xd0b);
    PcieRegWrite(Port, 0x8a8, 0x103ff21);
    if (0x1610 == soctype)
    {
        PcieRegWrite(Port, 0x164, 0x44444444);
        PcieRegWrite(Port, 0x168, 0x44444444);
        PcieRegWrite(Port, 0x16c, 0x44444444);
        PcieRegWrite(Port, 0x170, 0x44444444);
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1000 + 0x2d0, Value);
        Value &= (~0x3f);
        Value |= 0x5;
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1000 + 0x2d0, Value);

    }
    else
    {
        Value = PcieRegRead(Port, 0x80);
        Value |= 0x80;
        PcieRegWrite(Port, 0x80, Value);

        PcieRegWrite(Port, 0x184, 0x44444444);
        PcieRegWrite(Port, 0x188, 0x44444444);
        PcieRegWrite(Port, 0x18c, 0x44444444);
        PcieRegWrite(Port, 0x190, 0x44444444);
    }
}


EFI_STATUS PciePortReset(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }


    if(PcieIsLinkUp(soctype, HostBridgeNum, Port) && mPcieIntCfg.PortIsInitilized[Port])
    {
        (VOID)PcieDisableItssm(soctype, HostBridgeNum, Port);
    }

    mPcieIntCfg.PortIsInitilized[Port] = FALSE;
    mPcieIntCfg.DmaResource[Port] = (VOID *)NULL;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_READ] = 0;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_WRITE] = 0;
    ZeroMem(&mPcieIntCfg.Dev[Port], sizeof(DRIVER_CFG_U));

    if(Port <= 2)
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum]+ PCIE_SUBCTRL_SC_PCIE0_RESET_REQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_DREQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);
    }
    else
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_REQ_REG,0x1);
        MicroSecondDelay(0x1000);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_DREQ_REG,0x1);
        MicroSecondDelay(0x1000);
    }
    return EFI_SUCCESS;
}

EFI_STATUS AssertPcieCoreReset(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(PcieIsLinkUp(soctype, HostBridgeNum, Port))
    {
        (VOID)PcieDisableItssm(soctype, HostBridgeNum, Port);
    }

    if (0x1610 == soctype)
    {
        if(Port <= 2)
        {
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_REQ_REG + (UINT32)(8 * Port), 0x3);
            MicroSecondDelay(0x1000);
        }
        else
        {
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_REQ_REG, 0x3);
            MicroSecondDelay(0x1000);
        }
    }
    else
    {
        if(Port <= 2)
        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_REQ_REG + (UINT32)(8 * Port), 0x1);
            MicroSecondDelay(0x1000);
        }
        else
        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_REQ_REG,0x1);
            MicroSecondDelay(0x1000);
        }
    }

    return EFI_SUCCESS;
}

EFI_STATUS DeassertPcieCoreReset(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(PcieIsLinkUp(soctype, HostBridgeNum, Port))
    {
        (VOID)PcieDisableItssm(soctype, HostBridgeNum, Port);
    }

    if (0x1610 == soctype)
    {
        if(Port <= 2)
        {
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_DREQ_REG + (UINT32)(8 * Port), 0x3);
            MicroSecondDelay(0x1000);
        }
        else
        {
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_DREQ_REG,0x3);
            MicroSecondDelay(0x1000);
        }
    }
    else
    {
        if(Port <= 2)
        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_DREQ_REG + (UINT32)(8 * Port), 0x1);
            MicroSecondDelay(0x1000);
        }
        else
        {
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_DREQ_REG,0x1);
            MicroSecondDelay(0x1000);
        }
    }

    return EFI_SUCCESS;
}

EFI_STATUS AssertPciePcsReset(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    u_sc_pcie_hilink_pcs_reset_req reset_req;
    if (0x1610 == soctype)
    {
        if(Port <= 3)
        {
            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_REQ_REG, reset_req.UInt32);
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_APB_RESET_REQ_REG, reset_req.UInt32);

            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_REQ_REG, reset_req.UInt32);
            MicroSecondDelay(0x1000);
        }
    }
    else
    {
        if(Port <= 3)
        {
            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_REQ_REG, reset_req.UInt32);

            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_REQ_REG, reset_req.UInt32);
            MicroSecondDelay(0x1000);
        }
    }
    return EFI_SUCCESS;
}

EFI_STATUS DeassertPciePcsReset(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    u_sc_pcie_hilink_pcs_reset_req reset_req;
    if (0x1610 == soctype)
    {
        if(Port <= 3)
        {
            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + 0xacc, reset_req.UInt32);
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_DREQ_REG, reset_req.UInt32);

            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_DREQ_REG, reset_req.UInt32);
            MicroSecondDelay(0x1000);
        }
    }
    else
    {
        if(Port <= 3)
        {
            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_DREQ_REG, reset_req.UInt32);

            reset_req.UInt32 = 0;
            reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_DREQ_REG, reset_req.UInt32);
            MicroSecondDelay(0x1000);
        }
    }

    return EFI_SUCCESS;
}

EFI_STATUS HisiPcieClockCtrl(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, BOOLEAN Clock)
{
    UINT32 reg_clock_disable;
    UINT32 reg_clock_enable;

    if (Port == 3) {
        reg_clock_disable = PCIE_SUBCTRL_SC_PCIE3_CLK_DIS_REG;
        reg_clock_enable = PCIE_SUBCTRL_SC_PCIE3_CLK_EN_REG;
    } else {
        reg_clock_disable = PCIE_SUBCTRL_SC_PCIE0_2_CLK_DIS_REG(Port);
        reg_clock_enable = PCIE_SUBCTRL_SC_PCIE0_2_CLK_EN_REG(Port);
    }

    if (0x1610 == soctype)
    {
        if (Clock)
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + reg_clock_enable, 0x7);
        else
            RegWrite(pcie_subctrl_base_1610[HostBridgeNum] + reg_clock_disable, 0x7);
    }
    else
    {
        if (Clock)
            RegWrite(pcie_subctrl_base[HostBridgeNum] + reg_clock_enable, 0x3);
        else
            RegWrite(pcie_subctrl_base[HostBridgeNum] + reg_clock_disable, 0x3);
    }
    return EFI_SUCCESS;
}

VOID PciePortNumSet(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, UINT8 Num)
{
    if (0x1610 == soctype)
    {
        UINT32 Value = 0;
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1000 + 0x1c, Value);
        Value &= ~(0xff);
        Value |= Num;
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x1000 + 0x1c, Value);
    }
    return;
}

VOID PcieLaneReversalSet(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32 Value = 0;
    if (0x1610 == soctype)
    {
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + PCIE_EP_PORT_LOGIC22_REG, Value);
        Value |= BIT16;
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + PCIE_EP_PORT_LOGIC22_REG, Value);
    }
    return;
}
EFI_STATUS PcieMaskLinkUpInit(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32 Value = 0;
    if (0x1610 == soctype)
    {
        Value = PcieRegRead(Port, 0x120);
        Value |= 1 << 25;
        PcieRegWrite(Port,0x120, Value);
    }
    else
    {
        PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
        Value = PcieRegRead(Port, 0x1d0);
        Value |= 1 << 12;
        PcieRegWrite(Port,0x1d0, Value);
        PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);
    }
    return EFI_SUCCESS;
}

BOOLEAN PcieIsLinkUp(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32                     Value = 0;
    U_SC_PCIE0_SYS_STATE4      PcieStat;
    if (0x1610 == soctype)
    {
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x131c, PcieStat.UInt32);
        Value = PcieStat.UInt32;
        if ((Value & PCIE_LTSSM_STATE_MASK) == PCIE_LTSSM_LINKUP_STATE)
            return TRUE;
        return FALSE;
    }
    else
    {
        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
        Value = PcieStat.UInt32;
        if ((Value & PCIE_LTSSM_STATE_MASK) == PCIE_LTSSM_LINKUP_STATE)
            return TRUE;
        return FALSE;
    }
}

BOOLEAN PcieClockIsLock(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32 Value = 0;
    if (0x1610 == soctype)
    {
        RegRead( PCIE_PHY_BASE_1610[HostBridgeNum][Port] + 0x504, Value);
        return ((Value & 0x3) == 0x3);
    }
    else return TRUE;

}

VOID PcieSpdSet(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, UINT8 Spd)
{
    UINT32 Value = 0;
    if (0x1610 == soctype)
    {
        RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0xa0, Value);
        Value &= ~(0xf);
        Value |= Spd;
        RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0xa0, Value);
        return;
    }
    return;
}

VOID PcieWriteOwnConfig(UINT32 HostBridgeNum, UINT32 Port, UINT32 Offset, UINT32 Data)
{
     UINT32 Value = 0;
     {
         RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + (Offset & (~0x3)), Value);
         Value &= 0x0000ffff;
         Value |= 0x06040000;
         RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + (Offset & (~0x3)), Value);
         return;
     }
}

void PcieConfigContextHi1610(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32 Value = 0;

    RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x11b4, PcdGet64 (PcdPcieMsiTargetAddress));
    RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x11c4, 0);
    RegRead(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x11c8, Value);
    Value |= (1 << 12);
    RegWrite(PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][Port] + 0x11c8, Value);

    return;
}

EFI_STATUS
EFIAPI
PciePortInit (
  IN UINT32                 soctype,
  IN UINT32                 HostBridgeNum,
  IN PCIE_DRIVER_CFG        *PcieCfg
  )
{
     UINT16              Count = 0;
     UINT32             PortIndex = PcieCfg->PortIndex;

     if(PortIndex >= PCIE_MAX_PORT_NUM)
     {
        return EFI_INVALID_PARAMETER;
     }

     if (0x1610 == soctype)
     {
         mPcieIntCfg.RegResource[PortIndex] = (VOID *)PCIE_APB_SLAVE_BASE_1610[HostBridgeNum][PortIndex];
         DEBUG((EFI_D_INFO, "Soc type is 1610\n"));
     }
     else
     {
         mPcieIntCfg.RegResource[PortIndex] = (VOID *)(UINTN)PCIE_REG_BASE(HostBridgeNum, PortIndex);
         DEBUG((EFI_D_INFO, "Soc type is 660\n"));
     }

     /* assert reset signals */
     (VOID)AssertPcieCoreReset(soctype, HostBridgeNum, PortIndex);
     (VOID)AssertPciePcsReset(soctype, HostBridgeNum, PortIndex);
     (VOID)HisiPcieClockCtrl(soctype, HostBridgeNum, PortIndex, 0);
     (VOID)DeassertPcieCoreReset(soctype, HostBridgeNum, PortIndex);
     /* de-assert phy reset */
     (VOID)DeassertPciePcsReset(soctype, HostBridgeNum, PortIndex);

     /* de-assert core reset */
     (VOID)HisiPcieClockCtrl(soctype, HostBridgeNum, PortIndex, 1);

     while (!PcieClockIsLock(soctype, HostBridgeNum, PortIndex)) {
         MicroSecondDelay(1000);
         Count++;
         if (Count >= 50) {
            DEBUG((EFI_D_ERROR, "HostBridge %d, Port %d PLL Lock failed\n", HostBridgeNum, PortIndex));
            return PCIE_ERR_LINK_OVER_TIME;
         }
     }
     /* initialize phy */
     (VOID)PciePcsInit(soctype, HostBridgeNum, PortIndex);

     (VOID)PcieModeSet(soctype, HostBridgeNum, PortIndex,PcieCfg->PortInfo.PortType);
     (VOID)PcieLaneReversalSet(soctype, HostBridgeNum, PortIndex);
     (VOID)PcieSpdSet(soctype, HostBridgeNum, PortIndex, 3);
     (VOID)PciePortNumSet(soctype, HostBridgeNum, PortIndex, 0);
     /* setup root complex */
     (VOID)PcieSetupRC(PortIndex,PcieCfg->PortInfo.PortWidth);

     /* disable link up interrupt */
     (VOID)PcieMaskLinkUpInit(soctype, HostBridgeNum, PortIndex);

     /* Pcie Equalization*/
     (VOID)PcieEqualization(soctype ,HostBridgeNum, PortIndex);

     /* assert LTSSM enable */
     (VOID)PcieEnableItssm(soctype, HostBridgeNum, PortIndex);

     PcieConfigContextHi1610(soctype, HostBridgeNum, PortIndex);
     /*
     * The default size of BAR0 in Hi1610 host bridge is 0x10000000,
     * which will bring problem when most resource has been allocated
     * to BAR0 in host bridge.However, we need not use BAR0 in host bridge
     * in RC mode. Here we just disable it
     */
     PcieRegWrite(PortIndex, 0x10, 0);
     (VOID)PcieWriteOwnConfig(HostBridgeNum, PortIndex, 0xa, 0x0604);
     /* check if the link is up or not */
     while (!PcieIsLinkUp(soctype, HostBridgeNum, PortIndex)) {
         MicroSecondDelay(1000);
         Count++;
         if (Count >= 1000) {
            DEBUG((EFI_D_ERROR, "HostBridge %d, Port %d link up failed\n", HostBridgeNum, PortIndex));
            return PCIE_ERR_LINK_OVER_TIME;
         }
     }
     DEBUG((EFI_D_INFO, "HostBridge %d, Port %d Link up ok\n", HostBridgeNum, PortIndex));

     PcieRegWrite(PortIndex, 0x8BC, 0);

     return EFI_SUCCESS;
}




EFI_STATUS PcieSetDBICS2Enable(UINT32 HostBridgeNum, UINT32 Port, UINT32 Enable)
{
    PCIE_SYS_CTRL20_U dbi_ro_enable;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
    dbi_ro_enable.UInt32 = PcieRegRead(Port, PCIE_SYS_CTRL20_REG);
    dbi_ro_enable.Bits.ro_sel = Enable;
    PcieRegWrite(Port, PCIE_SYS_CTRL20_REG, dbi_ro_enable.UInt32);
    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

    return EFI_SUCCESS;

}

VOID PcieDelay(UINT32 dCount)
{
    volatile UINT32 *uwCnt = &dCount;

    while(*uwCnt > 0)
    {
        *uwCnt = *uwCnt - 1;
    }

}

