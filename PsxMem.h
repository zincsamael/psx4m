/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PSXMEMORY_H__
#define __PSXMEMORY_H__

#if defined(__MACOSX__)

#error Byteswapping may not work!

#define _SWAP16(b) ((((unsigned char*)&(b))[0]&0xff) | (((unsigned char*)&(b))[1]&0xff)<<8)
#define _SWAP32(b) ((((unsigned char*)&(b))[0]&0xff) | ((((unsigned char*)&(b))[1]&0xff)<<8) | ((((unsigned char*)&(b))[2]&0xff)<<16) | (((unsigned char*)&(b))[3]<<24))

#define SWAP16(v) ((((v)&0xff00)>>8) +(((v)&0xff)<<8))
#define SWAP32(v) ((((v)&0xff000000ul)>>24) + (((v)&0xff0000ul)>>8) + (((v)&0xff00ul)<<8) +(((v)&0xfful)<<24))
#define SWAPu32(v) SWAP32((u32)(v))
#define SWAPs32(v) SWAP32((s32)(v))

#define SWAPu16(v) SWAP16((u16)(v))
#define SWAPs16(v) SWAP16((s16)(v))

#else

#define SWAP16(b) (b)
#define SWAP32(b) (b)

#define SWAPu16(b) (b)
#define SWAPu32(b) (b)

#endif

extern s8 *psxM;
#define psxMs8(mem)		psxM[(mem) & 0x1fffff]
#define psxMs16(mem)	(SWAP16(*(s16*)&psxM[(mem) & 0x1fffff]))
#define psxMs32(mem)	(SWAP32(*(s32*)&psxM[(mem) & 0x1fffff]))
#define psxMu8(mem)		(*(u8*)&psxM[(mem) & 0x1fffff]))
#define psxMu16(mem)	(SWAP16(*(u16*)&psxM[(mem) & 0x1fffff]))
#define psxMu32(mem)	(SWAP32(*(u32*)&psxM[(mem) & 0x1fffff]))

#define psxMs8ref(mem)	psxM[(mem) & 0x1fffff]
#define psxMs16ref(mem)	(*(s16*)&psxM[(mem) & 0x1fffff])
#define psxMs32ref(mem)	(*(s32*)&psxM[(mem) & 0x1fffff])
#define psxMu8ref(mem)	(*(u8*) &psxM[(mem) & 0x1fffff])
#define psxMu16ref(mem)	(*(u16*)&psxM[(mem) & 0x1fffff])
#define psxMu32ref(mem)	(*(u32*)&psxM[(mem) & 0x1fffff])

extern s8 *psxP;
#define psxPs8(mem)	    psxP[(mem) & 0xffff]
#define psxPs16(mem)	(SWAP16(*(s16*)&psxP[(mem) & 0xffff]))
#define psxPs32(mem)	(SWAP32(*(s32*)&psxP[(mem) & 0xffff]))
#define psxPu8(mem)		(*(u8*) &psxP[(mem) & 0xffff])
#define psxPu16(mem)	(SWAP16(*(u16*)&psxP[(mem) & 0xffff]))
#define psxPu32(mem)	(SWAP32(*(u32*)&psxP[(mem) & 0xffff]))

#define psxPs8ref(mem)	psxP[(mem) & 0xffff]
#define psxPs16ref(mem)	(*(s16*)&psxP[(mem) & 0xffff])
#define psxPs32ref(mem)	(*(s32*)&psxP[(mem) & 0xffff])
#define psxPu8ref(mem)	(*(u8*) &psxP[(mem) & 0xffff])
#define psxPu16ref(mem)	(*(u16*)&psxP[(mem) & 0xffff])
#define psxPu32ref(mem)	(*(u32*)&psxP[(mem) & 0xffff])

extern s8 *psxR;
#define psxRs8(mem)		psxR[(mem) & 0x7ffff]
#define psxRs16(mem)	(SWAP16(*(s16*)&psxR[(mem) & 0x7ffff]))
#define psxRs32(mem)	(SWAP32(*(s32*)&psxR[(mem) & 0x7ffff]))
#define psxRu8(mem)		(*(u8* )&psxR[(mem) & 0x7ffff])
#define psxRu16(mem)	(SWAP16(*(u16*)&psxR[(mem) & 0x7ffff]))
#define psxRu32(mem)	(SWAP32(*(u32*)&psxR[(mem) & 0x7ffff]))

