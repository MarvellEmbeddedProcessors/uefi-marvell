/**  <at> file
*  Prototypes and definitions for BSD/OS-compatible network interface media selection.
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
/*  $NetBSD: if_media.h,v 1.3 1997/03/26 01:19:27 thorpej Exp $  */
/* $FreeBSD: src/sys/net/if_media.h,v 1.47.2.1.4.1 2010/06/14 02:09:06 kensmith Exp $ */

/*-
 * Copyright (c) 1997
 *  Jonathan Stone and Jason R. Thorpe.  All rights reserved.
 *
 * This software is derived from information provided by Matt Thomas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by Jonathan Stone
 *  and Jason R. Thorpe for the NetBSD Project.
 * 4. The names of the authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NET_IF_MEDIA_H_
#define _NET_IF_MEDIA_H_

/*
 * Prototypes and definitions for BSD/OS-compatible network interface
 * media selection.
 *
 * Where it is safe to do so, this code strays slightly from the BSD/OS
 * design.  Software which uses the API (device drivers, basically)
 * shouldn't notice any difference.
 *
 * Many thanks to Matt Thomas for providing the information necessary
 * to implement this interface.
 */

/*
 * if_media Options word:
 *  Bits  Use
 *  ----  -------
 *  0-4  Media variant
 *  5-7  Media type
 *  8-15  Type specific options
 *  16-18  Mode (for multi-mode devices)
 *  19  RFU
 *  20-27  Shared (global) options
 *  28-31  Instance
 */

/*
 * Ethernet
 */
#define  IFM_ETHER          0x00000020
#define  IFM_10_T           3   /* 10BaseT - RJ45 */
#define  IFM_10_2           4   /* 10Base2 - Thinnet */
#define  IFM_10_5           5   /* 10Base5 - AUI */
#define  IFM_100_TX         6   /* 100BaseTX - RJ45 */
#define  IFM_100_FX         7   /* 100BaseFX - Fiber */
#define  IFM_100_T4         8   /* 100BaseT4 - 4 pair cat 3 */
#define  IFM_100_VG         9   /* 100VG-AnyLAN */
#define  IFM_100_T2         10  /* 100BaseT2 */
#define  IFM_1000_SX        11  /* 1000BaseSX - multi-mode fiber */
#define  IFM_10_STP         12  /* 10BaseT over shielded TP */
#define  IFM_10_FL          13  /* 10BaseFL - Fiber */
#define  IFM_1000_LX        14  /* 1000baseLX - single-mode fiber */
#define  IFM_1000_CX        15  /* 1000baseCX - 150ohm STP */
#define  IFM_1000_T         16  /* 1000baseT - 4 pair cat 5 */
#define  IFM_HPNA_1         17  /* HomePNA 1.0 (1Mb/s) */
#define  IFM_10G_LR         18  /* 10GBase-LR 1310nm Single-mode */
#define  IFM_10G_SR         19  /* 10GBase-SR 850nm Multi-mode */
#define  IFM_10G_CX4        20  /* 10GBase CX4 copper */
#define IFM_2500_SX         21  /* 2500BaseSX - multi-mode fiber */
#define IFM_10G_TWINAX      22  /* 10GBase Twinax copper */
#define IFM_10G_TWINAX_LONG 23  /* 10GBase Twinax Long copper */
#define IFM_10G_LRM         24  /* 10GBase-LRM 850nm Multi-mode */
#define IFM_UNKNOWN         25  /* media types not defined yet */
#define IFM_10G_T           26  /* 10GBase-T - RJ45 */


/* note 31 is the max! */

#define  IFM_ETH_MASTER  0x00000100  /* master mode (1000baseT) */

/*
 * Token ring
 */
