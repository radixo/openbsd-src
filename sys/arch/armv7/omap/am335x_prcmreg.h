/* $OpenBSD: am335x_prcmreg.h,v 1.3 2013/10/24 19:39:46 syl Exp $ */
/*
 * Copyright (c) 2013 Raphael Graf <r@undefined.ch>
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

#define AM335X_CLKCTRL_MODULEMODE_ENABLE	2
#define AM335X_CLKCTRL_MODULEMODE_DISABLE	0
#define AM335X_CLKCTRL_MODULEMODE_MASK		0x00000003

#define PRCM_AM335X_CM_PER		0x0000
#define PRCM_AM335X_USB0_CLKCTRL	0x001c
#define PRCM_AM335X_TPTC0_CLKCTRL	0x0024
#define PRCM_AM335X_MMC0_CLKCTRL	0x003c
#define PRCM_AM335X_I2C2_CLKCTRL	0x0044
#define PRCM_AM335X_I2C1_CLKCTRL	0x0048
#define PRCM_AM335X_TIMER2_CLKCTRL	0x0080
#define PRCM_AM335X_TIMER3_CLKCTRL	0x0084
#define PRCM_AM335X_GPIO1_CLKCTRL	0x00ac
#define PRCM_AM335X_GPIO2_CLKCTRL	0x00b0
#define PRCM_AM335X_GPIO3_CLKCTRL	0x00b4
#define PRCM_AM335X_TPCC_CLKCTRL	0x00bc
#define PRCM_AM335X_MMC1_CLKCTRL	0x00f4
#define PRCM_AM335X_MMC2_CLKCTRL	0x00f8
#define PRCM_AM335X_TPTC1_CLKCTRL	0x00fc
#define PRCM_AM335X_TPTC2_CLKCTRL	0x0100
#define PRCM_AM335X_CM_WKUP		0x0400
#define PRCM_AM335X_GPIO0_CLKCTRL	0x0408
#define PRCM_AM335X_TIMER0_CLKCTRL	0x0410
#define PRCM_AM335X_I2C0_CLKCTRL	0x04b8
#define PRCM_AM335X_CM_DPLL		0x0500
#define PRCM_AM335X_CLKSEL_TIMER2_CLK	0x0508
#define PRCM_AM335X_CLKSEL_TIMER3_CLK	0x050c
#define PRCM_AM335X_CM_MPU		0x0600
#define PRCM_AM335X_CM_DEVICE		0x0700
#define PRCM_AM335X_CM_RTC		0x0800
#define PRCM_AM335X_CM_GFX		0x0900
#define PRCM_AM335X_CM_CEFUSE		0x0a00
#define PRCM_AM335X_PRM_IRQ		0x0b00
#define PRCM_AM335X_PRM_PER		0x0c00
#define PRCM_AM335X_PRM_WKUP		0x0d00
#define PRCM_AM335X_PRM_MPU		0x0e00
#define PRCM_AM335X_PRM_DEVICE		0x0f00
#define PRCM_AM335X_PRM_RTC		0x1000
#define PRCM_AM335X_PRM_GFX		0x1100
#define PRCM_AM335X_PRM_CEFUSE		0x1200
