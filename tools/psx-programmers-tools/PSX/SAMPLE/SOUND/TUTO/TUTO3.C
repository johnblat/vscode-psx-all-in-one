/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto3.c,v $
 *
 * Copyright (C) 1994 by Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/
/*
 * $PSLibId: Run-time Library Release 4.4$
 */

#ifndef lint
static char rcsid [] = "$Id: tuto3.c,v 1.10 1997/08/29 07:35:05 tetsu Exp $ : \
	Copyright (C) by 1994 Sony Computer Entertainment Inc.";
#endif

#include <r3000.h>
#include <asm.h>
#include <libapi.h>

#include <sys/types.h>
#include <libetc.h>
#include <libspu.h>
#ifdef __psx__
#include <libsn.h>
#else
#define pollhost()
#endif

/* #define DEBUG /**/

#ifdef DEBUG
#define PRINTF(x) printf x
#else
#define PRINTF(x)
#endif

int quitF;

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

#define SIN_DATA_SIZE (0x10 * 10)
unsigned char sin_wave [] = {
#include "sin.h"
};
#define PF_DATA_SIZE 18704
unsigned char pf_wave [] = {
#include "pf_wave.h"
};

SpuIRQCallbackProc spu_intr (void);

void padHandle (unsigned long);

#define MALLOC_MAX 2
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