#define  IFM_TOKEN        0x00000040
#define  IFM_TOK_STP4     3    /* Shielded twisted pair 4m - DB9 */
#define  IFM_TOK_STP16    4    /* Shielded twisted pair 16m - DB9 */
#define  IFM_TOK_UTP4     5    /* Unshielded twisted pair 4m - RJ45 */
#define  IFM_TOK_UTP16    6    /* Unshielded twisted pair 16m - RJ45 */
#define  IFM_TOK_STP100   7    /* Shielded twisted pair 100m - DB9 */
#define  IFM_TOK_UTP100   8    /* Unshielded twisted pair 100m - RJ45 */
#define  IFM_TOK_ETR      0x00000200  /* Early token release */
#define  IFM_TOK_SRCRT    0x00000400  /* Enable source routing features */
#define  IFM_TOK_ALLR     0x00000800  /* All routes / Single route bcast */
#define  IFM_TOK_DTR      0x00002000  /* Dedicated token ring */
#define  IFM_TOK_CLASSIC  0x00004000  /* Classic token ring */
#define  IFM_TOK_AUTO     0x00008000  /* Automatic Dedicate/Classic token ring */

/*
 * FDDI
 */
#define  IFM_FDDI       0x00000060
#define  IFM_FDDI_SMF   3    /* Single-mode fiber */
#define  IFM_FDDI_MMF   4    /* Multi-mode fiber */
#define  IFM_FDDI_UTP   5    /* CDDI / UTP */
#define  IFM_FDDI_DA    0x00000100  /* Dual attach / single attach */

/*
 * IEEE 802.11 Wireless
 */
#define  IFM_IEEE80211          0x00000080
/* NB: 0,1,2 are auto, manual, none defined below */
#define  IFM_IEEE80211_FH1      3  /* Frequency Hopping 1Mbps */
#define  IFM_IEEE80211_FH2      4  /* Frequency Hopping 2Mbps */
#define  IFM_IEEE80211_DS1      5  /* Direct Sequence 1Mbps */
#define  IFM_IEEE80211_DS2      6  /* Direct Sequence 2Mbps */
#define  IFM_IEEE80211_DS5      7  /* Direct Sequence 5.5Mbps */
#define  IFM_IEEE80211_DS11     8  /* Direct Sequence 11Mbps */
#define  IFM_IEEE80211_DS22     9  /* Direct Sequence 22Mbps */
#define  IFM_IEEE80211_OFDM6    10  /* OFDM 6Mbps */
#define  IFM_IEEE80211_OFDM9    11  /* OFDM 9Mbps */
#define  IFM_IEEE80211_OFDM12   12  /* OFDM 12Mbps */
#define  IFM_IEEE80211_OFDM18   13  /* OFDM 18Mbps */
#define  IFM_IEEE80211_OFDM24   14  /* OFDM 24Mbps */
#define  IFM_IEEE80211_OFDM36   15  /* OFDM 36Mbps */
#define  IFM_IEEE80211_OFDM48   16  /* OFDM 48Mbps */
#define  IFM_IEEE80211_OFDM54   17  /* OFDM 54Mbps */
#define  IFM_IEEE80211_OFDM72   18  /* OFDM 72Mbps */
#define  IFM_IEEE80211_DS354k   19  /* Direct Sequence 354Kbps */
#define  IFM_IEEE80211_DS512k   20  /* Direct Sequence 512Kbps */
#define  IFM_IEEE80211_OFDM3    21  /* OFDM 3Mbps */
#define  IFM_IEEE80211_OFDM4    22  /* OFDM 4.5Mbps */
#define  IFM_IEEE80211_OFDM27   23  /* OFDM 27Mbps */
/* NB: not enough bits to express MCS fully */
#define  IFM_IEEE80211_MCS      24  /* HT MCS rate */

#define  IFM_IEEE80211_ADHOC    0x00000100  /* Operate in Adhoc mode */
#define  IFM_IEEE80211_HOSTAP   0x00000200  /* Operate in Host AP mode */
#define  IFM_IEEE80211_IBSS     0x00000400  /* Operate in IBSS mode */
#define  IFM_IEEE80211_WDS      0x00000800  /* Operate in WDS mode */
#define  IFM_IEEE80211_TURBO    0x00001000  /* Operate in turbo mode */
#define  IFM_IEEE80211_MONITOR  0x00002000  /* Operate in monitor mode */
#define  IFM_IEEE80211_MBSS     0x00004000  /* Operate in MBSS mode */

