#ifndef DEFINES_H
#define DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#define REGSIZE __riscv_xlen/8

#if __riscv_xlen == 64
#define LREG ld
#define SREG sd
#else
#define LREG lw
#define SREG sw
#endif

#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000

#ifdef __cplusplus
}
#endif

#endif