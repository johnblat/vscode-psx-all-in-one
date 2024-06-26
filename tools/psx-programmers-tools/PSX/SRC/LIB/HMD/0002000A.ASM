/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_0002000a		/* */

	

	.text


/************************************************************************
* Colored Flat Triangle with lighting and fog
*
*	a0: (GsARGUNIT *)sp
*
*	typedef struct {
*		u_long	*primp;		// 0(a0)
*		GsOT 	*tagp;		// 4(a0)
*		int	shift;		// 8(a0)
*		int	offset;		// 12(a0)
*		PACKET	*out_packetp;	// 16(a0)
*		u_long	*primtop;	// 20(a0)
*		SVECTOR	*vertop;	// 24(a0)
*		SVECTOR	*nortop;	// 28(a0)
*	}	GsARGUNIT;
*	num:				// (*primtop) >> 16
*************************************************************************/

GsU_0002000a:

	.set    noreorder

	lw	a3,0(a0)		/* sp->primp */
	lw	t9,4(a0)		/* sp->tagp */
	lw	t8,0(a3)		/* *(sp->primp) */
	lw	t7,8(a0)		/* sp->shift */
	addu	a3,a3,4			/* skip size,num */
	lw	a2,0(a3)		/* prim idx:*(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* t8: *(sp->primp)>>16=num */
	beq	t8,0,CF3L_3		/* if(n==0) goto CF3L_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	lw	t0,4(t9)		/* t0: otp->org */
	lw	t1,12(a0)		/* t1: sp->offset */
	addu	v0,a0,16		/* t9: pkt_base */
	lw	t9,0(v0)
	lw	t2,12(a3)		/* t2: primtop->v0,n0 */
	lw	t3,16(a3)		/* t3: primtop->v1,v2 */

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */
	srl	t4,t2,16		/* t4: v0 * 8 */
	sll	t4,t4,3
	addu	t4,t4,a1		/* t4: (vertop+primtop->v0) */

CF3L_1:
	lwc2	C2_VXY0,(t4)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t4)

	sll	t5,t3,16		/* t5: (v1 & 0x0000ffff) * 8 */
	srl	t5,t5,13
	addu	t5,t5,a1		/* t5: (vertop+primtop->v1) */
	lwc2	C2_VXY1,(t5)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t5)

	srl	t5,t3,16		/* t5: v2 * 8 */
	sll	t5,t5,3
	addu	t5,t5,a1		/* t5: (vertop+primtop->v2) */
	lwc2	C2_VXY2,(t5)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t5)
	nop
	sll	t5,t2,16		/* t5: (n0 & 0x0000ffff) * 8 */

	/****/
        RTPT				
	/****/

	srl	t5,t5,13		/* t5: (n0 & 0x0000ffff) * 8 [contd] */
	addu	t5,t5,a2		/* t5: (nortop+primtop->n0) */

	lw	t2,0(a3)		/* R0,G0,B0,cd <-- primtop->R0G0B0cd */
	lw	t3,4(a3)		/* R1,G1,B1,cd <-- primtop->R1G1B1cd */
	lw	t4,8(a3)		/* R2,G2,B2,cd <-- primtop->R2G2B2cd */
	mtc2	t2,C2_RGB		
	addu	a3,a3,20		/* primtop++ */
	lw	t2,12(a3)		/* v0: primtop->v0,n0 */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,CF3L_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/


	mfc2	v0,C2_MAC0		/* opz read */
	nop
	ble	v0,0,CF3L_2		/* Normal clip if MAC0(OPZ)<=0 */
	nop

	/****/
	AVSZ3				
	/****/

	lwc2	C2_VXY0,(t5)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t5)

	mfc2	t6,C2_OTZ		/* t6: otz */
	nop
	nop
	/****/
        NCDS				
	/****/
	
	subu 	t6,t6,t1		/* sz = sz(otz) - sp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - sp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(tagp->org[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */

	li	v0,0x6000000		/* (sizeof(POLY_G3)/4-1)<<24) */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_G3)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	nop
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

    	swc2    C2_SXY0,8(t9)		/* sxy0 -->8(pkt_base) */
	swc2	C2_RGB2,4(t9)		/* R2,G2,B2,cd -->4(pkt_base) */
	mtc2	t3,C2_RGB		
	nop
	nop
	/****/
        NCDS				
	/****/
	
    	swc2    C2_SXY1,16(t9)		/* sxy1 -->16(pkt_base) */
	swc2	C2_RGB2,12(t9)		/* R2,G2,B2,cd -->12(pkt_base) */
	mtc2	t4,C2_RGB		
	nop
	nop
	/****/
        NCDS				
	/****/
	
	swc2    C2_SXY2,24(t9) 		/* sxy2 -->24(pkt_base) */
	swc2	C2_RGB2,20(t9)		/* R2,G2,B2,cd -->20(pkt_base) */

	addu	t9,t9,28		/* pkt_base++; */

CF3L_2:

	lw	t3,16(a3)		/* v1: primtop->v1,v2 */

	srl	t4,t2,16		/* t4: v0 * 8 */
	sll	t4,t4,3
	addu	t4,t4,a1		/* t4: (vertop+primtop->v0) */

	subu	t8,t8,1
	bne	t8,0,CF3L_1		/* if(primtop != primtop+n) goto CF3L_1 */
	nop

CF3L_3:
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

	.set    reorder
    	j       ra			/* return(primtop) */