/* operating mode for multi-mode devices */
#define  IFM_IEEE80211_11A      0x00010000  /* 5Ghz, OFDM mode */
#define  IFM_IEEE80211_11B      0x00020000  /* Direct Sequence mode */
#define  IFM_IEEE80211_11G      0x00030000  /* 2Ghz, CCK mode */
#define  IFM_IEEE80211_FH       0x00040000  /* 2Ghz, GFSK mode */
#define  IFM_IEEE80211_11NA     0x00050000  /* 5Ghz, HT mode */
#define  IFM_IEEE80211_11NG     0x00060000  /* 2Ghz, HT mode */

/*
 * ATM
 */
#define IFM_ATM             0x000000a0
#define IFM_ATM_UNKNOWN     3
#define IFM_ATM_UTP_25      4
#define IFM_ATM_TAXI_100    5
#define IFM_ATM_TAXI_140    6
#define IFM_ATM_MM_155      7
#define IFM_ATM_SM_155      8
#define IFM_ATM_UTP_155     9
#define IFM_ATM_MM_622      10
#define IFM_ATM_SM_622      11
#define  IFM_ATM_VIRTUAL    12
#define IFM_ATM_SDH         0x00000100  /* SDH instead of SONET */
#define IFM_ATM_NOSCRAMB    0x00000200  /* no scrambling */
#define IFM_ATM_UNASSIGNED  0x00000400  /* unassigned cells */

/*
 * CARP Common Address Redundancy Protocol
 */
#define  IFM_CARP  0x000000c0

/*
 * Shared media sub-types
 */
#define  IFM_AUTO   0    /* Autoselect best media */
#define  IFM_MANUAL 1    /* Jumper/dipswitch selects media */
#define  IFM_NONE   2    /* Deselect all media */

/*
 * Shared options
 */
#define  IFM_FDX   0x00100000  /* Force full duplex */
#define  IFM_HDX   0x00200000  /* Force half duplex */
#define  IFM_FLAG0 0x01000000  /* Driver defined flag */
#define  IFM_FLAG1 0x02000000  /* Driver defined flag */
#define  IFM_FLAG2 0x04000000  /* Driver defined flag */
#define  IFM_LOOP  0x08000000  /* Put hardware in loopback */

/*
 * Masks
 */
#define  IFM_NMASK  0x000000e0  /* Network type */
#define  IFM_TMASK  0x0000001f  /* Media sub-type */
#define  IFM_IMASK  0xf0000000  /* Instance */
#define  IFM_ISHIFT 28    /* Instance shift */
#define  IFM_OMASK  0x0000ff00  /* Type specific options */
#define  IFM_MMASK  0x00070000  /* Mode */
#define  IFM_MSHIFT 16    /* Mode shift */
#define  IFM_GMASK  0x0ff00000  /* Global options */

/*
 * Status bits
 */
#define  IFM_AVALID  0x00000001  /* Active bit valid */
#define  IFM_ACTIVE  0x00000002  /* Interface attached to working net */

/* Mask of "status valid" bits, for ifconfig(8). */
#define  IFM_STATUS_VALID  IFM_AVALID

/* List of "status valid" bits, for ifconfig(8). */
#define IFM_STATUS_VALID_LIST {            \
  IFM_AVALID,              \
  0                \
  }

/*
 * Macros to extract various bits of information from the media word.
 */
#define  IFM_TYPE(x)         ((x) & IFM_NMASK)
#define  IFM_SUBTYPE(x)      ((x) & IFM_TMASK)
#define  IFM_TYPE_OPTIONS(x) ((x) & IFM_OMASK)
#define  IFM_INST(x)         (((x) & IFM_IMASK) >> IFM_ISHIFT)
#define  IFM_OPTIONS(x)      ((x) & (IFM_OMASK|IFM_GMASK))
#define  IFM_MODE(x)         ((x) & IFM_MMASK)

#define  IFM_INST_MAX  IFM_INST(IFM_IMASK)

/*
 * Macro to create a media word.
 */
#define  IFM_MAKEWORD(type, subtype, options, instance)      \
  ((type) | (subtype) | (options) | ((instance) << IFM_ISHIFT))
#define  IFM_MAKEMODE(mode) \
  (((mode) << IFM_MSHIFT) & IFM_MMASK)


#endif  /* _NET_IF_MEDIA_H_ */