#define psxRs8ref(mem)	psxR[(mem) & 0x7ffff]
#define psxRs16ref(mem)	(*(s16*)&psxR[(mem) & 0x7ffff])
#define psxRs32ref(mem)	(*(s32*)&psxR[(mem) & 0x7ffff])
#define psxRu8ref(mem)	(*(u8* )&psxR[(mem) & 0x7ffff])
#define psxRu16ref(mem)	(*(u16*)&psxR[(mem) & 0x7ffff])
#define psxRu32ref(mem)	(*(u32*)&psxR[(mem) & 0x7ffff])

extern s8 *psxH;
#define psxHs8(mem)		psxH[(mem) & 0xffff]
#define psxHs16(mem)	(SWAP16(*(s16*)&psxH[(mem) & 0xffff]))
#define psxHs32(mem)	(SWAP32(*(s32*)&psxH[(mem) & 0xffff]))
#define psxHu8(mem)		(*(u8*) &psxH[(mem) & 0xffff])
#define psxHu16(mem)	(SWAP16(*(u16*)&psxH[(mem) & 0xffff]))
#define psxHu32(mem)	(SWAP32(*(u32*)&psxH[(mem) & 0xffff]))

#define psxHs8ref(mem)	psxH[(mem) & 0xffff]
#define psxHs16ref(mem)	(*(s16*)&psxH[(mem) & 0xffff])
#define psxHs32ref(mem)	(*(s32*)&psxH[(mem) & 0xffff])
#define psxHu8ref(mem)	(*(u8*) &psxH[(mem) & 0xffff])
#define psxHu16ref(mem)	(*(u16*)&psxH[(mem) & 0xffff])
#define psxHu32ref(mem)	(*(u32*)&psxH[(mem) & 0xffff])

extern u32 *psxMemWLUT;
extern u32 *psxMemRLUT;

#define PSXM(mem)		(psxMemRLUT[(mem) >> 16] == 0 ? NULL : (void*)(psxMemRLUT[(mem) >> 16] + ((mem) & 0xffff)))
#define PSXMs8(mem)		(*(s8 *)PSXM(mem))
#define PSXMs16(mem)	(SWAP16(*(s16*)PSXM(mem)))
#define PSXMs32(mem)	(SWAP32(*(s32*)PSXM(mem)))
#define PSXMu8(mem)		(*(u8 *)PSXM(mem))
#define PSXMu16(mem)	(SWAP16(*(u16*)PSXM(mem)))
#define PSXMu32(mem)	(SWAP32(*(u32*)PSXM(mem)))

#define PSXM8(mem)	PSXM(mem)
#define PSXM16(mem)	((void*)((u32)PSXM(mem)))
#define PSXM32(mem)	((void*)((u32)PSXM(mem)))

#define PSXMu32ref(mem)	(*(u32*)PSXM(mem))

#ifdef DYNAREC
extern u32 psxRecLUT[0x010000];
#define PC_REC(x)	(psxRecLUT[(x) >> 16] + ((x) & 0xfffc))
#define PC_REC32(x) 	(*(u32*)PC_REC(x))
#define REC_CLEARM(mem) PC_REC32(mem) = 0;
#endif

int  psxMemInit();
void psxMemReset();
void psxMemShutdown();

extern s8 psxMemReadS8(u32 mem);
extern u8 psxMemRead8(u32 mem);
extern s16 psxMemReadS16(u32 mem);
extern u16 psxMemRead16(u32 mem);
extern u32 psxMemRead32(u32 mem);
extern void psxMemWrite8(u32 mem, u32 value);
extern void psxMemWrite16(u32 mem, u32 value);
extern void psxMemWrite32(u32 mem, u32 value);
void *psxMemPointer(u32 mem);

#endif /* __PSXMEMORY_H__ */