main (void)
{
    unsigned long s_addr, p_addr, i_addr;
    unsigned long size;
    SpuVoiceAttr s_attr;
    SpuCommonAttr c_attr;

    long top;

    /*		割り込み環境の初期化
     * ---------------------------------------------------------------- */

    ResetCallback();

    /*		グラフィックスの初期化
     * ---------------------------------------------------------------- */

    ResetGraph (0);

    /*		SPU の初期化
     * ---------------------------------------------------------------- */

    SpuInit ();
    SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);

    /*		コントロールパッド の初期化
     * ---------------------------------------------------------------- */

    PadInit (0);

    /*		共通属性設定
     * ---------------------------------------------------------------- */

    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR);

    c_attr.mvol.left  = 0x3fff;
    c_attr.mvol.right = 0x3fff;

    SpuSetCommonAttr (&c_attr);

    /*		波形転送
     * ---------------------------------------------------------------- */

    /* 転送モード */
    SpuSetTransferMode (SpuTransByDMA);

    /*		サイン波の転送 */

    s_addr = SpuMalloc (SIN_DATA_SIZE);

    /* 転送先頭アドレス */
    top = SpuSetTransferStartAddr (s_addr);
    PRINTF (("Set start addr    : %08x\n", s_addr));
    PRINTF (("Return start addr : %08x\n", top));
    PRINTF (("Get start addr    : %08x\n", SpuGetTransferStartAddr ()));
    
    /* 転送 */
    size = SpuWrite (sin_wave, SIN_DATA_SIZE);
    SpuIsTransferCompleted (SPU_TRANSFER_WAIT);

    PRINTF (("Send size   : %08x\n", SIN_DATA_SIZE));
    PRINTF (("Return size : %08x\n", size));

    /*		ピアノ音の転送 */

    p_addr = SpuMalloc (PF_DATA_SIZE);

    /* 転送先頭アドレス */
    top = SpuSetTransferStartAddr (p_addr);
    PRINTF (("Set start addr    : %08x\n", p_addr));
    PRINTF (("Return start addr : %08x\n", top));
    PRINTF (("Get start addr    : %08x\n", SpuGetTransferStartAddr ()));
    
    /* 転送 */
    size = SpuWrite (pf_wave, PF_DATA_SIZE);
    SpuIsTransferCompleted (SPU_TRANSFER_WAIT);

    /*		割り込み callback 関数の設定
     * ---------------------------------------------------------------- */

    i_addr = SpuSetIRQAddr (p_addr + (PF_DATA_SIZE / 2));
    SpuSetIRQCallback ((SpuIRQCallbackProc) spu_intr);

    PRINTF (("Set IRQ addr    : %08x\n", p_addr + (PF_DATA_SIZE / 2)));
    PRINTF (("Return IRQ addr : %08x\n", i_addr));
    PRINTF (("Get IRQ addr    : %08x\n", SpuGetIRQAddr ()));

    /*		ボイス属性設定
     * ---------------------------------------------------------------- */

    /* ボイス属性 */
    s_attr.mask = (SPU_VOICE_VOLL |
		   SPU_VOICE_VOLR |
		   SPU_VOICE_PITCH |
		   SPU_VOICE_WDSA |
		   SPU_VOICE_ADSR_AMODE |
		   SPU_VOICE_ADSR_SMODE |
		   SPU_VOICE_ADSR_RMODE |
		   SPU_VOICE_ADSR_AR |
		   SPU_VOICE_ADSR_DR |
		   SPU_VOICE_ADSR_SR |
		   SPU_VOICE_ADSR_RR |
		   SPU_VOICE_ADSR_SL
		   );

    /* ピアノ音 */
    s_attr.voice = (SPU_0CH);

    s_attr.volume.left  = 0x1fff;
    s_attr.volume.right = 0x1fff;
    s_attr.pitch        = 0x1000;
    s_attr.addr         = p_addr;
    s_attr.a_mode       = SPU_VOICE_LINEARIncN;
    s_attr.s_mode       = SPU_VOICE_LINEARIncN;
    s_attr.r_mode       = SPU_VOICE_LINEARDecN;
    s_attr.ar           = 0x0;
    s_attr.dr           = 0x0;
    s_attr.sr           = 0x0;
    s_attr.rr           = 0x0;
    s_attr.sl           = 0xf;

    PRINTF (("set voice attr (piano):\n"));
    SpuSetVoiceAttr (&s_attr);

    /* サイン波 */
    s_attr.voice = (SPU_1CH);

    s_attr.volume.left  = 0x1fff;
    s_attr.volume.right = 0x1fff;
    s_attr.pitch        = 0x2000;
    s_attr.addr         = s_addr;

    PRINTF (("set voice attr (sin):\n"));
    SpuSetVoiceAttr (&s_attr);

    /*		発音
     * ---------------------------------------------------------------- */

    quitF = False;
    while (! quitF) {
	VSync (0);
#if 0
	pollhost ();
#endif
	padHandle (PadRead (0));
    }

    /*		終了処理
     * ---------------------------------------------------------------- */

    SpuSetIRQ (SPU_OFF);

    SpuSetKey (SPU_OFF, SPU_ALLCH);
    SpuQuit ();

    PadStop ();		/* pad stop */

    ResetGraph (3);
    StopCallback ();

    return;
}

SpuIRQCallbackProc
spu_intr (void)
{
    SpuSetIRQ (SpuOff); /* 割り込み禁止 */
    SpuSetKey (SpuOn, (SPU_1CH));

    PRINTF (("."));
}

void
padHandle (unsigned long padd)
{
    static int key_q = False;
    static int key_h = False;
    static int key_k = False;

    /* 終了 */
    if ((padd & PADk) &&
	(padd & PADh)) {
	if ((key_q == False)) {
	    key_q = True;
	}
    } else {
	if (key_q == True) {
	    key_q = False;
	    quitF = True;
	}
    }

    if (padd & PADh) {
	if ((key_h == False)) {
	    SpuSetIRQ (SpuOn);	/* 割り込み許可 */
	    SpuSetKey (SpuOn, (SPU_0CH));
	    key_h = True;
	}
    } else {
	if (key_h == True) {
	    key_h = False;
	}
    }

    if (padd & PADk) {
	if ((key_k == False)) {
	    SpuSetIRQ (SpuOff); /* 割り込み禁止 */
	    SpuSetKey (SpuOff, (SPU_0CH | SPU_1CH));
	    key_k = True;
	}
    } else {
	if (key_k == True) {
	    key_k = False;
	}
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
