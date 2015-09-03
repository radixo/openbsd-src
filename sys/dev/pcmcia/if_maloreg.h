/*	$OpenBSD: if_maloreg.h,v 1.14 2007/08/09 14:50:06 mglocker Exp $ */

/*
 * Copyright (c) 2007 Marcus Glocker <mglocker@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* registers */
#define MALO_REG_HOST_STATUS		0x00
#define MALO_REG_CARD_INTR_CAUSE	0x02
#define MALO_REG_HOST_INTR_MASK		0x04
#define MALO_REG_DATA_READ		0x10
#define MALO_REG_CMD_READ		0x12
#define MALO_REG_DATA_WRITE_LEN		0x14
#define MALO_REG_DATA_WRITE		0x16
#define MALO_REG_CMD_WRITE_LEN		0x18
#define MALO_REG_CMD_WRITE		0x1a
#define MALO_REG_CARD_STATUS		0x20
#define MALO_REG_HOST_INTR_CAUSE	0x22
#define MALO_REG_DATA_READ_LEN		0x24
#define MALO_REG_RBAL			0x28
#define MALO_REG_CMD_READ_LEN		0x30
#define MALO_REG_SCRATCH		0x3f
#define MALO_REG_CARD_INTR_MASK		0x44

/* register values */
#define MALO_VAL_SCRATCH_READY		0x00
#define MALO_VAL_TX_DL_OVER		0x01
#define MALO_VAL_RX_DL_OVER		0x02
#define MALO_VAL_CMD_DL_OVER		0x04
#define MALO_VAL_SCRATCH_FW_LOADED	0x5a
#define MALO_VAL_HOST_INTR_MASK_ON	0x001f
#define MALO_VAL_CARD_STATUS_MASK	0x7f00

/* interrupt reasons */
#define MALO_VAL_HOST_INTR_TX		(1 << 0)
#define MALO_VAL_HOST_INTR_RX		(1 << 1)
#define MALO_VAL_HOST_INTR_CMD		(1 << 3)
#define MALO_VAL_HOST_INTR_EVENT	(1 << 4)

/* FW commands */
#define MALO_CMD_RESP			0x8000
#define MALO_CMD_HWSPEC			0x0003
#define MALO_CMD_RESET			0x0005
#define MALO_CMD_SCAN			0x0006
#define MALO_CMD_AUTH			0x0011
#define MALO_CMD_WEP			0x0013
#define MALO_CMD_SNMP			0x0016
#define MALO_CMD_RADIO			0x001c
#define MALO_CMD_CHANNEL		0x001d
#define MALO_CMD_TXPOWER		0x001e
#define MALO_CMD_ANTENNA		0x0020
#define MALO_CMD_MACCTRL		0x0028
#define MALO_CMD_MACADDR		0x004d
#define MALO_CMD_ASSOC			0x0050
#define MALO_CMD_80211D			0x005b
#define MALO_CMD_BGSCAN_CONFIG		0x006b
#define MALO_CMD_BGSCAN_QUERY		0x006c
#define MALO_CMD_RATE			0x0076

/* FW command values */
#define MALO_CMD_RADIO_OFF		0x0000
#define MALO_CMD_RADIO_ON		0x0001
#define MALO_CMD_RADIO_LONG_P		0x0000
#define MALO_CMD_RADIO_SHORT_P		0x0002
#define MALO_CMD_RADIO_AUTO_P		0x0004
#define MALO_CMD_MACCTRL_RX_ON		0x0001
#define MALO_CMD_MACCTRL_TX_ON		0x0002
#define MALO_CMD_MACCTRL_PROMISC_ON	0x0080

/* events */
#define MALO_EVENT_DEAUTH		0x0008
#define MALO_EVENT_DISASSOC		0x0009
