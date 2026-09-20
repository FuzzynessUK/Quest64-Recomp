#include "recomp.h"
#include "funcs.h"

RECOMP_FUNC void hm_func_80410C7C(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80410C7C: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80410C80: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x80410C84: lw          $a2, 0x4EE4($v0)
    ctx->r6 = MEM_W(ctx->r2, 0X4EE4);
    // 0x80410C88: addiu       $a1, $a1, 0xDB0
    ctx->r5 = ADD32(ctx->r5, 0XDB0);
    // 0x80410C8C: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80410C90: lw          $t0, 0x4EE8($v0)
    ctx->r8 = MEM_W(ctx->r2, 0X4EE8);
    // 0x80410C94: or          $v1, $zero, $zero
    ctx->r3 = 0 | 0;
    // 0x80410C98: or          $v0, $a1, $zero
    ctx->r2 = ctx->r5 | 0;
    // 0x80410C9C: addiu       $a3, $zero, 0x3F
    ctx->r7 = ADD32(0, 0X3F);
L_80410CA0:
    // 0x80410CA0: lhu         $a0, 0x0($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X0);
    // 0x80410CA4: bnel        $a0, $a2, L_80410D9C
    if (ctx->r4 != ctx->r6) {
        // 0x80410CA8: addiu       $v1, $v1, 0x1
        ctx->r3 = ADD32(ctx->r3, 0X1);
            goto L_80410D9C;
    }
    goto skip_0;
    // 0x80410CA8: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    skip_0:
    // 0x80410CAC: lhu         $a0, 0x2($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X2);
    // 0x80410CB0: bnel        $a0, $t0, L_80410D9C
    if (ctx->r4 != ctx->r8) {
        // 0x80410CB4: addiu       $v1, $v1, 0x1
        ctx->r3 = ADD32(ctx->r3, 0X1);
            goto L_80410D9C;
    }
    goto skip_1;
    // 0x80410CB4: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    skip_1:
    // 0x80410CB8: sll         $v0, $v1, 1
    ctx->r2 = S32(ctx->r3 << 1);
    // 0x80410CBC: addu        $v1, $v0, $v1
    ctx->r3 = ADD32(ctx->r2, ctx->r3);
    // 0x80410CC0: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x80410CC4: addu        $a1, $a1, $v1
    ctx->r5 = ADD32(ctx->r5, ctx->r3);
    // 0x80410CC8: lhu         $v0, 0x4($a1)
    ctx->r2 = MEM_HU(ctx->r5, 0X4);
    // 0x80410CCC: addiu       $sp, $sp, -0x38
    ctx->r29 = ADD32(ctx->r29, -0X38);
    // 0x80410CD0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80410CD4: sw          $v0, 0x6A00($v1)
    MEM_W(0X6A00, ctx->r3) = ctx->r2;
    // 0x80410CD8: sw          $s1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r17;
    // 0x80410CDC: sw          $ra, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r31;
    // 0x80410CE0: lw          $s1, 0x8($a1)
    ctx->r17 = MEM_W(ctx->r5, 0X8);
    // 0x80410CE4: sw          $s5, 0x30($sp)
    MEM_W(0X30, ctx->r29) = ctx->r21;
    // 0x80410CE8: sw          $s4, 0x2C($sp)
    MEM_W(0X2C, ctx->r29) = ctx->r20;
    // 0x80410CEC: sw          $s3, 0x28($sp)
    MEM_W(0X28, ctx->r29) = ctx->r19;
    // 0x80410CF0: sw          $s2, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r18;
    // 0x80410CF4: beq         $v0, $zero, L_80410D78
    if (ctx->r2 == 0) {
        // 0x80410CF8: sw          $s0, 0x1C($sp)
        MEM_W(0X1C, ctx->r29) = ctx->r16;
            goto L_80410D78;
    }
    // 0x80410CF8: sw          $s0, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r16;
    // 0x80410CFC: sll         $s3, $v0, 1
    ctx->r19 = S32(ctx->r2 << 1);
    // 0x80410D00: addu        $s3, $s3, $v0
    ctx->r19 = ADD32(ctx->r19, ctx->r2);
    // 0x80410D04: lui         $s0, 0x8008
    ctx->r16 = S32(0X8008 << 16);
    // 0x80410D08: addiu       $s0, $s0, 0x6A08
    ctx->r16 = ADD32(ctx->r16, 0X6A08);
    // 0x80410D0C: sll         $s3, $s3, 3
    ctx->r19 = S32(ctx->r19 << 3);
    // 0x80410D10: lui         $s2, 0x8008
    ctx->r18 = S32(0X8008 << 16);
    // 0x80410D14: addu        $s3, $s0, $s3
    ctx->r19 = ADD32(ctx->r16, ctx->r19);
    // 0x80410D18: addiu       $s2, $s2, 0x6AC8
    ctx->r18 = ADD32(ctx->r18, 0X6AC8);
    // 0x80410D1C: addiu       $s5, $zero, -0x1
    ctx->r21 = ADD32(0, -0X1);
    // 0x80410D20: lui         $s4, 0x8000
    ctx->r20 = S32(0X8000 << 16);
L_80410D24:
    // 0x80410D24: sh          $s5, 0x12($s0)
    MEM_H(0X12, ctx->r16) = ctx->r21;
    // 0x80410D28: lbu         $a0, 0x8($s1)
    ctx->r4 = MEM_BU(ctx->r17, 0X8);
    // 0x80410D2C: jal         0x80012700
    // 0x80410D30: addiu       $s0, $s0, 0x18
    ctx->r16 = ADD32(ctx->r16, 0X18);
    func_80012700(rdram, ctx);
        goto after_0;
    // 0x80410D30: addiu       $s0, $s0, 0x18
    ctx->r16 = ADD32(ctx->r16, 0X18);
    after_0:
    // 0x80410D34: lwc1        $f12, 0x0($s1)
    ctx->f12.u32l = MEM_W(ctx->r17, 0X0);
    // 0x80410D38: mtc1        $zero, $f14
    ctx->f14.u32l = 0;
    // 0x80410D3C: swc1        $f12, -0x18($s0)
    MEM_W(-0X18, ctx->r16) = ctx->f12.u32l;
    // 0x80410D40: lw          $a2, 0x4($s1)
    ctx->r6 = MEM_W(ctx->r17, 0X4);
    // 0x80410D44: addiu       $a3, $zero, 0xA00
    ctx->r7 = ADD32(0, 0XA00);
    // 0x80410D48: xor         $a2, $a2, $s4
    ctx->r6 = ctx->r6 ^ ctx->r20;
    // 0x80410D4C: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80410D50: sh          $v0, -0x8($s0)
    MEM_H(-0X8, ctx->r16) = ctx->r2;
    // 0x80410D54: jal         0x8000EE60
    // 0x80410D58: sw          $a2, -0x10($s0)
    MEM_W(-0X10, ctx->r16) = ctx->r6;
    func_8000EE60(rdram, ctx);
        goto after_1;
    // 0x80410D58: sw          $a2, -0x10($s0)
    MEM_W(-0X10, ctx->r16) = ctx->r6;
    after_1:
    // 0x80410D5C: lwc1        $f0, 0x4($s2)
    ctx->f0.u32l = MEM_W(ctx->r18, 0X4);
    // 0x80410D60: sw          $zero, -0xC($s0)
    MEM_W(-0XC, ctx->r16) = 0;
    // 0x80410D64: swc1        $f0, -0x14($s0)
    MEM_W(-0X14, ctx->r16) = ctx->f0.u32l;
    // 0x80410D68: lbu         $v0, 0x8($s1)
    ctx->r2 = MEM_BU(ctx->r17, 0X8);
    // 0x80410D6C: addiu       $s1, $s1, 0xC
    ctx->r17 = ADD32(ctx->r17, 0XC);
    // 0x80410D70: bne         $s3, $s0, L_80410D24
    if (ctx->r19 != ctx->r16) {
        // 0x80410D74: sb          $v0, -0x4($s0)
        MEM_B(-0X4, ctx->r16) = ctx->r2;
            goto L_80410D24;
    }
    // 0x80410D74: sb          $v0, -0x4($s0)
    MEM_B(-0X4, ctx->r16) = ctx->r2;
L_80410D78:
    // 0x80410D78: lw          $ra, 0x34($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X34);
    // 0x80410D7C: lw          $s5, 0x30($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X30);
    // 0x80410D80: lw          $s4, 0x2C($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X2C);
    // 0x80410D84: lw          $s3, 0x28($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X28);
    // 0x80410D88: lw          $s2, 0x24($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X24);
    // 0x80410D8C: lw          $s1, 0x20($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X20);
    // 0x80410D90: lw          $s0, 0x1C($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X1C);
    // 0x80410D94: jr          $ra
    // 0x80410D98: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
    return;
    // 0x80410D98: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
L_80410D9C:
    // 0x80410D9C: bne         $v1, $a3, L_80410CA0
    if (ctx->r3 != ctx->r7) {
        // 0x80410DA0: addiu       $v0, $v0, 0xC
        ctx->r2 = ADD32(ctx->r2, 0XC);
            goto L_80410CA0;
    }
    // 0x80410DA0: addiu       $v0, $v0, 0xC
    ctx->r2 = ADD32(ctx->r2, 0XC);
    // 0x80410DA4: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80410DA8: jr          $ra
    // 0x80410DAC: sw          $zero, 0x6A00($v0)
    MEM_W(0X6A00, ctx->r2) = 0;
    return;
    // 0x80410DAC: sw          $zero, 0x6A00($v0)
    MEM_W(0X6A00, ctx->r2) = 0;
;}
RECOMP_FUNC void hm_func_80413F7C(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80413F7C: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80413F80: jr          $ra
    // 0x80413F84: ori         $v0, $v0, 0xCF78
    ctx->r2 = ctx->r2 | 0XCF78;
    return;
    // 0x80413F84: ori         $v0, $v0, 0xCF78
    ctx->r2 = ctx->r2 | 0XCF78;
;}
RECOMP_FUNC void hm_func_80413F88(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80413F88: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80413F8C: jr          $ra
    // 0x80413F90: ori         $v0, $v0, 0xCFC3
    ctx->r2 = ctx->r2 | 0XCFC3;
    return;
    // 0x80413F90: ori         $v0, $v0, 0xCFC3
    ctx->r2 = ctx->r2 | 0XCFC3;
;}
RECOMP_FUNC void hm_func_80413F94(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80413F94: lui         $a3, 0x803A
    ctx->r7 = S32(0X803A << 16);
    // 0x80413F98: ori         $a3, $a3, 0x93D3
    ctx->r7 = ctx->r7 | 0X93D3;
    // 0x80413F9C: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80413FA0: lui         $t3, 0x8042
    ctx->r11 = S32(0X8042 << 16);
    // 0x80413FA4: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x80413FA8: lui         $t1, 0xCCCC
    ctx->r9 = S32(0XCCCC << 16);
    // 0x80413FAC: addiu       $a0, $a0, -0x5B80
    ctx->r4 = ADD32(ctx->r4, -0X5B80);
    // 0x80413FB0: addiu       $t3, $t3, -0x5AB4
    ctx->r11 = ADD32(ctx->r11, -0X5AB4);
    // 0x80413FB4: or          $v0, $zero, $zero
    ctx->r2 = 0 | 0;
    // 0x80413FB8: addiu       $t0, $a3, -0x1
    ctx->r8 = ADD32(ctx->r7, -0X1);
    // 0x80413FBC: ori         $t2, $t2, 0xCFC3
    ctx->r10 = ctx->r10 | 0XCFC3;
    // 0x80413FC0: b           L_80413FD4
    // 0x80413FC4: ori         $t1, $t1, 0xCCCD
    ctx->r9 = ctx->r9 | 0XCCCD;
        goto L_80413FD4;
    // 0x80413FC4: ori         $t1, $t1, 0xCCCD
    ctx->r9 = ctx->r9 | 0XCCCD;
L_80413FC8:
    // 0x80413FC8: lw          $a3, 0x8($a0)
    ctx->r7 = MEM_W(ctx->r4, 0X8);
    // 0x80413FCC: lbu         $v0, 0x0($a0)
    ctx->r2 = MEM_BU(ctx->r4, 0X0);
    // 0x80413FD0: addiu       $a0, $a0, 0xC
    ctx->r4 = ADD32(ctx->r4, 0XC);
L_80413FD4:
    // 0x80413FD4: addu        $v0, $v0, $t2
    ctx->r2 = ADD32(ctx->r2, ctx->r10);
    // 0x80413FD8: lbu         $v0, 0x0($v0)
    ctx->r2 = MEM_BU(ctx->r2, 0X0);
    // 0x80413FDC: andi        $v0, $v0, 0x7F
    ctx->r2 = ctx->r2 & 0X7F;
    // 0x80413FE0: addiu       $v0, $v0, 0x1
    ctx->r2 = ADD32(ctx->r2, 0X1);
    // 0x80413FE4: multu       $v0, $t1
    result = U64(U32(ctx->r2)) * U64(U32(ctx->r9)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80413FE8: mfhi        $a1
    ctx->r5 = hi;
    // 0x80413FEC: srl         $a2, $a1, 3
    ctx->r6 = S32(U32(ctx->r5) >> 3);
    // 0x80413FF0: sll         $v1, $a2, 2
    ctx->r3 = S32(ctx->r6 << 2);
    // 0x80413FF4: addu        $v1, $v1, $a2
    ctx->r3 = ADD32(ctx->r3, ctx->r6);
    // 0x80413FF8: sll         $v1, $v1, 1
    ctx->r3 = S32(ctx->r3 << 1);
    // 0x80413FFC: subu        $v0, $v0, $v1
    ctx->r2 = SUB32(ctx->r2, ctx->r3);
    // 0x80414000: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
    // 0x80414004: sb          $a2, 0x0($t0)
    MEM_B(0X0, ctx->r8) = ctx->r6;
    // 0x80414008: sb          $v0, 0x0($a3)
    MEM_B(0X0, ctx->r7) = ctx->r2;
    // 0x8041400C: bnel        $a0, $t3, L_80413FC8
    if (ctx->r4 != ctx->r11) {
        // 0x80414010: lw          $t0, 0x4($a0)
        ctx->r8 = MEM_W(ctx->r4, 0X4);
            goto L_80413FC8;
    }
    goto skip_0;
    // 0x80414010: lw          $t0, 0x4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X4);
    skip_0:
    // 0x80414014: jr          $ra
    // 0x80414018: nop

    return;
    // 0x80414018: nop

;}
RECOMP_FUNC void hm_func_8041401C(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041401C: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80414020: lhu         $a0, 0x69F4($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X69F4);
    // 0x80414024: lui         $v1, 0xCCCC
    ctx->r3 = S32(0XCCCC << 16);
    // 0x80414028: andi        $a0, $a0, 0xFFFF
    ctx->r4 = ctx->r4 & 0XFFFF;
    // 0x8041402C: ori         $v1, $v1, 0xCCCD
    ctx->r3 = ctx->r3 | 0XCCCD;
    // 0x80414030: multu       $a0, $v1
    result = U64(U32(ctx->r4)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414034: lui         $a2, 0x1062
    ctx->r6 = S32(0X1062 << 16);
    // 0x80414038: addiu       $a2, $a2, 0x4DD3
    ctx->r6 = ADD32(ctx->r6, 0X4DD3);
    // 0x8041403C: lui         $v0, 0x51EB
    ctx->r2 = S32(0X51EB << 16);
    // 0x80414040: ori         $v0, $v0, 0x851F
    ctx->r2 = ctx->r2 | 0X851F;
    // 0x80414044: mfhi        $a1
    ctx->r5 = hi;
    // 0x80414048: srl         $t2, $a1, 3
    ctx->r10 = S32(U32(ctx->r5) >> 3);
    // 0x8041404C: sll         $a1, $t2, 2
    ctx->r5 = S32(ctx->r10 << 2);
    // 0x80414050: multu       $a0, $a2
    result = U64(U32(ctx->r4)) * U64(U32(ctx->r6)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414054: addu        $a1, $a1, $t2
    ctx->r5 = ADD32(ctx->r5, ctx->r10);
    // 0x80414058: sll         $a1, $a1, 1
    ctx->r5 = S32(ctx->r5 << 1);
    // 0x8041405C: subu        $a1, $a0, $a1
    ctx->r5 = SUB32(ctx->r4, ctx->r5);
    // 0x80414060: lui         $a3, 0x800C
    ctx->r7 = S32(0X800C << 16);
    // 0x80414064: mfhi        $a2
    ctx->r6 = hi;
    // 0x80414068: srl         $t3, $a2, 6
    ctx->r11 = S32(U32(ctx->r6) >> 6);
    // 0x8041406C: andi        $a1, $a1, 0xFF
    ctx->r5 = ctx->r5 & 0XFF;
    // 0x80414070: multu       $a0, $v0
    result = U64(U32(ctx->r4)) * U64(U32(ctx->r2)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414074: mfhi        $v0
    ctx->r2 = hi;
    // 0x80414078: srl         $v0, $v0, 5
    ctx->r2 = S32(U32(ctx->r2) >> 5);
    // 0x8041407C: nop

    // 0x80414080: multu       $t3, $v1
    result = U64(U32(ctx->r11)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414084: mfhi        $a2
    ctx->r6 = hi;
    // 0x80414088: srl         $a2, $a2, 3
    ctx->r6 = S32(U32(ctx->r6) >> 3);
    // 0x8041408C: sll         $t0, $a2, 2
    ctx->r8 = S32(ctx->r6 << 2);
    // 0x80414090: multu       $v0, $v1
    result = U64(U32(ctx->r2)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414094: addu        $a2, $t0, $a2
    ctx->r6 = ADD32(ctx->r8, ctx->r6);
    // 0x80414098: sll         $a2, $a2, 1
    ctx->r6 = S32(ctx->r6 << 1);
    // 0x8041409C: subu        $a2, $t3, $a2
    ctx->r6 = SUB32(ctx->r11, ctx->r6);
    // 0x804140A0: andi        $a2, $a2, 0xFF
    ctx->r6 = ctx->r6 & 0XFF;
    // 0x804140A4: mfhi        $t1
    ctx->r9 = hi;
    // 0x804140A8: srl         $t0, $t1, 3
    ctx->r8 = S32(U32(ctx->r9) >> 3);
    // 0x804140AC: sll         $t1, $t0, 2
    ctx->r9 = S32(ctx->r8 << 2);
    // 0x804140B0: multu       $t2, $v1
    result = U64(U32(ctx->r10)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x804140B4: addu        $t1, $t1, $t0
    ctx->r9 = ADD32(ctx->r9, ctx->r8);
    // 0x804140B8: lui         $t0, 0xD1B7
    ctx->r8 = S32(0XD1B7 << 16);
    // 0x804140BC: addiu       $t0, $t0, 0x1759
    ctx->r8 = ADD32(ctx->r8, 0X1759);
    // 0x804140C0: sll         $t1, $t1, 1
    ctx->r9 = S32(ctx->r9 << 1);
    // 0x804140C4: mfhi        $v1
    ctx->r3 = hi;
    // 0x804140C8: srl         $v1, $v1, 3
    ctx->r3 = S32(U32(ctx->r3) >> 3);
    // 0x804140CC: subu        $v0, $v0, $t1
    ctx->r2 = SUB32(ctx->r2, ctx->r9);
    // 0x804140D0: multu       $a0, $t0
    result = U64(U32(ctx->r4)) * U64(U32(ctx->r8)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x804140D4: sll         $t0, $v1, 2
    ctx->r8 = S32(ctx->r3 << 2);
    // 0x804140D8: addu        $v1, $t0, $v1
    ctx->r3 = ADD32(ctx->r8, ctx->r3);
    // 0x804140DC: sll         $v1, $v1, 1
    ctx->r3 = S32(ctx->r3 << 1);
    // 0x804140E0: subu        $v1, $t2, $v1
    ctx->r3 = SUB32(ctx->r10, ctx->r3);
    // 0x804140E4: mfhi        $a0
    ctx->r4 = hi;
    // 0x804140E8: srl         $a0, $a0, 13
    ctx->r4 = S32(U32(ctx->r4) >> 13);
    // 0x804140EC: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
    // 0x804140F0: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x804140F4: sb          $a0, 0x3B2($a3)
    MEM_B(0X3B2, ctx->r7) = ctx->r4;
    // 0x804140F8: sb          $a2, 0x3B3($a3)
    MEM_B(0X3B3, ctx->r7) = ctx->r6;
    // 0x804140FC: sb          $v0, 0x3B4($a3)
    MEM_B(0X3B4, ctx->r7) = ctx->r2;
    // 0x80414100: sb          $v1, 0x3B5($a3)
    MEM_B(0X3B5, ctx->r7) = ctx->r3;
    // 0x80414104: sb          $a1, 0x3B6($a3)
    MEM_B(0X3B6, ctx->r7) = ctx->r5;
    // 0x80414108: jr          $ra
    // 0x8041410C: nop

    return;
    // 0x8041410C: nop

;}
RECOMP_FUNC void hm_func_80414110(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414110: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80414114: ori         $v0, $v0, 0xCFD1
    ctx->r2 = ctx->r2 | 0XCFD1;
    // 0x80414118: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x8041411C: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x80414120: bnel        $a0, $v1, L_80414134
    if (ctx->r4 != ctx->r3) {
        // 0x80414124: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80414134;
    }
    goto skip_0;
    // 0x80414124: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_0:
    // 0x80414128: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x8041412C: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x80414130: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_80414134:
    // 0x80414134: ori         $v0, $v0, 0xCFD2
    ctx->r2 = ctx->r2 | 0XCFD2;
    // 0x80414138: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x8041413C: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x80414140: bnel        $a0, $v1, L_80414154
    if (ctx->r4 != ctx->r3) {
        // 0x80414144: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80414154;
    }
    goto skip_1;
    // 0x80414144: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_1:
    // 0x80414148: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x8041414C: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x80414150: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_80414154:
    // 0x80414154: ori         $v0, $v0, 0xCFD3
    ctx->r2 = ctx->r2 | 0XCFD3;
    // 0x80414158: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x8041415C: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x80414160: bnel        $a0, $v1, L_80414174
    if (ctx->r4 != ctx->r3) {
        // 0x80414164: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80414174;
    }
    goto skip_2;
    // 0x80414164: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_2:
    // 0x80414168: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x8041416C: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x80414170: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_80414174:
    // 0x80414174: ori         $v0, $v0, 0xCFD4
    ctx->r2 = ctx->r2 | 0XCFD4;
    // 0x80414178: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x8041417C: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x80414180: bnel        $a0, $v1, L_80414194
    if (ctx->r4 != ctx->r3) {
        // 0x80414184: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80414194;
    }
    goto skip_3;
    // 0x80414184: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_3:
    // 0x80414188: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x8041418C: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x80414190: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_80414194:
    // 0x80414194: ori         $v0, $v0, 0xCFD5
    ctx->r2 = ctx->r2 | 0XCFD5;
    // 0x80414198: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x8041419C: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x804141A0: bnel        $a0, $v1, L_804141B4
    if (ctx->r4 != ctx->r3) {
        // 0x804141A4: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_804141B4;
    }
    goto skip_4;
    // 0x804141A4: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_4:
    // 0x804141A8: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x804141AC: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x804141B0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_804141B4:
    // 0x804141B4: ori         $v0, $v0, 0xCFD6
    ctx->r2 = ctx->r2 | 0XCFD6;
    // 0x804141B8: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x804141BC: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x804141C0: bnel        $a0, $v1, L_804141D4
    if (ctx->r4 != ctx->r3) {
        // 0x804141C4: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_804141D4;
    }
    goto skip_5;
    // 0x804141C4: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_5:
    // 0x804141C8: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x804141CC: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
    // 0x804141D0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_804141D4:
    // 0x804141D4: ori         $v0, $v0, 0xCFE1
    ctx->r2 = ctx->r2 | 0XCFE1;
    // 0x804141D8: lbu         $v1, 0x0($v0)
    ctx->r3 = MEM_BU(ctx->r2, 0X0);
    // 0x804141DC: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x804141E0: bne         $a0, $v1, L_804141EC
    if (ctx->r4 != ctx->r3) {
        // 0x804141E4: addiu       $v1, $zero, -0x7F
        ctx->r3 = ADD32(0, -0X7F);
            goto L_804141EC;
    }
    // 0x804141E4: addiu       $v1, $zero, -0x7F
    ctx->r3 = ADD32(0, -0X7F);
    // 0x804141E8: sb          $v1, 0x0($v0)
    MEM_B(0X0, ctx->r2) = ctx->r3;
L_804141EC:
    // 0x804141EC: jr          $ra
    // 0x804141F0: nop

    return;
    // 0x804141F0: nop

;}
RECOMP_FUNC void hm_func_804141F4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804141F4: sltu        $v0, $a1, $a0
    ctx->r2 = ctx->r5 < ctx->r4 ? 1 : 0;
    // 0x804141F8: bne         $v0, $zero, L_80414240
    if (ctx->r2 != 0) {
        // 0x804141FC: nop
    
            goto L_80414240;
    }
    // 0x804141FC: nop

    // 0x80414200: or          $v0, $zero, $zero
    ctx->r2 = 0 | 0;
L_80414204:
    // 0x80414204: lbu         $v1, 0x0($a0)
    ctx->r3 = MEM_BU(ctx->r4, 0X0);
    // 0x80414208: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041420C: beq         $v1, $zero, L_80414228
    if (ctx->r3 == 0) {
        // 0x80414210: nop
    
            goto L_80414228;
    }
    // 0x80414210: nop

L_80414214:
    // 0x80414214: andi        $a2, $v1, 0x1
    ctx->r6 = ctx->r3 & 0X1;
    // 0x80414218: addu        $v0, $v0, $a2
    ctx->r2 = ADD32(ctx->r2, ctx->r6);
    // 0x8041421C: srl         $v1, $v1, 1
    ctx->r3 = S32(U32(ctx->r3) >> 1);
    // 0x80414220: bne         $v1, $zero, L_80414214
    if (ctx->r3 != 0) {
        // 0x80414224: andi        $v0, $v0, 0xFF
        ctx->r2 = ctx->r2 & 0XFF;
            goto L_80414214;
    }
    // 0x80414224: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
L_80414228:
    // 0x80414228: beq         $a1, $a0, L_80414238
    if (ctx->r5 == ctx->r4) {
        // 0x8041422C: addiu       $v1, $a0, 0x1
        ctx->r3 = ADD32(ctx->r4, 0X1);
            goto L_80414238;
    }
    // 0x8041422C: addiu       $v1, $a0, 0x1
    ctx->r3 = ADD32(ctx->r4, 0X1);
    // 0x80414230: b           L_80414204
    // 0x80414234: or          $a0, $v1, $zero
    ctx->r4 = ctx->r3 | 0;
        goto L_80414204;
    // 0x80414234: or          $a0, $v1, $zero
    ctx->r4 = ctx->r3 | 0;
L_80414238:
    // 0x80414238: jr          $ra
    // 0x8041423C: nop

    return;
    // 0x8041423C: nop

L_80414240:
    // 0x80414240: jr          $ra
    // 0x80414244: or          $v0, $zero, $zero
    ctx->r2 = 0 | 0;
    return;
    // 0x80414244: or          $v0, $zero, $zero
    ctx->r2 = 0 | 0;
;}
RECOMP_FUNC void hm_func_80414248(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414248: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041424C: lbu         $v1, 0x69F3($a1)
    ctx->r3 = MEM_BU(ctx->r5, 0X69F3);
    // 0x80414250: lbu         $a2, 0x69F0($a1)
    ctx->r6 = MEM_BU(ctx->r5, 0X69F0);
    // 0x80414254: lbu         $a0, 0x69E4($a1)
    ctx->r4 = MEM_BU(ctx->r5, 0X69E4);
    // 0x80414258: andi        $v0, $v1, 0x1
    ctx->r2 = ctx->r3 & 0X1;
    // 0x8041425C: srl         $a2, $a2, 7
    ctx->r6 = S32(U32(ctx->r6) >> 7);
    // 0x80414260: srl         $a3, $a0, 7
    ctx->r7 = S32(U32(ctx->r4) >> 7);
    // 0x80414264: addu        $v0, $v0, $a2
    ctx->r2 = ADD32(ctx->r2, ctx->r6);
    // 0x80414268: srl         $a2, $v1, 1
    ctx->r6 = S32(U32(ctx->r3) >> 1);
    // 0x8041426C: addu        $v0, $v0, $a3
    ctx->r2 = ADD32(ctx->r2, ctx->r7);
    // 0x80414270: andi        $a2, $a2, 0x1
    ctx->r6 = ctx->r6 & 0X1;
    // 0x80414274: srl         $a3, $v1, 2
    ctx->r7 = S32(U32(ctx->r3) >> 2);
    // 0x80414278: addu        $v0, $v0, $a2
    ctx->r2 = ADD32(ctx->r2, ctx->r6);
    // 0x8041427C: andi        $a3, $a3, 0x1
    ctx->r7 = ctx->r7 & 0X1;
    // 0x80414280: srl         $a2, $v1, 3
    ctx->r6 = S32(U32(ctx->r3) >> 3);
    // 0x80414284: addu        $v0, $v0, $a3
    ctx->r2 = ADD32(ctx->r2, ctx->r7);
    // 0x80414288: andi        $a2, $a2, 0x1
    ctx->r6 = ctx->r6 & 0X1;
    // 0x8041428C: srl         $v1, $v1, 4
    ctx->r3 = S32(U32(ctx->r3) >> 4);
    // 0x80414290: lbu         $a1, 0x69E5($a1)
    ctx->r5 = MEM_BU(ctx->r5, 0X69E5);
    // 0x80414294: addu        $v0, $v0, $a2
    ctx->r2 = ADD32(ctx->r2, ctx->r6);
    // 0x80414298: andi        $v1, $v1, 0x1
    ctx->r3 = ctx->r3 & 0X1;
    // 0x8041429C: srl         $a2, $a0, 1
    ctx->r6 = S32(U32(ctx->r4) >> 1);
    // 0x804142A0: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x804142A4: srl         $a0, $a0, 4
    ctx->r4 = S32(U32(ctx->r4) >> 4);
    // 0x804142A8: andi        $v1, $a2, 0x1
    ctx->r3 = ctx->r6 & 0X1;
    // 0x804142AC: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x804142B0: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
    // 0x804142B4: srl         $v1, $a1, 2
    ctx->r3 = S32(U32(ctx->r5) >> 2);
    // 0x804142B8: addu        $v0, $v0, $a0
    ctx->r2 = ADD32(ctx->r2, ctx->r4);
    // 0x804142BC: andi        $v1, $v1, 0x1
    ctx->r3 = ctx->r3 & 0X1;
    // 0x804142C0: srl         $a1, $a1, 5
    ctx->r5 = S32(U32(ctx->r5) >> 5);
    // 0x804142C4: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x804142C8: andi        $a1, $a1, 0x1
    ctx->r5 = ctx->r5 & 0X1;
    // 0x804142CC: addu        $v0, $v0, $a1
    ctx->r2 = ADD32(ctx->r2, ctx->r5);
    // 0x804142D0: jr          $ra
    // 0x804142D4: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
    return;
    // 0x804142D4: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
;}
RECOMP_FUNC void hm_func_804142D8(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804142D8: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x804142DC: addiu       $sp, $sp, -0x18
    ctx->r29 = ADD32(ctx->r29, -0X18);
    // 0x804142E0: addiu       $a1, $a3, 0x6AF7
    ctx->r5 = ADD32(ctx->r7, 0X6AF7);
    // 0x804142E4: sw          $ra, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r31;
    // 0x804142E8: jal         0x804141F4
    // 0x804142EC: addiu       $a0, $a3, 0x6AE8
    ctx->r4 = ADD32(ctx->r7, 0X6AE8);
    hm_func_804141F4(rdram, ctx);
        goto after_0;
    // 0x804142EC: addiu       $a0, $a3, 0x6AE8
    ctx->r4 = ADD32(ctx->r7, 0X6AE8);
    after_0:
    // 0x804142F0: addiu       $a1, $a3, 0x69E3
    ctx->r5 = ADD32(ctx->r7, 0X69E3);
    // 0x804142F4: addiu       $a0, $a3, 0x69D8
    ctx->r4 = ADD32(ctx->r7, 0X69D8);
    // 0x804142F8: jal         0x804141F4
    // 0x804142FC: or          $t0, $v0, $zero
    ctx->r8 = ctx->r2 | 0;
    hm_func_804141F4(rdram, ctx);
        goto after_1;
    // 0x804142FC: or          $t0, $v0, $zero
    ctx->r8 = ctx->r2 | 0;
    after_1:
    // 0x80414300: jal         0x80414248
    // 0x80414304: or          $t1, $v0, $zero
    ctx->r9 = ctx->r2 | 0;
    hm_func_80414248(rdram, ctx);
        goto after_2;
    // 0x80414304: or          $t1, $v0, $zero
    ctx->r9 = ctx->r2 | 0;
    after_2:
    // 0x80414308: lui         $v1, 0xCCCC
    ctx->r3 = S32(0XCCCC << 16);
    // 0x8041430C: ori         $v1, $v1, 0xCCCD
    ctx->r3 = ctx->r3 | 0XCCCD;
    // 0x80414310: multu       $t0, $v1
    result = U64(U32(ctx->r8)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414314: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x80414318: mfhi        $a1
    ctx->r5 = hi;
    // 0x8041431C: srl         $a1, $a1, 3
    ctx->r5 = S32(U32(ctx->r5) >> 3);
    // 0x80414320: andi        $t2, $a1, 0xFF
    ctx->r10 = ctx->r5 & 0XFF;
    // 0x80414324: multu       $t1, $v1
    result = U64(U32(ctx->r9)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414328: sll         $t7, $a1, 2
    ctx->r15 = S32(ctx->r5 << 2);
    // 0x8041432C: addu        $t7, $t7, $a1
    ctx->r15 = ADD32(ctx->r15, ctx->r5);
    // 0x80414330: sll         $t7, $t7, 1
    ctx->r15 = S32(ctx->r15 << 1);
    // 0x80414334: mfhi        $a0
    ctx->r4 = hi;
    // 0x80414338: srl         $a0, $a0, 3
    ctx->r4 = S32(U32(ctx->r4) >> 3);
    // 0x8041433C: andi        $a3, $a0, 0xFF
    ctx->r7 = ctx->r4 & 0XFF;
    // 0x80414340: multu       $v0, $v1
    result = U64(U32(ctx->r2)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414344: sll         $t5, $a0, 2
    ctx->r13 = S32(ctx->r4 << 2);
    // 0x80414348: addu        $t5, $t5, $a0
    ctx->r13 = ADD32(ctx->r13, ctx->r4);
    // 0x8041434C: sll         $t5, $t5, 1
    ctx->r13 = S32(ctx->r13 << 1);
    // 0x80414350: subu        $t1, $t1, $t5
    ctx->r9 = SUB32(ctx->r9, ctx->r13);
    // 0x80414354: mfhi        $t3
    ctx->r11 = hi;
    // 0x80414358: srl         $a0, $t3, 3
    ctx->r4 = S32(U32(ctx->r11) >> 3);
    // 0x8041435C: sll         $t3, $a0, 2
    ctx->r11 = S32(ctx->r4 << 2);
    // 0x80414360: multu       $t2, $v1
    result = U64(U32(ctx->r10)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414364: andi        $a1, $a0, 0xFF
    ctx->r5 = ctx->r4 & 0XFF;
    // 0x80414368: addu        $t3, $t3, $a0
    ctx->r11 = ADD32(ctx->r11, ctx->r4);
    // 0x8041436C: sll         $t3, $t3, 1
    ctx->r11 = S32(ctx->r11 << 1);
    // 0x80414370: subu        $v0, $v0, $t3
    ctx->r2 = SUB32(ctx->r2, ctx->r11);
    // 0x80414374: mfhi        $t8
    ctx->r24 = hi;
    // 0x80414378: srl         $a0, $t8, 3
    ctx->r4 = S32(U32(ctx->r24) >> 3);
    // 0x8041437C: sll         $t8, $a0, 2
    ctx->r24 = S32(ctx->r4 << 2);
    // 0x80414380: multu       $a3, $v1
    result = U64(U32(ctx->r7)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80414384: addu        $t8, $t8, $a0
    ctx->r24 = ADD32(ctx->r24, ctx->r4);
    // 0x80414388: sll         $t8, $t8, 1
    ctx->r24 = S32(ctx->r24 << 1);
    // 0x8041438C: subu        $t2, $t2, $t8
    ctx->r10 = SUB32(ctx->r10, ctx->r24);
    // 0x80414390: andi        $t2, $t2, 0xFF
    ctx->r10 = ctx->r10 & 0XFF;
    // 0x80414394: mfhi        $t6
    ctx->r14 = hi;
    // 0x80414398: srl         $a0, $t6, 3
    ctx->r4 = S32(U32(ctx->r14) >> 3);
    // 0x8041439C: sll         $t6, $a0, 2
    ctx->r14 = S32(ctx->r4 << 2);
    // 0x804143A0: multu       $a1, $v1
    result = U64(U32(ctx->r5)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x804143A4: addu        $t6, $t6, $a0
    ctx->r14 = ADD32(ctx->r14, ctx->r4);
    // 0x804143A8: lui         $a0, 0x51EB
    ctx->r4 = S32(0X51EB << 16);
    // 0x804143AC: ori         $a0, $a0, 0x851F
    ctx->r4 = ctx->r4 | 0X851F;
    // 0x804143B0: sll         $t6, $t6, 1
    ctx->r14 = S32(ctx->r14 << 1);
    // 0x804143B4: mfhi        $v1
    ctx->r3 = hi;
    // 0x804143B8: srl         $v1, $v1, 3
    ctx->r3 = S32(U32(ctx->r3) >> 3);
    // 0x804143BC: sll         $t4, $v1, 2
    ctx->r12 = S32(ctx->r3 << 2);
    // 0x804143C0: multu       $t0, $a0
    result = U64(U32(ctx->r8)) * U64(U32(ctx->r4)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x804143C4: addu        $v1, $t4, $v1
    ctx->r3 = ADD32(ctx->r12, ctx->r3);
    // 0x804143C8: sll         $v1, $v1, 1
    ctx->r3 = S32(ctx->r3 << 1);
    // 0x804143CC: subu        $t0, $t0, $t7
    ctx->r8 = SUB32(ctx->r8, ctx->r15);
    // 0x804143D0: subu        $a3, $a3, $t6
    ctx->r7 = SUB32(ctx->r7, ctx->r14);
    // 0x804143D4: mfhi        $a0
    ctx->r4 = hi;
    // 0x804143D8: srl         $a0, $a0, 5
    ctx->r4 = S32(U32(ctx->r4) >> 5);
    // 0x804143DC: subu        $a1, $a1, $v1
    ctx->r5 = SUB32(ctx->r5, ctx->r3);
    // 0x804143E0: andi        $a0, $a0, 0xFF
    ctx->r4 = ctx->r4 & 0XFF;
    // 0x804143E4: andi        $t0, $t0, 0xFF
    ctx->r8 = ctx->r8 & 0XFF;
    // 0x804143E8: andi        $a3, $a3, 0xFF
    ctx->r7 = ctx->r7 & 0XFF;
    // 0x804143EC: andi        $t1, $t1, 0xFF
    ctx->r9 = ctx->r9 & 0XFF;
    // 0x804143F0: andi        $a1, $a1, 0xFF
    ctx->r5 = ctx->r5 & 0XFF;
    // 0x804143F4: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
    // 0x804143F8: sb          $a0, 0x4E11($a2)
    MEM_B(0X4E11, ctx->r6) = ctx->r4;
    // 0x804143FC: sb          $t2, 0x4E12($a2)
    MEM_B(0X4E12, ctx->r6) = ctx->r10;
    // 0x80414400: sb          $t0, 0x4E13($a2)
    MEM_B(0X4E13, ctx->r6) = ctx->r8;
    // 0x80414404: sb          $a3, 0x4E24($a2)
    MEM_B(0X4E24, ctx->r6) = ctx->r7;
    // 0x80414408: sb          $t1, 0x4E25($a2)
    MEM_B(0X4E25, ctx->r6) = ctx->r9;
    // 0x8041440C: sb          $a1, 0x4E3B($a2)
    MEM_B(0X4E3B, ctx->r6) = ctx->r5;
    // 0x80414410: sb          $v0, 0x4E3C($a2)
    MEM_B(0X4E3C, ctx->r6) = ctx->r2;
    // 0x80414414: lw          $ra, 0x14($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X14);
    // 0x80414418: jr          $ra
    // 0x8041441C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
    return;
    // 0x8041441C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
;}
RECOMP_FUNC void hm_func_80414420(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414420: lui         $v0, 0x8007
    ctx->r2 = S32(0X8007 << 16);
    // 0x80414424: ori         $v0, $v0, 0xB2E4
    ctx->r2 = ctx->r2 | 0XB2E4;
    // 0x80414428: addiu       $sp, $sp, -0x38
    ctx->r29 = ADD32(ctx->r29, -0X38);
    // 0x8041442C: lhu         $a0, 0x0($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X0);
    // 0x80414430: sw          $ra, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r31;
    // 0x80414434: jal         0x80413F88
    // 0x80414438: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    hm_func_80413F88(rdram, ctx);
        goto after_0;
    // 0x80414438: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    after_0:
    // 0x8041443C: beq         $a0, $v1, L_80414678
    if (ctx->r4 == ctx->r3) {
        // 0x80414440: or          $a3, $v0, $zero
        ctx->r7 = ctx->r2 | 0;
            goto L_80414678;
    }
    // 0x80414440: or          $a3, $v0, $zero
    ctx->r7 = ctx->r2 | 0;
    // 0x80414444: jal         0x80413F7C
    // 0x80414448: addiu       $v1, $sp, 0x10
    ctx->r3 = ADD32(ctx->r29, 0X10);
    hm_func_80413F7C(rdram, ctx);
        goto after_1;
    // 0x80414448: addiu       $v1, $sp, 0x10
    ctx->r3 = ADD32(ctx->r29, 0X10);
    after_1:
    // 0x8041444C: addiu       $a0, $sp, 0x30
    ctx->r4 = ADD32(ctx->r29, 0X30);
    // 0x80414450: sb          $zero, 0x0($v1)
    MEM_B(0X0, ctx->r3) = 0;
L_80414454:
    // 0x80414454: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x80414458: bnel        $a0, $v1, L_80414454
    if (ctx->r4 != ctx->r3) {
        // 0x8041445C: sb          $zero, 0x0($v1)
        MEM_B(0X0, ctx->r3) = 0;
            goto L_80414454;
    }
    goto skip_0;
    // 0x8041445C: sb          $zero, 0x0($v1)
    MEM_B(0X0, ctx->r3) = 0;
    skip_0:
    // 0x80414460: lbu         $a0, 0x4A($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X4A);
    // 0x80414464: addiu       $v1, $zero, 0xFF
    ctx->r3 = ADD32(0, 0XFF);
    // 0x80414468: beq         $a0, $v1, L_8041448C
    if (ctx->r4 == ctx->r3) {
        // 0x8041446C: addiu       $v1, $v0, 0x69
        ctx->r3 = ADD32(ctx->r2, 0X69);
            goto L_8041448C;
    }
    // 0x8041446C: addiu       $v1, $v0, 0x69
    ctx->r3 = ADD32(ctx->r2, 0X69);
    // 0x80414470: addiu       $a1, $v0, 0x49
    ctx->r5 = ADD32(ctx->r2, 0X49);
L_80414474:
    // 0x80414474: lbu         $a0, 0x0($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X0);
    // 0x80414478: addiu       $v1, $v1, -0x1
    ctx->r3 = ADD32(ctx->r3, -0X1);
    // 0x8041447C: bne         $a1, $v1, L_80414474
    if (ctx->r5 != ctx->r3) {
        // 0x80414480: sb          $a0, 0x2($v1)
        MEM_B(0X2, ctx->r3) = ctx->r4;
            goto L_80414474;
    }
    // 0x80414480: sb          $a0, 0x2($v1)
    MEM_B(0X2, ctx->r3) = ctx->r4;
    // 0x80414484: addiu       $v1, $zero, -0x1
    ctx->r3 = ADD32(0, -0X1);
    // 0x80414488: sb          $v1, 0x4A($v0)
    MEM_B(0X4A, ctx->r2) = ctx->r3;
L_8041448C:
    // 0x8041448C: lui         $t7, 0x8042
    ctx->r15 = S32(0X8042 << 16);
    // 0x80414490: or          $t3, $zero, $zero
    ctx->r11 = 0 | 0;
    // 0x80414494: or          $t0, $v0, $zero
    ctx->r8 = ctx->r2 | 0;
    // 0x80414498: addiu       $t4, $zero, 0xFF
    ctx->r12 = ADD32(0, 0XFF);
    // 0x8041449C: addiu       $t5, $zero, 0x1
    ctx->r13 = ADD32(0, 0X1);
    // 0x804144A0: addiu       $t6, $zero, -0x80
    ctx->r14 = ADD32(0, -0X80);
    // 0x804144A4: addiu       $t7, $t7, -0x5A94
    ctx->r15 = ADD32(ctx->r15, -0X5A94);
    // 0x804144A8: addiu       $t2, $v0, 0x4A
    ctx->r10 = ADD32(ctx->r2, 0X4A);
    // 0x804144AC: addiu       $t8, $zero, -0x1
    ctx->r24 = ADD32(0, -0X1);
    // 0x804144B0: addiu       $t9, $zero, -0x80
    ctx->r25 = ADD32(0, -0X80);
L_804144B4:
    // 0x804144B4: lbu         $v1, 0x0($t0)
    ctx->r3 = MEM_BU(ctx->r8, 0X0);
    // 0x804144B8: beql        $v1, $t4, L_80414510
    if (ctx->r3 == ctx->r12) {
        // 0x804144BC: or          $v1, $a3, $zero
        ctx->r3 = ctx->r7 | 0;
            goto L_80414510;
    }
    goto skip_1;
    // 0x804144BC: or          $v1, $a3, $zero
    ctx->r3 = ctx->r7 | 0;
    skip_1:
    // 0x804144C0: addiu       $a0, $sp, 0x10
    ctx->r4 = ADD32(ctx->r29, 0X10);
L_804144C4:
    // 0x804144C4: addu        $a1, $a0, $v1
    ctx->r5 = ADD32(ctx->r4, ctx->r3);
    // 0x804144C8: lbu         $t1, 0x0($a1)
    ctx->r9 = MEM_BU(ctx->r5, 0X0);
    // 0x804144CC: addu        $a2, $a3, $v1
    ctx->r6 = ADD32(ctx->r7, ctx->r3);
    // 0x804144D0: bne         $t1, $zero, L_80414630
    if (ctx->r9 != 0) {
        // 0x804144D4: lbu         $a0, 0x0($a2)
        ctx->r4 = MEM_BU(ctx->r6, 0X0);
            goto L_80414630;
    }
    // 0x804144D4: lbu         $a0, 0x0($a2)
    ctx->r4 = MEM_BU(ctx->r6, 0X0);
    // 0x804144D8: sll         $a0, $a0, 24
    ctx->r4 = S32(ctx->r4 << 24);
    // 0x804144DC: sra         $a0, $a0, 24
    ctx->r4 = S32(SIGNED(ctx->r4) >> 24);
    // 0x804144E0: sb          $t5, 0x0($a1)
    MEM_B(0X0, ctx->r5) = ctx->r13;
    // 0x804144E4: bltz        $a0, L_804144F0
    if (SIGNED(ctx->r4) < 0) {
        // 0x804144E8: addiu       $t3, $t3, 0x1
        ctx->r11 = ADD32(ctx->r11, 0X1);
            goto L_804144F0;
    }
    // 0x804144E8: addiu       $t3, $t3, 0x1
    ctx->r11 = ADD32(ctx->r11, 0X1);
    // 0x804144EC: sb          $t6, 0x0($a2)
    MEM_B(0X0, ctx->r6) = ctx->r14;
L_804144F0:
    // 0x804144F0: slti        $v1, $t3, 0x4B
    ctx->r3 = SIGNED(ctx->r11) < 0X4B ? 1 : 0;
    // 0x804144F4: beq         $v1, $zero, L_80414510
    if (ctx->r3 == 0) {
        // 0x804144F8: or          $v1, $a3, $zero
        ctx->r3 = ctx->r7 | 0;
            goto L_80414510;
    }
    // 0x804144F8: or          $v1, $a3, $zero
    ctx->r3 = ctx->r7 | 0;
    // 0x804144FC: addu        $t0, $v0, $t3
    ctx->r8 = ADD32(ctx->r2, ctx->r11);
    // 0x80414500: lbu         $v1, 0x0($t0)
    ctx->r3 = MEM_BU(ctx->r8, 0X0);
    // 0x80414504: bne         $v1, $t4, L_804144C4
    if (ctx->r3 != ctx->r12) {
        // 0x80414508: addiu       $a0, $sp, 0x10
        ctx->r4 = ADD32(ctx->r29, 0X10);
            goto L_804144C4;
    }
    // 0x80414508: addiu       $a0, $sp, 0x10
    ctx->r4 = ADD32(ctx->r29, 0X10);
    // 0x8041450C: or          $v1, $a3, $zero
    ctx->r3 = ctx->r7 | 0;
L_80414510:
    // 0x80414510: addiu       $a2, $a3, 0x20
    ctx->r6 = ADD32(ctx->r7, 0X20);
    // 0x80414514: b           L_80414524
    // 0x80414518: addiu       $a1, $zero, 0xFF
    ctx->r5 = ADD32(0, 0XFF);
        goto L_80414524;
    // 0x80414518: addiu       $a1, $zero, 0xFF
    ctx->r5 = ADD32(0, 0XFF);
L_8041451C:
    // 0x8041451C: beq         $v1, $a2, L_80414544
    if (ctx->r3 == ctx->r6) {
        // 0x80414520: or          $a0, $zero, $zero
        ctx->r4 = 0 | 0;
            goto L_80414544;
    }
    // 0x80414520: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
L_80414524:
    // 0x80414524: lbu         $a0, 0x0($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X0);
L_80414528:
    // 0x80414528: bnel        $a0, $a1, L_8041451C
    if (ctx->r4 != ctx->r5) {
        // 0x8041452C: addiu       $v1, $v1, 0x1
        ctx->r3 = ADD32(ctx->r3, 0X1);
            goto L_8041451C;
    }
    goto skip_2;
    // 0x8041452C: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    skip_2:
    // 0x80414530: sb          $zero, 0x0($v1)
    MEM_B(0X0, ctx->r3) = 0;
    // 0x80414534: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x80414538: bnel        $v1, $a2, L_80414528
    if (ctx->r3 != ctx->r6) {
        // 0x8041453C: lbu         $a0, 0x0($v1)
        ctx->r4 = MEM_BU(ctx->r3, 0X0);
            goto L_80414528;
    }
    goto skip_3;
    // 0x8041453C: lbu         $a0, 0x0($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X0);
    skip_3:
    // 0x80414540: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
L_80414544:
    // 0x80414544: addiu       $t2, $v0, 0x4B
    ctx->r10 = ADD32(ctx->r2, 0X4B);
    // 0x80414548: addiu       $t1, $zero, 0xFF
    ctx->r9 = ADD32(0, 0XFF);
    // 0x8041454C: addiu       $t5, $zero, -0x80
    ctx->r13 = ADD32(0, -0X80);
    // 0x80414550: b           L_80414560
    // 0x80414554: addiu       $t4, $zero, 0x20
    ctx->r12 = ADD32(0, 0X20);
        goto L_80414560;
    // 0x80414554: addiu       $t4, $zero, 0x20
    ctx->r12 = ADD32(0, 0X20);
L_80414558:
    // 0x80414558: beq         $a0, $t4, L_804145C4
    if (ctx->r4 == ctx->r12) {
        // 0x8041455C: slti        $v1, $t3, 0x2
        ctx->r3 = SIGNED(ctx->r11) < 0X2 ? 1 : 0;
            goto L_804145C4;
    }
    // 0x8041455C: slti        $v1, $t3, 0x2
    ctx->r3 = SIGNED(ctx->r11) < 0X2 ? 1 : 0;
L_80414560:
    // 0x80414560: addu        $t0, $a3, $a0
    ctx->r8 = ADD32(ctx->r7, ctx->r4);
    // 0x80414564: lbu         $a1, 0x0($t0)
    ctx->r5 = MEM_BU(ctx->r8, 0X0);
    // 0x80414568: andi        $a1, $a1, 0x7F
    ctx->r5 = ctx->r5 & 0X7F;
    // 0x8041456C: beql        $a1, $zero, L_80414558
    if (ctx->r5 == 0) {
        // 0x80414570: addiu       $a0, $a0, 0x1
        ctx->r4 = ADD32(ctx->r4, 0X1);
            goto L_80414558;
    }
    goto skip_4;
    // 0x80414570: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    skip_4:
    // 0x80414574: addiu       $v1, $sp, 0x10
    ctx->r3 = ADD32(ctx->r29, 0X10);
    // 0x80414578: addu        $v1, $v1, $a0
    ctx->r3 = ADD32(ctx->r3, ctx->r4);
    // 0x8041457C: lbu         $v1, 0x0($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X0);
    // 0x80414580: bnel        $v1, $zero, L_80414558
    if (ctx->r3 != 0) {
        // 0x80414584: addiu       $a0, $a0, 0x1
        ctx->r4 = ADD32(ctx->r4, 0X1);
            goto L_80414558;
    }
    goto skip_5;
    // 0x80414584: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    skip_5:
    // 0x80414588: b           L_80414598
    // 0x8041458C: or          $v1, $v0, $zero
    ctx->r3 = ctx->r2 | 0;
        goto L_80414598;
    // 0x8041458C: or          $v1, $v0, $zero
    ctx->r3 = ctx->r2 | 0;
L_80414590:
    // 0x80414590: beql        $t2, $v1, L_80414558
    if (ctx->r10 == ctx->r3) {
        // 0x80414594: addiu       $a0, $a0, 0x1
        ctx->r4 = ADD32(ctx->r4, 0X1);
            goto L_80414558;
    }
    goto skip_6;
    // 0x80414594: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    skip_6:
L_80414598:
    // 0x80414598: lbu         $a2, 0x0($v1)
    ctx->r6 = MEM_BU(ctx->r3, 0X0);
    // 0x8041459C: bnel        $a2, $t1, L_80414590
    if (ctx->r6 != ctx->r9) {
        // 0x804145A0: addiu       $v1, $v1, 0x1
        ctx->r3 = ADD32(ctx->r3, 0X1);
            goto L_80414590;
    }
    goto skip_7;
    // 0x804145A0: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    skip_7:
    // 0x804145A4: addiu       $a1, $a1, -0x1
    ctx->r5 = ADD32(ctx->r5, -0X1);
    // 0x804145A8: sb          $a0, 0x0($v1)
    MEM_B(0X0, ctx->r3) = ctx->r4;
    // 0x804145AC: or          $a1, $a1, $t5
    ctx->r5 = ctx->r5 | ctx->r13;
    // 0x804145B0: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x804145B4: addiu       $t3, $t3, 0x1
    ctx->r11 = ADD32(ctx->r11, 0X1);
    // 0x804145B8: bne         $a0, $t4, L_80414560
    if (ctx->r4 != ctx->r12) {
        // 0x804145BC: sb          $a1, 0x0($t0)
        MEM_B(0X0, ctx->r8) = ctx->r5;
            goto L_80414560;
    }
    // 0x804145BC: sb          $a1, 0x0($t0)
    MEM_B(0X0, ctx->r8) = ctx->r5;
    // 0x804145C0: slti        $v1, $t3, 0x2
    ctx->r3 = SIGNED(ctx->r11) < 0X2 ? 1 : 0;
L_804145C4:
    // 0x804145C4: bne         $v1, $zero, L_80414628
    if (ctx->r3 != 0) {
        // 0x804145C8: lw          $ra, 0x34($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X34);
            goto L_80414628;
    }
    // 0x804145C8: lw          $ra, 0x34($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X34);
    // 0x804145CC: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x804145D0: addiu       $t1, $v0, 0x1
    ctx->r9 = ADD32(ctx->r2, 0X1);
    // 0x804145D4: addu        $t3, $t3, $v0
    ctx->r11 = ADD32(ctx->r11, ctx->r2);
    // 0x804145D8: addiu       $a3, $a3, -0x5AB4
    ctx->r7 = ADD32(ctx->r7, -0X5AB4);
L_804145DC:
    // 0x804145DC: lbu         $t2, 0x0($t1)
    ctx->r10 = MEM_BU(ctx->r9, 0X0);
    // 0x804145E0: or          $v1, $t1, $zero
    ctx->r3 = ctx->r9 | 0;
    // 0x804145E4: addu        $a0, $a3, $t2
    ctx->r4 = ADD32(ctx->r7, ctx->r10);
    // 0x804145E8: lbu         $t0, 0x0($a0)
    ctx->r8 = MEM_BU(ctx->r4, 0X0);
    // 0x804145EC: lbu         $a1, -0x1($v1)
    ctx->r5 = MEM_BU(ctx->r3, -0X1);
L_804145F0:
    // 0x804145F0: addu        $a0, $a3, $a1
    ctx->r4 = ADD32(ctx->r7, ctx->r5);
    // 0x804145F4: lbu         $a0, 0x0($a0)
    ctx->r4 = MEM_BU(ctx->r4, 0X0);
    // 0x804145F8: sltu        $a0, $t0, $a0
    ctx->r4 = ctx->r8 < ctx->r4 ? 1 : 0;
    // 0x804145FC: beq         $a0, $zero, L_80414618
    if (ctx->r4 == 0) {
        // 0x80414600: or          $a2, $v1, $zero
        ctx->r6 = ctx->r3 | 0;
            goto L_80414618;
    }
    // 0x80414600: or          $a2, $v1, $zero
    ctx->r6 = ctx->r3 | 0;
    // 0x80414604: sb          $a1, 0x0($v1)
    MEM_B(0X0, ctx->r3) = ctx->r5;
    // 0x80414608: addiu       $v1, $a2, -0x1
    ctx->r3 = ADD32(ctx->r6, -0X1);
    // 0x8041460C: bnel        $v0, $v1, L_804145F0
    if (ctx->r2 != ctx->r3) {
        // 0x80414610: lbu         $a1, -0x1($v1)
        ctx->r5 = MEM_BU(ctx->r3, -0X1);
            goto L_804145F0;
    }
    goto skip_8;
    // 0x80414610: lbu         $a1, -0x1($v1)
    ctx->r5 = MEM_BU(ctx->r3, -0X1);
    skip_8:
    // 0x80414614: or          $a2, $v0, $zero
    ctx->r6 = ctx->r2 | 0;
L_80414618:
    // 0x80414618: addiu       $t1, $t1, 0x1
    ctx->r9 = ADD32(ctx->r9, 0X1);
    // 0x8041461C: bne         $t3, $t1, L_804145DC
    if (ctx->r11 != ctx->r9) {
        // 0x80414620: sb          $t2, 0x0($a2)
        MEM_B(0X0, ctx->r6) = ctx->r10;
            goto L_804145DC;
    }
    // 0x80414620: sb          $t2, 0x0($a2)
    MEM_B(0X0, ctx->r6) = ctx->r10;
    // 0x80414624: lw          $ra, 0x34($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X34);
L_80414628:
    // 0x80414628: jr          $ra
    // 0x8041462C: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
    return;
    // 0x8041462C: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
L_80414630:
    // 0x80414630: addu        $v1, $v1, $t7
    ctx->r3 = ADD32(ctx->r3, ctx->r15);
    // 0x80414634: lbu         $v1, 0x0($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X0);
    // 0x80414638: andi        $a0, $a0, 0x7F
    ctx->r4 = ctx->r4 & 0X7F;
    // 0x8041463C: sltu        $v1, $a0, $v1
    ctx->r3 = ctx->r4 < ctx->r3 ? 1 : 0;
    // 0x80414640: beql        $v1, $zero, L_80414658
    if (ctx->r3 == 0) {
        // 0x80414644: slti        $a0, $t3, 0x4A
        ctx->r4 = SIGNED(ctx->r11) < 0X4A ? 1 : 0;
            goto L_80414658;
    }
    goto skip_9;
    // 0x80414644: slti        $a0, $t3, 0x4A
    ctx->r4 = SIGNED(ctx->r11) < 0X4A ? 1 : 0;
    skip_9:
    // 0x80414648: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x8041464C: or          $v1, $a0, $t9
    ctx->r3 = ctx->r4 | ctx->r25;
    // 0x80414650: sb          $v1, 0x0($a2)
    MEM_B(0X0, ctx->r6) = ctx->r3;
    // 0x80414654: slti        $a0, $t3, 0x4A
    ctx->r4 = SIGNED(ctx->r11) < 0X4A ? 1 : 0;
L_80414658:
    // 0x80414658: beq         $a0, $zero, L_80414670
    if (ctx->r4 == 0) {
        // 0x8041465C: or          $v1, $t0, $zero
        ctx->r3 = ctx->r8 | 0;
            goto L_80414670;
    }
    // 0x8041465C: or          $v1, $t0, $zero
    ctx->r3 = ctx->r8 | 0;
L_80414660:
    // 0x80414660: lbu         $a0, 0x1($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X1);
    // 0x80414664: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x80414668: bne         $t2, $v1, L_80414660
    if (ctx->r10 != ctx->r3) {
        // 0x8041466C: sb          $a0, -0x1($v1)
        MEM_B(-0X1, ctx->r3) = ctx->r4;
            goto L_80414660;
    }
    // 0x8041466C: sb          $a0, -0x1($v1)
    MEM_B(-0X1, ctx->r3) = ctx->r4;
L_80414670:
    // 0x80414670: b           L_804144B4
    // 0x80414674: sb          $t8, 0x4A($v0)
    MEM_B(0X4A, ctx->r2) = ctx->r24;
        goto L_804144B4;
    // 0x80414674: sb          $t8, 0x4A($v0)
    MEM_B(0X4A, ctx->r2) = ctx->r24;
L_80414678:
    // 0x80414678: addiu       $v0, $v0, 0x20
    ctx->r2 = ADD32(ctx->r2, 0X20);
    // 0x8041467C: sb          $zero, 0x0($a3)
    MEM_B(0X0, ctx->r7) = 0;
L_80414680:
    // 0x80414680: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x80414684: bnel        $v0, $a3, L_80414680
    if (ctx->r2 != ctx->r7) {
        // 0x80414688: sb          $zero, 0x0($a3)
        MEM_B(0X0, ctx->r7) = 0;
            goto L_80414680;
    }
    goto skip_10;
    // 0x80414688: sb          $zero, 0x0($a3)
    MEM_B(0X0, ctx->r7) = 0;
    skip_10:
    // 0x8041468C: lw          $ra, 0x34($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X34);
    // 0x80414690: jr          $ra
    // 0x80414694: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
    return;
    // 0x80414694: addiu       $sp, $sp, 0x38
    ctx->r29 = ADD32(ctx->r29, 0X38);
;}
RECOMP_FUNC void hm_func_804146D0(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804146D0: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x804146D4: addiu       $v0, $v0, -0x5C24
    ctx->r2 = ADD32(ctx->r2, -0X5C24);
    // 0x804146D8: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x804146DC: addiu       $v1, $v1, -0x3088
    ctx->r3 = ADD32(ctx->r3, -0X3088);
    // 0x804146E0: addiu       $a1, $v0, 0x96
    ctx->r5 = ADD32(ctx->r2, 0X96);
L_804146E4:
    // 0x804146E4: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x804146E8: addiu       $v0, $v0, 0x1
    ctx->r2 = ADD32(ctx->r2, 0X1);
    // 0x804146EC: sb          $a0, 0x0($v1)
    MEM_B(0X0, ctx->r3) = ctx->r4;
    // 0x804146F0: bne         $v0, $a1, L_804146E4
    if (ctx->r2 != ctx->r5) {
        // 0x804146F4: addiu       $v1, $v1, 0x1
        ctx->r3 = ADD32(ctx->r3, 0X1);
            goto L_804146E4;
    }
    // 0x804146F4: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x804146F8: jr          $ra
    // 0x804146FC: nop

    return;
    // 0x804146FC: nop

;}
RECOMP_FUNC void hm_func_80414764(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414764: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80414768: addiu       $v0, $zero, 0x3
    ctx->r2 = ADD32(0, 0X3);
    // 0x8041476C: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x80414770: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414774: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80414778: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x8041477C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x80414780: jal         0x80029B58
    // 0x80414784: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x80414784: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    after_0:
    // 0x80414788: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x8041478C: jr          $ra
    // 0x80414790: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x80414790: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_80414794(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414794: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80414798: addiu       $v0, $zero, 0x7
    ctx->r2 = ADD32(0, 0X7);
    // 0x8041479C: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x804147A0: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x804147A4: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x804147A8: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x804147AC: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x804147B0: jal         0x80029B58
    // 0x804147B4: addiu       $a0, $zero, 0x24
    ctx->r4 = ADD32(0, 0X24);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x804147B4: addiu       $a0, $zero, 0x24
    ctx->r4 = ADD32(0, 0X24);
    after_0:
    // 0x804147B8: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x804147BC: jr          $ra
    // 0x804147C0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x804147C0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_804147C4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804147C4: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x804147C8: addiu       $v0, $zero, 0xC
    ctx->r2 = ADD32(0, 0XC);
    // 0x804147CC: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x804147D0: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x804147D4: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x804147D8: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x804147DC: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x804147E0: jal         0x80029B58
    // 0x804147E4: addiu       $a0, $zero, 0x22
    ctx->r4 = ADD32(0, 0X22);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x804147E4: addiu       $a0, $zero, 0x22
    ctx->r4 = ADD32(0, 0X22);
    after_0:
    // 0x804147E8: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x804147EC: jr          $ra
    // 0x804147F0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x804147F0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_804147F4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804147F4: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x804147F8: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x804147FC: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x80414800: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414804: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80414808: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x8041480C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x80414810: jal         0x80029B58
    // 0x80414814: addiu       $a0, $zero, 0x2A
    ctx->r4 = ADD32(0, 0X2A);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x80414814: addiu       $a0, $zero, 0x2A
    ctx->r4 = ADD32(0, 0X2A);
    after_0:
    // 0x80414818: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x8041481C: jr          $ra
    // 0x80414820: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x80414820: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_80414824(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414824: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80414828: addiu       $v0, $zero, 0x8
    ctx->r2 = ADD32(0, 0X8);
    // 0x8041482C: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x80414830: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414834: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80414838: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x8041483C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x80414840: jal         0x80029B58
    // 0x80414844: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x80414844: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    after_0:
    // 0x80414848: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x8041484C: jr          $ra
    // 0x80414850: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x80414850: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_80414854(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414854: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80414858: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x8041485C: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x80414860: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414864: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80414868: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x8041486C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x80414870: jal         0x80029B58
    // 0x80414874: addiu       $a0, $zero, 0x27
    ctx->r4 = ADD32(0, 0X27);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x80414874: addiu       $a0, $zero, 0x27
    ctx->r4 = ADD32(0, 0X27);
    after_0:
    // 0x80414878: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x8041487C: jr          $ra
    // 0x80414880: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x80414880: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_80414884(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80414884: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80414888: addiu       $v0, $zero, 0x2
    ctx->r2 = ADD32(0, 0X2);
    // 0x8041488C: or          $a3, $a2, $zero
    ctx->r7 = ctx->r6 | 0;
    // 0x80414890: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414894: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80414898: or          $a1, $a0, $zero
    ctx->r5 = ctx->r4 | 0;
    // 0x8041489C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x804148A0: jal         0x80029B58
    // 0x804148A4: addiu       $a0, $zero, 0x3D
    ctx->r4 = ADD32(0, 0X3D);
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x804148A4: addiu       $a0, $zero, 0x3D
    ctx->r4 = ADD32(0, 0X3D);
    after_0:
    // 0x804148A8: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x804148AC: jr          $ra
    // 0x804148B0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x804148B0: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
;}
RECOMP_FUNC void hm_func_804148B4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
L_80414884:
    // 0x804148B4: addiu       $sp, $sp, -0x18
    ctx->r29 = ADD32(ctx->r29, -0X18);
    // 0x804148B8: addiu       $a2, $zero, 0x54
    ctx->r6 = ADD32(0, 0X54);
    // 0x804148BC: addiu       $a1, $zero, 0x9A
    ctx->r5 = ADD32(0, 0X9A);
    // 0x804148C0: sw          $s0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r16;
    // 0x804148C4: or          $s0, $a0, $zero
    ctx->r16 = ctx->r4 | 0;
    // 0x804148C8: sw          $ra, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r31;
    // 0x804148CC: jal         0x80414794
    // 0x804148D0: addiu       $a0, $a0, 0x8D
    ctx->r4 = ADD32(ctx->r4, 0X8D);
    hm_func_80414794(rdram, ctx);
        goto after_0;
    // 0x804148D0: addiu       $a0, $a0, 0x8D
    ctx->r4 = ADD32(ctx->r4, 0X8D);
    after_0:
    // 0x804148D4: addiu       $a0, $s0, 0x8E
    ctx->r4 = ADD32(ctx->r16, 0X8E);
    // 0x804148D8: addiu       $a2, $zero, 0x50
    ctx->r6 = ADD32(0, 0X50);
    // 0x804148DC: jal         0x80414764
    // 0x804148E0: addiu       $a1, $zero, 0x9B
    ctx->r5 = ADD32(0, 0X9B);
    hm_func_80414764(rdram, ctx);
        goto after_1;
    // 0x804148E0: addiu       $a1, $zero, 0x9B
    ctx->r5 = ADD32(0, 0X9B);
    after_1:
    // 0x804148E4: addiu       $a0, $s0, 0x31
    ctx->r4 = ADD32(ctx->r16, 0X31);
    // 0x804148E8: addiu       $a2, $zero, 0x34
    ctx->r6 = ADD32(0, 0X34);
    // 0x804148EC: jal         0x804147C4
    // 0x804148F0: addiu       $a1, $zero, 0x8F
    ctx->r5 = ADD32(0, 0X8F);
    hm_func_804147C4(rdram, ctx);
        goto after_2;
    // 0x804148F0: addiu       $a1, $zero, 0x8F
    ctx->r5 = ADD32(0, 0X8F);
    after_2:
    // 0x804148F4: addiu       $a0, $s0, 0x6C
    ctx->r4 = ADD32(ctx->r16, 0X6C);
    // 0x804148F8: addiu       $a2, $zero, 0x14
    ctx->r6 = ADD32(0, 0X14);
    // 0x804148FC: jal         0x804147F4
    // 0x80414900: addiu       $a1, $zero, 0x95
    ctx->r5 = ADD32(0, 0X95);
    hm_func_804147F4(rdram, ctx);
        goto after_3;
    // 0x80414900: addiu       $a1, $zero, 0x95
    ctx->r5 = ADD32(0, 0X95);
    after_3:
    // 0x80414904: addiu       $a0, $s0, 0xEA
    ctx->r4 = ADD32(ctx->r16, 0XEA);
    // 0x80414908: addiu       $a2, $zero, 0x18
    ctx->r6 = ADD32(0, 0X18);
    // 0x8041490C: jal         0x80414824
    // 0x80414910: addiu       $a1, $zero, 0x97
    ctx->r5 = ADD32(0, 0X97);
    hm_func_80414824(rdram, ctx);
        goto after_4;
    // 0x80414910: addiu       $a1, $zero, 0x97
    ctx->r5 = ADD32(0, 0X97);
    after_4:
    // 0x80414914: addiu       $a0, $s0, 0xB3
    ctx->r4 = ADD32(ctx->r16, 0XB3);
    // 0x80414918: addiu       $a2, $zero, 0x8
    ctx->r6 = ADD32(0, 0X8);
    // 0x8041491C: jal         0x80414854
    // 0x80414920: addiu       $a1, $zero, 0x92
    ctx->r5 = ADD32(0, 0X92);
    hm_func_80414854(rdram, ctx);
        goto after_5;
    // 0x80414920: addiu       $a1, $zero, 0x92
    ctx->r5 = ADD32(0, 0X92);
    after_5:
    // 0x80414924: lw          $ra, 0x14($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X14);
    // 0x80414928: addiu       $a0, $s0, 0x5D
    ctx->r4 = ADD32(ctx->r16, 0X5D);
    // 0x8041492C: lw          $s0, 0x10($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X10);
    // 0x80414930: addiu       $a2, $zero, 0xA7
    ctx->r6 = ADD32(0, 0XA7);
    // 0x80414934: addiu       $a1, $zero, 0x84
    ctx->r5 = ADD32(0, 0X84);
    // 0x80414938: j           L_80414884
    // 0x8041493C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
    hm_func_80414884(rdram, ctx);
    return;
    // 0x8041493C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
;}
RECOMP_FUNC void hm_func_80414940(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
L_800268D4:
    // 0x80414940: addiu       $sp, $sp, -0x40
    ctx->r29 = ADD32(ctx->r29, -0X40);
    // 0x80414944: sw          $s4, 0x30($sp)
    MEM_W(0X30, ctx->r29) = ctx->r20;
    // 0x80414948: addiu       $s4, $zero, 0x9F
    ctx->r20 = ADD32(0, 0X9F);
    // 0x8041494C: addiu       $a1, $a0, 0x28
    ctx->r5 = ADD32(ctx->r4, 0X28);
    // 0x80414950: sw          $s1, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r17;
    // 0x80414954: sw          $s0, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r16;
    // 0x80414958: addiu       $s1, $zero, 0x3
    ctx->r17 = ADD32(0, 0X3);
    // 0x8041495C: or          $s0, $a0, $zero
    ctx->r16 = ctx->r4 | 0;
    // 0x80414960: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x80414964: addiu       $a3, $zero, 0xF0
    ctx->r7 = ADD32(0, 0XF0);
    // 0x80414968: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x8041496C: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x80414970: sw          $ra, 0x3C($sp)
    MEM_W(0X3C, ctx->r29) = ctx->r31;
    // 0x80414974: sw          $s6, 0x38($sp)
    MEM_W(0X38, ctx->r29) = ctx->r22;
    // 0x80414978: sw          $s5, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r21;
    // 0x8041497C: sw          $s3, 0x2C($sp)
    MEM_W(0X2C, ctx->r29) = ctx->r19;
    // 0x80414980: jal         0x80029B58
    // 0x80414984: sw          $s2, 0x28($sp)
    MEM_W(0X28, ctx->r29) = ctx->r18;
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x80414984: sw          $s2, 0x28($sp)
    MEM_W(0X28, ctx->r29) = ctx->r18;
    after_0:
    // 0x80414988: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041498C: addiu       $a3, $zero, 0xF0
    ctx->r7 = ADD32(0, 0XF0);
    // 0x80414990: addiu       $a2, $zero, 0xBE
    ctx->r6 = ADD32(0, 0XBE);
    // 0x80414994: addiu       $a1, $s0, 0x2B
    ctx->r5 = ADD32(ctx->r16, 0X2B);
    // 0x80414998: jal         0x80029B58
    // 0x8041499C: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_1;
    // 0x8041499C: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_1:
    // 0x804149A0: addiu       $v0, $zero, 0x9A
    ctx->r2 = ADD32(0, 0X9A);
    // 0x804149A4: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x804149A8: addiu       $s2, $zero, 0x1
    ctx->r18 = ADD32(0, 0X1);
    // 0x804149AC: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x804149B0: addiu       $a2, $zero, 0x24
    ctx->r6 = ADD32(0, 0X24);
    // 0x804149B4: addiu       $a1, $s0, 0x119
    ctx->r5 = ADD32(ctx->r16, 0X119);
    // 0x804149B8: jal         0x80029B58
    // 0x804149BC: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_2;
    // 0x804149BC: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_2:
    // 0x804149C0: addiu       $a1, $s0, 0x27
    ctx->r5 = ADD32(ctx->r16, 0X27);
    // 0x804149C4: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x804149C8: addiu       $a3, $zero, 0xF2
    ctx->r7 = ADD32(0, 0XF2);
    // 0x804149CC: addiu       $a2, $zero, 0x1D
    ctx->r6 = ADD32(0, 0X1D);
    // 0x804149D0: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    // 0x804149D4: jal         0x80029B58
    // 0x804149D8: sw          $a1, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r5;
    func_80029B58(rdram, ctx);
        goto after_3;
    // 0x804149D8: sw          $a1, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r5;
    after_3:
    // 0x804149DC: lw          $a1, 0x18($sp)
    ctx->r5 = MEM_W(ctx->r29, 0X18);
    // 0x804149E0: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x804149E4: addiu       $a3, $zero, 0xF2
    ctx->r7 = ADD32(0, 0XF2);
    // 0x804149E8: addiu       $a2, $zero, 0xBD
    ctx->r6 = ADD32(0, 0XBD);
    // 0x804149EC: jal         0x80029B58
    // 0x804149F0: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_4;
    // 0x804149F0: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_4:
    // 0x804149F4: lw          $a1, 0x18($sp)
    ctx->r5 = MEM_W(ctx->r29, 0X18);
    // 0x804149F8: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x804149FC: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80414A00: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x80414A04: jal         0x80029B58
    // 0x80414A08: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_5;
    // 0x80414A08: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_5:
    // 0x80414A0C: addiu       $s3, $s0, 0x29
    ctx->r19 = ADD32(ctx->r16, 0X29);
    // 0x80414A10: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x80414A14: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80414A18: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x80414A1C: addiu       $a1, $s0, 0x118
    ctx->r5 = ADD32(ctx->r16, 0X118);
    // 0x80414A20: jal         0x80029B58
    // 0x80414A24: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_6;
    // 0x80414A24: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_6:
    // 0x80414A28: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80414A2C: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414A30: addiu       $a3, $zero, 0xED
    ctx->r7 = ADD32(0, 0XED);
    // 0x80414A34: addiu       $a2, $zero, 0x1F
    ctx->r6 = ADD32(0, 0X1F);
    // 0x80414A38: jal         0x80029B58
    // 0x80414A3C: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    func_80029B58(rdram, ctx);
        goto after_7;
    // 0x80414A3C: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    after_7:
    // 0x80414A40: addiu       $v0, $zero, 0x9B
    ctx->r2 = ADD32(0, 0X9B);
    // 0x80414A44: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80414A48: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414A4C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80414A50: addiu       $a2, $zero, 0x20
    ctx->r6 = ADD32(0, 0X20);
    // 0x80414A54: jal         0x80029B58
    // 0x80414A58: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    func_80029B58(rdram, ctx);
        goto after_8;
    // 0x80414A58: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    after_8:
    // 0x80414A5C: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80414A60: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414A64: addiu       $a3, $zero, 0xEE
    ctx->r7 = ADD32(0, 0XEE);
    // 0x80414A68: addiu       $a2, $zero, 0xBB
    ctx->r6 = ADD32(0, 0XBB);
    // 0x80414A6C: jal         0x80029B58
    // 0x80414A70: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_9;
    // 0x80414A70: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    after_9:
    // 0x80414A74: addiu       $v0, $zero, 0x9C
    ctx->r2 = ADD32(0, 0X9C);
    // 0x80414A78: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414A7C: addiu       $s2, $zero, 0x7
    ctx->r18 = ADD32(0, 0X7);
    // 0x80414A80: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80414A84: addiu       $a2, $zero, 0x1F
    ctx->r6 = ADD32(0, 0X1F);
    // 0x80414A88: addiu       $a1, $s0, 0x116
    ctx->r5 = ADD32(ctx->r16, 0X116);
    // 0x80414A8C: jal         0x80029B58
    // 0x80414A90: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_10;
    // 0x80414A90: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    after_10:
    // 0x80414A94: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414A98: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
    // 0x80414A9C: addiu       $a2, $zero, 0x1C
    ctx->r6 = ADD32(0, 0X1C);
    // 0x80414AA0: addiu       $a1, $s0, 0x45
    ctx->r5 = ADD32(ctx->r16, 0X45);
    // 0x80414AA4: jal         0x80029B58
    // 0x80414AA8: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    func_80029B58(rdram, ctx);
        goto after_11;
    // 0x80414AA8: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    after_11:
    // 0x80414AAC: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414AB0: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
    // 0x80414AB4: addiu       $a2, $zero, 0x1C
    ctx->r6 = ADD32(0, 0X1C);
    // 0x80414AB8: addiu       $a1, $s0, 0xF3
    ctx->r5 = ADD32(ctx->r16, 0XF3);
    // 0x80414ABC: jal         0x80029B58
    // 0x80414AC0: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    func_80029B58(rdram, ctx);
        goto after_12;
    // 0x80414AC0: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    after_12:
    // 0x80414AC4: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x80414AC8: lw          $v1, -0x2F0($v0)
    ctx->r3 = MEM_W(ctx->r2, -0X2F0);
    // 0x80414ACC: addiu       $s4, $zero, 0xB
    ctx->r20 = ADD32(0, 0XB);
    // 0x80414AD0: sra         $v0, $v1, 31
    ctx->r2 = S32(SIGNED(ctx->r3) >> 31);
    // 0x80414AD4: andi        $v0, $v0, 0x3
    ctx->r2 = ctx->r2 & 0X3;
    // 0x80414AD8: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80414ADC: sra         $v1, $v0, 31
    ctx->r3 = S32(SIGNED(ctx->r2) >> 31);
    // 0x80414AE0: srl         $v1, $v1, 28
    ctx->r3 = S32(U32(ctx->r3) >> 28);
    // 0x80414AE4: sra         $v0, $v0, 2
    ctx->r2 = S32(SIGNED(ctx->r2) >> 2);
    // 0x80414AE8: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80414AEC: andi        $v0, $v0, 0xF
    ctx->r2 = ctx->r2 & 0XF;
    // 0x80414AF0: subu        $v0, $v0, $v1
    ctx->r2 = SUB32(ctx->r2, ctx->r3);
    // 0x80414AF4: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x80414AF8: addiu       $v1, $v1, -0xF40
    ctx->r3 = ADD32(ctx->r3, -0XF40);
    // 0x80414AFC: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80414B00: lb          $s2, 0x0($v0)
    ctx->r18 = MEM_B(ctx->r2, 0X0);
    // 0x80414B04: addiu       $a3, $zero, 0xE
    ctx->r7 = ADD32(0, 0XE);
    // 0x80414B08: addu        $a1, $s0, $s2
    ctx->r5 = ADD32(ctx->r16, ctx->r18);
    // 0x80414B0C: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x80414B10: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x80414B14: addiu       $a1, $a1, 0x36
    ctx->r5 = ADD32(ctx->r5, 0X36);
    // 0x80414B18: jal         0x80029B58
    // 0x80414B1C: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    func_80029B58(rdram, ctx);
        goto after_13;
    // 0x80414B1C: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    after_13:
    // 0x80414B20: subu        $a1, $s0, $s2
    ctx->r5 = SUB32(ctx->r16, ctx->r18);
    // 0x80414B24: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x80414B28: addiu       $a3, $zero, 0xE
    ctx->r7 = ADD32(0, 0XE);
    // 0x80414B2C: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x80414B30: addiu       $a1, $a1, 0xFB
    ctx->r5 = ADD32(ctx->r5, 0XFB);
    // 0x80414B34: jal         0x80029B58
    // 0x80414B38: addiu       $a0, $zero, 0x7
    ctx->r4 = ADD32(0, 0X7);
    func_80029B58(rdram, ctx);
        goto after_14;
    // 0x80414B38: addiu       $a0, $zero, 0x7
    ctx->r4 = ADD32(0, 0X7);
    after_14:
    // 0x80414B3C: addiu       $v0, $zero, 0x10
    ctx->r2 = ADD32(0, 0X10);
    // 0x80414B40: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414B44: addiu       $s2, $zero, 0x6
    ctx->r18 = ADD32(0, 0X6);
    // 0x80414B48: addiu       $s3, $s0, 0xA5
    ctx->r19 = ADD32(ctx->r16, 0XA5);
    // 0x80414B4C: addiu       $a3, $zero, 0x3A
    ctx->r7 = ADD32(0, 0X3A);
    // 0x80414B50: addiu       $a2, $zero, 0x19
    ctx->r6 = ADD32(0, 0X19);
    // 0x80414B54: addiu       $a1, $s0, 0x81
    ctx->r5 = ADD32(ctx->r16, 0X81);
    // 0x80414B58: jal         0x80029B58
    // 0x80414B5C: addiu       $a0, $zero, 0x2D
    ctx->r4 = ADD32(0, 0X2D);
    func_80029B58(rdram, ctx);
        goto after_15;
    // 0x80414B5C: addiu       $a0, $zero, 0x2D
    ctx->r4 = ADD32(0, 0X2D);
    after_15:
    // 0x80414B60: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80414B64: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414B68: addiu       $a3, $zero, 0x35
    ctx->r7 = ADD32(0, 0X35);
    // 0x80414B6C: addiu       $a2, $zero, 0x35
    ctx->r6 = ADD32(0, 0X35);
    // 0x80414B70: jal         0x80029B58
    // 0x80414B74: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    func_80029B58(rdram, ctx);
        goto after_16;
    // 0x80414B74: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    after_16:
    // 0x80414B78: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414B7C: addiu       $a3, $zero, 0x35
    ctx->r7 = ADD32(0, 0X35);
    // 0x80414B80: addiu       $a2, $zero, 0x78
    ctx->r6 = ADD32(0, 0X78);
    // 0x80414B84: addiu       $a1, $s0, 0x31
    ctx->r5 = ADD32(ctx->r16, 0X31);
    // 0x80414B88: jal         0x80029B58
    // 0x80414B8C: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    func_80029B58(rdram, ctx);
        goto after_17;
    // 0x80414B8C: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    after_17:
    // 0x80414B90: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414B94: addiu       $a3, $zero, 0x35
    ctx->r7 = ADD32(0, 0X35);
    // 0x80414B98: addiu       $a2, $zero, 0x78
    ctx->r6 = ADD32(0, 0X78);
    // 0x80414B9C: addiu       $a1, $s0, 0xD9
    ctx->r5 = ADD32(ctx->r16, 0XD9);
    // 0x80414BA0: jal         0x80029B58
    // 0x80414BA4: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    func_80029B58(rdram, ctx);
        goto after_18;
    // 0x80414BA4: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    after_18:
    // 0x80414BA8: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80414BAC: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414BB0: addiu       $s3, $zero, 0xA
    ctx->r19 = ADD32(0, 0XA);
    // 0x80414BB4: addiu       $s2, $s0, 0x86
    ctx->r18 = ADD32(ctx->r16, 0X86);
    // 0x80414BB8: addiu       $a3, $zero, 0x35
    ctx->r7 = ADD32(0, 0X35);
    // 0x80414BBC: addiu       $a2, $zero, 0xAE
    ctx->r6 = ADD32(0, 0XAE);
    // 0x80414BC0: jal         0x80029B58
    // 0x80414BC4: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    func_80029B58(rdram, ctx);
        goto after_19;
    // 0x80414BC4: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
    after_19:
    // 0x80414BC8: or          $a1, $s2, $zero
    ctx->r5 = ctx->r18 | 0;
    // 0x80414BCC: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414BD0: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80414BD4: addiu       $a2, $zero, 0x34
    ctx->r6 = ADD32(0, 0X34);
    // 0x80414BD8: jal         0x80029B58
    // 0x80414BDC: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    func_80029B58(rdram, ctx);
        goto after_20;
    // 0x80414BDC: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    after_20:
    // 0x80414BE0: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414BE4: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80414BE8: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414BEC: addiu       $a1, $s0, 0x47
    ctx->r5 = ADD32(ctx->r16, 0X47);
    // 0x80414BF0: jal         0x80029B58
    // 0x80414BF4: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    func_80029B58(rdram, ctx);
        goto after_21;
    // 0x80414BF4: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    after_21:
    // 0x80414BF8: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414BFC: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80414C00: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414C04: addiu       $a1, $s0, 0xF0
    ctx->r5 = ADD32(ctx->r16, 0XF0);
    // 0x80414C08: jal         0x80029B58
    // 0x80414C0C: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    func_80029B58(rdram, ctx);
        goto after_22;
    // 0x80414C0C: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    after_22:
    // 0x80414C10: addiu       $s5, $s0, 0x9A
    ctx->r21 = ADD32(ctx->r16, 0X9A);
    // 0x80414C14: or          $a1, $s2, $zero
    ctx->r5 = ctx->r18 | 0;
    // 0x80414C18: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414C1C: addiu       $s2, $zero, 0xC
    ctx->r18 = ADD32(0, 0XC);
    // 0x80414C20: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80414C24: addiu       $a2, $zero, 0xAD
    ctx->r6 = ADD32(0, 0XAD);
    // 0x80414C28: jal         0x80029B58
    // 0x80414C2C: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    func_80029B58(rdram, ctx);
        goto after_23;
    // 0x80414C2C: addiu       $a0, $zero, 0x14
    ctx->r4 = ADD32(0, 0X14);
    after_23:
    // 0x80414C30: or          $a1, $s5, $zero
    ctx->r5 = ctx->r21 | 0;
    // 0x80414C34: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414C38: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x80414C3C: addiu       $a2, $zero, 0x3D
    ctx->r6 = ADD32(0, 0X3D);
    // 0x80414C40: jal         0x80029B58
    // 0x80414C44: addiu       $a0, $zero, 0x2E
    ctx->r4 = ADD32(0, 0X2E);
    func_80029B58(rdram, ctx);
        goto after_24;
    // 0x80414C44: addiu       $a0, $zero, 0x2E
    ctx->r4 = ADD32(0, 0X2E);
    after_24:
    // 0x80414C48: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414C4C: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x80414C50: addiu       $a2, $zero, 0x6F
    ctx->r6 = ADD32(0, 0X6F);
    // 0x80414C54: addiu       $a1, $s0, 0x67
    ctx->r5 = ADD32(ctx->r16, 0X67);
    // 0x80414C58: jal         0x80029B58
    // 0x80414C5C: addiu       $a0, $zero, 0x2F
    ctx->r4 = ADD32(0, 0X2F);
    func_80029B58(rdram, ctx);
        goto after_25;
    // 0x80414C5C: addiu       $a0, $zero, 0x2F
    ctx->r4 = ADD32(0, 0X2F);
    after_25:
    // 0x80414C60: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414C64: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x80414C68: addiu       $a2, $zero, 0x6F
    ctx->r6 = ADD32(0, 0X6F);
    // 0x80414C6C: addiu       $a1, $s0, 0xCC
    ctx->r5 = ADD32(ctx->r16, 0XCC);
    // 0x80414C70: jal         0x80029B58
    // 0x80414C74: addiu       $a0, $zero, 0x30
    ctx->r4 = ADD32(0, 0X30);
    func_80029B58(rdram, ctx);
        goto after_26;
    // 0x80414C74: addiu       $a0, $zero, 0x30
    ctx->r4 = ADD32(0, 0X30);
    after_26:
    // 0x80414C78: or          $a1, $s5, $zero
    ctx->r5 = ctx->r21 | 0;
    // 0x80414C7C: sw          $s2, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r18;
    // 0x80414C80: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x80414C84: addiu       $a2, $zero, 0xA1
    ctx->r6 = ADD32(0, 0XA1);
    // 0x80414C88: jal         0x80029B58
    // 0x80414C8C: addiu       $a0, $zero, 0x31
    ctx->r4 = ADD32(0, 0X31);
    func_80029B58(rdram, ctx);
        goto after_27;
    // 0x80414C8C: addiu       $a0, $zero, 0x31
    ctx->r4 = ADD32(0, 0X31);
    after_27:
    // 0x80414C90: addiu       $s2, $s0, 0x8D
    ctx->r18 = ADD32(ctx->r16, 0X8D);
    // 0x80414C94: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x80414C98: addiu       $a3, $zero, 0x14
    ctx->r7 = ADD32(0, 0X14);
    // 0x80414C9C: addiu       $a2, $zero, 0x92
    ctx->r6 = ADD32(0, 0X92);
    // 0x80414CA0: addiu       $a1, $s0, 0xA7
    ctx->r5 = ADD32(ctx->r16, 0XA7);
    // 0x80414CA4: jal         0x80029B58
    // 0x80414CA8: addiu       $a0, $zero, 0x20
    ctx->r4 = ADD32(0, 0X20);
    func_80029B58(rdram, ctx);
        goto after_28;
    // 0x80414CA8: addiu       $a0, $zero, 0x20
    ctx->r4 = ADD32(0, 0X20);
    after_28:
    // 0x80414CAC: or          $a1, $s2, $zero
    ctx->r5 = ctx->r18 | 0;
    // 0x80414CB0: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414CB4: addiu       $a3, $zero, 0x63
    ctx->r7 = ADD32(0, 0X63);
    // 0x80414CB8: addiu       $a2, $zero, 0x34
    ctx->r6 = ADD32(0, 0X34);
    // 0x80414CBC: jal         0x8002AB64
    // 0x80414CC0: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_29;
    // 0x80414CC0: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_29:
    // 0x80414CC4: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414CC8: addiu       $a3, $zero, 0x63
    ctx->r7 = ADD32(0, 0X63);
    // 0x80414CCC: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414CD0: addiu       $a1, $s0, 0x4E
    ctx->r5 = ADD32(ctx->r16, 0X4E);
    // 0x80414CD4: jal         0x8002AB64
    // 0x80414CD8: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_30;
    // 0x80414CD8: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_30:
    // 0x80414CDC: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414CE0: addiu       $a3, $zero, 0x63
    ctx->r7 = ADD32(0, 0X63);
    // 0x80414CE4: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414CE8: addiu       $a1, $s0, 0xF7
    ctx->r5 = ADD32(ctx->r16, 0XF7);
    // 0x80414CEC: jal         0x8002AB64
    // 0x80414CF0: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_31;
    // 0x80414CF0: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_31:
    // 0x80414CF4: or          $a1, $s2, $zero
    ctx->r5 = ctx->r18 | 0;
    // 0x80414CF8: lui         $s2, 0x8008
    ctx->r18 = S32(0X8008 << 16);
    // 0x80414CFC: addiu       $s2, $s2, -0x4580
    ctx->r18 = ADD32(ctx->r18, -0X4580);
    // 0x80414D00: addiu       $a3, $zero, 0x63
    ctx->r7 = ADD32(0, 0X63);
    // 0x80414D04: addiu       $a2, $zero, 0xAD
    ctx->r6 = ADD32(0, 0XAD);
    // 0x80414D08: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    // 0x80414D0C: jal         0x8002AB64
    // 0x80414D10: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    func_8002AB64(rdram, ctx);
        goto after_32;
    // 0x80414D10: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    after_32:
    // 0x80414D14: addiu       $s4, $s0, 0x78
    ctx->r20 = ADD32(ctx->r16, 0X78);
    // 0x80414D18: lbu         $a3, 0x24($s2)
    ctx->r7 = MEM_BU(ctx->r18, 0X24);
    // 0x80414D1C: or          $a1, $s4, $zero
    ctx->r5 = ctx->r20 | 0;
    // 0x80414D20: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414D24: addiu       $a2, $zero, 0x34
    ctx->r6 = ADD32(0, 0X34);
    // 0x80414D28: jal         0x8002AB64
    // 0x80414D2C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_33;
    // 0x80414D2C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_33:
    // 0x80414D30: lbu         $a3, 0x25($s2)
    ctx->r7 = MEM_BU(ctx->r18, 0X25);
    // 0x80414D34: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414D38: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414D3C: addiu       $a1, $s0, 0x39
    ctx->r5 = ADD32(ctx->r16, 0X39);
    // 0x80414D40: jal         0x8002AB64
    // 0x80414D44: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_34;
    // 0x80414D44: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_34:
    // 0x80414D48: lbu         $a3, 0x27($s2)
    ctx->r7 = MEM_BU(ctx->r18, 0X27);
    // 0x80414D4C: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x80414D50: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414D54: addiu       $a1, $s0, 0xE2
    ctx->r5 = ADD32(ctx->r16, 0XE2);
    // 0x80414D58: jal         0x8002AB64
    // 0x80414D5C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_35;
    // 0x80414D5C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_35:
    // 0x80414D60: lbu         $a3, 0x26($s2)
    ctx->r7 = MEM_BU(ctx->r18, 0X26);
    // 0x80414D64: or          $a1, $s4, $zero
    ctx->r5 = ctx->r20 | 0;
    // 0x80414D68: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x80414D6C: addiu       $a2, $zero, 0xAD
    ctx->r6 = ADD32(0, 0XAD);
    // 0x80414D70: jal         0x8002AB64
    // 0x80414D74: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    func_8002AB64(rdram, ctx);
        goto after_36;
    // 0x80414D74: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    after_36:
    // 0x80414D78: lbu         $a1, 0x25($s2)
    ctx->r5 = MEM_BU(ctx->r18, 0X25);
    // 0x80414D7C: lbu         $a0, 0x24($s2)
    ctx->r4 = MEM_BU(ctx->r18, 0X24);
    // 0x80414D80: mtc1        $a1, $f0
    ctx->f0.u32l = ctx->r5;
    // 0x80414D84: lbu         $v1, 0x27($s2)
    ctx->r3 = MEM_BU(ctx->r18, 0X27);
    // 0x80414D88: cvt.s.w     $f6, $f0
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 0);
    ctx->f6.fl = CVT_S_W(ctx->f0.u32l);
    // 0x80414D8C: mtc1        $a0, $f0
    ctx->f0.u32l = ctx->r4;
    // 0x80414D90: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x80414D94: lwc1        $f12, -0x5A74($v0)
    ctx->f12.u32l = MEM_W(ctx->r2, -0X5A74);
    // 0x80414D98: cvt.s.w     $f4, $f0
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 0);
    ctx->f4.fl = CVT_S_W(ctx->f0.u32l);
    // 0x80414D9C: lbu         $v0, 0x26($s2)
    ctx->r2 = MEM_BU(ctx->r18, 0X26);
    // 0x80414DA0: mtc1        $v1, $f0
    ctx->f0.u32l = ctx->r3;
    // 0x80414DA4: div.s       $f6, $f6, $f12
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 12);
    NAN_CHECK(ctx->f6.fl); NAN_CHECK(ctx->f12.fl); 
    ctx->f6.fl = DIV_S(ctx->f6.fl, ctx->f12.fl);
    // 0x80414DA8: cvt.s.w     $f2, $f0
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    ctx->f2.fl = CVT_S_W(ctx->f0.u32l);
    // 0x80414DAC: mtc1        $v0, $f0
    ctx->f0.u32l = ctx->r2;
    // 0x80414DB0: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x80414DB4: cvt.s.w     $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.fl = CVT_S_W(ctx->f0.u32l);
    // 0x80414DB8: lwc1        $f10, -0x5A70($v0)
    ctx->f10.u32l = MEM_W(ctx->r2, -0X5A70);
    // 0x80414DBC: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x80414DC0: lwc1        $f8, -0x5A6C($v0)
    ctx->f8.u32l = MEM_W(ctx->r2, -0X5A6C);
    // 0x80414DC4: addiu       $s6, $s0, 0xA6
    ctx->r22 = ADD32(ctx->r16, 0XA6);
    // 0x80414DC8: or          $a1, $s6, $zero
    ctx->r5 = ctx->r22 | 0;
    // 0x80414DCC: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x80414DD0: addiu       $a2, $zero, 0x36
    ctx->r6 = ADD32(0, 0X36);
    // 0x80414DD4: div.s       $f4, $f4, $f12
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 12);
    NAN_CHECK(ctx->f4.fl); NAN_CHECK(ctx->f12.fl); 
    ctx->f4.fl = DIV_S(ctx->f4.fl, ctx->f12.fl);
    // 0x80414DD8: div.s       $f2, $f2, $f12
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 12);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f12.fl); 
    ctx->f2.fl = DIV_S(ctx->f2.fl, ctx->f12.fl);
    // 0x80414DDC: div.s       $f0, $f0, $f12
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 12);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f12.fl); 
    ctx->f0.fl = DIV_S(ctx->f0.fl, ctx->f12.fl);
    // 0x80414DE0: add.s       $f6, $f6, $f10
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 10);
    NAN_CHECK(ctx->f6.fl); NAN_CHECK(ctx->f10.fl); 
    ctx->f6.fl = ctx->f6.fl + ctx->f10.fl;
    // 0x80414DE4: add.s       $f4, $f4, $f10
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 10);
    NAN_CHECK(ctx->f4.fl); NAN_CHECK(ctx->f10.fl); 
    ctx->f4.fl = ctx->f4.fl + ctx->f10.fl;
    // 0x80414DE8: add.s       $f2, $f2, $f10
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 10);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f10.fl); 
    ctx->f2.fl = ctx->f2.fl + ctx->f10.fl;
    // 0x80414DEC: add.s       $f0, $f0, $f10
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 10);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f10.fl); 
    ctx->f0.fl = ctx->f0.fl + ctx->f10.fl;
    // 0x80414DF0: mul.s       $f6, $f6, $f8
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 8);
    NAN_CHECK(ctx->f6.fl); NAN_CHECK(ctx->f8.fl); 
    ctx->f6.fl = MUL_S(ctx->f6.fl, ctx->f8.fl);
    // 0x80414DF4: mul.s       $f4, $f4, $f8
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 8);
    NAN_CHECK(ctx->f4.fl); NAN_CHECK(ctx->f8.fl); 
    ctx->f4.fl = MUL_S(ctx->f4.fl, ctx->f8.fl);
    // 0x80414DF8: trunc.w.s   $f6, $f6
    CHECK_FR(ctx, 6);
    CHECK_FR(ctx, 6);
    ctx->f6.u32l = TRUNC_W_S(ctx->f6.fl);
    // 0x80414DFC: mfc1        $s5, $f6
    ctx->r21 = (int32_t)ctx->f6.u32l;
    // 0x80414E00: mul.s       $f2, $f2, $f8
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 8);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f8.fl); 
    ctx->f2.fl = MUL_S(ctx->f2.fl, ctx->f8.fl);
    // 0x80414E04: trunc.w.s   $f4, $f4
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 4);
    ctx->f4.u32l = TRUNC_W_S(ctx->f4.fl);
    // 0x80414E08: mfc1        $a3, $f4
    ctx->r7 = (int32_t)ctx->f4.u32l;
    // 0x80414E0C: mul.s       $f0, $f0, $f8
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 8);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f8.fl); 
    ctx->f0.fl = MUL_S(ctx->f0.fl, ctx->f8.fl);
    // 0x80414E10: trunc.w.s   $f2, $f2
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    ctx->f2.u32l = TRUNC_W_S(ctx->f2.fl);
    // 0x80414E14: mfc1        $s4, $f2
    ctx->r20 = (int32_t)ctx->f2.u32l;
    // 0x80414E18: trunc.w.s   $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.u32l = TRUNC_W_S(ctx->f0.fl);
    // 0x80414E1C: mfc1        $s3, $f0
    ctx->r19 = (int32_t)ctx->f0.u32l;
    // 0x80414E20: jal         0x80029B58
    // 0x80414E24: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    func_80029B58(rdram, ctx);
        goto after_37;
    // 0x80414E24: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    after_37:
    // 0x80414E28: or          $a3, $s5, $zero
    ctx->r7 = ctx->r21 | 0;
    // 0x80414E2C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x80414E30: addiu       $a2, $zero, 0x79
    ctx->r6 = ADD32(0, 0X79);
    // 0x80414E34: addiu       $a1, $s0, 0x32
    ctx->r5 = ADD32(ctx->r16, 0X32);
    // 0x80414E38: jal         0x80029B58
    // 0x80414E3C: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    func_80029B58(rdram, ctx);
        goto after_38;
    // 0x80414E3C: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    after_38:
    // 0x80414E40: or          $a3, $s4, $zero
    ctx->r7 = ctx->r20 | 0;
    // 0x80414E44: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x80414E48: addiu       $a2, $zero, 0x79
    ctx->r6 = ADD32(0, 0X79);
    // 0x80414E4C: addiu       $a1, $s0, 0xDA
    ctx->r5 = ADD32(ctx->r16, 0XDA);
    // 0x80414E50: jal         0x80029B58
    // 0x80414E54: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    func_80029B58(rdram, ctx);
        goto after_39;
    // 0x80414E54: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    after_39:
    // 0x80414E58: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x80414E5C: or          $a3, $s3, $zero
    ctx->r7 = ctx->r19 | 0;
    // 0x80414E60: addiu       $a2, $zero, 0xAF
    ctx->r6 = ADD32(0, 0XAF);
    // 0x80414E64: or          $a1, $s6, $zero
    ctx->r5 = ctx->r22 | 0;
    // 0x80414E68: jal         0x80029B58
    // 0x80414E6C: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    func_80029B58(rdram, ctx);
        goto after_40;
    // 0x80414E6C: addiu       $a0, $zero, 0x33
    ctx->r4 = ADD32(0, 0X33);
    after_40:
    // 0x80414E70: lbu         $s1, 0x34($s2)
    ctx->r17 = MEM_BU(ctx->r18, 0X34);
    // 0x80414E74: slti        $v0, $s1, 0x64
    ctx->r2 = SIGNED(ctx->r17) < 0X64 ? 1 : 0;
    // 0x80414E78: beql        $v0, $zero, L_80414E80
    if (ctx->r2 == 0) {
        // 0x80414E7C: addiu       $s1, $zero, 0x63
        ctx->r17 = ADD32(0, 0X63);
            goto L_80414E80;
    }
    goto skip_0;
    // 0x80414E7C: addiu       $s1, $zero, 0x63
    ctx->r17 = ADD32(0, 0X63);
    skip_0:
L_80414E80:
    // 0x80414E80: lw          $s4, 0x10($s2)
    ctx->r20 = MEM_W(ctx->r18, 0X10);
    // 0x80414E84: sll         $s3, $s1, 2
    ctx->r19 = S32(ctx->r17 << 2);
    // 0x80414E88: sll         $a0, $s4, 1
    ctx->r4 = S32(ctx->r20 << 1);
    // 0x80414E8C: lui         $s1, 0x8005
    ctx->r17 = S32(0X8005 << 16);
    // 0x80414E90: addiu       $s1, $s1, 0x3D3C
    ctx->r17 = ADD32(ctx->r17, 0X3D3C);
    // 0x80414E94: addu        $a0, $a0, $s4
    ctx->r4 = ADD32(ctx->r4, ctx->r20);
    // 0x80414E98: addu        $v0, $s1, $s3
    ctx->r2 = ADD32(ctx->r17, ctx->r19);
    // 0x80414E9C: sll         $a0, $a0, 3
    ctx->r4 = S32(ctx->r4 << 3);
    // 0x80414EA0: lw          $s5, 0x0($v0)
    ctx->r21 = MEM_W(ctx->r2, 0X0);
    // 0x80414EA4: addu        $a0, $a0, $s4
    ctx->r4 = ADD32(ctx->r4, ctx->r20);
    // 0x80414EA8: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x80414EAC: divu        $zero, $a0, $s5
    lo = S32(U32(ctx->r4) / U32(ctx->r21)); hi = S32(U32(ctx->r4) % U32(ctx->r21));
    // 0x80414EB0: nop

    // 0x80414EB4: mflo        $a0
    ctx->r4 = lo;
    // 0x80414EB8: slti        $v0, $a0, 0x65
    ctx->r2 = SIGNED(ctx->r4) < 0X65 ? 1 : 0;
    // 0x80414EBC: beql        $v0, $zero, L_80414EC4
    if (ctx->r2 == 0) {
        // 0x80414EC0: addiu       $a0, $zero, 0x64
        ctx->r4 = ADD32(0, 0X64);
            goto L_80414EC4;
    }
    goto skip_1;
    // 0x80414EC0: addiu       $a0, $zero, 0x64
    ctx->r4 = ADD32(0, 0X64);
    skip_1:
L_80414EC4:
    // 0x80414EC4: or          $a1, $s0, $zero
    ctx->r5 = ctx->r16 | 0;
    // 0x80414EC8: jal         0x8002A0B8
    // 0x80414ECC: addu        $s1, $s1, $s3
    ctx->r17 = ADD32(ctx->r17, ctx->r19);
    func_8002A0B8(rdram, ctx);
        goto after_41;
    // 0x80414ECC: addu        $s1, $s1, $s3
    ctx->r17 = ADD32(ctx->r17, ctx->r19);
    after_41:
    // 0x80414ED0: lw          $v0, 0x10($s2)
    ctx->r2 = MEM_W(ctx->r18, 0X10);
    // 0x80414ED4: lw          $v1, 0x0($s1)
    ctx->r3 = MEM_W(ctx->r17, 0X0);
    // 0x80414ED8: slt         $v0, $v1, $v0
    ctx->r2 = SIGNED(ctx->r3) < SIGNED(ctx->r2) ? 1 : 0;
    // 0x80414EDC: beq         $v0, $zero, L_80414FB8
    if (ctx->r2 == 0) {
        // 0x80414EE0: addiu       $a1, $s0, 0x7B
        ctx->r5 = ADD32(ctx->r16, 0X7B);
            goto L_80414FB8;
    }
    // 0x80414EE0: addiu       $a1, $s0, 0x7B
    ctx->r5 = ADD32(ctx->r16, 0X7B);
    // 0x80414EE4: addiu       $v0, $zero, 0xA
    ctx->r2 = ADD32(0, 0XA);
    // 0x80414EE8: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414EEC: or          $a3, $zero, $zero
    ctx->r7 = 0 | 0;
    // 0x80414EF0: addiu       $a2, $zero, 0x92
    ctx->r6 = ADD32(0, 0X92);
    // 0x80414EF4: jal         0x8002AB64
    // 0x80414EF8: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    func_8002AB64(rdram, ctx);
        goto after_42;
    // 0x80414EF8: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    after_42:
    // 0x80414EFC: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_80414F00:
    // 0x80414F00: lhu         $v0, -0x2F4($v1)
    ctx->r2 = MEM_HU(ctx->r3, -0X2F4);
    // 0x80414F04: andi        $a0, $v0, 0x2000
    ctx->r4 = ctx->r2 & 0X2000;
    // 0x80414F08: bnel        $a0, $zero, L_80414F94
    if (ctx->r4 != 0) {
        // 0x80414F0C: lw          $ra, 0x3C($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X3C);
            goto L_80414F94;
    }
    goto skip_2;
    // 0x80414F0C: lw          $ra, 0x3C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X3C);
    skip_2:
    // 0x80414F10: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x80414F14: lb          $a0, 0x2871($a0)
    ctx->r4 = MEM_B(ctx->r4, 0X2871);
    // 0x80414F18: slti        $a1, $a0, 0x15
    ctx->r5 = SIGNED(ctx->r4) < 0X15 ? 1 : 0;
    // 0x80414F1C: beq         $a1, $zero, L_80414FD8
    if (ctx->r5 == 0) {
        // 0x80414F20: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_80414FD8;
    }
    // 0x80414F20: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80414F24: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80414F28: andi        $a2, $a1, 0x10
    ctx->r6 = ctx->r5 & 0X10;
    // 0x80414F2C: bne         $a2, $zero, L_80414FD8
    if (ctx->r6 != 0) {
        // 0x80414F30: slti        $a0, $a0, -0x14
        ctx->r4 = SIGNED(ctx->r4) < -0X14 ? 1 : 0;
            goto L_80414FD8;
    }
    // 0x80414F30: slti        $a0, $a0, -0x14
    ctx->r4 = SIGNED(ctx->r4) < -0X14 ? 1 : 0;
    // 0x80414F34: bne         $a0, $zero, L_80414F44
    if (ctx->r4 != 0) {
        // 0x80414F38: andi        $a1, $a1, 0x2000
        ctx->r5 = ctx->r5 & 0X2000;
            goto L_80414F44;
    }
    // 0x80414F38: andi        $a1, $a1, 0x2000
    ctx->r5 = ctx->r5 & 0X2000;
    // 0x80414F3C: beq         $a1, $zero, L_80414F94
    if (ctx->r5 == 0) {
        // 0x80414F40: lw          $ra, 0x3C($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X3C);
            goto L_80414F94;
    }
    // 0x80414F40: lw          $ra, 0x3C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X3C);
L_80414F44:
    // 0x80414F44: lw          $ra, 0x3C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X3C);
    // 0x80414F48: lw          $s6, 0x38($sp)
    ctx->r22 = MEM_W(ctx->r29, 0X38);
    // 0x80414F4C: lw          $s5, 0x34($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X34);
    // 0x80414F50: lw          $s4, 0x30($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X30);
    // 0x80414F54: lw          $s3, 0x2C($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X2C);
    // 0x80414F58: lw          $s2, 0x28($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X28);
    // 0x80414F5C: lw          $s1, 0x24($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X24);
    // 0x80414F60: lw          $s0, 0x20($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X20);
    // 0x80414F64: lui         $a3, 0x8009
    ctx->r7 = S32(0X8009 << 16);
    // 0x80414F68: ori         $v0, $v0, 0x3000
    ctx->r2 = ctx->r2 | 0X3000;
    // 0x80414F6C: sw          $zero, -0x2FC($a3)
    MEM_W(-0X2FC, ctx->r7) = 0;
    // 0x80414F70: addiu       $t0, $zero, -0x140
    ctx->r8 = ADD32(0, -0X140);
    // 0x80414F74: lui         $a3, 0x8009
    ctx->r7 = S32(0X8009 << 16);
    // 0x80414F78: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x80414F7C: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x80414F80: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x80414F84: addiu       $sp, $sp, 0x40
    ctx->r29 = ADD32(ctx->r29, 0X40);
    // 0x80414F88: sw          $t0, -0x2F8($a3)
    MEM_W(-0X2F8, ctx->r7) = ctx->r8;
    // 0x80414F8C: j           L_800268D4
    // 0x80414F90: sh          $v0, -0x2F4($v1)
    MEM_H(-0X2F4, ctx->r3) = ctx->r2;
    func_800268D4(rdram, ctx);
    return;
    // 0x80414F90: sh          $v0, -0x2F4($v1)
    MEM_H(-0X2F4, ctx->r3) = ctx->r2;
L_80414F94:
    // 0x80414F94: lw          $s6, 0x38($sp)
    ctx->r22 = MEM_W(ctx->r29, 0X38);
    // 0x80414F98: lw          $s5, 0x34($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X34);
    // 0x80414F9C: lw          $s4, 0x30($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X30);
    // 0x80414FA0: lw          $s3, 0x2C($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X2C);
    // 0x80414FA4: lw          $s2, 0x28($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X28);
    // 0x80414FA8: lw          $s1, 0x24($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X24);
    // 0x80414FAC: lw          $s0, 0x20($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X20);
    // 0x80414FB0: jr          $ra
    // 0x80414FB4: addiu       $sp, $sp, 0x40
    ctx->r29 = ADD32(ctx->r29, 0X40);
    return;
    // 0x80414FB4: addiu       $sp, $sp, 0x40
    ctx->r29 = ADD32(ctx->r29, 0X40);
L_80414FB8:
    // 0x80414FB8: addiu       $v0, $zero, 0xA
    ctx->r2 = ADD32(0, 0XA);
    // 0x80414FBC: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80414FC0: subu        $a3, $s5, $s4
    ctx->r7 = SUB32(ctx->r21, ctx->r20);
    // 0x80414FC4: addiu       $a2, $zero, 0x92
    ctx->r6 = ADD32(0, 0X92);
    // 0x80414FC8: jal         0x8002AB64
    // 0x80414FCC: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    func_8002AB64(rdram, ctx);
        goto after_43;
    // 0x80414FCC: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    after_43:
    // 0x80414FD0: b           L_80414F00
    // 0x80414FD4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
        goto L_80414F00;
    // 0x80414FD4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_80414FD8:
    // 0x80414FD8: lw          $ra, 0x3C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X3C);
    // 0x80414FDC: lw          $s6, 0x38($sp)
    ctx->r22 = MEM_W(ctx->r29, 0X38);
    // 0x80414FE0: lw          $s5, 0x34($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X34);
    // 0x80414FE4: lw          $s4, 0x30($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X30);
    // 0x80414FE8: lw          $s3, 0x2C($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X2C);
    // 0x80414FEC: lw          $s2, 0x28($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X28);
    // 0x80414FF0: lw          $s1, 0x24($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X24);
    // 0x80414FF4: lw          $s0, 0x20($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X20);
    // 0x80414FF8: lui         $a3, 0x8009
    ctx->r7 = S32(0X8009 << 16);
    // 0x80414FFC: addiu       $t0, $zero, 0x140
    ctx->r8 = ADD32(0, 0X140);
    // 0x80415000: ori         $v0, $v0, 0x2000
    ctx->r2 = ctx->r2 | 0X2000;
    // 0x80415004: sw          $t0, -0x2FC($a3)
    MEM_W(-0X2FC, ctx->r7) = ctx->r8;
    // 0x80415008: lui         $a3, 0x8009
    ctx->r7 = S32(0X8009 << 16);
    // 0x8041500C: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x80415010: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x80415014: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x80415018: addiu       $sp, $sp, 0x40
    ctx->r29 = ADD32(ctx->r29, 0X40);
    // 0x8041501C: sw          $zero, -0x2F8($a3)
    MEM_W(-0X2F8, ctx->r7) = 0;
    // 0x80415020: j           L_800268D4
    // 0x80415024: sh          $v0, -0x2F4($v1)
    MEM_H(-0X2F4, ctx->r3) = ctx->r2;
    func_800268D4(rdram, ctx);
    return;
    // 0x80415024: sh          $v0, -0x2F4($v1)
    MEM_H(-0X2F4, ctx->r3) = ctx->r2;
;}
RECOMP_FUNC void hm_func_80415028(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80415028: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041502C: lbu         $a2, -0x2E64($v0)
    ctx->r6 = MEM_BU(ctx->r2, -0X2E64);
    // 0x80415030: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x80415034: or          $v0, $zero, $zero
    ctx->r2 = 0 | 0;
    // 0x80415038: addiu       $a1, $zero, 0x9
    ctx->r5 = ADD32(0, 0X9);
L_8041503C:
    // 0x8041503C: srav        $v1, $a2, $v0
    ctx->r3 = S32(SIGNED(ctx->r6) >> (ctx->r2 & 31));
    // 0x80415040: andi        $v1, $v1, 0x1
    ctx->r3 = ctx->r3 & 0X1;
    // 0x80415044: addiu       $v0, $v0, 0x1
    ctx->r2 = ADD32(ctx->r2, 0X1);
    // 0x80415048: bne         $v0, $a1, L_8041503C
    if (ctx->r2 != ctx->r5) {
        // 0x8041504C: addu        $a0, $a0, $v1
        ctx->r4 = ADD32(ctx->r4, ctx->r3);
            goto L_8041503C;
    }
    // 0x8041504C: addu        $a0, $a0, $v1
    ctx->r4 = ADD32(ctx->r4, ctx->r3);
    // 0x80415050: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x80415054: jr          $ra
    // 0x80415058: sw          $a0, -0x5C28($v0)
    MEM_W(-0X5C28, ctx->r2) = ctx->r4;
    return;
    // 0x80415058: sw          $a0, -0x5C28($v0)
    MEM_W(-0X5C28, ctx->r2) = ctx->r4;
;}
RECOMP_FUNC void hm_func_8041505C(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041505C: addiu       $sp, $sp, -0x48
    ctx->r29 = ADD32(ctx->r29, -0X48);
    // 0x80415060: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x80415064: sw          $s0, 0x2C($sp)
    MEM_W(0X2C, ctx->r29) = ctx->r16;
    // 0x80415068: addiu       $s0, $v0, -0x5CDC
    ctx->r16 = ADD32(ctx->r2, -0X5CDC);
    // 0x8041506C: lw          $a1, -0x5CDC($v0)
    ctx->r5 = MEM_W(ctx->r2, -0X5CDC);
    // 0x80415070: lw          $a2, 0x4($s0)
    ctx->r6 = MEM_W(ctx->r16, 0X4);
    // 0x80415074: addiu       $v0, $zero, 0x20
    ctx->r2 = ADD32(0, 0X20);
    // 0x80415078: sw          $s5, 0x40($sp)
    MEM_W(0X40, ctx->r29) = ctx->r21;
    // 0x8041507C: sw          $s4, 0x3C($sp)
    MEM_W(0X3C, ctx->r29) = ctx->r20;
    // 0x80415080: addiu       $s5, $zero, 0x10
    ctx->r21 = ADD32(0, 0X10);
    // 0x80415084: addiu       $s4, $zero, 0xC
    ctx->r20 = ADD32(0, 0XC);
    // 0x80415088: sw          $s3, 0x38($sp)
    MEM_W(0X38, ctx->r29) = ctx->r19;
    // 0x8041508C: sw          $s2, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r18;
    // 0x80415090: lui         $s3, 0x803A
    ctx->r19 = S32(0X803A << 16);
    // 0x80415094: addiu       $s2, $zero, 0x400
    ctx->r18 = ADD32(0, 0X400);
    // 0x80415098: sw          $s1, 0x30($sp)
    MEM_W(0X30, ctx->r29) = ctx->r17;
    // 0x8041509C: sw          $s2, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r18;
    // 0x804150A0: or          $s1, $a0, $zero
    ctx->r17 = ctx->r4 | 0;
    // 0x804150A4: sw          $s2, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r18;
    // 0x804150A8: sw          $v0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r2;
    // 0x804150AC: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x804150B0: addiu       $a2, $a2, -0x6
    ctx->r6 = ADD32(ctx->r6, -0X6);
    // 0x804150B4: addiu       $a1, $a1, -0x6
    ctx->r5 = ADD32(ctx->r5, -0X6);
    // 0x804150B8: sw          $s5, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r21;
    // 0x804150BC: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    // 0x804150C0: sw          $ra, 0x44($sp)
    MEM_W(0X44, ctx->r29) = ctx->r31;
    // 0x804150C4: jal         0x800210FC
    // 0x804150C8: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    func_800210FC(rdram, ctx);
        goto after_0;
    // 0x804150C8: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    after_0:
    // 0x804150CC: lw          $a2, 0xC($s0)
    ctx->r6 = MEM_W(ctx->r16, 0XC);
    // 0x804150D0: lw          $a1, 0x8($s0)
    ctx->r5 = MEM_W(ctx->r16, 0X8);
    // 0x804150D4: addiu       $v0, $zero, 0x2C
    ctx->r2 = ADD32(0, 0X2C);
    // 0x804150D8: sw          $s2, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r18;
    // 0x804150DC: sw          $s2, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r18;
    // 0x804150E0: sw          $v0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r2;
    // 0x804150E4: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x804150E8: addiu       $a2, $a2, -0x6
    ctx->r6 = ADD32(ctx->r6, -0X6);
    // 0x804150EC: addiu       $a1, $a1, -0x6
    ctx->r5 = ADD32(ctx->r5, -0X6);
    // 0x804150F0: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    // 0x804150F4: sw          $s5, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r21;
    // 0x804150F8: jal         0x800210FC
    // 0x804150FC: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    func_800210FC(rdram, ctx);
        goto after_1;
    // 0x804150FC: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    after_1:
    // 0x80415100: lw          $a2, 0x14($s0)
    ctx->r6 = MEM_W(ctx->r16, 0X14);
    // 0x80415104: lw          $a1, 0x10($s0)
    ctx->r5 = MEM_W(ctx->r16, 0X10);
    // 0x80415108: addiu       $v0, $zero, 0x38
    ctx->r2 = ADD32(0, 0X38);
    // 0x8041510C: sw          $s2, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r18;
    // 0x80415110: sw          $s2, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r18;
    // 0x80415114: sw          $v0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r2;
    // 0x80415118: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x8041511C: addiu       $a2, $a2, -0x6
    ctx->r6 = ADD32(ctx->r6, -0X6);
    // 0x80415120: addiu       $a1, $a1, -0x6
    ctx->r5 = ADD32(ctx->r5, -0X6);
    // 0x80415124: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    // 0x80415128: sw          $s5, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r21;
    // 0x8041512C: jal         0x800210FC
    // 0x80415130: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    func_800210FC(rdram, ctx);
        goto after_2;
    // 0x80415130: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    after_2:
    // 0x80415134: lw          $a2, 0x1C($s0)
    ctx->r6 = MEM_W(ctx->r16, 0X1C);
    // 0x80415138: lw          $a1, 0x18($s0)
    ctx->r5 = MEM_W(ctx->r16, 0X18);
    // 0x8041513C: addiu       $v0, $zero, 0x44
    ctx->r2 = ADD32(0, 0X44);
    // 0x80415140: sw          $s2, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r18;
    // 0x80415144: sw          $s2, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r18;
    // 0x80415148: sw          $v0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r2;
    // 0x8041514C: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
    // 0x80415150: addiu       $a2, $a2, -0x6
    ctx->r6 = ADD32(ctx->r6, -0X6);
    // 0x80415154: addiu       $a1, $a1, -0x6
    ctx->r5 = ADD32(ctx->r5, -0X6);
    // 0x80415158: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    // 0x8041515C: sw          $s5, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r21;
    // 0x80415160: jal         0x800210FC
    // 0x80415164: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    func_800210FC(rdram, ctx);
        goto after_3;
    // 0x80415164: sw          $s4, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r20;
    after_3:
    // 0x80415168: addiu       $v0, $zero, 0xA
    ctx->r2 = ADD32(0, 0XA);
    // 0x8041516C: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x80415170: addiu       $a3, $zero, 0x80
    ctx->r7 = ADD32(0, 0X80);
    // 0x80415174: addiu       $a2, $zero, 0x1D
    ctx->r6 = ADD32(0, 0X1D);
    // 0x80415178: addiu       $a1, $zero, 0x20
    ctx->r5 = ADD32(0, 0X20);
    // 0x8041517C: jal         0x80020E2C
    // 0x80415180: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    func_80020E2C(rdram, ctx);
        goto after_4;
    // 0x80415180: addiu       $a0, $s3, 0x6F40
    ctx->r4 = ADD32(ctx->r19, 0X6F40);
    after_4:
    // 0x80415184: addiu       $s1, $s1, 0x24
    ctx->r17 = ADD32(ctx->r17, 0X24);
    // 0x80415188: addiu       $s2, $s0, 0x20
    ctx->r18 = ADD32(ctx->r16, 0X20);
L_8041518C:
    // 0x8041518C: lbu         $a3, 0x0($s1)
    ctx->r7 = MEM_BU(ctx->r17, 0X0);
    // 0x80415190: lw          $a2, 0x4($s0)
    ctx->r6 = MEM_W(ctx->r16, 0X4);
    // 0x80415194: lw          $a1, 0x0($s0)
    ctx->r5 = MEM_W(ctx->r16, 0X0);
    // 0x80415198: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041519C: jal         0x80020D4C
    // 0x804151A0: addiu       $s0, $s0, 0x8
    ctx->r16 = ADD32(ctx->r16, 0X8);
    func_80020D4C(rdram, ctx);
        goto after_5;
    // 0x804151A0: addiu       $s0, $s0, 0x8
    ctx->r16 = ADD32(ctx->r16, 0X8);
    after_5:
    // 0x804151A4: bne         $s0, $s2, L_8041518C
    if (ctx->r16 != ctx->r18) {
        // 0x804151A8: addiu       $s1, $s1, 0x1
        ctx->r17 = ADD32(ctx->r17, 0X1);
            goto L_8041518C;
    }
    // 0x804151A8: addiu       $s1, $s1, 0x1
    ctx->r17 = ADD32(ctx->r17, 0X1);
    // 0x804151AC: lw          $ra, 0x44($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X44);
    // 0x804151B0: lw          $s5, 0x40($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X40);
    // 0x804151B4: lw          $s4, 0x3C($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X3C);
    // 0x804151B8: lw          $s3, 0x38($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X38);
    // 0x804151BC: lw          $s2, 0x34($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X34);
    // 0x804151C0: lw          $s1, 0x30($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X30);
    // 0x804151C4: lw          $s0, 0x2C($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X2C);
    // 0x804151C8: jr          $ra
    // 0x804151CC: addiu       $sp, $sp, 0x48
    ctx->r29 = ADD32(ctx->r29, 0X48);
    return;
    // 0x804151CC: addiu       $sp, $sp, 0x48
    ctx->r29 = ADD32(ctx->r29, 0X48);
;}
RECOMP_FUNC void hm_func_804151EC(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x804151EC: addiu       $sp, $sp, -0x58
    ctx->r29 = ADD32(ctx->r29, -0X58);
    // 0x804151F0: sw          $s4, 0x44($sp)
    MEM_W(0X44, ctx->r29) = ctx->r20;
    // 0x804151F4: or          $s4, $a0, $zero
    ctx->r20 = ctx->r4 | 0;
    // 0x804151F8: sw          $s3, 0x40($sp)
    MEM_W(0X40, ctx->r29) = ctx->r19;
    // 0x804151FC: sw          $s1, 0x38($sp)
    MEM_W(0X38, ctx->r29) = ctx->r17;
    // 0x80415200: or          $s3, $a2, $zero
    ctx->r19 = ctx->r6 | 0;
    // 0x80415204: or          $s1, $a1, $zero
    ctx->r17 = ctx->r5 | 0;
    // 0x80415208: or          $a0, $a3, $zero
    ctx->r4 = ctx->r7 | 0;
    // 0x8041520C: addiu       $a1, $sp, 0x20
    ctx->r5 = ADD32(ctx->r29, 0X20);
    // 0x80415210: or          $a2, $s4, $zero
    ctx->r6 = ctx->r20 | 0;
    // 0x80415214: sw          $ra, 0x54($sp)
    MEM_W(0X54, ctx->r29) = ctx->r31;
    // 0x80415218: sw          $s7, 0x50($sp)
    MEM_W(0X50, ctx->r29) = ctx->r23;
    // 0x8041521C: sw          $s6, 0x4C($sp)
    MEM_W(0X4C, ctx->r29) = ctx->r22;
    // 0x80415220: sw          $s5, 0x48($sp)
    MEM_W(0X48, ctx->r29) = ctx->r21;
    // 0x80415224: sw          $s2, 0x3C($sp)
    MEM_W(0X3C, ctx->r29) = ctx->r18;
    // 0x80415228: jal         0x80024330
    // 0x8041522C: sw          $s0, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r16;
    count_digits(rdram, ctx);
        goto after_0;
    // 0x8041522C: sw          $s0, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r16;
    after_0:
    // 0x80415230: beq         $v0, $zero, L_804152A8
    if (ctx->r2 == 0) {
        // 0x80415234: addiu       $v1, $sp, 0x20
        ctx->r3 = ADD32(ctx->r29, 0X20);
            goto L_804152A8;
    }
    // 0x80415234: addiu       $v1, $sp, 0x20
    ctx->r3 = ADD32(ctx->r29, 0X20);
    // 0x80415238: addu        $s2, $v1, $v0
    ctx->r18 = ADD32(ctx->r3, ctx->r2);
    // 0x8041523C: or          $s0, $v1, $zero
    ctx->r16 = ctx->r3 | 0;
    // 0x80415240: addiu       $s6, $zero, 0x20
    ctx->r22 = ADD32(0, 0X20);
    // 0x80415244: andi        $s4, $s4, 0x6
    ctx->r20 = ctx->r20 & 0X6;
    // 0x80415248: addiu       $s7, $zero, 0x400
    ctx->r23 = ADD32(0, 0X400);
    // 0x8041524C: b           L_80415278
    // 0x80415250: addiu       $s5, $zero, 0x1D
    ctx->r21 = ADD32(0, 0X1D);
        goto L_80415278;
    // 0x80415250: addiu       $s5, $zero, 0x1D
    ctx->r21 = ADD32(0, 0X1D);
L_80415254:
    // 0x80415254: sw          $s7, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r23;
    // 0x80415258: sw          $s7, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r23;
    // 0x8041525C: sw          $s5, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r21;
    // 0x80415260: jal         0x80020F8C
    // 0x80415264: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    func_80020F8C(rdram, ctx);
        goto after_1;
    // 0x80415264: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    after_1:
    // 0x80415268: addiu       $s1, $s1, 0x7
    ctx->r17 = ADD32(ctx->r17, 0X7);
    // 0x8041526C: addiu       $s0, $s0, 0x1
    ctx->r16 = ADD32(ctx->r16, 0X1);
L_80415270:
    // 0x80415270: beq         $s2, $s0, L_804152AC
    if (ctx->r18 == ctx->r16) {
        // 0x80415274: lw          $ra, 0x54($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X54);
            goto L_804152AC;
    }
    // 0x80415274: lw          $ra, 0x54($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X54);
L_80415278:
    // 0x80415278: lbu         $v1, 0x0($s0)
    ctx->r3 = MEM_BU(ctx->r16, 0X0);
    // 0x8041527C: or          $a0, $s1, $zero
    ctx->r4 = ctx->r17 | 0;
    // 0x80415280: addiu       $v0, $v1, 0x4
    ctx->r2 = ADD32(ctx->r3, 0X4);
    // 0x80415284: sll         $v0, $v0, 3
    ctx->r2 = S32(ctx->r2 << 3);
    // 0x80415288: addiu       $a3, $zero, 0xA
    ctx->r7 = ADD32(0, 0XA);
    // 0x8041528C: addiu       $a2, $zero, 0x8
    ctx->r6 = ADD32(0, 0X8);
    // 0x80415290: bne         $v1, $s6, L_80415254
    if (ctx->r3 != ctx->r22) {
        // 0x80415294: or          $a1, $s3, $zero
        ctx->r5 = ctx->r19 | 0;
            goto L_80415254;
    }
    // 0x80415294: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x80415298: beq         $s4, $zero, L_80415270
    if (ctx->r20 == 0) {
        // 0x8041529C: addiu       $s0, $s0, 0x1
        ctx->r16 = ADD32(ctx->r16, 0X1);
            goto L_80415270;
    }
    // 0x8041529C: addiu       $s0, $s0, 0x1
    ctx->r16 = ADD32(ctx->r16, 0X1);
    // 0x804152A0: bne         $s2, $s0, L_80415278
    if (ctx->r18 != ctx->r16) {
        // 0x804152A4: addiu       $s1, $s1, 0x7
        ctx->r17 = ADD32(ctx->r17, 0X7);
            goto L_80415278;
    }
    // 0x804152A4: addiu       $s1, $s1, 0x7
    ctx->r17 = ADD32(ctx->r17, 0X7);
L_804152A8:
    // 0x804152A8: lw          $ra, 0x54($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X54);
L_804152AC:
    // 0x804152AC: lw          $s7, 0x50($sp)
    ctx->r23 = MEM_W(ctx->r29, 0X50);
    // 0x804152B0: lw          $s6, 0x4C($sp)
    ctx->r22 = MEM_W(ctx->r29, 0X4C);
    // 0x804152B4: lw          $s5, 0x48($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X48);
    // 0x804152B8: lw          $s4, 0x44($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X44);
    // 0x804152BC: lw          $s3, 0x40($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X40);
    // 0x804152C0: lw          $s2, 0x3C($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X3C);
    // 0x804152C4: lw          $s1, 0x38($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X38);
    // 0x804152C8: lw          $s0, 0x34($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X34);
    // 0x804152CC: jr          $ra
    // 0x804152D0: addiu       $sp, $sp, 0x58
    ctx->r29 = ADD32(ctx->r29, 0X58);
    return;
    // 0x804152D0: addiu       $sp, $sp, 0x58
    ctx->r29 = ADD32(ctx->r29, 0X58);
;}
RECOMP_FUNC void hm_func_804152D4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
L_800268D4:
    // 0x804152D4: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x804152D8: lhu         $v0, -0x3A6E($v0)
    ctx->r2 = MEM_HU(ctx->r2, -0X3A6E);
    // 0x804152DC: addiu       $sp, $sp, -0x28
    ctx->r29 = ADD32(ctx->r29, -0X28);
    // 0x804152E0: andi        $v1, $v0, 0x1
    ctx->r3 = ctx->r2 & 0X1;
    // 0x804152E4: lw          $v0, 0x10($a0)
    ctx->r2 = MEM_W(ctx->r4, 0X10);
    // 0x804152E8: sw          $s0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r16;
    // 0x804152EC: sw          $ra, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r31;
    // 0x804152F0: sw          $s3, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r19;
    // 0x804152F4: sw          $s2, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r18;
    // 0x804152F8: sw          $s1, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r17;
    // 0x804152FC: bne         $v1, $zero, L_8041537C
    if (ctx->r3 != 0) {
        // 0x80415300: or          $s0, $a0, $zero
        ctx->r16 = ctx->r4 | 0;
            goto L_8041537C;
    }
    // 0x80415300: or          $s0, $a0, $zero
    ctx->r16 = ctx->r4 | 0;
    // 0x80415304: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415308: lhu         $v1, 0x4F10($v1)
    ctx->r3 = MEM_HU(ctx->r3, 0X4F10);
    // 0x8041530C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415310: andi        $v1, $v1, 0x1
    ctx->r3 = ctx->r3 & 0X1;
    // 0x80415314: beq         $v1, $zero, L_804155A8
    if (ctx->r3 == 0) {
        // 0x80415318: lwc1        $f0, -0x45A4($a0)
        ctx->f0.u32l = MEM_W(ctx->r4, -0X45A4);
            goto L_804155A8;
    }
    // 0x80415318: lwc1        $f0, -0x45A4($a0)
    ctx->f0.u32l = MEM_W(ctx->r4, -0X45A4);
    // 0x8041531C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415320: lwc1        $f2, -0x43F0($v1)
    ctx->f2.u32l = MEM_W(ctx->r3, -0X43F0);
    // 0x80415324: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80415328: add.s       $f2, $f0, $f2
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f2.fl = ctx->f0.fl + ctx->f2.fl;
    // 0x8041532C: lwc1        $f4, -0x5A68($a0)
    ctx->f4.u32l = MEM_W(ctx->r4, -0X5A68);
    // 0x80415330: c.lt.s      $f4, $f2
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 2);
    c1cs = ctx->f4.fl < ctx->f2.fl;
    // 0x80415334: nop

    // 0x80415338: bc1tl       L_80415340
    if (c1cs) {
        // 0x8041533C: sub.s       $f2, $f2, $f4
        CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 4);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f4.fl); 
    ctx->f2.fl = ctx->f2.fl - ctx->f4.fl;
            goto L_80415340;
    }
    goto skip_0;
    // 0x8041533C: sub.s       $f2, $f2, $f4
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 4);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f4.fl); 
    ctx->f2.fl = ctx->f2.fl - ctx->f4.fl;
    skip_0:
L_80415340:
    // 0x80415340: swc1        $f2, -0x43F0($v1)
    MEM_W(-0X43F0, ctx->r3) = ctx->f2.u32l;
    // 0x80415344: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415348: lwc1        $f2, -0x43E8($v1)
    ctx->f2.u32l = MEM_W(ctx->r3, -0X43E8);
    // 0x8041534C: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80415350: add.s       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f0.fl = ctx->f0.fl + ctx->f2.fl;
    // 0x80415354: lwc1        $f2, -0x5A64($a0)
    ctx->f2.u32l = MEM_W(ctx->r4, -0X5A64);
    // 0x80415358: c.lt.s      $f2, $f0
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    c1cs = ctx->f2.fl < ctx->f0.fl;
    // 0x8041535C: nop

    // 0x80415360: bc1fl       L_8041537C
    if (!c1cs) {
        // 0x80415364: swc1        $f0, -0x43E8($v1)
        MEM_W(-0X43E8, ctx->r3) = ctx->f0.u32l;
            goto L_8041537C;
    }
    goto skip_1;
    // 0x80415364: swc1        $f0, -0x43E8($v1)
    MEM_W(-0X43E8, ctx->r3) = ctx->f0.u32l;
    skip_1:
    // 0x80415368: lhu         $a0, 0x2C($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X2C);
    // 0x8041536C: sub.s       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f0.fl = ctx->f0.fl - ctx->f2.fl;
    // 0x80415370: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x80415374: sh          $a0, 0x2C($v0)
    MEM_H(0X2C, ctx->r2) = ctx->r4;
    // 0x80415378: swc1        $f0, -0x43E8($v1)
    MEM_W(-0X43E8, ctx->r3) = ctx->f0.u32l;
L_8041537C:
    // 0x8041537C: lw          $v1, 0x64($a1)
    ctx->r3 = MEM_W(ctx->r5, 0X64);
    // 0x80415380: lhu         $a0, 0x6($v1)
    ctx->r4 = MEM_HU(ctx->r3, 0X6);
    // 0x80415384: sltiu       $a1, $a0, 0x3E7
    ctx->r5 = ctx->r4 < 0X3E7 ? 1 : 0;
    // 0x80415388: beql        $a1, $zero, L_804153B8
    if (ctx->r5 == 0) {
        // 0x8041538C: sh          $zero, 0x28($v0)
        MEM_H(0X28, ctx->r2) = 0;
            goto L_804153B8;
    }
    goto skip_2;
    // 0x8041538C: sh          $zero, 0x28($v0)
    MEM_H(0X28, ctx->r2) = 0;
    skip_2:
    // 0x80415390: lbu         $a3, 0x30($v0)
    ctx->r7 = MEM_BU(ctx->r2, 0X30);
    // 0x80415394: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x80415398: addiu       $a2, $a2, 0x3ECC
    ctx->r6 = ADD32(ctx->r6, 0X3ECC);
    // 0x8041539C: sll         $a1, $a3, 1
    ctx->r5 = S32(ctx->r7 << 1);
    // 0x804153A0: addu        $a1, $a1, $a2
    ctx->r5 = ADD32(ctx->r5, ctx->r6);
    // 0x804153A4: lhu         $a1, 0x0($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X0);
    // 0x804153A8: lhu         $a2, 0x28($v0)
    ctx->r6 = MEM_HU(ctx->r2, 0X28);
    // 0x804153AC: sltu        $t0, $a2, $a1
    ctx->r8 = ctx->r6 < ctx->r5 ? 1 : 0;
    // 0x804153B0: beql        $t0, $zero, L_8041557C
    if (ctx->r8 == 0) {
        // 0x804153B4: lhu         $t0, 0x4($v1)
        ctx->r8 = MEM_HU(ctx->r3, 0X4);
            goto L_8041557C;
    }
    goto skip_3;
    // 0x804153B4: lhu         $t0, 0x4($v1)
    ctx->r8 = MEM_HU(ctx->r3, 0X4);
    skip_3:
L_804153B8:
    // 0x804153B8: lhu         $a0, 0xA($v1)
    ctx->r4 = MEM_HU(ctx->r3, 0XA);
    // 0x804153BC: sltiu       $a1, $a0, 0x3E7
    ctx->r5 = ctx->r4 < 0X3E7 ? 1 : 0;
    // 0x804153C0: beql        $a1, $zero, L_804153F4
    if (ctx->r5 == 0) {
        // 0x804153C4: sh          $zero, 0x2A($v0)
        MEM_H(0X2A, ctx->r2) = 0;
            goto L_804153F4;
    }
    goto skip_4;
    // 0x804153C4: sh          $zero, 0x2A($v0)
    MEM_H(0X2A, ctx->r2) = 0;
    skip_4:
    // 0x804153C8: lbu         $a3, 0x31($v0)
    ctx->r7 = MEM_BU(ctx->r2, 0X31);
    // 0x804153CC: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x804153D0: addiu       $a2, $a2, 0x3ECC
    ctx->r6 = ADD32(ctx->r6, 0X3ECC);
    // 0x804153D4: sll         $a1, $a3, 1
    ctx->r5 = S32(ctx->r7 << 1);
    // 0x804153D8: addu        $a1, $a1, $a2
    ctx->r5 = ADD32(ctx->r5, ctx->r6);
    // 0x804153DC: lhu         $a1, 0x0($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X0);
    // 0x804153E0: lhu         $a2, 0x2A($v0)
    ctx->r6 = MEM_HU(ctx->r2, 0X2A);
    // 0x804153E4: sll         $a1, $a1, 2
    ctx->r5 = S32(ctx->r5 << 2);
    // 0x804153E8: slt         $t0, $a2, $a1
    ctx->r8 = SIGNED(ctx->r6) < SIGNED(ctx->r5) ? 1 : 0;
    // 0x804153EC: beql        $t0, $zero, L_80415644
    if (ctx->r8 == 0) {
        // 0x804153F0: lhu         $t0, 0x8($v1)
        ctx->r8 = MEM_HU(ctx->r3, 0X8);
            goto L_80415644;
    }
    goto skip_5;
    // 0x804153F0: lhu         $t0, 0x8($v1)
    ctx->r8 = MEM_HU(ctx->r3, 0X8);
    skip_5:
L_804153F4:
    // 0x804153F4: lhu         $a0, 0xC($v1)
    ctx->r4 = MEM_HU(ctx->r3, 0XC);
    // 0x804153F8: sltiu       $a1, $a0, 0x3E7
    ctx->r5 = ctx->r4 < 0X3E7 ? 1 : 0;
    // 0x804153FC: beql        $a1, $zero, L_8041542C
    if (ctx->r5 == 0) {
        // 0x80415400: sh          $zero, 0x2C($v0)
        MEM_H(0X2C, ctx->r2) = 0;
            goto L_8041542C;
    }
    goto skip_6;
    // 0x80415400: sh          $zero, 0x2C($v0)
    MEM_H(0X2C, ctx->r2) = 0;
    skip_6:
    // 0x80415404: lbu         $a3, 0x32($v0)
    ctx->r7 = MEM_BU(ctx->r2, 0X32);
    // 0x80415408: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x8041540C: addiu       $a2, $a2, 0x3ECC
    ctx->r6 = ADD32(ctx->r6, 0X3ECC);
    // 0x80415410: sll         $a1, $a3, 1
    ctx->r5 = S32(ctx->r7 << 1);
    // 0x80415414: addu        $a1, $a1, $a2
    ctx->r5 = ADD32(ctx->r5, ctx->r6);
    // 0x80415418: lhu         $a1, 0x0($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X0);
    // 0x8041541C: lhu         $a2, 0x2C($v0)
    ctx->r6 = MEM_HU(ctx->r2, 0X2C);
    // 0x80415420: sltu        $t0, $a2, $a1
    ctx->r8 = ctx->r6 < ctx->r5 ? 1 : 0;
    // 0x80415424: beql        $t0, $zero, L_8041560C
    if (ctx->r8 == 0) {
        // 0x80415428: subu        $a2, $a2, $a1
        ctx->r6 = SUB32(ctx->r6, ctx->r5);
            goto L_8041560C;
    }
    goto skip_7;
    // 0x80415428: subu        $a2, $a2, $a1
    ctx->r6 = SUB32(ctx->r6, ctx->r5);
    skip_7:
L_8041542C:
    // 0x8041542C: lhu         $a0, 0xE($v1)
    ctx->r4 = MEM_HU(ctx->r3, 0XE);
    // 0x80415430: sltiu       $a1, $a0, 0x3E7
    ctx->r5 = ctx->r4 < 0X3E7 ? 1 : 0;
    // 0x80415434: beql        $a1, $zero, L_80415468
    if (ctx->r5 == 0) {
        // 0x80415438: sh          $zero, 0x2E($v0)
        MEM_H(0X2E, ctx->r2) = 0;
            goto L_80415468;
    }
    goto skip_8;
    // 0x80415438: sh          $zero, 0x2E($v0)
    MEM_H(0X2E, ctx->r2) = 0;
    skip_8:
    // 0x8041543C: lbu         $a3, 0x33($v0)
    ctx->r7 = MEM_BU(ctx->r2, 0X33);
    // 0x80415440: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x80415444: addiu       $a2, $a2, 0x3ECC
    ctx->r6 = ADD32(ctx->r6, 0X3ECC);
    // 0x80415448: sll         $a1, $a3, 1
    ctx->r5 = S32(ctx->r7 << 1);
    // 0x8041544C: addu        $a1, $a1, $a2
    ctx->r5 = ADD32(ctx->r5, ctx->r6);
    // 0x80415450: lhu         $a1, 0x0($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X0);
    // 0x80415454: lhu         $a2, 0x2E($v0)
    ctx->r6 = MEM_HU(ctx->r2, 0X2E);
    // 0x80415458: sll         $a1, $a1, 1
    ctx->r5 = S32(ctx->r5 << 1);
    // 0x8041545C: slt         $t0, $a2, $a1
    ctx->r8 = SIGNED(ctx->r6) < SIGNED(ctx->r5) ? 1 : 0;
    // 0x80415460: beq         $t0, $zero, L_80415628
    if (ctx->r8 == 0) {
        // 0x80415464: subu        $a1, $a2, $a1
        ctx->r5 = SUB32(ctx->r6, ctx->r5);
            goto L_80415628;
    }
    // 0x80415464: subu        $a1, $a2, $a1
    ctx->r5 = SUB32(ctx->r6, ctx->r5);
L_80415468:
    // 0x80415468: lui         $s1, 0x8008
    ctx->r17 = S32(0X8008 << 16);
    // 0x8041546C: lw          $a1, -0x4D1C($s1)
    ctx->r5 = MEM_W(ctx->r17, -0X4D1C);
    // 0x80415470: andi        $a0, $a1, 0x8
    ctx->r4 = ctx->r5 & 0X8;
    // 0x80415474: bnel        $a0, $zero, L_804154CC
    if (ctx->r4 != 0) {
        // 0x80415478: andi        $a1, $a1, 0xA
        ctx->r5 = ctx->r5 & 0XA;
            goto L_804154CC;
    }
    goto skip_9;
    // 0x80415478: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
    skip_9:
    // 0x8041547C: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80415480: lw          $a3, -0x5C28($a0)
    ctx->r7 = MEM_W(ctx->r4, -0X5C28);
    // 0x80415484: lbu         $t1, 0x24($v0)
    ctx->r9 = MEM_BU(ctx->r2, 0X24);
    // 0x80415488: sll         $t0, $a3, 2
    ctx->r8 = S32(ctx->r7 << 2);
    // 0x8041548C: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x80415490: lbu         $t2, 0x25($v0)
    ctx->r10 = MEM_BU(ctx->r2, 0X25);
    // 0x80415494: addiu       $a3, $a3, -0x5CBC
    ctx->r7 = ADD32(ctx->r7, -0X5CBC);
    // 0x80415498: addu        $a0, $a3, $t0
    ctx->r4 = ADD32(ctx->r7, ctx->r8);
    // 0x8041549C: lbu         $t3, 0x26($v0)
    ctx->r11 = MEM_BU(ctx->r2, 0X26);
    // 0x804154A0: lw          $a2, 0x0($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X0);
    // 0x804154A4: lbu         $t4, 0x27($v0)
    ctx->r12 = MEM_BU(ctx->r2, 0X27);
    // 0x804154A8: addu        $a0, $t1, $t2
    ctx->r4 = ADD32(ctx->r9, ctx->r10);
    // 0x804154AC: addu        $a0, $a0, $t3
    ctx->r4 = ADD32(ctx->r4, ctx->r11);
    // 0x804154B0: addu        $a0, $a0, $t4
    ctx->r4 = ADD32(ctx->r4, ctx->r12);
    // 0x804154B4: sll         $a2, $a2, 2
    ctx->r6 = S32(ctx->r6 << 2);
    // 0x804154B8: slt         $a0, $a0, $a2
    ctx->r4 = SIGNED(ctx->r4) < SIGNED(ctx->r6) ? 1 : 0;
    // 0x804154BC: bnel        $a0, $zero, L_80415500
    if (ctx->r4 != 0) {
        // 0x804154C0: lbu         $a2, 0x34($v0)
        ctx->r6 = MEM_BU(ctx->r2, 0X34);
            goto L_80415500;
    }
    goto skip_10;
    // 0x804154C0: lbu         $a2, 0x34($v0)
    ctx->r6 = MEM_BU(ctx->r2, 0X34);
    skip_10:
    // 0x804154C4: sw          $zero, 0x10($v1)
    MEM_W(0X10, ctx->r3) = 0;
L_804154C8:
    // 0x804154C8: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
L_804154CC:
    // 0x804154CC: bne         $a1, $zero, L_804154E8
    if (ctx->r5 != 0) {
        // 0x804154D0: lw          $ra, 0x24($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X24);
            goto L_804154E8;
    }
    // 0x804154D0: lw          $ra, 0x24($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X24);
    // 0x804154D4: lui         $s2, 0x8008
    ctx->r18 = S32(0X8008 << 16);
    // 0x804154D8: lw          $a0, -0x4590($s2)
    ctx->r4 = MEM_W(ctx->r18, -0X4590);
    // 0x804154DC: addiu       $s3, $zero, 0xFF
    ctx->r19 = ADD32(0, 0XFF);
    // 0x804154E0: bne         $a0, $s3, L_8041566C
    if (ctx->r4 != ctx->r19) {
        // 0x804154E4: nop
    
            goto L_8041566C;
    }
    // 0x804154E4: nop

L_804154E8:
    // 0x804154E8: lw          $s3, 0x20($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X20);
    // 0x804154EC: lw          $s2, 0x1C($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X1C);
    // 0x804154F0: lw          $s1, 0x18($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X18);
    // 0x804154F4: lw          $s0, 0x14($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X14);
    // 0x804154F8: jr          $ra
    // 0x804154FC: addiu       $sp, $sp, 0x28
    ctx->r29 = ADD32(ctx->r29, 0X28);
    return;
    // 0x804154FC: addiu       $sp, $sp, 0x28
    ctx->r29 = ADD32(ctx->r29, 0X28);
L_80415500:
    // 0x80415500: lw          $t6, 0x10($v1)
    ctx->r14 = MEM_W(ctx->r3, 0X10);
    // 0x80415504: sltiu       $t5, $a2, 0x64
    ctx->r13 = ctx->r6 < 0X64 ? 1 : 0;
    // 0x80415508: beq         $t5, $zero, L_804156C8
    if (ctx->r13 == 0) {
        // 0x8041550C: or          $a0, $a2, $zero
        ctx->r4 = ctx->r6 | 0;
            goto L_804156C8;
    }
    // 0x8041550C: or          $a0, $a2, $zero
    ctx->r4 = ctx->r6 | 0;
    // 0x80415510: andi        $a0, $a0, 0xFF
    ctx->r4 = ctx->r4 & 0XFF;
    // 0x80415514: lui         $t5, 0x8005
    ctx->r13 = S32(0X8005 << 16);
    // 0x80415518: addiu       $t5, $t5, 0x3D3C
    ctx->r13 = ADD32(ctx->r13, 0X3D3C);
    // 0x8041551C: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x80415520: addu        $a0, $a0, $t5
    ctx->r4 = ADD32(ctx->r4, ctx->r13);
    // 0x80415524: lw          $a0, 0x0($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X0);
    // 0x80415528: sltu        $t5, $t6, $a0
    ctx->r13 = ctx->r14 < ctx->r4 ? 1 : 0;
    // 0x8041552C: bnel        $t5, $zero, L_804154CC
    if (ctx->r13 != 0) {
        // 0x80415530: andi        $a1, $a1, 0xA
        ctx->r5 = ctx->r5 & 0XA;
            goto L_804154CC;
    }
    goto skip_11;
    // 0x80415530: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
    skip_11:
    // 0x80415534: subu        $t6, $t6, $a0
    ctx->r14 = SUB32(ctx->r14, ctx->r4);
L_80415538:
    // 0x80415538: sw          $t6, 0x10($v1)
    MEM_W(0X10, ctx->r3) = ctx->r14;
    // 0x8041553C: addu        $a3, $a3, $t0
    ctx->r7 = ADD32(ctx->r7, ctx->r8);
    // 0x80415540: lw          $v1, 0x0($a3)
    ctx->r3 = MEM_W(ctx->r7, 0X0);
    // 0x80415544: addiu       $a2, $a2, 0x1
    ctx->r6 = ADD32(ctx->r6, 0X1);
    // 0x80415548: bne         $t1, $v1, L_80415558
    if (ctx->r9 != ctx->r3) {
        // 0x8041554C: sb          $a2, 0x34($v0)
        MEM_B(0X34, ctx->r2) = ctx->r6;
            goto L_80415558;
    }
    // 0x8041554C: sb          $a2, 0x34($v0)
    MEM_B(0X34, ctx->r2) = ctx->r6;
    // 0x80415550: beq         $t2, $t1, L_804156F8
    if (ctx->r10 == ctx->r9) {
        // 0x80415554: nop
    
            goto L_804156F8;
    }
    // 0x80415554: nop

L_80415558:
    // 0x80415558: ori         $a1, $a1, 0x8
    ctx->r5 = ctx->r5 | 0X8;
L_8041555C:
    // 0x8041555C: sw          $a1, -0x4D1C($s1)
    MEM_W(-0X4D1C, ctx->r17) = ctx->r5;
    // 0x80415560: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x80415564: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x80415568: sh          $zero, 0x0($s0)
    MEM_H(0X0, ctx->r16) = 0;
    // 0x8041556C: jal         0x800268D4
    // 0x80415570: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    func_800268D4(rdram, ctx);
        goto after_0;
    // 0x80415570: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    after_0:
    // 0x80415574: b           L_804154C8
    // 0x80415578: lw          $a1, -0x4D1C($s1)
    ctx->r5 = MEM_W(ctx->r17, -0X4D1C);
        goto L_804154C8;
    // 0x80415578: lw          $a1, -0x4D1C($s1)
    ctx->r5 = MEM_W(ctx->r17, -0X4D1C);
L_8041557C:
    // 0x8041557C: subu        $a2, $a2, $a1
    ctx->r6 = SUB32(ctx->r6, ctx->r5);
    // 0x80415580: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x80415584: addiu       $t0, $t0, 0x1
    ctx->r8 = ADD32(ctx->r8, 0X1);
    // 0x80415588: sltiu       $a1, $a3, 0x29
    ctx->r5 = ctx->r7 < 0X29 ? 1 : 0;
    // 0x8041558C: sh          $a2, 0x28($v0)
    MEM_H(0X28, ctx->r2) = ctx->r6;
    // 0x80415590: sh          $a0, 0x6($v1)
    MEM_H(0X6, ctx->r3) = ctx->r4;
    // 0x80415594: beq         $a1, $zero, L_804153B8
    if (ctx->r5 == 0) {
        // 0x80415598: sh          $t0, 0x4($v1)
        MEM_H(0X4, ctx->r3) = ctx->r8;
            goto L_804153B8;
    }
    // 0x80415598: sh          $t0, 0x4($v1)
    MEM_H(0X4, ctx->r3) = ctx->r8;
    // 0x8041559C: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x804155A0: b           L_804153B8
    // 0x804155A4: sb          $a3, 0x30($v0)
    MEM_B(0X30, ctx->r2) = ctx->r7;
        goto L_804153B8;
    // 0x804155A4: sb          $a3, 0x30($v0)
    MEM_B(0X30, ctx->r2) = ctx->r7;
L_804155A8:
    // 0x804155A8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804155AC: lwc1        $f2, -0x43EC($v1)
    ctx->f2.u32l = MEM_W(ctx->r3, -0X43EC);
    // 0x804155B0: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x804155B4: add.s       $f2, $f0, $f2
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f2.fl = ctx->f0.fl + ctx->f2.fl;
    // 0x804155B8: lwc1        $f4, -0x5A60($a0)
    ctx->f4.u32l = MEM_W(ctx->r4, -0X5A60);
    // 0x804155BC: c.lt.s      $f4, $f2
    CHECK_FR(ctx, 4);
    CHECK_FR(ctx, 2);
    c1cs = ctx->f4.fl < ctx->f2.fl;
    // 0x804155C0: nop

    // 0x804155C4: bc1tl       L_804155CC
    if (c1cs) {
        // 0x804155C8: sub.s       $f2, $f2, $f4
        CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 4);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f4.fl); 
    ctx->f2.fl = ctx->f2.fl - ctx->f4.fl;
            goto L_804155CC;
    }
    goto skip_12;
    // 0x804155C8: sub.s       $f2, $f2, $f4
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 4);
    NAN_CHECK(ctx->f2.fl); NAN_CHECK(ctx->f4.fl); 
    ctx->f2.fl = ctx->f2.fl - ctx->f4.fl;
    skip_12:
L_804155CC:
    // 0x804155CC: swc1        $f2, -0x43EC($v1)
    MEM_W(-0X43EC, ctx->r3) = ctx->f2.u32l;
    // 0x804155D0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804155D4: lwc1        $f2, -0x43E4($v1)
    ctx->f2.u32l = MEM_W(ctx->r3, -0X43E4);
    // 0x804155D8: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x804155DC: add.s       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f0.fl = ctx->f0.fl + ctx->f2.fl;
    // 0x804155E0: lwc1        $f2, -0x5A5C($a0)
    ctx->f2.u32l = MEM_W(ctx->r4, -0X5A5C);
    // 0x804155E4: c.lt.s      $f2, $f0
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    c1cs = ctx->f2.fl < ctx->f0.fl;
    // 0x804155E8: nop

    // 0x804155EC: bc1fl       L_8041537C
    if (!c1cs) {
        // 0x804155F0: swc1        $f0, -0x43E4($v1)
        MEM_W(-0X43E4, ctx->r3) = ctx->f0.u32l;
            goto L_8041537C;
    }
    goto skip_13;
    // 0x804155F0: swc1        $f0, -0x43E4($v1)
    MEM_W(-0X43E4, ctx->r3) = ctx->f0.u32l;
    skip_13:
    // 0x804155F4: lhu         $a0, 0x2C($v0)
    ctx->r4 = MEM_HU(ctx->r2, 0X2C);
    // 0x804155F8: sub.s       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.fl); NAN_CHECK(ctx->f2.fl); 
    ctx->f0.fl = ctx->f0.fl - ctx->f2.fl;
    // 0x804155FC: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x80415600: sh          $a0, 0x2C($v0)
    MEM_H(0X2C, ctx->r2) = ctx->r4;
    // 0x80415604: b           L_8041537C
    // 0x80415608: swc1        $f0, -0x43E4($v1)
    MEM_W(-0X43E4, ctx->r3) = ctx->f0.u32l;
        goto L_8041537C;
    // 0x80415608: swc1        $f0, -0x43E4($v1)
    MEM_W(-0X43E4, ctx->r3) = ctx->f0.u32l;
L_8041560C:
    // 0x8041560C: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x80415610: sh          $a2, 0x2C($v0)
    MEM_H(0X2C, ctx->r2) = ctx->r6;
    // 0x80415614: bne         $a3, $zero, L_8041542C
    if (ctx->r7 != 0) {
        // 0x80415618: sh          $a0, 0xC($v1)
        MEM_H(0XC, ctx->r3) = ctx->r4;
            goto L_8041542C;
    }
    // 0x80415618: sh          $a0, 0xC($v1)
    MEM_H(0XC, ctx->r3) = ctx->r4;
    // 0x8041561C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80415620: b           L_8041542C
    // 0x80415624: sb          $a0, 0x32($v0)
    MEM_B(0X32, ctx->r2) = ctx->r4;
        goto L_8041542C;
    // 0x80415624: sb          $a0, 0x32($v0)
    MEM_B(0X32, ctx->r2) = ctx->r4;
L_80415628:
    // 0x80415628: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x8041562C: sh          $a1, 0x2E($v0)
    MEM_H(0X2E, ctx->r2) = ctx->r5;
    // 0x80415630: bne         $a3, $zero, L_80415468
    if (ctx->r7 != 0) {
        // 0x80415634: sh          $a0, 0xE($v1)
        MEM_H(0XE, ctx->r3) = ctx->r4;
            goto L_80415468;
    }
    // 0x80415634: sh          $a0, 0xE($v1)
    MEM_H(0XE, ctx->r3) = ctx->r4;
    // 0x80415638: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041563C: b           L_80415468
    // 0x80415640: sb          $a0, 0x33($v0)
    MEM_B(0X33, ctx->r2) = ctx->r4;
        goto L_80415468;
    // 0x80415640: sb          $a0, 0x33($v0)
    MEM_B(0X33, ctx->r2) = ctx->r4;
L_80415644:
    // 0x80415644: subu        $a1, $a2, $a1
    ctx->r5 = SUB32(ctx->r6, ctx->r5);
    // 0x80415648: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x8041564C: addiu       $a2, $t0, 0x1
    ctx->r6 = ADD32(ctx->r8, 0X1);
    // 0x80415650: sh          $a1, 0x2A($v0)
    MEM_H(0X2A, ctx->r2) = ctx->r5;
    // 0x80415654: sh          $a0, 0xA($v1)
    MEM_H(0XA, ctx->r3) = ctx->r4;
    // 0x80415658: bne         $a3, $zero, L_804153F4
    if (ctx->r7 != 0) {
        // 0x8041565C: sh          $a2, 0x8($v1)
        MEM_H(0X8, ctx->r3) = ctx->r6;
            goto L_804153F4;
    }
    // 0x8041565C: sh          $a2, 0x8($v1)
    MEM_H(0X8, ctx->r3) = ctx->r6;
    // 0x80415660: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80415664: b           L_804153F4
    // 0x80415668: sb          $a0, 0x31($v0)
    MEM_B(0X31, ctx->r2) = ctx->r4;
        goto L_804153F4;
    // 0x80415668: sb          $a0, 0x31($v0)
    MEM_B(0X31, ctx->r2) = ctx->r4;
L_8041566C:
    // 0x8041566C: jal         0x800120C0
    // 0x80415670: nop

    func_800120C0(rdram, ctx);
        goto after_1;
    // 0x80415670: nop

    after_1:
    // 0x80415674: jal         0x8002E768
    // 0x80415678: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    func_8002E768(rdram, ctx);
        goto after_2;
    // 0x80415678: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    after_2:
    // 0x8041567C: jal         0x800212A0
    // 0x80415680: lbu         $a0, -0x458D($s2)
    ctx->r4 = MEM_BU(ctx->r18, -0X458D);
    func_800212A0(rdram, ctx);
        goto after_3;
    // 0x80415680: lbu         $a0, -0x458D($s2)
    ctx->r4 = MEM_BU(ctx->r18, -0X458D);
    after_3:
    // 0x80415684: lhu         $v1, 0x8($s0)
    ctx->r3 = MEM_HU(ctx->r16, 0X8);
    // 0x80415688: lw          $v0, -0x4D1C($s1)
    ctx->r2 = MEM_W(ctx->r17, -0X4D1C);
    // 0x8041568C: ori         $v1, $v1, 0x2
    ctx->r3 = ctx->r3 | 0X2;
    // 0x80415690: ori         $v0, $v0, 0x2
    ctx->r2 = ctx->r2 | 0X2;
    // 0x80415694: lw          $ra, 0x24($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X24);
    // 0x80415698: sh          $v1, 0x8($s0)
    MEM_H(0X8, ctx->r16) = ctx->r3;
    // 0x8041569C: sw          $s3, -0x4590($s2)
    MEM_W(-0X4590, ctx->r18) = ctx->r19;
    // 0x804156A0: lw          $s0, 0x14($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X14);
    // 0x804156A4: lw          $s3, 0x20($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X20);
    // 0x804156A8: lw          $s2, 0x1C($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X1C);
    // 0x804156AC: sw          $v0, -0x4D1C($s1)
    MEM_W(-0X4D1C, ctx->r17) = ctx->r2;
    // 0x804156B0: lw          $s1, 0x18($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X18);
    // 0x804156B4: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x804156B8: addiu       $a1, $zero, 0x3B
    ctx->r5 = ADD32(0, 0X3B);
    // 0x804156BC: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x804156C0: j           L_800268D4
    // 0x804156C4: addiu       $sp, $sp, 0x28
    ctx->r29 = ADD32(ctx->r29, 0X28);
    func_800268D4(rdram, ctx);
    return;
    // 0x804156C4: addiu       $sp, $sp, 0x28
    ctx->r29 = ADD32(ctx->r29, 0X28);
L_804156C8:
    // 0x804156C8: addiu       $a0, $zero, 0x63
    ctx->r4 = ADD32(0, 0X63);
    // 0x804156CC: andi        $a0, $a0, 0xFF
    ctx->r4 = ctx->r4 & 0XFF;
    // 0x804156D0: lui         $t5, 0x8005
    ctx->r13 = S32(0X8005 << 16);
    // 0x804156D4: addiu       $t5, $t5, 0x3D3C
    ctx->r13 = ADD32(ctx->r13, 0X3D3C);
    // 0x804156D8: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x804156DC: addu        $a0, $a0, $t5
    ctx->r4 = ADD32(ctx->r4, ctx->r13);
    // 0x804156E0: lw          $a0, 0x0($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X0);
    // 0x804156E4: sltu        $t5, $t6, $a0
    ctx->r13 = ctx->r14 < ctx->r4 ? 1 : 0;
    // 0x804156E8: bnel        $t5, $zero, L_804154CC
    if (ctx->r13 != 0) {
        // 0x804156EC: andi        $a1, $a1, 0xA
        ctx->r5 = ctx->r5 & 0XA;
            goto L_804154CC;
    }
    goto skip_14;
    // 0x804156EC: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
    skip_14:
    // 0x804156F0: b           L_80415538
    // 0x804156F4: subu        $t6, $t6, $a0
    ctx->r14 = SUB32(ctx->r14, ctx->r4);
        goto L_80415538;
    // 0x804156F4: subu        $t6, $t6, $a0
    ctx->r14 = SUB32(ctx->r14, ctx->r4);
L_804156F8:
    // 0x804156F8: bnel        $t3, $t2, L_8041555C
    if (ctx->r11 != ctx->r10) {
        // 0x804156FC: ori         $a1, $a1, 0x8
        ctx->r5 = ctx->r5 | 0X8;
            goto L_8041555C;
    }
    goto skip_15;
    // 0x804156FC: ori         $a1, $a1, 0x8
    ctx->r5 = ctx->r5 | 0X8;
    skip_15:
    // 0x80415700: bnel        $t4, $t3, L_8041555C
    if (ctx->r12 != ctx->r11) {
        // 0x80415704: ori         $a1, $a1, 0x8
        ctx->r5 = ctx->r5 | 0X8;
            goto L_8041555C;
    }
    goto skip_16;
    // 0x80415704: ori         $a1, $a1, 0x8
    ctx->r5 = ctx->r5 | 0X8;
    skip_16:
    // 0x80415708: b           L_804154CC
    // 0x8041570C: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
        goto L_804154CC;
    // 0x8041570C: andi        $a1, $a1, 0xA
    ctx->r5 = ctx->r5 & 0XA;
;}
RECOMP_FUNC void hm_func_80415710(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x80415710: addiu       $sp, $sp, -0x20
    ctx->r29 = ADD32(ctx->r29, -0X20);
    // 0x80415714: sw          $s0, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r16;
    // 0x80415718: lui         $s0, 0x8008
    ctx->r16 = S32(0X8008 << 16);
    // 0x8041571C: sw          $ra, 0x1C($sp)
    MEM_W(0X1C, ctx->r29) = ctx->r31;
    // 0x80415720: jal         0x80415028
    // 0x80415724: sw          $s1, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r17;
    hm_func_80415028(rdram, ctx);
        goto after_0;
    // 0x80415724: sw          $s1, 0x18($sp)
    MEM_W(0X18, ctx->r29) = ctx->r17;
    after_0:
    // 0x80415728: jal         0x8041A5C0
    // 0x8041572C: nop

    hm_func_8041A5C0(rdram, ctx);
        goto after_1;
    // 0x8041572C: nop

    after_1:
    // 0x80415730: jal         0x8041AC98
    // 0x80415734: nop

    hm_func_8041AC98(rdram, ctx);
        goto after_2;
    // 0x80415734: nop

    after_2:
    // 0x80415738: jal         0x8041B3D4
    // 0x8041573C: nop

    hm_func_8041B3D4(rdram, ctx);
        goto after_3;
    // 0x8041573C: nop

    after_3:
    // 0x80415740: jal         0x8041B938
    // 0x80415744: nop

    hm_func_8041B938(rdram, ctx);
        goto after_4;
    // 0x80415744: nop

    after_4:
    // 0x80415748: jal         0x8041C0A8
    // 0x8041574C: nop

    hm_func_8041C0A8(rdram, ctx);
        goto after_5;
    // 0x8041574C: nop

    after_5:
    // 0x80415750: lw          $v0, 0x4EE4($s0)
    ctx->r2 = MEM_W(ctx->r16, 0X4EE4);
    // 0x80415754: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
    // 0x80415758: beq         $v0, $v1, L_804173B8
    if (ctx->r2 == ctx->r3) {
        // 0x8041575C: addiu       $v1, $zero, 0x3
        ctx->r3 = ADD32(0, 0X3);
            goto L_804173B8;
    }
    // 0x8041575C: addiu       $v1, $zero, 0x3
    ctx->r3 = ADD32(0, 0X3);
    // 0x80415760: bne         $v0, $v1, L_80415F58
    if (ctx->r2 != ctx->r3) {
        // 0x80415764: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415F58;
    }
    // 0x80415764: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415768: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x8041576C: bne         $v1, $zero, L_804157A0
    if (ctx->r3 != 0) {
        // 0x80415770: addiu       $v1, $zero, 0x11
        ctx->r3 = ADD32(0, 0X11);
            goto L_804157A0;
    }
    // 0x80415770: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
    // 0x80415774: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415778: lbu         $v1, 0x69E4($a0)
    ctx->r3 = MEM_BU(ctx->r4, 0X69E4);
    // 0x8041577C: andi        $a1, $v1, 0x4
    ctx->r5 = ctx->r3 & 0X4;
    // 0x80415780: beql        $a1, $zero, L_804157A0
    if (ctx->r5 == 0) {
        // 0x80415784: addiu       $v1, $zero, 0x11
        ctx->r3 = ADD32(0, 0X11);
            goto L_804157A0;
    }
    goto skip_0;
    // 0x80415784: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
    skip_0:
    // 0x80415788: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041578C: lbu         $a2, -0x35F3($a1)
    ctx->r6 = MEM_BU(ctx->r5, -0X35F3);
    // 0x80415790: addiu       $a1, $zero, 0xE
    ctx->r5 = ADD32(0, 0XE);
    // 0x80415794: beq         $a2, $a1, L_804199F8
    if (ctx->r6 == ctx->r5) {
        // 0x80415798: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_804199F8;
    }
    // 0x80415798: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
L_8041579C:
    // 0x8041579C: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
L_804157A0:
    // 0x804157A0: bne         $v0, $v1, L_80416070
    if (ctx->r2 != ctx->r3) {
        // 0x804157A4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80416070;
    }
    // 0x804157A4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804157A8: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x804157AC: beq         $v1, $zero, L_804187E4
    if (ctx->r3 == 0) {
        // 0x804157B0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804187E4;
    }
    // 0x804157B0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804157B4:
    // 0x804157B4: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
L_804157B8:
    // 0x804157B8: addiu       $v1, $zero, 0x14
    ctx->r3 = ADD32(0, 0X14);
L_804157BC:
    // 0x804157BC: bne         $v0, $v1, L_804162A0
    if (ctx->r2 != ctx->r3) {
        // 0x804157C0: lbu         $a3, 0x69F0($t1)
        ctx->r7 = MEM_BU(ctx->r9, 0X69F0);
            goto L_804162A0;
    }
    // 0x804157C0: lbu         $a3, 0x69F0($t1)
    ctx->r7 = MEM_BU(ctx->r9, 0X69F0);
    // 0x804157C4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804157C8: lw          $a1, 0x4EE8($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X4EE8);
    // 0x804157CC: addiu       $a0, $zero, 0x7
    ctx->r4 = ADD32(0, 0X7);
    // 0x804157D0: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
    // 0x804157D4: beq         $a1, $a0, L_80418A3C
    if (ctx->r5 == ctx->r4) {
        // 0x804157D8: sra         $v1, $v1, 24
        ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
            goto L_80418A3C;
    }
    // 0x804157D8: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
L_804157DC:
    // 0x804157DC: bltz        $v1, L_804160A8
    if (SIGNED(ctx->r3) < 0) {
        // 0x804157E0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804160A8;
    }
    // 0x804157E0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804157E4:
    // 0x804157E4: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
L_804157E8:
    // 0x804157E8: lbu         $a0, 0x69F2($a2)
    ctx->r4 = MEM_BU(ctx->r6, 0X69F2);
    // 0x804157EC: addiu       $v1, $zero, 0x20
    ctx->r3 = ADD32(0, 0X20);
    // 0x804157F0: beq         $v0, $v1, L_80417EEC
    if (ctx->r2 == ctx->r3) {
        // 0x804157F4: andi        $a1, $a0, 0x10
        ctx->r5 = ctx->r4 & 0X10;
            goto L_80417EEC;
    }
    // 0x804157F4: andi        $a1, $a0, 0x10
    ctx->r5 = ctx->r4 & 0X10;
L_804157F8:
    // 0x804157F8: beq         $a1, $zero, L_80416214
    if (ctx->r5 == 0) {
        // 0x804157FC: lui         $v1, 0x800C
        ctx->r3 = S32(0X800C << 16);
            goto L_80416214;
    }
    // 0x804157FC: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80415800: lhu         $a3, 0x1B28($v1)
    ctx->r7 = MEM_HU(ctx->r3, 0X1B28);
    // 0x80415804: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x80415808: beq         $a3, $a1, L_80418A00
    if (ctx->r7 == ctx->r5) {
        // 0x8041580C: addiu       $a1, $zero, 0x1
        ctx->r5 = ADD32(0, 0X1);
            goto L_80418A00;
    }
    // 0x8041580C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_80415810:
    // 0x80415810: addiu       $v1, $zero, 0xE
    ctx->r3 = ADD32(0, 0XE);
L_80415814:
    // 0x80415814: beq         $v0, $v1, L_80416250
    if (ctx->r2 == ctx->r3) {
        // 0x80415818: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80416250;
    }
    // 0x80415818: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041581C: addiu       $v1, $zero, 0x19
    ctx->r3 = ADD32(0, 0X19);
L_80415820:
    // 0x80415820: beq         $v0, $v1, L_80417E9C
    if (ctx->r2 == ctx->r3) {
        // 0x80415824: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80417E9C;
    }
    // 0x80415824: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415828: addiu       $v1, $zero, 0x15
    ctx->r3 = ADD32(0, 0X15);
    // 0x8041582C: bne         $v0, $v1, L_80417FE0
    if (ctx->r2 != ctx->r3) {
        // 0x80415830: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80417FE0;
    }
    // 0x80415830: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415834: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80415838: addiu       $v1, $zero, 0xB
    ctx->r3 = ADD32(0, 0XB);
    // 0x8041583C: beq         $a0, $v1, L_80418AA0
    if (ctx->r4 == ctx->r3) {
        // 0x80415840: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418AA0;
    }
    // 0x80415840: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415844:
    // 0x80415844: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
L_80415848:
    // 0x80415848: lhu         $v1, -0x457A($a3)
    ctx->r3 = MEM_HU(ctx->r7, -0X457A);
L_8041584C:
    // 0x8041584C: sltiu       $v1, $v1, 0x3E8
    ctx->r3 = ctx->r3 < 0X3E8 ? 1 : 0;
    // 0x80415850: bne         $v1, $zero, L_80415860
    if (ctx->r3 != 0) {
        // 0x80415854: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_80415860;
    }
    // 0x80415854: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80415858: addiu       $v1, $zero, 0x3E7
    ctx->r3 = ADD32(0, 0X3E7);
    // 0x8041585C: sh          $v1, -0x457A($a3)
    MEM_H(-0X457A, ctx->r7) = ctx->r3;
L_80415860:
    // 0x80415860: lhu         $v1, -0x457C($a2)
    ctx->r3 = MEM_HU(ctx->r6, -0X457C);
    // 0x80415864: sltiu       $v1, $v1, 0x3E8
    ctx->r3 = ctx->r3 < 0X3E8 ? 1 : 0;
    // 0x80415868: bne         $v1, $zero, L_80415878
    if (ctx->r3 != 0) {
        // 0x8041586C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415878;
    }
    // 0x8041586C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415870: addiu       $v1, $zero, 0x3E7
    ctx->r3 = ADD32(0, 0X3E7);
    // 0x80415874: sh          $v1, -0x457C($a2)
    MEM_H(-0X457C, ctx->r6) = ctx->r3;
L_80415878:
    // 0x80415878: lhu         $v1, -0x4576($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X4576);
    // 0x8041587C: sltiu       $v1, $v1, 0x3E8
    ctx->r3 = ctx->r3 < 0X3E8 ? 1 : 0;
    // 0x80415880: bnel        $v1, $zero, L_80415894
    if (ctx->r3 != 0) {
        // 0x80415884: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415894;
    }
    goto skip_1;
    // 0x80415884: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_1:
    // 0x80415888: addiu       $v1, $zero, 0x3E7
    ctx->r3 = ADD32(0, 0X3E7);
    // 0x8041588C: sh          $v1, -0x4576($a0)
    MEM_H(-0X4576, ctx->r4) = ctx->r3;
    // 0x80415890: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415894:
    // 0x80415894: lhu         $v1, -0x4578($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X4578);
    // 0x80415898: sltiu       $v1, $v1, 0x3E8
    ctx->r3 = ctx->r3 < 0X3E8 ? 1 : 0;
    // 0x8041589C: bnel        $v1, $zero, L_804158B0
    if (ctx->r3 != 0) {
        // 0x804158A0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804158B0;
    }
    goto skip_2;
    // 0x804158A0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_2:
    // 0x804158A4: addiu       $v1, $zero, 0x3E7
    ctx->r3 = ADD32(0, 0X3E7);
    // 0x804158A8: sh          $v1, -0x4578($a0)
    MEM_H(-0X4578, ctx->r4) = ctx->r3;
    // 0x804158AC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804158B0:
    // 0x804158B0: lw          $v1, -0x454C($a0)
    ctx->r3 = MEM_W(ctx->r4, -0X454C);
    // 0x804158B4: lui         $a1, 0xFF00
    ctx->r5 = S32(0XFF00 << 16);
    // 0x804158B8: beq         $v1, $a1, L_80417FC0
    if (ctx->r3 == ctx->r5) {
        // 0x804158BC: lui         $t0, 0xFE00
        ctx->r8 = S32(0XFE00 << 16);
            goto L_80417FC0;
    }
    // 0x804158BC: lui         $t0, 0xFE00
    ctx->r8 = S32(0XFE00 << 16);
    // 0x804158C0: beq         $v1, $t0, L_80418A84
    if (ctx->r3 == ctx->r8) {
        // 0x804158C4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418A84;
    }
    // 0x804158C4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804158C8:
    // 0x804158C8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804158CC:
    // 0x804158CC: lw          $a1, -0x442C($v1)
    ctx->r5 = MEM_W(ctx->r3, -0X442C);
    // 0x804158D0: addiu       $v1, $zero, 0x6
    ctx->r3 = ADD32(0, 0X6);
    // 0x804158D4: beq         $a1, $v1, L_804161A8
    if (ctx->r5 == ctx->r3) {
        // 0x804158D8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804161A8;
    }
    // 0x804158D8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804158DC: lui         $s1, 0x8008
    ctx->r17 = S32(0X8008 << 16);
L_804158E0:
    // 0x804158E0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804158E4:
    // 0x804158E4: lui         $v1, 0x3D0F
    ctx->r3 = S32(0X3D0F << 16);
    // 0x804158E8: lw          $t0, -0x4510($a0)
    ctx->r8 = MEM_W(ctx->r4, -0X4510);
    // 0x804158EC: addiu       $v1, $v1, 0x5C29
    ctx->r3 = ADD32(ctx->r3, 0X5C29);
    // 0x804158F0: beq         $t0, $v1, L_80418040
    if (ctx->r8 == ctx->r3) {
        // 0x804158F4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418040;
    }
    // 0x804158F4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804158F8:
    // 0x804158F8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804158FC: lw          $v1, 0x73E4($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X73E4);
    // 0x80415900: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80415904: beql        $v1, $a0, L_80418070
    if (ctx->r3 == ctx->r4) {
        // 0x80415908: lui         $v1, 0x8005
        ctx->r3 = S32(0X8005 << 16);
            goto L_80418070;
    }
    goto skip_3;
    // 0x80415908: lui         $v1, 0x8005
    ctx->r3 = S32(0X8005 << 16);
    skip_3:
    // 0x8041590C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
L_80415910:
    // 0x80415910: beql        $v1, $a0, L_80418104
    if (ctx->r3 == ctx->r4) {
        // 0x80415914: lui         $v1, 0x8005
        ctx->r3 = S32(0X8005 << 16);
            goto L_80418104;
    }
    goto skip_4;
    // 0x80415914: lui         $v1, 0x8005
    ctx->r3 = S32(0X8005 << 16);
    skip_4:
    // 0x80415918: addiu       $a0, $zero, 0x3
    ctx->r4 = ADD32(0, 0X3);
    // 0x8041591C: bne         $v1, $a0, L_80418110
    if (ctx->r3 != ctx->r4) {
        // 0x80415920: lui         $a0, 0x8005
        ctx->r4 = S32(0X8005 << 16);
            goto L_80418110;
    }
    // 0x80415920: lui         $a0, 0x8005
    ctx->r4 = S32(0X8005 << 16);
    // 0x80415924: lui         $v1, 0xFF40
    ctx->r3 = S32(0XFF40 << 16);
    // 0x80415928: addiu       $v1, $v1, 0x4000
    ctx->r3 = ADD32(ctx->r3, 0X4000);
    // 0x8041592C: sw          $v1, -0x305C($a0)
    MEM_W(-0X305C, ctx->r4) = ctx->r3;
L_80415930:
    // 0x80415930: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415934: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415938: lw          $v1, -0x3594($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X3594);
    // 0x8041593C: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x80415940: beq         $v1, $t0, L_804174D0
    if (ctx->r3 == ctx->r8) {
        // 0x80415944: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_804174D0;
    }
    // 0x80415944: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x80415948: beql        $v1, $t0, L_80417C6C
    if (ctx->r3 == ctx->r8) {
        // 0x8041594C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417C6C;
    }
    goto skip_5;
    // 0x8041594C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_5:
    // 0x80415950: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415954: bne         $v1, $t0, L_804179E0
    if (ctx->r3 != ctx->r8) {
        // 0x80415958: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_804179E0;
    }
    // 0x80415958: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x8041595C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415960: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415964: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415968: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041596C: beq         $t0, $a0, L_80418BC0
    if (ctx->r8 == ctx->r4) {
        // 0x80415970: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418BC0;
    }
    // 0x80415970: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415974:
    // 0x80415974: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415978: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x8041597C: bne         $v1, $a0, L_80417A00
    if (ctx->r3 != ctx->r4) {
        // 0x80415980: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417A00;
    }
    // 0x80415980: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415984: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415988: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041598C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415990: beq         $t0, $a0, L_80418B80
    if (ctx->r8 == ctx->r4) {
        // 0x80415994: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418B80;
    }
    // 0x80415994: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415998:
    // 0x80415998: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041599C: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x804159A0: bne         $v1, $a0, L_80417A48
    if (ctx->r3 != ctx->r4) {
        // 0x804159A4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417A48;
    }
    // 0x804159A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804159A8: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x804159AC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804159B0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804159B4: beq         $t0, $a0, L_80418BA0
    if (ctx->r8 == ctx->r4) {
        // 0x804159B8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418BA0;
    }
    // 0x804159B8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804159BC:
    // 0x804159BC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804159C0: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x804159C4: bne         $v1, $a0, L_80417A70
    if (ctx->r3 != ctx->r4) {
        // 0x804159C8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417A70;
    }
    // 0x804159C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804159CC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804159D0: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x804159D4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804159D8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804159DC: beq         $a0, $v1, L_80418B40
    if (ctx->r4 == ctx->r3) {
        // 0x804159E0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418B40;
    }
    // 0x804159E0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804159E4:
    // 0x804159E4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804159E8:
    // 0x804159E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804159EC: lw          $v1, -0x346C($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X346C);
    // 0x804159F0: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x804159F4: beq         $v1, $t0, L_804177F0
    if (ctx->r3 == ctx->r8) {
        // 0x804159F8: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_804177F0;
    }
    // 0x804159F8: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x804159FC: beql        $v1, $t0, L_80417C34
    if (ctx->r3 == ctx->r8) {
        // 0x80415A00: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417C34;
    }
    goto skip_6;
    // 0x80415A00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_6:
    // 0x80415A04: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415A08: bne         $v1, $t0, L_8041759C
    if (ctx->r3 != ctx->r8) {
        // 0x80415A0C: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_8041759C;
    }
    // 0x80415A0C: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x80415A10: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A14: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415A18: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415A1C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415A20: beq         $t0, $a0, L_80418B60
    if (ctx->r8 == ctx->r4) {
        // 0x80415A24: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418B60;
    }
    // 0x80415A24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415A28:
    // 0x80415A28: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A2C: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x80415A30: bne         $v1, $a0, L_804175BC
    if (ctx->r3 != ctx->r4) {
        // 0x80415A34: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804175BC;
    }
    // 0x80415A34: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A38: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415A3C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415A40: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415A44: beq         $t0, $a0, L_80418B20
    if (ctx->r8 == ctx->r4) {
        // 0x80415A48: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418B20;
    }
    // 0x80415A48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415A4C:
    // 0x80415A4C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A50: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x80415A54: bne         $v1, $a0, L_804175E4
    if (ctx->r3 != ctx->r4) {
        // 0x80415A58: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804175E4;
    }
    // 0x80415A58: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A5C: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x80415A60: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415A64: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415A68: beq         $t0, $a0, L_80418B00
    if (ctx->r8 == ctx->r4) {
        // 0x80415A6C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418B00;
    }
    // 0x80415A6C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415A70:
    // 0x80415A70: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A74: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x80415A78: bne         $v1, $a0, L_8041760C
    if (ctx->r3 != ctx->r4) {
        // 0x80415A7C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041760C;
    }
    // 0x80415A7C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415A80: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415A84: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x80415A88: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80415A8C: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80415A90: beq         $a0, $v1, L_80418AE0
    if (ctx->r4 == ctx->r3) {
        // 0x80415A94: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418AE0;
    }
    // 0x80415A94: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415A98:
    // 0x80415A98: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415A9C:
    // 0x80415A9C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415AA0: lw          $v1, -0x3344($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X3344);
    // 0x80415AA4: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x80415AA8: beq         $v1, $t0, L_804178DC
    if (ctx->r3 == ctx->r8) {
        // 0x80415AAC: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_804178DC;
    }
    // 0x80415AAC: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x80415AB0: beql        $v1, $t0, L_804179A8
    if (ctx->r3 == ctx->r8) {
        // 0x80415AB4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804179A8;
    }
    goto skip_7;
    // 0x80415AB4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_7:
    // 0x80415AB8: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415ABC: bne         $v1, $t0, L_80416E3C
    if (ctx->r3 != ctx->r8) {
        // 0x80415AC0: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_80416E3C;
    }
    // 0x80415AC0: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x80415AC4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415AC8: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415ACC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415AD0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415AD4: beq         $t0, $a0, L_804193D8
    if (ctx->r8 == ctx->r4) {
        // 0x80415AD8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804193D8;
    }
    // 0x80415AD8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415ADC:
    // 0x80415ADC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415AE0: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x80415AE4: bne         $v1, $a0, L_80416E7C
    if (ctx->r3 != ctx->r4) {
        // 0x80415AE8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416E7C;
    }
    // 0x80415AE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415AEC: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415AF0: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415AF4: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415AF8: beq         $t0, $a0, L_80419298
    if (ctx->r8 == ctx->r4) {
        // 0x80415AFC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419298;
    }
    // 0x80415AFC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415B00:
    // 0x80415B00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B04: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x80415B08: bne         $v1, $a0, L_80416EC4
    if (ctx->r3 != ctx->r4) {
        // 0x80415B0C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416EC4;
    }
    // 0x80415B0C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B10: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x80415B14: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415B18: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415B1C: beq         $t0, $a0, L_80419378
    if (ctx->r8 == ctx->r4) {
        // 0x80415B20: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419378;
    }
    // 0x80415B20: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415B24:
    // 0x80415B24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B28: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x80415B2C: bne         $v1, $a0, L_80416F0C
    if (ctx->r3 != ctx->r4) {
        // 0x80415B30: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416F0C;
    }
    // 0x80415B30: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B34: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415B38: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x80415B3C: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80415B40: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80415B44: beq         $a0, $v1, L_80419118
    if (ctx->r4 == ctx->r3) {
        // 0x80415B48: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80419118;
    }
    // 0x80415B48: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415B4C:
    // 0x80415B4C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415B50:
    // 0x80415B50: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B54: lw          $v1, -0x321C($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X321C);
    // 0x80415B58: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x80415B5C: beq         $v1, $t0, L_80416E00
    if (ctx->r3 == ctx->r8) {
        // 0x80415B60: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_80416E00;
    }
    // 0x80415B60: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x80415B64: beql        $v1, $t0, L_80416CD8
    if (ctx->r3 == ctx->r8) {
        // 0x80415B68: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416CD8;
    }
    goto skip_8;
    // 0x80415B68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_8:
    // 0x80415B6C: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415B70: bne         $v1, $t0, L_804169E4
    if (ctx->r3 != ctx->r8) {
        // 0x80415B74: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_804169E4;
    }
    // 0x80415B74: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x80415B78: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B7C: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415B80: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415B84: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415B88: beq         $t0, $a0, L_804193B8
    if (ctx->r8 == ctx->r4) {
        // 0x80415B8C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804193B8;
    }
    // 0x80415B8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415B90:
    // 0x80415B90: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415B94: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x80415B98: bne         $v1, $a0, L_80416A24
    if (ctx->r3 != ctx->r4) {
        // 0x80415B9C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416A24;
    }
    // 0x80415B9C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415BA0: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415BA4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415BA8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415BAC: beq         $t0, $a0, L_80419218
    if (ctx->r8 == ctx->r4) {
        // 0x80415BB0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419218;
    }
    // 0x80415BB0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415BB4:
    // 0x80415BB4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415BB8: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x80415BBC: bne         $v1, $a0, L_80416A6C
    if (ctx->r3 != ctx->r4) {
        // 0x80415BC0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416A6C;
    }
    // 0x80415BC0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415BC4: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x80415BC8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415BCC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415BD0: beq         $t0, $a0, L_80419318
    if (ctx->r8 == ctx->r4) {
        // 0x80415BD4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419318;
    }
    // 0x80415BD4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415BD8:
    // 0x80415BD8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415BDC: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x80415BE0: bne         $v1, $a0, L_80416AB4
    if (ctx->r3 != ctx->r4) {
        // 0x80415BE4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416AB4;
    }
    // 0x80415BE4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415BE8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415BEC: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x80415BF0: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80415BF4: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80415BF8: beq         $a0, $v1, L_80418F18
    if (ctx->r4 == ctx->r3) {
        // 0x80415BFC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418F18;
    }
    // 0x80415BFC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415C00:
    // 0x80415C00: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415C04:
    // 0x80415C04: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C08: lw          $v1, -0x30F4($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X30F4);
    // 0x80415C0C: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x80415C10: beq         $v1, $t0, L_80416918
    if (ctx->r3 == ctx->r8) {
        // 0x80415C14: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_80416918;
    }
    // 0x80415C14: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x80415C18: beql        $v1, $t0, L_804171E4
    if (ctx->r3 == ctx->r8) {
        // 0x80415C1C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804171E4;
    }
    goto skip_9;
    // 0x80415C1C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_9:
    // 0x80415C20: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415C24: bne         $v1, $t0, L_80416F54
    if (ctx->r3 != ctx->r8) {
        // 0x80415C28: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_80416F54;
    }
    // 0x80415C28: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x80415C2C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C30: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415C34: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415C38: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415C3C: beq         $t0, $a0, L_80419398
    if (ctx->r8 == ctx->r4) {
        // 0x80415C40: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419398;
    }
    // 0x80415C40: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415C44:
    // 0x80415C44: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C48: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x80415C4C: bne         $v1, $a0, L_80416F74
    if (ctx->r3 != ctx->r4) {
        // 0x80415C50: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416F74;
    }
    // 0x80415C50: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C54: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415C58: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415C5C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415C60: beq         $t0, $a0, L_80419258
    if (ctx->r8 == ctx->r4) {
        // 0x80415C64: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419258;
    }
    // 0x80415C64: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415C68:
    // 0x80415C68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C6C: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x80415C70: bne         $v1, $a0, L_80416FBC
    if (ctx->r3 != ctx->r4) {
        // 0x80415C74: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416FBC;
    }
    // 0x80415C74: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C78: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x80415C7C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415C80: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415C84: beq         $t0, $a0, L_80419358
    if (ctx->r8 == ctx->r4) {
        // 0x80415C88: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419358;
    }
    // 0x80415C88: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415C8C:
    // 0x80415C8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C90: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x80415C94: bne         $v1, $a0, L_80416FE4
    if (ctx->r3 != ctx->r4) {
        // 0x80415C98: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416FE4;
    }
    // 0x80415C98: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415C9C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415CA0: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x80415CA4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80415CA8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80415CAC: beq         $a0, $v1, L_80419018
    if (ctx->r4 == ctx->r3) {
        // 0x80415CB0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80419018;
    }
    // 0x80415CB0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415CB4:
    // 0x80415CB4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415CB8:
    // 0x80415CB8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415CBC: lw          $v1, -0x2FCC($v1)
    ctx->r3 = MEM_W(ctx->r3, -0X2FCC);
    // 0x80415CC0: addiu       $t0, $a0, 0x75A8
    ctx->r8 = ADD32(ctx->r4, 0X75A8);
    // 0x80415CC4: beq         $v1, $t0, L_804171A8
    if (ctx->r3 == ctx->r8) {
        // 0x80415CC8: addiu       $t0, $a0, 0x7600
        ctx->r8 = ADD32(ctx->r4, 0X7600);
            goto L_804171A8;
    }
    // 0x80415CC8: addiu       $t0, $a0, 0x7600
    ctx->r8 = ADD32(ctx->r4, 0X7600);
    // 0x80415CCC: beql        $v1, $t0, L_804167F0
    if (ctx->r3 == ctx->r8) {
        // 0x80415CD0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804167F0;
    }
    goto skip_10;
    // 0x80415CD0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_10:
    // 0x80415CD4: addiu       $t0, $a0, 0x7658
    ctx->r8 = ADD32(ctx->r4, 0X7658);
    // 0x80415CD8: bne         $v1, $t0, L_804164FC
    if (ctx->r3 != ctx->r8) {
        // 0x80415CDC: addiu       $t0, $a0, 0x76B0
        ctx->r8 = ADD32(ctx->r4, 0X76B0);
            goto L_804164FC;
    }
    // 0x80415CDC: addiu       $t0, $a0, 0x76B0
    ctx->r8 = ADD32(ctx->r4, 0X76B0);
    // 0x80415CE0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415CE4: lw          $t0, 0x7694($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7694);
    // 0x80415CE8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415CEC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415CF0: beq         $t0, $a0, L_804191D8
    if (ctx->r8 == ctx->r4) {
        // 0x80415CF4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804191D8;
    }
    // 0x80415CF4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415CF8:
    // 0x80415CF8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415CFC: addiu       $a0, $a0, 0x77B8
    ctx->r4 = ADD32(ctx->r4, 0X77B8);
    // 0x80415D00: bne         $v1, $a0, L_8041653C
    if (ctx->r3 != ctx->r4) {
        // 0x80415D04: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041653C;
    }
    // 0x80415D04: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415D08: lw          $t0, 0x77F4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X77F4);
    // 0x80415D0C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415D10: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415D14: beq         $t0, $a0, L_804191B8
    if (ctx->r8 == ctx->r4) {
        // 0x80415D18: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804191B8;
    }
    // 0x80415D18: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415D1C:
    // 0x80415D1C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415D20: addiu       $a0, $a0, 0x7970
    ctx->r4 = ADD32(ctx->r4, 0X7970);
    // 0x80415D24: bne         $v1, $a0, L_80416584
    if (ctx->r3 != ctx->r4) {
        // 0x80415D28: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416584;
    }
    // 0x80415D28: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415D2C: lw          $t0, 0x79AC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X79AC);
    // 0x80415D30: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80415D34: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80415D38: beq         $t0, $a0, L_804192D8
    if (ctx->r8 == ctx->r4) {
        // 0x80415D3C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804192D8;
    }
    // 0x80415D3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80415D40:
    // 0x80415D40: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415D44: addiu       $a0, $a0, 0x7B28
    ctx->r4 = ADD32(ctx->r4, 0X7B28);
    // 0x80415D48: bne         $v1, $a0, L_804165CC
    if (ctx->r3 != ctx->r4) {
        // 0x80415D4C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804165CC;
    }
    // 0x80415D4C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415D50: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415D54: lw          $a0, 0x7B64($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7B64);
    // 0x80415D58: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80415D5C: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80415D60: beq         $a0, $v1, L_80418DCC
    if (ctx->r4 == ctx->r3) {
        // 0x80415D64: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418DCC;
    }
    // 0x80415D64: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415D68:
    // 0x80415D68: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80415D6C:
    // 0x80415D6C: addiu       $t0, $zero, 0x30C
    ctx->r8 = ADD32(0, 0X30C);
    // 0x80415D70: beq         $a1, $t0, L_80416428
    if (ctx->r5 == ctx->r8) {
        // 0x80415D74: lbu         $a0, 0x69F9($v1)
        ctx->r4 = MEM_BU(ctx->r3, 0X69F9);
            goto L_80416428;
    }
    // 0x80415D74: lbu         $a0, 0x69F9($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X69F9);
    // 0x80415D78: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_80415D7C:
    // 0x80415D7C: beql        $a0, $t0, L_80416474
    if (ctx->r4 == ctx->r8) {
        // 0x80415D80: lhu         $a0, -0x457C($a2)
        ctx->r4 = MEM_HU(ctx->r6, -0X457C);
            goto L_80416474;
    }
    goto skip_11;
    // 0x80415D80: lhu         $a0, -0x457C($a2)
    ctx->r4 = MEM_HU(ctx->r6, -0X457C);
    skip_11:
L_80415D84:
    // 0x80415D84: addiu       $v1, $zero, 0x309
    ctx->r3 = ADD32(0, 0X309);
L_80415D88:
    // 0x80415D88: beql        $a1, $v1, L_804163E8
    if (ctx->r5 == ctx->r3) {
        // 0x80415D8C: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_804163E8;
    }
    goto skip_12;
    // 0x80415D8C: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    skip_12:
L_80415D90:
    // 0x80415D90: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
L_80415D94:
    // 0x80415D94: beq         $v0, $v1, L_80416384
    if (ctx->r2 == ctx->r3) {
        // 0x80415D98: addiu       $v1, $zero, 0x11
        ctx->r3 = ADD32(0, 0X11);
            goto L_80416384;
    }
    // 0x80415D98: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
    // 0x80415D9C: beq         $v0, $v1, L_80416328
    if (ctx->r2 == ctx->r3) {
        // 0x80415DA0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80416328;
    }
    // 0x80415DA0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415DA4: addiu       $v1, $zero, 0x14
    ctx->r3 = ADD32(0, 0X14);
    // 0x80415DA8: bne         $v0, $v1, L_80417E38
    if (ctx->r2 != ctx->r3) {
        // 0x80415DAC: addiu       $a0, $zero, 0x16
        ctx->r4 = ADD32(0, 0X16);
            goto L_80417E38;
    }
    // 0x80415DAC: addiu       $a0, $zero, 0x16
    ctx->r4 = ADD32(0, 0X16);
    // 0x80415DB0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415DB4: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80415DB8: addiu       $v1, $zero, 0x2
    ctx->r3 = ADD32(0, 0X2);
    // 0x80415DBC: beq         $a0, $v1, L_80418E6C
    if (ctx->r4 == ctx->r3) {
        // 0x80415DC0: lui         $a0, 0x8017
        ctx->r4 = S32(0X8017 << 16);
            goto L_80418E6C;
    }
    // 0x80415DC0: lui         $a0, 0x8017
    ctx->r4 = S32(0X8017 << 16);
L_80415DC4:
    // 0x80415DC4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_80415DC8:
    // 0x80415DC8: lw          $a2, 0x2874($v1)
    ctx->r6 = MEM_W(ctx->r3, 0X2874);
    // 0x80415DCC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415DD0: lui         $a1, 0x800
    ctx->r5 = S32(0X800 << 16);
    // 0x80415DD4: beq         $a2, $a1, L_80417D80
    if (ctx->r6 == ctx->r5) {
        // 0x80415DD8: lw          $v1, -0x4D1C($a0)
        ctx->r3 = MEM_W(ctx->r4, -0X4D1C);
            goto L_80417D80;
    }
    // 0x80415DD8: lw          $v1, -0x4D1C($a0)
    ctx->r3 = MEM_W(ctx->r4, -0X4D1C);
    // 0x80415DDC: addiu       $a1, $zero, 0x5A0
    ctx->r5 = ADD32(0, 0X5A0);
L_80415DE0:
    // 0x80415DE0: beq         $v1, $a1, L_80418148
    if (ctx->r3 == ctx->r5) {
        // 0x80415DE4: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_80418148;
    }
    // 0x80415DE4: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
L_80415DE8:
    // 0x80415DE8: addiu       $a1, $zero, 0x10
    ctx->r5 = ADD32(0, 0X10);
L_80415DEC:
    // 0x80415DEC: beq         $v0, $a1, L_80417CA4
    if (ctx->r2 == ctx->r5) {
        // 0x80415DF0: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80417CA4;
    }
    // 0x80415DF0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
L_80415DF4:
    // 0x80415DF4: jal         0x80414420
    // 0x80415DF8: nop

    hm_func_80414420(rdram, ctx);
        goto after_6;
    // 0x80415DF8: nop

    after_6:
    // 0x80415DFC: lui         $v1, 0x8007
    ctx->r3 = S32(0X8007 << 16);
    // 0x80415E00: ori         $v1, $v1, 0xBA86
    ctx->r3 = ctx->r3 | 0XBA86;
    // 0x80415E04: lhu         $a0, 0x0($v1)
    ctx->r4 = MEM_HU(ctx->r3, 0X0);
    // 0x80415E08: lbu         $a2, 0x1E($v1)
    ctx->r6 = MEM_BU(ctx->r3, 0X1E);
    // 0x80415E0C: lbu         $v0, 0x1F($v1)
    ctx->r2 = MEM_BU(ctx->r3, 0X1F);
    // 0x80415E10: lbu         $a1, 0x20($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X20);
    // 0x80415E14: addu        $v0, $v0, $a2
    ctx->r2 = ADD32(ctx->r2, ctx->r6);
    // 0x80415E18: lbu         $v1, 0x21($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X21);
    // 0x80415E1C: addu        $v0, $v0, $a1
    ctx->r2 = ADD32(ctx->r2, ctx->r5);
    // 0x80415E20: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80415E24: sltiu       $v1, $v0, 0x4
    ctx->r3 = ctx->r2 < 0X4 ? 1 : 0;
    // 0x80415E28: beq         $v1, $zero, L_80415E34
    if (ctx->r3 == 0) {
        // 0x80415E2C: andi        $a0, $a0, 0xFFFF
        ctx->r4 = ctx->r4 & 0XFFFF;
            goto L_80415E34;
    }
    // 0x80415E2C: andi        $a0, $a0, 0xFFFF
    ctx->r4 = ctx->r4 & 0XFFFF;
    // 0x80415E30: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
L_80415E34:
    // 0x80415E34: addiu       $v0, $v0, -0x4
    ctx->r2 = ADD32(ctx->r2, -0X4);
    // 0x80415E38: andi        $v1, $v0, 0xFFFF
    ctx->r3 = ctx->r2 & 0XFFFF;
    // 0x80415E3C: sll         $v0, $v1, 6
    ctx->r2 = S32(ctx->r3 << 6);
    // 0x80415E40: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80415E44: sll         $v0, $v0, 5
    ctx->r2 = S32(ctx->r2 << 5);
    // 0x80415E48: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80415E4C: sll         $v0, $v0, 2
    ctx->r2 = S32(ctx->r2 << 2);
    // 0x80415E50: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80415E54: lui         $v1, 0x1197
    ctx->r3 = S32(0X1197 << 16);
    // 0x80415E58: srl         $v0, $v0, 4
    ctx->r2 = S32(U32(ctx->r2) >> 4);
    // 0x80415E5C: ori         $v1, $v1, 0xF35B
    ctx->r3 = ctx->r3 | 0XF35B;
    // 0x80415E60: multu       $v0, $v1
    result = U64(U32(ctx->r2)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80415E64: sltiu       $v1, $a0, 0x32
    ctx->r3 = ctx->r4 < 0X32 ? 1 : 0;
    // 0x80415E68: mfhi        $v0
    ctx->r2 = hi;
    // 0x80415E6C: beq         $v1, $zero, L_80415E78
    if (ctx->r3 == 0) {
        // 0x80415E70: srl         $v0, $v0, 8
        ctx->r2 = S32(U32(ctx->r2) >> 8);
            goto L_80415E78;
    }
    // 0x80415E70: srl         $v0, $v0, 8
    ctx->r2 = S32(U32(ctx->r2) >> 8);
    // 0x80415E74: addiu       $a0, $zero, 0x32
    ctx->r4 = ADD32(0, 0X32);
L_80415E78:
    // 0x80415E78: addiu       $a0, $a0, -0x32
    ctx->r4 = ADD32(ctx->r4, -0X32);
    // 0x80415E7C: andi        $v1, $a0, 0xFFFF
    ctx->r3 = ctx->r4 & 0XFFFF;
    // 0x80415E80: sll         $a0, $v1, 1
    ctx->r4 = S32(ctx->r3 << 1);
    // 0x80415E84: addu        $a0, $a0, $v1
    ctx->r4 = ADD32(ctx->r4, ctx->r3);
    // 0x80415E88: sll         $a0, $a0, 3
    ctx->r4 = S32(ctx->r4 << 3);
    // 0x80415E8C: subu        $a0, $a0, $v1
    ctx->r4 = SUB32(ctx->r4, ctx->r3);
    // 0x80415E90: sll         $a0, $a0, 3
    ctx->r4 = S32(ctx->r4 << 3);
    // 0x80415E94: addu        $v1, $a0, $v1
    ctx->r3 = ADD32(ctx->r4, ctx->r3);
    // 0x80415E98: sll         $a0, $v1, 4
    ctx->r4 = S32(ctx->r3 << 4);
    // 0x80415E9C: subu        $v1, $a0, $v1
    ctx->r3 = SUB32(ctx->r4, ctx->r3);
    // 0x80415EA0: lui         $a0, 0xB0C9
    ctx->r4 = S32(0XB0C9 << 16);
    // 0x80415EA4: ori         $a0, $a0, 0xD267
    ctx->r4 = ctx->r4 | 0XD267;
    // 0x80415EA8: multu       $v1, $a0
    result = U64(U32(ctx->r3)) * U64(U32(ctx->r4)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80415EAC: mfhi        $v1
    ctx->r3 = hi;
    // 0x80415EB0: srl         $v1, $v1, 16
    ctx->r3 = S32(U32(ctx->r3) >> 16);
    // 0x80415EB4: addiu       $v1, $v1, 0x10
    ctx->r3 = ADD32(ctx->r3, 0X10);
    // 0x80415EB8: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x80415EBC: sltiu       $v1, $v0, 0x100
    ctx->r3 = ctx->r2 < 0X100 ? 1 : 0;
    // 0x80415EC0: beql        $v1, $zero, L_80415EC8
    if (ctx->r3 == 0) {
        // 0x80415EC4: addiu       $v0, $zero, 0xFF
        ctx->r2 = ADD32(0, 0XFF);
            goto L_80415EC8;
    }
    goto skip_13;
    // 0x80415EC4: addiu       $v0, $zero, 0xFF
    ctx->r2 = ADD32(0, 0XFF);
    skip_13:
L_80415EC8:
    // 0x80415EC8: sll         $v1, $v0, 1
    ctx->r3 = S32(ctx->r2 << 1);
    // 0x80415ECC: addu        $a0, $v1, $v0
    ctx->r4 = ADD32(ctx->r3, ctx->r2);
    // 0x80415ED0: sltiu       $a2, $a0, 0x200
    ctx->r6 = ctx->r4 < 0X200 ? 1 : 0;
    // 0x80415ED4: andi        $v0, $v0, 0xFF
    ctx->r2 = ctx->r2 & 0XFF;
    // 0x80415ED8: beq         $a2, $zero, L_80415EE8
    if (ctx->r6 == 0) {
        // 0x80415EDC: addiu       $a1, $zero, 0xFF
        ctx->r5 = ADD32(0, 0XFF);
            goto L_80415EE8;
    }
    // 0x80415EDC: addiu       $a1, $zero, 0xFF
    ctx->r5 = ADD32(0, 0XFF);
    // 0x80415EE0: srl         $a0, $a0, 1
    ctx->r4 = S32(U32(ctx->r4) >> 1);
    // 0x80415EE4: andi        $a1, $a0, 0xFF
    ctx->r5 = ctx->r4 & 0XFF;
L_80415EE8:
    // 0x80415EE8: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80415EEC: sltiu       $a0, $v1, 0x100
    ctx->r4 = ctx->r3 < 0X100 ? 1 : 0;
    // 0x80415EF0: sb          $v0, -0x4460($s1)
    MEM_B(-0X4460, ctx->r17) = ctx->r2;
    // 0x80415EF4: bne         $a0, $zero, L_80415F00
    if (ctx->r4 != 0) {
        // 0x80415EF8: sb          $a1, -0x445E($a2)
        MEM_B(-0X445E, ctx->r6) = ctx->r5;
            goto L_80415F00;
    }
    // 0x80415EF8: sb          $a1, -0x445E($a2)
    MEM_B(-0X445E, ctx->r6) = ctx->r5;
    // 0x80415EFC: addiu       $v1, $zero, 0xFF
    ctx->r3 = ADD32(0, 0XFF);
L_80415F00:
    // 0x80415F00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80415F04: sb          $v1, -0x445C($a0)
    MEM_B(-0X445C, ctx->r4) = ctx->r3;
    // 0x80415F08: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415F0C: sb          $v0, -0x445A($v1)
    MEM_B(-0X445A, ctx->r3) = ctx->r2;
    // 0x80415F10: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415F14: jal         0x80413F94
    // 0x80415F18: sb          $v0, -0x4458($v1)
    MEM_B(-0X4458, ctx->r3) = ctx->r2;
    hm_func_80413F94(rdram, ctx);
        goto after_7;
    // 0x80415F18: sb          $v0, -0x4458($v1)
    MEM_B(-0X4458, ctx->r3) = ctx->r2;
    after_7:
    // 0x80415F1C: jal         0x80414110
    // 0x80415F20: nop

    hm_func_80414110(rdram, ctx);
        goto after_8;
    // 0x80415F20: nop

    after_8:
    // 0x80415F24: jal         0x8041401C
    // 0x80415F28: nop

    hm_func_8041401C(rdram, ctx);
        goto after_9;
    // 0x80415F28: nop

    after_9:
    // 0x80415F2C: jal         0x804142D8
    // 0x80415F30: nop

    hm_func_804142D8(rdram, ctx);
        goto after_10;
    // 0x80415F30: nop

    after_10:
    // 0x80415F34: lw          $v1, 0x4EE4($s0)
    ctx->r3 = MEM_W(ctx->r16, 0X4EE4);
    // 0x80415F38: addiu       $v0, $zero, 0x1E
    ctx->r2 = ADD32(0, 0X1E);
    // 0x80415F3C: beql        $v1, $v0, L_8041807C
    if (ctx->r3 == ctx->r2) {
        // 0x80415F40: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_8041807C;
    }
    goto skip_14;
    // 0x80415F40: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    skip_14:
L_80415F44:
    // 0x80415F44: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
L_80415F48:
    // 0x80415F48: lw          $s1, 0x18($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X18);
L_80415F4C:
    // 0x80415F4C: lw          $s0, 0x14($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X14);
    // 0x80415F50: jr          $ra
    // 0x80415F54: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
    return;
    // 0x80415F54: addiu       $sp, $sp, 0x20
    ctx->r29 = ADD32(ctx->r29, 0X20);
L_80415F58:
    // 0x80415F58: addiu       $v1, $zero, 0x17
    ctx->r3 = ADD32(0, 0X17);
    // 0x80415F5C: bne         $v0, $v1, L_8041A12C
    if (ctx->r2 != ctx->r3) {
        // 0x80415F60: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_8041A12C;
    }
    // 0x80415F60: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415F64: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80415F68: addiu       $a0, $zero, 0xF
    ctx->r4 = ADD32(0, 0XF);
    // 0x80415F6C: beq         $v1, $a0, L_80418BE0
    if (ctx->r3 == ctx->r4) {
        // 0x80415F70: addiu       $a0, $zero, 0x2
        ctx->r4 = ADD32(0, 0X2);
            goto L_80418BE0;
    }
    // 0x80415F70: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    // 0x80415F74: bne         $v1, $a0, L_80416070
    if (ctx->r3 != ctx->r4) {
        // 0x80415F78: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80416070;
    }
    // 0x80415F78: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80415F7C: lbu         $a0, 0x69E5($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X69E5);
    // 0x80415F80: andi        $a1, $a0, 0x1
    ctx->r5 = ctx->r4 & 0X1;
    // 0x80415F84: bne         $a1, $zero, L_80415FAC
    if (ctx->r5 != 0) {
        // 0x80415F88: andi        $a1, $a0, 0x2
        ctx->r5 = ctx->r4 & 0X2;
            goto L_80415FAC;
    }
    // 0x80415F88: andi        $a1, $a0, 0x2
    ctx->r5 = ctx->r4 & 0X2;
    // 0x80415F8C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80415F90: lw          $a2, 0x67C($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X67C);
    // 0x80415F94: lui         $a1, 0xA0C0
    ctx->r5 = S32(0XA0C0 << 16);
    // 0x80415F98: ori         $a1, $a1, 0x810B
    ctx->r5 = ctx->r5 | 0X810B;
    // 0x80415F9C: beq         $a2, $a1, L_80419B7C
    if (ctx->r6 == ctx->r5) {
        // 0x80415FA0: ori         $a0, $a0, 0x1
        ctx->r4 = ctx->r4 | 0X1;
            goto L_80419B7C;
    }
    // 0x80415FA0: ori         $a0, $a0, 0x1
    ctx->r4 = ctx->r4 | 0X1;
L_80415FA4:
    // 0x80415FA4: lbu         $a0, 0x69E5($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X69E5);
    // 0x80415FA8: andi        $a1, $a0, 0x2
    ctx->r5 = ctx->r4 & 0X2;
L_80415FAC:
    // 0x80415FAC: beql        $a1, $zero, L_80416064
    if (ctx->r5 == 0) {
        // 0x80415FB0: andi        $a0, $a0, 0x6
        ctx->r4 = ctx->r4 & 0X6;
            goto L_80416064;
    }
    goto skip_15;
    // 0x80415FB0: andi        $a0, $a0, 0x6
    ctx->r4 = ctx->r4 & 0X6;
    skip_15:
    // 0x80415FB4: andi        $a1, $a0, 0x4
    ctx->r5 = ctx->r4 & 0X4;
    // 0x80415FB8: bne         $a1, $zero, L_80415FCC
    if (ctx->r5 != 0) {
        // 0x80415FBC: or          $a2, $a1, $zero
        ctx->r6 = ctx->r5 | 0;
            goto L_80415FCC;
    }
    // 0x80415FBC: or          $a2, $a1, $zero
    ctx->r6 = ctx->r5 | 0;
    // 0x80415FC0: lui         $a1, 0x8019
    ctx->r5 = S32(0X8019 << 16);
    // 0x80415FC4: addiu       $a3, $zero, -0x4300
    ctx->r7 = ADD32(0, -0X4300);
    // 0x80415FC8: sh          $a3, -0x3146($a1)
    MEM_H(-0X3146, ctx->r5) = ctx->r7;
L_80415FCC:
    // 0x80415FCC: bnel        $a2, $zero, L_80416064
    if (ctx->r6 != 0) {
        // 0x80415FD0: andi        $a0, $a0, 0x6
        ctx->r4 = ctx->r4 & 0X6;
            goto L_80416064;
    }
    goto skip_16;
    // 0x80415FD0: andi        $a0, $a0, 0x6
    ctx->r4 = ctx->r4 & 0X6;
    skip_16:
    // 0x80415FD4: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80415FD8: lw          $a2, 0x67C($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X67C);
    // 0x80415FDC: lui         $a1, 0xA0C0
    ctx->r5 = S32(0XA0C0 << 16);
    // 0x80415FE0: ori         $a1, $a1, 0x8107
    ctx->r5 = ctx->r5 | 0X8107;
    // 0x80415FE4: bnel        $a2, $a1, L_80416064
    if (ctx->r6 != ctx->r5) {
        // 0x80415FE8: andi        $a0, $a0, 0x6
        ctx->r4 = ctx->r4 & 0X6;
            goto L_80416064;
    }
    goto skip_17;
    // 0x80415FE8: andi        $a0, $a0, 0x6
    ctx->r4 = ctx->r4 & 0X6;
    skip_17:
    // 0x80415FEC: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80415FF0: lw          $a1, -0x456C($t3)
    ctx->r5 = MEM_W(ctx->r11, -0X456C);
    // 0x80415FF4: lui         $t8, 0x8008
    ctx->r24 = S32(0X8008 << 16);
    // 0x80415FF8: lui         $t7, 0x8008
    ctx->r15 = S32(0X8008 << 16);
    // 0x80415FFC: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80416000: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80416004: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80416008: lhu         $t2, -0x4558($t8)
    ctx->r10 = MEM_HU(ctx->r24, -0X4558);
    // 0x8041600C: lhu         $t1, -0x4556($t7)
    ctx->r9 = MEM_HU(ctx->r15, -0X4556);
    // 0x80416010: lhu         $t0, -0x4552($t6)
    ctx->r8 = MEM_HU(ctx->r14, -0X4552);
    // 0x80416014: lhu         $a3, -0x4554($t5)
    ctx->r7 = MEM_HU(ctx->r13, -0X4554);
    // 0x80416018: lw          $a2, -0x4570($t4)
    ctx->r6 = MEM_W(ctx->r12, -0X4570);
    // 0x8041601C: addiu       $a1, $a1, 0x3A98
    ctx->r5 = ADD32(ctx->r5, 0X3A98);
    // 0x80416020: ori         $a0, $a0, 0x4
    ctx->r4 = ctx->r4 | 0X4;
    // 0x80416024: addiu       $t2, $t2, 0x3C
    ctx->r10 = ADD32(ctx->r10, 0X3C);
    // 0x80416028: addiu       $t1, $t1, 0x64
    ctx->r9 = ADD32(ctx->r9, 0X64);
    // 0x8041602C: addiu       $t0, $t0, 0x14
    ctx->r8 = ADD32(ctx->r8, 0X14);
    // 0x80416030: addiu       $a3, $a3, 0x32
    ctx->r7 = ADD32(ctx->r7, 0X32);
    // 0x80416034: addiu       $a2, $a2, 0x7530
    ctx->r6 = ADD32(ctx->r6, 0X7530);
    // 0x80416038: sb          $a0, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r4;
    // 0x8041603C: sw          $a1, -0x456C($t3)
    MEM_W(-0X456C, ctx->r11) = ctx->r5;
    // 0x80416040: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416044: addiu       $a1, $zero, 0x1A
    ctx->r5 = ADD32(0, 0X1A);
    // 0x80416048: sh          $t2, -0x4558($t8)
    MEM_H(-0X4558, ctx->r24) = ctx->r10;
    // 0x8041604C: sh          $t1, -0x4556($t7)
    MEM_H(-0X4556, ctx->r15) = ctx->r9;
    // 0x80416050: sh          $t0, -0x4552($t6)
    MEM_H(-0X4552, ctx->r14) = ctx->r8;
    // 0x80416054: sh          $a3, -0x4554($t5)
    MEM_H(-0X4554, ctx->r13) = ctx->r7;
    // 0x80416058: sw          $a2, -0x4570($t4)
    MEM_W(-0X4570, ctx->r12) = ctx->r6;
    // 0x8041605C: sb          $a1, -0x458D($v1)
    MEM_B(-0X458D, ctx->r3) = ctx->r5;
    // 0x80416060: andi        $a0, $a0, 0x6
    ctx->r4 = ctx->r4 & 0X6;
L_80416064:
    // 0x80416064: addiu       $v1, $zero, 0x6
    ctx->r3 = ADD32(0, 0X6);
    // 0x80416068: beq         $a0, $v1, L_804199AC
    if (ctx->r4 == ctx->r3) {
        // 0x8041606C: lui         $v1, 0x8019
        ctx->r3 = S32(0X8019 << 16);
            goto L_804199AC;
    }
    // 0x8041606C: lui         $v1, 0x8019
    ctx->r3 = S32(0X8019 << 16);
L_80416070:
    // 0x80416070: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
L_80416074:
    // 0x80416074: bne         $v0, $v1, L_804157B4
    if (ctx->r2 != ctx->r3) {
        // 0x80416078: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804157B4;
    }
    // 0x80416078: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041607C: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80416080: addiu       $v1, $zero, 0x5
    ctx->r3 = ADD32(0, 0X5);
    // 0x80416084: beq         $a0, $v1, L_80418898
    if (ctx->r4 == ctx->r3) {
        // 0x80416088: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418898;
    }
    // 0x80416088: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041608C: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
L_80416090:
    // 0x80416090: lbu         $a3, 0x69F0($t1)
    ctx->r7 = MEM_BU(ctx->r9, 0X69F0);
L_80416094:
    // 0x80416094: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
L_80416098:
    // 0x80416098: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x8041609C: bgez        $v1, L_804157E8
    if (SIGNED(ctx->r3) >= 0) {
        // 0x804160A0: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_804157E8;
    }
    // 0x804160A0: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
L_804160A4:
    // 0x804160A4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804160A8:
    // 0x804160A8: lw          $v1, 0x5368($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X5368);
    // 0x804160AC: addiu       $t2, $v1, -0x1B
    ctx->r10 = ADD32(ctx->r3, -0X1B);
    // 0x804160B0: bne         $v1, $zero, L_80418160
    if (ctx->r3 != 0) {
        // 0x804160B4: sltiu       $t4, $t2, 0x6
        ctx->r12 = ctx->r10 < 0X6 ? 1 : 0;
            goto L_80418160;
    }
    // 0x804160B4: sltiu       $t4, $t2, 0x6
    ctx->r12 = ctx->r10 < 0X6 ? 1 : 0;
    // 0x804160B8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804160BC: lw          $a1, 0x536C($a0)
    ctx->r5 = MEM_W(ctx->r4, 0X536C);
    // 0x804160C0: addiu       $a0, $zero, 0x3
    ctx->r4 = ADD32(0, 0X3);
    // 0x804160C4: beq         $a1, $a0, L_804188E8
    if (ctx->r5 == ctx->r4) {
        // 0x804160C8: addiu       $ra, $zero, 0x1
        ctx->r31 = ADD32(0, 0X1);
            goto L_804188E8;
    }
    // 0x804160C8: addiu       $ra, $zero, 0x1
    ctx->r31 = ADD32(0, 0X1);
    // 0x804160CC: or          $t5, $zero, $zero
    ctx->r13 = 0 | 0;
    // 0x804160D0: or          $t6, $zero, $zero
    ctx->r14 = 0 | 0;
    // 0x804160D4: or          $t8, $zero, $zero
    ctx->r24 = 0 | 0;
    // 0x804160D8: or          $t7, $zero, $zero
    ctx->r15 = 0 | 0;
    // 0x804160DC: or          $ra, $zero, $zero
    ctx->r31 = 0 | 0;
    // 0x804160E0: or          $t0, $zero, $zero
    ctx->r8 = 0 | 0;
    // 0x804160E4: or          $t9, $zero, $zero
    ctx->r25 = 0 | 0;
    // 0x804160E8: or          $s1, $zero, $zero
    ctx->r17 = 0 | 0;
L_804160EC:
    // 0x804160EC: addiu       $a0, $zero, -0x5
    ctx->r4 = ADD32(0, -0X5);
    // 0x804160F0: and         $a0, $v1, $a0
    ctx->r4 = ctx->r3 & ctx->r4;
    // 0x804160F4: xori        $a0, $a0, 0x13
    ctx->r4 = ctx->r4 ^ 0X13;
    // 0x804160F8: sltiu       $a0, $a0, 0x1
    ctx->r4 = ctx->r4 < 0X1 ? 1 : 0;
    // 0x804160FC: or          $ra, $a0, $ra
    ctx->r31 = ctx->r4 | ctx->r31;
L_80416100:
    // 0x80416100: xori        $a2, $v1, 0x13
    ctx->r6 = ctx->r3 ^ 0X13;
    // 0x80416104: xori        $a1, $v1, 0x17
    ctx->r5 = ctx->r3 ^ 0X17;
    // 0x80416108: addiu       $a0, $zero, 0x1A
    ctx->r4 = ADD32(0, 0X1A);
    // 0x8041610C: sltiu       $a2, $a2, 0x1
    ctx->r6 = ctx->r6 < 0X1 ? 1 : 0;
    // 0x80416110: bne         $v1, $a0, L_8041949C
    if (ctx->r3 != ctx->r4) {
        // 0x80416114: sltiu       $a1, $a1, 0x1
        ctx->r5 = ctx->r5 < 0X1 ? 1 : 0;
            goto L_8041949C;
    }
    // 0x80416114: sltiu       $a1, $a1, 0x1
    ctx->r5 = ctx->r5 < 0X1 ? 1 : 0;
    // 0x80416118: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041611C: lw          $a0, 0x536C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X536C);
    // 0x80416120: xori        $a0, $a0, 0x3
    ctx->r4 = ctx->r4 ^ 0X3;
    // 0x80416124: sltu        $a0, $zero, $a0
    ctx->r4 = 0 < ctx->r4 ? 1 : 0;
    // 0x80416128: or          $a0, $t4, $a0
    ctx->r4 = ctx->r12 | ctx->r4;
    // 0x8041612C: or          $a0, $a0, $ra
    ctx->r4 = ctx->r4 | ctx->r31;
    // 0x80416130: addiu       $t3, $v1, -0x21
    ctx->r11 = ADD32(ctx->r3, -0X21);
L_80416134:
    // 0x80416134: sltiu       $t3, $t3, 0x3
    ctx->r11 = ctx->r11 < 0X3 ? 1 : 0;
    // 0x80416138: or          $a0, $t3, $a0
    ctx->r4 = ctx->r11 | ctx->r4;
    // 0x8041613C: bne         $a0, $zero, L_804181D4
    if (ctx->r4 != 0) {
        // 0x80416140: or          $t3, $t9, $s1
        ctx->r11 = ctx->r25 | ctx->r17;
            goto L_804181D4;
    }
    // 0x80416140: or          $t3, $t9, $s1
    ctx->r11 = ctx->r25 | ctx->r17;
    // 0x80416144: bne         $v1, $zero, L_804193F8
    if (ctx->r3 != 0) {
        // 0x80416148: or          $a1, $a2, $a1
        ctx->r5 = ctx->r6 | ctx->r5;
            goto L_804193F8;
    }
    // 0x80416148: or          $a1, $a2, $a1
    ctx->r5 = ctx->r6 | ctx->r5;
    // 0x8041614C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416150: lw          $a0, 0x536C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X536C);
    // 0x80416154: or          $t7, $t8, $t7
    ctx->r15 = ctx->r24 | ctx->r15;
    // 0x80416158: xori        $a0, $a0, 0x3
    ctx->r4 = ctx->r4 ^ 0X3;
    // 0x8041615C: sltiu       $a0, $a0, 0x1
    ctx->r4 = ctx->r4 < 0X1 ? 1 : 0;
    // 0x80416160: or          $a0, $a0, $t7
    ctx->r4 = ctx->r4 | ctx->r15;
    // 0x80416164: or          $a0, $t6, $a0
    ctx->r4 = ctx->r14 | ctx->r4;
    // 0x80416168: or          $a0, $t5, $a0
    ctx->r4 = ctx->r13 | ctx->r4;
L_8041616C:
    // 0x8041616C: or          $t0, $t0, $t3
    ctx->r8 = ctx->r8 | ctx->r11;
    // 0x80416170: or          $a1, $t0, $a1
    ctx->r5 = ctx->r8 | ctx->r5;
    // 0x80416174: or          $a1, $a1, $a0
    ctx->r5 = ctx->r5 | ctx->r4;
L_80416178:
    // 0x80416178: sltiu       $t2, $t2, 0x5
    ctx->r10 = ctx->r10 < 0X5 ? 1 : 0;
    // 0x8041617C: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x80416180: bne         $v1, $a2, L_80419418
    if (ctx->r3 != ctx->r6) {
        // 0x80416184: or          $a0, $t2, $zero
        ctx->r4 = ctx->r10 | 0;
            goto L_80419418;
    }
    // 0x80416184: or          $a0, $t2, $zero
    ctx->r4 = ctx->r10 | 0;
    // 0x80416188: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041618C: lw          $a0, 0x536C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X536C);
    // 0x80416190: addiu       $v1, $zero, 0x3
    ctx->r3 = ADD32(0, 0X3);
    // 0x80416194: beq         $a0, $v1, L_8041941C
    if (ctx->r4 == ctx->r3) {
        // 0x80416198: or          $a1, $t2, $a1
        ctx->r5 = ctx->r10 | ctx->r5;
            goto L_8041941C;
    }
    // 0x80416198: or          $a1, $t2, $a1
    ctx->r5 = ctx->r10 | ctx->r5;
    // 0x8041619C: andi        $a3, $a3, 0xBF
    ctx->r7 = ctx->r7 & 0XBF;
    // 0x804161A0: b           L_804181DC
    // 0x804161A4: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
        goto L_804181DC;
    // 0x804161A4: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
L_804161A8:
    // 0x804161A8: lw          $t0, -0x4428($v1)
    ctx->r8 = MEM_W(ctx->r3, -0X4428);
    // 0x804161AC: lui         $a0, 0x2
    ctx->r4 = S32(0X2 << 16);
    // 0x804161B0: bne         $t0, $a0, L_804158E0
    if (ctx->r8 != ctx->r4) {
        // 0x804161B4: lui         $s1, 0x8008
        ctx->r17 = S32(0X8008 << 16);
            goto L_804158E0;
    }
    // 0x804161B4: lui         $s1, 0x8008
    ctx->r17 = S32(0X8008 << 16);
    // 0x804161B8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804161BC: lbu         $t1, -0x443D($t0)
    ctx->r9 = MEM_BU(ctx->r8, -0X443D);
    // 0x804161C0: lbu         $a0, -0x4460($s1)
    ctx->r4 = MEM_BU(ctx->r17, -0X4460);
    // 0x804161C4: bnel        $a0, $t1, L_804158E4
    if (ctx->r4 != ctx->r9) {
        // 0x804161C8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804158E4;
    }
    goto skip_18;
    // 0x804161C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_18:
    // 0x804161CC: mtc1        $a0, $f0
    ctx->f0.u32l = ctx->r4;
    // 0x804161D0: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x804161D4: ldc1        $f2, -0x5A58($a0)
    CHECK_FR(ctx, 2);
    ctx->f2.u64 = LD(ctx->r4, -0X5A58);
    // 0x804161D8: cvt.d.w     $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.d = CVT_D_W(ctx->f0.u32l);
    // 0x804161DC: mul.d       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = MUL_D(ctx->f0.d, ctx->f2.d);
    // 0x804161E0: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x804161E4: ldc1        $f2, -0x5A50($a0)
    CHECK_FR(ctx, 2);
    ctx->f2.u64 = LD(ctx->r4, -0X5A50);
    // 0x804161E8: c.le.d      $f2, $f0
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    c1cs = ctx->f2.d <= ctx->f0.d;
    // 0x804161EC: nop

    // 0x804161F0: bc1tl       L_80419EB0
    if (c1cs) {
        // 0x804161F4: sub.d       $f0, $f0, $f2
        CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = ctx->f0.d - ctx->f2.d;
            goto L_80419EB0;
    }
    goto skip_19;
    // 0x804161F4: sub.d       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = ctx->f0.d - ctx->f2.d;
    skip_19:
    // 0x804161F8: trunc.w.d   $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.u32l = TRUNC_W_D(ctx->f0.d);
    // 0x804161FC: mfc1        $a0, $f0
    ctx->r4 = (int32_t)ctx->f0.u32l;
    // 0x80416200: nop

L_80416204:
    // 0x80416204: sb          $a0, -0x443D($t0)
    MEM_B(-0X443D, ctx->r8) = ctx->r4;
    // 0x80416208: lui         $a0, 0x1
    ctx->r4 = S32(0X1 << 16);
    // 0x8041620C: b           L_804158E0
    // 0x80416210: sw          $a0, -0x4428($v1)
    MEM_W(-0X4428, ctx->r3) = ctx->r4;
        goto L_804158E0;
    // 0x80416210: sw          $a0, -0x4428($v1)
    MEM_W(-0X4428, ctx->r3) = ctx->r4;
L_80416214:
    // 0x80416214: lhu         $a1, 0x1B28($v1)
    ctx->r5 = MEM_HU(ctx->r3, 0X1B28);
    // 0x80416218: bnel        $a1, $zero, L_80415814
    if (ctx->r5 != 0) {
        // 0x8041621C: addiu       $v1, $zero, 0xE
        ctx->r3 = ADD32(0, 0XE);
            goto L_80415814;
    }
    goto skip_20;
    // 0x8041621C: addiu       $v1, $zero, 0xE
    ctx->r3 = ADD32(0, 0XE);
    skip_20:
L_80416220:
    // 0x80416220: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x80416224: sh          $a1, 0x1B28($v1)
    MEM_H(0X1B28, ctx->r3) = ctx->r5;
    // 0x80416228: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x8041622C: addiu       $a1, $zero, 0x2
    ctx->r5 = ADD32(0, 0X2);
    // 0x80416230: sh          $a1, 0x1B2A($v1)
    MEM_H(0X1B2A, ctx->r3) = ctx->r5;
    // 0x80416234: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80416238: addiu       $a1, $zero, 0x3
    ctx->r5 = ADD32(0, 0X3);
    // 0x8041623C: sh          $a1, 0x1B2C($v1)
    MEM_H(0X1B2C, ctx->r3) = ctx->r5;
    // 0x80416240: addiu       $v1, $zero, 0xE
    ctx->r3 = ADD32(0, 0XE);
    // 0x80416244: bne         $v0, $v1, L_80415820
    if (ctx->r2 != ctx->r3) {
        // 0x80416248: addiu       $v1, $zero, 0x19
        ctx->r3 = ADD32(0, 0X19);
            goto L_80415820;
    }
    // 0x80416248: addiu       $v1, $zero, 0x19
    ctx->r3 = ADD32(0, 0X19);
    // 0x8041624C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80416250:
    // 0x80416250: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80416254: bne         $v1, $v0, L_80415848
    if (ctx->r3 != ctx->r2) {
        // 0x80416258: lui         $a3, 0x8008
        ctx->r7 = S32(0X8008 << 16);
            goto L_80415848;
    }
    // 0x80416258: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x8041625C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416260: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80416264: sltiu       $v1, $v1, 0x7
    ctx->r3 = ctx->r3 < 0X7 ? 1 : 0;
    // 0x80416268: bne         $v1, $zero, L_8041584C
    if (ctx->r3 != 0) {
        // 0x8041626C: lhu         $v1, -0x457A($a3)
        ctx->r3 = MEM_HU(ctx->r7, -0X457A);
            goto L_8041584C;
    }
    // 0x8041626C: lhu         $v1, -0x457A($a3)
    ctx->r3 = MEM_HU(ctx->r7, -0X457A);
    // 0x80416270: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80416274: lw          $a1, 0x67C($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X67C);
    // 0x80416278: lui         $v1, 0x80C0
    ctx->r3 = S32(0X80C0 << 16);
    // 0x8041627C: ori         $v1, $v1, 0xE07F
    ctx->r3 = ctx->r3 | 0XE07F;
    // 0x80416280: beq         $a1, $v1, L_80419C70
    if (ctx->r5 == ctx->r3) {
        // 0x80416284: lbu         $v1, 0x69F2($a2)
        ctx->r3 = MEM_BU(ctx->r6, 0X69F2);
            goto L_80419C70;
    }
    // 0x80416284: lbu         $v1, 0x69F2($a2)
    ctx->r3 = MEM_BU(ctx->r6, 0X69F2);
    // 0x80416288: andi        $v1, $v1, 0x8
    ctx->r3 = ctx->r3 & 0X8;
    // 0x8041628C: bne         $v1, $zero, L_80419CD4
    if (ctx->r3 != 0) {
        // 0x80416290: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419CD4;
    }
    // 0x80416290: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80416294: addiu       $a0, $zero, -0x3000
    ctx->r4 = ADD32(0, -0X3000);
    // 0x80416298: b           L_80415848
    // 0x8041629C: sh          $a0, 0x3D5A($v1)
    MEM_H(0X3D5A, ctx->r3) = ctx->r4;
        goto L_80415848;
    // 0x8041629C: sh          $a0, 0x3D5A($v1)
    MEM_H(0X3D5A, ctx->r3) = ctx->r4;
L_804162A0:
    // 0x804162A0: addiu       $v1, $zero, 0x1A
    ctx->r3 = ADD32(0, 0X1A);
    // 0x804162A4: bne         $v0, $v1, L_80416094
    if (ctx->r2 != ctx->r3) {
        // 0x804162A8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80416094;
    }
    // 0x804162A8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804162AC: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x804162B0: addiu       $v1, $zero, 0x3
    ctx->r3 = ADD32(0, 0X3);
    // 0x804162B4: bne         $a0, $v1, L_80416098
    if (ctx->r4 != ctx->r3) {
        // 0x804162B8: sll         $v1, $a3, 24
        ctx->r3 = S32(ctx->r7 << 24);
            goto L_80416098;
    }
    // 0x804162B8: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
    // 0x804162BC: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804162C0: lbu         $a2, 0x69F1($t0)
    ctx->r6 = MEM_BU(ctx->r8, 0X69F1);
    // 0x804162C4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804162C8: andi        $a1, $a2, 0x1
    ctx->r5 = ctx->r6 & 0X1;
    // 0x804162CC: beq         $a1, $zero, L_8041950C
    if (ctx->r5 == 0) {
        // 0x804162D0: lbu         $a0, -0x2E64($v1)
        ctx->r4 = MEM_BU(ctx->r3, -0X2E64);
            goto L_8041950C;
    }
    // 0x804162D0: lbu         $a0, -0x2E64($v1)
    ctx->r4 = MEM_BU(ctx->r3, -0X2E64);
    // 0x804162D4: lui         $t2, 0x8017
    ctx->r10 = S32(0X8017 << 16);
    // 0x804162D8: addiu       $v1, $zero, -0x47A0
    ctx->r3 = ADD32(0, -0X47A0);
    // 0x804162DC: andi        $a0, $a0, 0x20
    ctx->r4 = ctx->r4 & 0X20;
    // 0x804162E0: bne         $a0, $zero, L_804199B8
    if (ctx->r4 != 0) {
        // 0x804162E4: sh          $v1, -0x24B6($t2)
        MEM_H(-0X24B6, ctx->r10) = ctx->r3;
            goto L_804199B8;
    }
    // 0x804162E4: sh          $v1, -0x24B6($t2)
    MEM_H(-0X24B6, ctx->r10) = ctx->r3;
L_804162E8:
    // 0x804162E8: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
L_804162EC:
    // 0x804162EC: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x804162F0: bltzl       $v1, L_80419E2C
    if (SIGNED(ctx->r3) < 0) {
        // 0x804162F4: lui         $t2, 0x8017
        ctx->r10 = S32(0X8017 << 16);
            goto L_80419E2C;
    }
    goto skip_21;
    // 0x804162F4: lui         $t2, 0x8017
    ctx->r10 = S32(0X8017 << 16);
    skip_21:
    // 0x804162F8: beql        $a0, $zero, L_804157E8
    if (ctx->r4 == 0) {
        // 0x804162FC: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_804157E8;
    }
    goto skip_22;
    // 0x804162FC: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    skip_22:
    // 0x80416300: beql        $a1, $zero, L_804157E8
    if (ctx->r5 == 0) {
        // 0x80416304: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_804157E8;
    }
    goto skip_23;
    // 0x80416304: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    skip_23:
    // 0x80416308: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
L_8041630C:
    // 0x8041630C: lw          $a1, 0x67C($a0)
    ctx->r5 = MEM_W(ctx->r4, 0X67C);
    // 0x80416310: lui         $a0, 0xA0C0
    ctx->r4 = S32(0XA0C0 << 16);
    // 0x80416314: ori         $a0, $a0, 0x8112
    ctx->r4 = ctx->r4 | 0X8112;
    // 0x80416318: bne         $a1, $a0, L_804157DC
    if (ctx->r5 != ctx->r4) {
        // 0x8041631C: ori         $a2, $a2, 0x2
        ctx->r6 = ctx->r6 | 0X2;
            goto L_804157DC;
    }
    // 0x8041631C: ori         $a2, $a2, 0x2
    ctx->r6 = ctx->r6 | 0X2;
    // 0x80416320: b           L_804157DC
    // 0x80416324: sb          $a2, 0x69F1($t0)
    MEM_B(0X69F1, ctx->r8) = ctx->r6;
        goto L_804157DC;
    // 0x80416324: sb          $a2, 0x69F1($t0)
    MEM_B(0X69F1, ctx->r8) = ctx->r6;
L_80416328:
    // 0x80416328: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x8041632C: bne         $v1, $zero, L_80415DC8
    if (ctx->r3 != 0) {
        // 0x80416330: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    // 0x80416330: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80416334: lui         $a0, 0x8017
    ctx->r4 = S32(0X8017 << 16);
    // 0x80416338: lhu         $v1, -0x547E($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X547E);
    // 0x8041633C: addiu       $a1, $zero, 0x5AD0
    ctx->r5 = ADD32(0, 0X5AD0);
    // 0x80416340: beq         $v1, $a1, L_80416350
    if (ctx->r3 == ctx->r5) {
        // 0x80416344: ori         $a1, $zero, 0xE000
        ctx->r5 = 0 | 0XE000;
            goto L_80416350;
    }
    // 0x80416344: ori         $a1, $zero, 0xE000
    ctx->r5 = 0 | 0XE000;
    // 0x80416348: bnel        $v1, $a1, L_80415DC8
    if (ctx->r3 != ctx->r5) {
        // 0x8041634C: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    goto skip_24;
    // 0x8041634C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_24:
L_80416350:
    // 0x80416350: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80416354: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80416358: lui         $a1, 0x80C0
    ctx->r5 = S32(0X80C0 << 16);
    // 0x8041635C: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80416360: addiu       $a1, $a1, 0xE18
    ctx->r5 = ADD32(ctx->r5, 0XE18);
    // 0x80416364: beq         $a3, $a1, L_80419E40
    if (ctx->r7 == ctx->r5) {
        // 0x80416368: lw          $v1, -0x456C($a2)
        ctx->r3 = MEM_W(ctx->r6, -0X456C);
            goto L_80419E40;
    }
    // 0x80416368: lw          $v1, -0x456C($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X456C);
L_8041636C:
    // 0x8041636C: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
L_80416370:
    // 0x80416370: bnel        $v1, $zero, L_80419984
    if (ctx->r3 != 0) {
        // 0x80416374: addiu       $v1, $zero, 0x5AD0
        ctx->r3 = ADD32(0, 0X5AD0);
            goto L_80419984;
    }
    goto skip_25;
    // 0x80416374: addiu       $v1, $zero, 0x5AD0
    ctx->r3 = ADD32(0, 0X5AD0);
    skip_25:
    // 0x80416378: addiu       $v1, $zero, -0x2000
    ctx->r3 = ADD32(0, -0X2000);
    // 0x8041637C: b           L_80415DC4
    // 0x80416380: sh          $v1, -0x547E($a0)
    MEM_H(-0X547E, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80416380: sh          $v1, -0x547E($a0)
    MEM_H(-0X547E, ctx->r4) = ctx->r3;
L_80416384:
    // 0x80416384: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416388: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x8041638C: addiu       $v1, $zero, 0xD
    ctx->r3 = ADD32(0, 0XD);
    // 0x80416390: bne         $a0, $v1, L_80415DC8
    if (ctx->r4 != ctx->r3) {
        // 0x80416394: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    // 0x80416394: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80416398: lui         $a0, 0x8018
    ctx->r4 = S32(0X8018 << 16);
    // 0x8041639C: lhu         $v1, 0x3412($a0)
    ctx->r3 = MEM_HU(ctx->r4, 0X3412);
    // 0x804163A0: addiu       $a1, $zero, 0x3A68
    ctx->r5 = ADD32(0, 0X3A68);
    // 0x804163A4: beq         $v1, $a1, L_804163B4
    if (ctx->r3 == ctx->r5) {
        // 0x804163A8: ori         $a1, $zero, 0xDF00
        ctx->r5 = 0 | 0XDF00;
            goto L_804163B4;
    }
    // 0x804163A8: ori         $a1, $zero, 0xDF00
    ctx->r5 = 0 | 0XDF00;
    // 0x804163AC: bnel        $v1, $a1, L_80415DC8
    if (ctx->r3 != ctx->r5) {
        // 0x804163B0: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    goto skip_26;
    // 0x804163B0: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_26:
L_804163B4:
    // 0x804163B4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x804163B8: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x804163BC: lui         $a1, 0x80C0
    ctx->r5 = S32(0X80C0 << 16);
    // 0x804163C0: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x804163C4: addiu       $a1, $a1, 0xE18
    ctx->r5 = ADD32(ctx->r5, 0XE18);
    // 0x804163C8: beq         $a3, $a1, L_80419AC4
    if (ctx->r7 == ctx->r5) {
        // 0x804163CC: lw          $v1, -0x456C($a2)
        ctx->r3 = MEM_W(ctx->r6, -0X456C);
            goto L_80419AC4;
    }
    // 0x804163CC: lw          $v1, -0x456C($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X456C);
L_804163D0:
    // 0x804163D0: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
L_804163D4:
    // 0x804163D4: bnel        $v1, $zero, L_80419994
    if (ctx->r3 != 0) {
        // 0x804163D8: addiu       $v1, $zero, 0x3A68
        ctx->r3 = ADD32(0, 0X3A68);
            goto L_80419994;
    }
    goto skip_27;
    // 0x804163D8: addiu       $v1, $zero, 0x3A68
    ctx->r3 = ADD32(0, 0X3A68);
    skip_27:
    // 0x804163DC: addiu       $v1, $zero, -0x2100
    ctx->r3 = ADD32(0, -0X2100);
    // 0x804163E0: b           L_80415DC4
    // 0x804163E4: sh          $v1, 0x3412($a0)
    MEM_H(0X3412, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x804163E4: sh          $v1, 0x3412($a0)
    MEM_H(0X3412, ctx->r4) = ctx->r3;
L_804163E8:
    // 0x804163E8: lbu         $v1, 0x69F8($a1)
    ctx->r3 = MEM_BU(ctx->r5, 0X69F8);
    // 0x804163EC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804163F0: andi        $v1, $v1, 0x1
    ctx->r3 = ctx->r3 & 0X1;
    // 0x804163F4: beq         $v1, $zero, L_80418868
    if (ctx->r3 == 0) {
        // 0x804163F8: lw          $a2, -0x4428($a0)
        ctx->r6 = MEM_W(ctx->r4, -0X4428);
            goto L_80418868;
    }
    // 0x804163F8: lw          $a2, -0x4428($a0)
    ctx->r6 = MEM_W(ctx->r4, -0X4428);
    // 0x804163FC: lui         $v1, 0x4
    ctx->r3 = S32(0X4 << 16);
    // 0x80416400: bne         $a2, $v1, L_80415D94
    if (ctx->r6 != ctx->r3) {
        // 0x80416404: addiu       $v1, $zero, 0x10
        ctx->r3 = ADD32(0, 0X10);
            goto L_80415D94;
    }
    // 0x80416404: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
    // 0x80416408: lui         $v1, 0x40
    ctx->r3 = S32(0X40 << 16);
    // 0x8041640C: addiu       $v1, $v1, 0x2
    ctx->r3 = ADD32(ctx->r3, 0X2);
    // 0x80416410: sb          $zero, 0x69F8($a1)
    MEM_B(0X69F8, ctx->r5) = 0;
    // 0x80416414: lui         $a1, 0x800C
    ctx->r5 = S32(0X800C << 16);
    // 0x80416418: sw          $v1, 0x2EA8($a1)
    MEM_W(0X2EA8, ctx->r5) = ctx->r3;
    // 0x8041641C: lui         $v1, 0x1
    ctx->r3 = S32(0X1 << 16);
    // 0x80416420: b           L_80415D90
    // 0x80416424: sw          $v1, -0x4428($a0)
    MEM_W(-0X4428, ctx->r4) = ctx->r3;
        goto L_80415D90;
    // 0x80416424: sw          $v1, -0x4428($a0)
    MEM_W(-0X4428, ctx->r4) = ctx->r3;
L_80416428:
    // 0x80416428: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041642C: lw          $t1, -0x4428($t0)
    ctx->r9 = MEM_W(ctx->r8, -0X4428);
    // 0x80416430: lui         $t0, 0x1
    ctx->r8 = S32(0X1 << 16);
    // 0x80416434: bne         $t1, $t0, L_80415D7C
    if (ctx->r9 != ctx->r8) {
        // 0x80416438: addiu       $t0, $zero, 0x1
        ctx->r8 = ADD32(0, 0X1);
            goto L_80415D7C;
    }
    // 0x80416438: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041643C: bne         $a0, $zero, L_80415D7C
    if (ctx->r4 != 0) {
        // 0x80416440: nop
    
            goto L_80415D7C;
    }
    // 0x80416440: nop

    // 0x80416444: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    // 0x80416448: lbu         $a0, -0x44C7($t1)
    ctx->r4 = MEM_BU(ctx->r9, -0X44C7);
    // 0x8041644C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x80416450: ori         $a0, $a0, 0x1
    ctx->r4 = ctx->r4 | 0X1;
    // 0x80416454: sb          $a0, -0x44C7($t1)
    MEM_B(-0X44C7, ctx->r9) = ctx->r4;
    // 0x80416458: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041645C: addiu       $t1, $zero, -0x1
    ctx->r9 = ADD32(0, -0X1);
    // 0x80416460: sb          $t1, -0x44C6($a0)
    MEM_B(-0X44C6, ctx->r4) = ctx->r9;
    // 0x80416464: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416468: sb          $t0, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = ctx->r8;
    // 0x8041646C: sb          $t0, -0x44B5($a0)
    MEM_B(-0X44B5, ctx->r4) = ctx->r8;
    // 0x80416470: lhu         $a0, -0x457C($a2)
    ctx->r4 = MEM_HU(ctx->r6, -0X457C);
L_80416474:
    // 0x80416474: bne         $a0, $zero, L_804164D0
    if (ctx->r4 != 0) {
        // 0x80416478: lui         $a0, 0x8009
        ctx->r4 = S32(0X8009 << 16);
            goto L_804164D0;
    }
    // 0x80416478: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x8041647C: lhu         $a0, -0x457A($a3)
    ctx->r4 = MEM_HU(ctx->r7, -0X457A);
    // 0x80416480: mtc1        $a0, $f0
    ctx->f0.u32l = ctx->r4;
    // 0x80416484: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80416488: ldc1        $f2, -0x5A48($a0)
    CHECK_FR(ctx, 2);
    ctx->f2.u64 = LD(ctx->r4, -0X5A48);
    // 0x8041648C: cvt.d.w     $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.d = CVT_D_W(ctx->f0.u32l);
    // 0x80416490: mul.d       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = MUL_D(ctx->f0.d, ctx->f2.d);
    // 0x80416494: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x80416498: ldc1        $f2, -0x5A50($a0)
    CHECK_FR(ctx, 2);
    ctx->f2.u64 = LD(ctx->r4, -0X5A50);
    // 0x8041649C: c.le.d      $f2, $f0
    CHECK_FR(ctx, 2);
    CHECK_FR(ctx, 0);
    c1cs = ctx->f2.d <= ctx->f0.d;
    // 0x804164A0: nop

    // 0x804164A4: bc1tl       L_8041945C
    if (c1cs) {
        // 0x804164A8: sub.d       $f0, $f0, $f2
        CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = ctx->f0.d - ctx->f2.d;
            goto L_8041945C;
    }
    goto skip_28;
    // 0x804164A8: sub.d       $f0, $f0, $f2
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 2);
    NAN_CHECK(ctx->f0.d); NAN_CHECK(ctx->f2.d); 
    ctx->f0.d = ctx->f0.d - ctx->f2.d;
    skip_28:
    // 0x804164AC: trunc.w.d   $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.u32l = TRUNC_W_D(ctx->f0.d);
    // 0x804164B0: mfc1        $a0, $f0
    ctx->r4 = (int32_t)ctx->f0.u32l;
L_804164B4:
    // 0x804164B4: sb          $zero, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = 0;
    // 0x804164B8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804164BC: sb          $zero, -0x44C6($v1)
    MEM_B(-0X44C6, ctx->r3) = 0;
    // 0x804164C0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804164C4: sh          $a0, -0x457C($a2)
    MEM_H(-0X457C, ctx->r6) = ctx->r4;
    // 0x804164C8: b           L_80415D84
    // 0x804164CC: sb          $zero, -0x44B5($v1)
    MEM_B(-0X44B5, ctx->r3) = 0;
        goto L_80415D84;
    // 0x804164CC: sb          $zero, -0x44B5($v1)
    MEM_B(-0X44B5, ctx->r3) = 0;
L_804164D0:
    // 0x804164D0: lw          $a0, -0x3A70($a0)
    ctx->r4 = MEM_W(ctx->r4, -0X3A70);
    // 0x804164D4: beql        $a0, $zero, L_80415D84
    if (ctx->r4 == 0) {
        // 0x804164D8: sb          $zero, 0x69F9($v1)
        MEM_B(0X69F9, ctx->r3) = 0;
            goto L_80415D84;
    }
    goto skip_29;
    // 0x804164D8: sb          $zero, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = 0;
    skip_29:
    // 0x804164DC: addiu       $a2, $zero, 0x16
    ctx->r6 = ADD32(0, 0X16);
    // 0x804164E0: beql        $a0, $a2, L_80415D84
    if (ctx->r4 == ctx->r6) {
        // 0x804164E4: sb          $zero, 0x69F9($v1)
        MEM_B(0X69F9, ctx->r3) = 0;
            goto L_80415D84;
    }
    goto skip_30;
    // 0x804164E4: sb          $zero, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = 0;
    skip_30:
    // 0x804164E8: addiu       $a2, $zero, 0x406
    ctx->r6 = ADD32(0, 0X406);
    // 0x804164EC: bnel        $a0, $a2, L_80415D88
    if (ctx->r4 != ctx->r6) {
        // 0x804164F0: addiu       $v1, $zero, 0x309
        ctx->r3 = ADD32(0, 0X309);
            goto L_80415D88;
    }
    goto skip_31;
    // 0x804164F0: addiu       $v1, $zero, 0x309
    ctx->r3 = ADD32(0, 0X309);
    skip_31:
    // 0x804164F4: b           L_80415D84
    // 0x804164F8: sb          $zero, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = 0;
        goto L_80415D84;
    // 0x804164F8: sb          $zero, 0x69F9($v1)
    MEM_B(0X69F9, ctx->r3) = 0;
L_804164FC:
    // 0x804164FC: bne         $v1, $t0, L_804166E0
    if (ctx->r3 != ctx->r8) {
        // 0x80416500: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_804166E0;
    }
    // 0x80416500: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x80416504: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416508: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x8041650C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416510: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416514: bne         $t0, $a0, L_80416540
    if (ctx->r8 != ctx->r4) {
        // 0x80416518: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416540;
    }
    // 0x80416518: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041651C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416520: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416524: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416528: bne         $t0, $zero, L_80419604
    if (ctx->r8 != 0) {
        // 0x8041652C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419604;
    }
    // 0x8041652C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416530: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416534: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416538: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_8041653C:
    // 0x8041653C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416540:
    // 0x80416540: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x80416544: bne         $v1, $t0, L_80416614
    if (ctx->r3 != ctx->r8) {
        // 0x80416548: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_80416614;
    }
    // 0x80416548: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x8041654C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416550: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x80416554: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416558: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041655C: bne         $t0, $a0, L_80416588
    if (ctx->r8 != ctx->r4) {
        // 0x80416560: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416588;
    }
    // 0x80416560: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416564: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416568: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x8041656C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416570: bne         $t0, $zero, L_804195B4
    if (ctx->r8 != 0) {
        // 0x80416574: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195B4;
    }
    // 0x80416574: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416578: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041657C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416580: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80416584:
    // 0x80416584: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416588:
    // 0x80416588: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x8041658C: bne         $v1, $a0, L_80416650
    if (ctx->r3 != ctx->r4) {
        // 0x80416590: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416650;
    }
    // 0x80416590: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416594: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x80416598: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041659C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804165A0: bne         $t0, $a0, L_804165CC
    if (ctx->r8 != ctx->r4) {
        // 0x804165A4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804165CC;
    }
    // 0x804165A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804165A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804165AC: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804165B0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804165B4: bne         $t0, $zero, L_804194E4
    if (ctx->r8 != 0) {
        // 0x804165B8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194E4;
    }
    // 0x804165B8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804165BC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804165C0: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804165C4: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_804165C8:
    // 0x804165C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804165CC:
    // 0x804165CC: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x804165D0: bne         $v1, $a0, L_80416698
    if (ctx->r3 != ctx->r4) {
        // 0x804165D4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416698;
    }
    // 0x804165D4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804165D8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804165DC: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x804165E0: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804165E4: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804165E8: bne         $a0, $v1, L_80415D6C
    if (ctx->r4 != ctx->r3) {
        // 0x804165EC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415D6C;
    }
    // 0x804165EC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804165F0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804165F4: lbu         $v1, -0x300B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X300B);
    // 0x804165F8: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804165FC: bnel        $a0, $zero, L_804194B4
    if (ctx->r4 != 0) {
        // 0x80416600: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804194B4;
    }
    goto skip_32;
    // 0x80416600: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_32:
    // 0x80416604: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416608: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041660C: b           L_80415D68
    // 0x80416610: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_80415D68;
    // 0x80416610: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_80416614:
    // 0x80416614: bne         $v1, $a0, L_8041671C
    if (ctx->r3 != ctx->r4) {
        // 0x80416618: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041671C;
    }
    // 0x80416618: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041661C: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80416620: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416624: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416628: bne         $t0, $a0, L_80416654
    if (ctx->r8 != ctx->r4) {
        // 0x8041662C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416654;
    }
    // 0x8041662C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416630: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416634: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416638: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x8041663C: bne         $t0, $zero, L_8041963C
    if (ctx->r8 != 0) {
        // 0x80416640: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041963C;
    }
    // 0x80416640: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416644: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416648: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x8041664C: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80416650:
    // 0x80416650: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416654:
    // 0x80416654: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80416658: bne         $v1, $a0, L_80416760
    if (ctx->r3 != ctx->r4) {
        // 0x8041665C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416760;
    }
    // 0x8041665C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416660: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x80416664: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416668: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041666C: bne         $t0, $a0, L_80416698
    if (ctx->r8 != ctx->r4) {
        // 0x80416670: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416698;
    }
    // 0x80416670: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416674: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416678: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x8041667C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416680: bne         $t0, $zero, L_8041973C
    if (ctx->r8 != 0) {
        // 0x80416684: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041973C;
    }
    // 0x80416684: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416688: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041668C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416690: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_80416694:
    // 0x80416694: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416698:
    // 0x80416698: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x8041669C: bne         $v1, $a0, L_804167A8
    if (ctx->r3 != ctx->r4) {
        // 0x804166A0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804167A8;
    }
    // 0x804166A0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804166A4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804166A8: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x804166AC: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804166B0: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804166B4: bne         $a0, $v1, L_80415D6C
    if (ctx->r4 != ctx->r3) {
        // 0x804166B8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415D6C;
    }
    // 0x804166B8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804166BC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804166C0: lbu         $v1, -0x300B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X300B);
    // 0x804166C4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804166C8: bnel        $a0, $zero, L_8041971C
    if (ctx->r4 != 0) {
        // 0x804166CC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041971C;
    }
    goto skip_33;
    // 0x804166CC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_33:
    // 0x804166D0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804166D4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804166D8: b           L_80415D68
    // 0x804166DC: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_80415D68;
    // 0x804166DC: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_804166E0:
    // 0x804166E0: bne         $v1, $a0, L_80416804
    if (ctx->r3 != ctx->r4) {
        // 0x804166E4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416804;
    }
    // 0x804166E4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804166E8: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x804166EC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804166F0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804166F4: bne         $t0, $a0, L_80416720
    if (ctx->r8 != ctx->r4) {
        // 0x804166F8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416720;
    }
    // 0x804166F8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804166FC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416700: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416704: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416708: bne         $t0, $zero, L_80419634
    if (ctx->r8 != 0) {
        // 0x8041670C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419634;
    }
    // 0x8041670C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416710: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416714: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416718: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_8041671C:
    // 0x8041671C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416720:
    // 0x80416720: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x80416724: bne         $v1, $a0, L_80416848
    if (ctx->r3 != ctx->r4) {
        // 0x80416728: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416848;
    }
    // 0x80416728: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041672C: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80416730: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416734: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416738: bne         $t0, $a0, L_80416764
    if (ctx->r8 != ctx->r4) {
        // 0x8041673C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416764;
    }
    // 0x8041673C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416740: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416744: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416748: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x8041674C: bne         $t0, $zero, L_80419714
    if (ctx->r8 != 0) {
        // 0x80416750: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419714;
    }
    // 0x80416750: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416754: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416758: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x8041675C: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80416760:
    // 0x80416760: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416764:
    // 0x80416764: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80416768: bne         $v1, $a0, L_8041688C
    if (ctx->r3 != ctx->r4) {
        // 0x8041676C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041688C;
    }
    // 0x8041676C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416770: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x80416774: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416778: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041677C: bne         $t0, $a0, L_804167A8
    if (ctx->r8 != ctx->r4) {
        // 0x80416780: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804167A8;
    }
    // 0x80416780: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416784: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416788: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x8041678C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416790: bne         $t0, $zero, L_8041977C
    if (ctx->r8 != 0) {
        // 0x80416794: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041977C;
    }
    // 0x80416794: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416798: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041679C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804167A0: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_804167A4:
    // 0x804167A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804167A8:
    // 0x804167A8: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x804167AC: bne         $v1, $a0, L_804168D4
    if (ctx->r3 != ctx->r4) {
        // 0x804167B0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804168D4;
    }
    // 0x804167B0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804167B4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804167B8: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x804167BC: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804167C0: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804167C4: bne         $a0, $v1, L_80415D6C
    if (ctx->r4 != ctx->r3) {
        // 0x804167C8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415D6C;
    }
    // 0x804167C8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804167CC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804167D0: lbu         $v1, -0x300B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X300B);
    // 0x804167D4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804167D8: bnel        $a0, $zero, L_80419754
    if (ctx->r4 != 0) {
        // 0x804167DC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419754;
    }
    goto skip_34;
    // 0x804167DC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_34:
    // 0x804167E0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804167E4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804167E8: b           L_80415D68
    // 0x804167EC: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_80415D68;
    // 0x804167EC: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_804167F0:
    // 0x804167F0: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x804167F4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804167F8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804167FC: beq         $t0, $a0, L_80418F38
    if (ctx->r8 == ctx->r4) {
        // 0x80416800: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418F38;
    }
    // 0x80416800: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416804:
    // 0x80416804: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416808:
    // 0x80416808: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x8041680C: bne         $v1, $a0, L_80415CF8
    if (ctx->r3 != ctx->r4) {
        // 0x80416810: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415CF8;
    }
    // 0x80416810: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416814: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x80416818: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041681C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416820: bne         $t0, $a0, L_8041684C
    if (ctx->r8 != ctx->r4) {
        // 0x80416824: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041684C;
    }
    // 0x80416824: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416828: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041682C: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416830: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416834: bne         $t0, $zero, L_80419624
    if (ctx->r8 != 0) {
        // 0x80416838: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419624;
    }
    // 0x80416838: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041683C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416840: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416844: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_80416848:
    // 0x80416848: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041684C:
    // 0x8041684C: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x80416850: bne         $v1, $a0, L_80415D1C
    if (ctx->r3 != ctx->r4) {
        // 0x80416854: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415D1C;
    }
    // 0x80416854: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416858: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x8041685C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416860: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416864: bne         $t0, $a0, L_80416890
    if (ctx->r8 != ctx->r4) {
        // 0x80416868: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416890;
    }
    // 0x80416868: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041686C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416870: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80416874: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416878: bne         $t0, $zero, L_8041975C
    if (ctx->r8 != 0) {
        // 0x8041687C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041975C;
    }
    // 0x8041687C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416880: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416884: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416888: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_8041688C:
    // 0x8041688C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416890:
    // 0x80416890: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80416894: bne         $v1, $a0, L_80415D40
    if (ctx->r3 != ctx->r4) {
        // 0x80416898: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415D40;
    }
    // 0x80416898: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041689C: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x804168A0: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804168A4: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804168A8: bne         $t0, $a0, L_804168D4
    if (ctx->r8 != ctx->r4) {
        // 0x804168AC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804168D4;
    }
    // 0x804168AC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804168B0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804168B4: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804168B8: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804168BC: bne         $t0, $zero, L_80419794
    if (ctx->r8 != 0) {
        // 0x804168C0: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419794;
    }
    // 0x804168C0: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804168C4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804168C8: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804168CC: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_804168D0:
    // 0x804168D0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804168D4:
    // 0x804168D4: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x804168D8: bne         $v1, $a0, L_80415D68
    if (ctx->r3 != ctx->r4) {
        // 0x804168DC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415D68;
    }
    // 0x804168DC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804168E0: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x804168E4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804168E8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804168EC: bne         $a0, $v1, L_80415D6C
    if (ctx->r4 != ctx->r3) {
        // 0x804168F0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415D6C;
    }
    // 0x804168F0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804168F4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804168F8: lbu         $v1, -0x300B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X300B);
    // 0x804168FC: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416900: bnel        $a0, $zero, L_8041976C
    if (ctx->r4 != 0) {
        // 0x80416904: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041976C;
    }
    goto skip_35;
    // 0x80416904: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_35:
    // 0x80416908: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041690C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416910: b           L_80415D68
    // 0x80416914: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_80415D68;
    // 0x80416914: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_80416918:
    // 0x80416918: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041691C: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x80416920: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416924: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416928: beq         $t0, $a0, L_804190F8
    if (ctx->r8 == ctx->r4) {
        // 0x8041692C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804190F8;
    }
    // 0x8041692C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416930:
    // 0x80416930: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416934:
    // 0x80416934: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x80416938: bne         $v1, $a0, L_80415C44
    if (ctx->r3 != ctx->r4) {
        // 0x8041693C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415C44;
    }
    // 0x8041693C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416940: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x80416944: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416948: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041694C: beq         $t0, $a0, L_80419178
    if (ctx->r8 == ctx->r4) {
        // 0x80416950: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419178;
    }
    // 0x80416950: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416954:
    // 0x80416954: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416958: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x8041695C: bne         $v1, $a0, L_80415C68
    if (ctx->r3 != ctx->r4) {
        // 0x80416960: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415C68;
    }
    // 0x80416960: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416964: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x80416968: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041696C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416970: beq         $t0, $a0, L_80418DEC
    if (ctx->r8 == ctx->r4) {
        // 0x80416974: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418DEC;
    }
    // 0x80416974: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416978:
    // 0x80416978: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041697C: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80416980: bne         $v1, $a0, L_80415C8C
    if (ctx->r3 != ctx->r4) {
        // 0x80416984: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415C8C;
    }
    // 0x80416984: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416988: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x8041698C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416990: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416994: beq         $t0, $a0, L_80419038
    if (ctx->r8 == ctx->r4) {
        // 0x80416998: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419038;
    }
    // 0x80416998: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041699C:
    // 0x8041699C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804169A0:
    // 0x804169A0: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x804169A4: bne         $v1, $a0, L_80415CB4
    if (ctx->r3 != ctx->r4) {
        // 0x804169A8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415CB4;
    }
    // 0x804169A8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804169AC: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x804169B0: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804169B4: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804169B8: bne         $a0, $v1, L_80415CB8
    if (ctx->r4 != ctx->r3) {
        // 0x804169BC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415CB8;
    }
    // 0x804169BC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804169C0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804169C4: lbu         $v1, -0x3133($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3133);
    // 0x804169C8: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804169CC: bne         $a0, $zero, L_8041979C
    if (ctx->r4 != 0) {
        // 0x804169D0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041979C;
    }
    // 0x804169D0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804169D4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804169D8: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804169DC: b           L_80415CB4
    // 0x804169E0: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_80415CB4;
    // 0x804169E0: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_804169E4:
    // 0x804169E4: bne         $v1, $t0, L_80416BC8
    if (ctx->r3 != ctx->r8) {
        // 0x804169E8: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_80416BC8;
    }
    // 0x804169E8: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x804169EC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804169F0: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x804169F4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804169F8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804169FC: bne         $t0, $a0, L_80416A28
    if (ctx->r8 != ctx->r4) {
        // 0x80416A00: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416A28;
    }
    // 0x80416A00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A04: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A08: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416A0C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416A10: bne         $t0, $zero, L_80419644
    if (ctx->r8 != 0) {
        // 0x80416A14: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419644;
    }
    // 0x80416A14: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416A18: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A1C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416A20: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_80416A24:
    // 0x80416A24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416A28:
    // 0x80416A28: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x80416A2C: bne         $v1, $t0, L_80416AFC
    if (ctx->r3 != ctx->r8) {
        // 0x80416A30: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_80416AFC;
    }
    // 0x80416A30: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x80416A34: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A38: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x80416A3C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416A40: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416A44: bne         $t0, $a0, L_80416A70
    if (ctx->r8 != ctx->r4) {
        // 0x80416A48: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416A70;
    }
    // 0x80416A48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A4C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A50: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416A54: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416A58: bne         $t0, $zero, L_8041951C
    if (ctx->r8 != 0) {
        // 0x80416A5C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041951C;
    }
    // 0x80416A5C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416A60: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A64: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416A68: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80416A6C:
    // 0x80416A6C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416A70:
    // 0x80416A70: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x80416A74: bne         $v1, $a0, L_80416B38
    if (ctx->r3 != ctx->r4) {
        // 0x80416A78: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416B38;
    }
    // 0x80416A78: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A7C: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x80416A80: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416A84: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416A88: bne         $t0, $a0, L_80416AB4
    if (ctx->r8 != ctx->r4) {
        // 0x80416A8C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416AB4;
    }
    // 0x80416A8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A90: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416A94: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416A98: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416A9C: bne         $t0, $zero, L_80419614
    if (ctx->r8 != 0) {
        // 0x80416AA0: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419614;
    }
    // 0x80416AA0: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416AA4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416AA8: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416AAC: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_80416AB0:
    // 0x80416AB0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416AB4:
    // 0x80416AB4: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x80416AB8: bne         $v1, $a0, L_80416B80
    if (ctx->r3 != ctx->r4) {
        // 0x80416ABC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416B80;
    }
    // 0x80416ABC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416AC0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416AC4: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x80416AC8: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416ACC: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80416AD0: bne         $a0, $v1, L_80415C04
    if (ctx->r4 != ctx->r3) {
        // 0x80416AD4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415C04;
    }
    // 0x80416AD4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416AD8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416ADC: lbu         $v1, -0x325B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X325B);
    // 0x80416AE0: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416AE4: bne         $a0, $zero, L_804197AC
    if (ctx->r4 != 0) {
        // 0x80416AE8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804197AC;
    }
    // 0x80416AE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416AEC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416AF0: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416AF4: b           L_80415C00
    // 0x80416AF8: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_80415C00;
    // 0x80416AF8: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_80416AFC:
    // 0x80416AFC: bne         $v1, $a0, L_80416C04
    if (ctx->r3 != ctx->r4) {
        // 0x80416B00: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C04;
    }
    // 0x80416B00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B04: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80416B08: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416B0C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416B10: bne         $t0, $a0, L_80416B3C
    if (ctx->r8 != ctx->r4) {
        // 0x80416B14: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416B3C;
    }
    // 0x80416B14: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B18: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B1C: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416B20: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416B24: bne         $t0, $zero, L_80419844
    if (ctx->r8 != 0) {
        // 0x80416B28: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419844;
    }
    // 0x80416B28: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416B2C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B30: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416B34: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80416B38:
    // 0x80416B38: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416B3C:
    // 0x80416B3C: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80416B40: bne         $v1, $a0, L_80416C48
    if (ctx->r3 != ctx->r4) {
        // 0x80416B44: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C48;
    }
    // 0x80416B44: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B48: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x80416B4C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416B50: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416B54: bne         $t0, $a0, L_80416B80
    if (ctx->r8 != ctx->r4) {
        // 0x80416B58: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416B80;
    }
    // 0x80416B58: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B5C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B60: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416B64: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416B68: bne         $t0, $zero, L_804197A4
    if (ctx->r8 != 0) {
        // 0x80416B6C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804197A4;
    }
    // 0x80416B6C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416B70: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B74: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416B78: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_80416B7C:
    // 0x80416B7C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416B80:
    // 0x80416B80: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x80416B84: bne         $v1, $a0, L_80416C90
    if (ctx->r3 != ctx->r4) {
        // 0x80416B88: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C90;
    }
    // 0x80416B88: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416B8C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416B90: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x80416B94: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416B98: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80416B9C: bne         $a0, $v1, L_80415C04
    if (ctx->r4 != ctx->r3) {
        // 0x80416BA0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415C04;
    }
    // 0x80416BA0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416BA4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416BA8: lbu         $v1, -0x325B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X325B);
    // 0x80416BAC: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416BB0: bne         $a0, $zero, L_8041983C
    if (ctx->r4 != 0) {
        // 0x80416BB4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041983C;
    }
    // 0x80416BB4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416BB8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416BBC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416BC0: b           L_80415C00
    // 0x80416BC4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_80415C00;
    // 0x80416BC4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_80416BC8:
    // 0x80416BC8: bne         $v1, $a0, L_80416CEC
    if (ctx->r3 != ctx->r4) {
        // 0x80416BCC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416CEC;
    }
    // 0x80416BCC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416BD0: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x80416BD4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416BD8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416BDC: bne         $t0, $a0, L_80416C08
    if (ctx->r8 != ctx->r4) {
        // 0x80416BE0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C08;
    }
    // 0x80416BE0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416BE4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416BE8: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416BEC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416BF0: bne         $t0, $zero, L_80419834
    if (ctx->r8 != 0) {
        // 0x80416BF4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419834;
    }
    // 0x80416BF4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416BF8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416BFC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416C00: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_80416C04:
    // 0x80416C04: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416C08:
    // 0x80416C08: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x80416C0C: bne         $v1, $a0, L_80416D30
    if (ctx->r3 != ctx->r4) {
        // 0x80416C10: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416D30;
    }
    // 0x80416C10: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C14: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80416C18: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416C1C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416C20: bne         $t0, $a0, L_80416C4C
    if (ctx->r8 != ctx->r4) {
        // 0x80416C24: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C4C;
    }
    // 0x80416C24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C28: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C2C: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416C30: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416C34: bne         $t0, $zero, L_804194EC
    if (ctx->r8 != 0) {
        // 0x80416C38: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194EC;
    }
    // 0x80416C38: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416C3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C40: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416C44: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80416C48:
    // 0x80416C48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416C4C:
    // 0x80416C4C: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80416C50: bne         $v1, $a0, L_80416D74
    if (ctx->r3 != ctx->r4) {
        // 0x80416C54: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416D74;
    }
    // 0x80416C54: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C58: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x80416C5C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416C60: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416C64: bne         $t0, $a0, L_80416C90
    if (ctx->r8 != ctx->r4) {
        // 0x80416C68: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416C90;
    }
    // 0x80416C68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C6C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C70: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416C74: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416C78: bne         $t0, $zero, L_8041984C
    if (ctx->r8 != 0) {
        // 0x80416C7C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041984C;
    }
    // 0x80416C7C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416C80: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C84: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416C88: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_80416C8C:
    // 0x80416C8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416C90:
    // 0x80416C90: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x80416C94: bne         $v1, $a0, L_80416DBC
    if (ctx->r3 != ctx->r4) {
        // 0x80416C98: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416DBC;
    }
    // 0x80416C98: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416C9C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416CA0: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x80416CA4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416CA8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80416CAC: bne         $a0, $v1, L_80415C04
    if (ctx->r4 != ctx->r3) {
        // 0x80416CB0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415C04;
    }
    // 0x80416CB0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416CB4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416CB8: lbu         $v1, -0x325B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X325B);
    // 0x80416CBC: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416CC0: bne         $a0, $zero, L_80419784
    if (ctx->r4 != 0) {
        // 0x80416CC4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419784;
    }
    // 0x80416CC4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416CC8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416CCC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416CD0: b           L_80415C00
    // 0x80416CD4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_80415C00;
    // 0x80416CD4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_80416CD8:
    // 0x80416CD8: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x80416CDC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416CE0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416CE4: beq         $t0, $a0, L_80418DAC
    if (ctx->r8 == ctx->r4) {
        // 0x80416CE8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418DAC;
    }
    // 0x80416CE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416CEC:
    // 0x80416CEC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416CF0:
    // 0x80416CF0: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x80416CF4: bne         $v1, $a0, L_80415B90
    if (ctx->r3 != ctx->r4) {
        // 0x80416CF8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415B90;
    }
    // 0x80416CF8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416CFC: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x80416D00: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416D04: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416D08: bne         $t0, $a0, L_80416D34
    if (ctx->r8 != ctx->r4) {
        // 0x80416D0C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416D34;
    }
    // 0x80416D0C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D10: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D14: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416D18: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416D1C: bne         $t0, $zero, L_80419764
    if (ctx->r8 != 0) {
        // 0x80416D20: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419764;
    }
    // 0x80416D20: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416D24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D28: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416D2C: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_80416D30:
    // 0x80416D30: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416D34:
    // 0x80416D34: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x80416D38: bne         $v1, $a0, L_80415BB4
    if (ctx->r3 != ctx->r4) {
        // 0x80416D3C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415BB4;
    }
    // 0x80416D3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D40: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x80416D44: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416D48: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416D4C: bne         $t0, $a0, L_80416D78
    if (ctx->r8 != ctx->r4) {
        // 0x80416D50: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416D78;
    }
    // 0x80416D50: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D54: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D58: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416D5C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416D60: bne         $t0, $zero, L_8041982C
    if (ctx->r8 != 0) {
        // 0x80416D64: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041982C;
    }
    // 0x80416D64: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416D68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D6C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416D70: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_80416D74:
    // 0x80416D74: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416D78:
    // 0x80416D78: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80416D7C: bne         $v1, $a0, L_80415BD8
    if (ctx->r3 != ctx->r4) {
        // 0x80416D80: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415BD8;
    }
    // 0x80416D80: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D84: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x80416D88: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416D8C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416D90: bne         $t0, $a0, L_80416DBC
    if (ctx->r8 != ctx->r4) {
        // 0x80416D94: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416DBC;
    }
    // 0x80416D94: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D98: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416D9C: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416DA0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416DA4: bne         $t0, $zero, L_80419774
    if (ctx->r8 != 0) {
        // 0x80416DA8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419774;
    }
    // 0x80416DA8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416DAC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416DB0: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416DB4: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_80416DB8:
    // 0x80416DB8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416DBC:
    // 0x80416DBC: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x80416DC0: bne         $v1, $a0, L_80415C00
    if (ctx->r3 != ctx->r4) {
        // 0x80416DC4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415C00;
    }
    // 0x80416DC4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416DC8: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x80416DCC: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416DD0: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80416DD4: bne         $a0, $v1, L_80415C04
    if (ctx->r4 != ctx->r3) {
        // 0x80416DD8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415C04;
    }
    // 0x80416DD8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416DDC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416DE0: lbu         $v1, -0x325B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X325B);
    // 0x80416DE4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416DE8: bne         $a0, $zero, L_80419684
    if (ctx->r4 != 0) {
        // 0x80416DEC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419684;
    }
    // 0x80416DEC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416DF0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416DF4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416DF8: b           L_80415C00
    // 0x80416DFC: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_80415C00;
    // 0x80416DFC: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_80416E00:
    // 0x80416E00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E04: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x80416E08: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416E0C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416E10: bne         $t0, $a0, L_80416CF0
    if (ctx->r8 != ctx->r4) {
        // 0x80416E14: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416CF0;
    }
    // 0x80416E14: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E18: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E1C: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80416E20: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416E24: bne         $t0, $zero, L_8041990C
    if (ctx->r8 != 0) {
        // 0x80416E28: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041990C;
    }
    // 0x80416E28: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416E2C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E30: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416E34: b           L_80416CEC
    // 0x80416E38: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_80416CEC;
    // 0x80416E38: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_80416E3C:
    // 0x80416E3C: bne         $v1, $t0, L_804172E8
    if (ctx->r3 != ctx->r8) {
        // 0x80416E40: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_804172E8;
    }
    // 0x80416E40: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x80416E44: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E48: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x80416E4C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416E50: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416E54: bne         $t0, $a0, L_80416E80
    if (ctx->r8 != ctx->r4) {
        // 0x80416E58: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416E80;
    }
    // 0x80416E58: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E5C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E60: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80416E64: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416E68: bne         $t0, $zero, L_8041972C
    if (ctx->r8 != 0) {
        // 0x80416E6C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041972C;
    }
    // 0x80416E6C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416E70: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E74: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416E78: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_80416E7C:
    // 0x80416E7C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416E80:
    // 0x80416E80: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x80416E84: bne         $v1, $t0, L_8041721C
    if (ctx->r3 != ctx->r8) {
        // 0x80416E88: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_8041721C;
    }
    // 0x80416E88: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x80416E8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416E90: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x80416E94: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416E98: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416E9C: bne         $t0, $a0, L_80416EC8
    if (ctx->r8 != ctx->r4) {
        // 0x80416EA0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416EC8;
    }
    // 0x80416EA0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416EA4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416EA8: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80416EAC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416EB0: bne         $t0, $zero, L_804195C4
    if (ctx->r8 != 0) {
        // 0x80416EB4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195C4;
    }
    // 0x80416EB4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416EB8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416EBC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416EC0: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80416EC4:
    // 0x80416EC4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416EC8:
    // 0x80416EC8: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x80416ECC: bne         $v1, $a0, L_80417258
    if (ctx->r3 != ctx->r4) {
        // 0x80416ED0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417258;
    }
    // 0x80416ED0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416ED4: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x80416ED8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416EDC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416EE0: bne         $t0, $a0, L_80416F0C
    if (ctx->r8 != ctx->r4) {
        // 0x80416EE4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416F0C;
    }
    // 0x80416EE4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416EE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416EEC: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80416EF0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416EF4: bne         $t0, $zero, L_8041970C
    if (ctx->r8 != 0) {
        // 0x80416EF8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041970C;
    }
    // 0x80416EF8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416EFC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F00: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416F04: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_80416F08:
    // 0x80416F08: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416F0C:
    // 0x80416F0C: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x80416F10: bne         $v1, $a0, L_804172A0
    if (ctx->r3 != ctx->r4) {
        // 0x80416F14: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804172A0;
    }
    // 0x80416F14: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F18: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416F1C: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x80416F20: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416F24: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80416F28: bne         $a0, $v1, L_80415B50
    if (ctx->r4 != ctx->r3) {
        // 0x80416F2C: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415B50;
    }
    // 0x80416F2C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416F30: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416F34: lbu         $v1, -0x3383($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3383);
    // 0x80416F38: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80416F3C: bne         $a0, $zero, L_80419674
    if (ctx->r4 != 0) {
        // 0x80416F40: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419674;
    }
    // 0x80416F40: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F44: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416F48: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80416F4C: b           L_80415B4C
    // 0x80416F50: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_80415B4C;
    // 0x80416F50: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_80416F54:
    // 0x80416F54: bne         $v1, $t0, L_804170D8
    if (ctx->r3 != ctx->r8) {
        // 0x80416F58: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_804170D8;
    }
    // 0x80416F58: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x80416F5C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F60: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x80416F64: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416F68: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416F6C: beq         $t0, $a0, L_80418FF8
    if (ctx->r8 == ctx->r4) {
        // 0x80416F70: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418FF8;
    }
    // 0x80416F70: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416F74:
    // 0x80416F74: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F78: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x80416F7C: bne         $v1, $t0, L_8041702C
    if (ctx->r3 != ctx->r8) {
        // 0x80416F80: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_8041702C;
    }
    // 0x80416F80: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x80416F84: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F88: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x80416F8C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416F90: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416F94: bne         $t0, $a0, L_80416FC0
    if (ctx->r8 != ctx->r4) {
        // 0x80416F98: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416FC0;
    }
    // 0x80416F98: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416F9C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416FA0: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80416FA4: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80416FA8: bne         $t0, $zero, L_8041994C
    if (ctx->r8 != 0) {
        // 0x80416FAC: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041994C;
    }
    // 0x80416FAC: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80416FB0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416FB4: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80416FB8: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80416FBC:
    // 0x80416FBC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416FC0:
    // 0x80416FC0: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x80416FC4: bne         $v1, $a0, L_80417068
    if (ctx->r3 != ctx->r4) {
        // 0x80416FC8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417068;
    }
    // 0x80416FC8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416FCC: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x80416FD0: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80416FD4: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80416FD8: beq         $t0, $a0, L_80419078
    if (ctx->r8 == ctx->r4) {
        // 0x80416FDC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419078;
    }
    // 0x80416FDC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416FE0:
    // 0x80416FE0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80416FE4:
    // 0x80416FE4: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x80416FE8: bne         $v1, $a0, L_80417090
    if (ctx->r3 != ctx->r4) {
        // 0x80416FEC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417090;
    }
    // 0x80416FEC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80416FF0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80416FF4: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x80416FF8: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80416FFC: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417000: bne         $a0, $v1, L_80415CB8
    if (ctx->r4 != ctx->r3) {
        // 0x80417004: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415CB8;
    }
    // 0x80417004: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417008: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041700C: lbu         $v1, -0x3133($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3133);
    // 0x80417010: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417014: bne         $a0, $zero, L_80419914
    if (ctx->r4 != 0) {
        // 0x80417018: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419914;
    }
    // 0x80417018: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041701C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417020: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80417024: b           L_80415CB4
    // 0x80417028: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_80415CB4;
    // 0x80417028: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_8041702C:
    // 0x8041702C: bne         $v1, $a0, L_804170F4
    if (ctx->r3 != ctx->r4) {
        // 0x80417030: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804170F4;
    }
    // 0x80417030: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417034: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80417038: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041703C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417040: bne         $t0, $a0, L_8041706C
    if (ctx->r8 != ctx->r4) {
        // 0x80417044: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041706C;
    }
    // 0x80417044: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417048: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041704C: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80417050: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417054: bne         $t0, $zero, L_8041992C
    if (ctx->r8 != 0) {
        // 0x80417058: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041992C;
    }
    // 0x80417058: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041705C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417060: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417064: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80417068:
    // 0x80417068: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041706C:
    // 0x8041706C: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80417070: bne         $v1, $a0, L_80417138
    if (ctx->r3 != ctx->r4) {
        // 0x80417074: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417138;
    }
    // 0x80417074: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417078: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x8041707C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417080: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417084: beq         $t0, $a0, L_80418E2C
    if (ctx->r8 == ctx->r4) {
        // 0x80417088: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418E2C;
    }
    // 0x80417088: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041708C:
    // 0x8041708C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417090:
    // 0x80417090: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x80417094: bne         $v1, $a0, L_80417160
    if (ctx->r3 != ctx->r4) {
        // 0x80417098: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417160;
    }
    // 0x80417098: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041709C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804170A0: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x804170A4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804170A8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804170AC: bne         $a0, $v1, L_80415CB8
    if (ctx->r4 != ctx->r3) {
        // 0x804170B0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415CB8;
    }
    // 0x804170B0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804170B4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804170B8: lbu         $v1, -0x3133($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3133);
    // 0x804170BC: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804170C0: bne         $a0, $zero, L_8041964C
    if (ctx->r4 != 0) {
        // 0x804170C4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041964C;
    }
    // 0x804170C4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804170C8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804170CC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804170D0: b           L_80415CB4
    // 0x804170D4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_80415CB4;
    // 0x804170D4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_804170D8:
    // 0x804170D8: bne         $v1, $a0, L_80416930
    if (ctx->r3 != ctx->r4) {
        // 0x804170DC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416930;
    }
    // 0x804170DC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804170E0: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x804170E4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804170E8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804170EC: beq         $t0, $a0, L_80418EF8
    if (ctx->r8 == ctx->r4) {
        // 0x804170F0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418EF8;
    }
    // 0x804170F0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804170F4:
    // 0x804170F4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804170F8: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x804170FC: bne         $v1, $a0, L_80416954
    if (ctx->r3 != ctx->r4) {
        // 0x80417100: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416954;
    }
    // 0x80417100: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417104: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80417108: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041710C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417110: bne         $t0, $a0, L_8041713C
    if (ctx->r8 != ctx->r4) {
        // 0x80417114: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041713C;
    }
    // 0x80417114: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417118: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041711C: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80417120: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417124: bne         $t0, $zero, L_80419664
    if (ctx->r8 != 0) {
        // 0x80417128: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419664;
    }
    // 0x80417128: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041712C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417130: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417134: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80417138:
    // 0x80417138: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041713C:
    // 0x8041713C: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80417140: bne         $v1, $a0, L_80416978
    if (ctx->r3 != ctx->r4) {
        // 0x80417144: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416978;
    }
    // 0x80417144: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417148: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x8041714C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417150: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417154: beq         $t0, $a0, L_80418F78
    if (ctx->r8 == ctx->r4) {
        // 0x80417158: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418F78;
    }
    // 0x80417158: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041715C:
    // 0x8041715C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417160:
    // 0x80417160: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x80417164: bne         $v1, $a0, L_804169A0
    if (ctx->r3 != ctx->r4) {
        // 0x80417168: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804169A0;
    }
    // 0x80417168: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041716C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417170: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x80417174: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417178: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x8041717C: bne         $a0, $v1, L_80415CB8
    if (ctx->r4 != ctx->r3) {
        // 0x80417180: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415CB8;
    }
    // 0x80417180: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417184: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417188: lbu         $v1, -0x3133($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3133);
    // 0x8041718C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417190: bne         $a0, $zero, L_80419924
    if (ctx->r4 != 0) {
        // 0x80417194: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419924;
    }
    // 0x80417194: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417198: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041719C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804171A0: b           L_80415CB4
    // 0x804171A4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_80415CB4;
    // 0x804171A4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_804171A8:
    // 0x804171A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171AC: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x804171B0: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804171B4: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804171B8: bne         $t0, $a0, L_80416808
    if (ctx->r8 != ctx->r4) {
        // 0x804171BC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416808;
    }
    // 0x804171BC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171C0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171C4: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804171C8: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804171CC: bne         $t0, $zero, L_8041991C
    if (ctx->r8 != 0) {
        // 0x804171D0: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041991C;
    }
    // 0x804171D0: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804171D4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171D8: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804171DC: b           L_80416804
    // 0x804171E0: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_80416804;
    // 0x804171E0: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_804171E4:
    // 0x804171E4: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x804171E8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804171EC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804171F0: bne         $t0, $a0, L_80416934
    if (ctx->r8 != ctx->r4) {
        // 0x804171F4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80416934;
    }
    // 0x804171F4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171F8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804171FC: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80417200: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417204: bne         $t0, $zero, L_804197B4
    if (ctx->r8 != 0) {
        // 0x80417208: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804197B4;
    }
    // 0x80417208: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041720C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417210: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417214: b           L_80416930
    // 0x80417218: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_80416930;
    // 0x80417218: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_8041721C:
    // 0x8041721C: bne         $v1, $a0, L_80417304
    if (ctx->r3 != ctx->r4) {
        // 0x80417220: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417304;
    }
    // 0x80417220: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417224: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80417228: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041722C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417230: bne         $t0, $a0, L_8041725C
    if (ctx->r8 != ctx->r4) {
        // 0x80417234: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041725C;
    }
    // 0x80417234: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417238: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041723C: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80417240: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417244: bne         $t0, $zero, L_804194CC
    if (ctx->r8 != 0) {
        // 0x80417248: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194CC;
    }
    // 0x80417248: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x8041724C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417250: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417254: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80417258:
    // 0x80417258: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041725C:
    // 0x8041725C: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80417260: bne         $v1, $a0, L_80417328
    if (ctx->r3 != ctx->r4) {
        // 0x80417264: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417328;
    }
    // 0x80417264: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417268: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x8041726C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417270: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417274: bne         $t0, $a0, L_804172A0
    if (ctx->r8 != ctx->r4) {
        // 0x80417278: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804172A0;
    }
    // 0x80417278: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041727C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417280: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80417284: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417288: bne         $t0, $zero, L_80419934
    if (ctx->r8 != 0) {
        // 0x8041728C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419934;
    }
    // 0x8041728C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417290: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417294: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417298: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_8041729C:
    // 0x8041729C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804172A0:
    // 0x804172A0: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x804172A4: bne         $v1, $a0, L_80417370
    if (ctx->r3 != ctx->r4) {
        // 0x804172A8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417370;
    }
    // 0x804172A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804172AC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804172B0: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x804172B4: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804172B8: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804172BC: bne         $a0, $v1, L_80415B50
    if (ctx->r4 != ctx->r3) {
        // 0x804172C0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415B50;
    }
    // 0x804172C0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804172C4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804172C8: lbu         $v1, -0x3383($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3383);
    // 0x804172CC: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804172D0: bne         $a0, $zero, L_80419954
    if (ctx->r4 != 0) {
        // 0x804172D4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419954;
    }
    // 0x804172D4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804172D8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804172DC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804172E0: b           L_80415B4C
    // 0x804172E4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_80415B4C;
    // 0x804172E4: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_804172E8:
    // 0x804172E8: bne         $v1, $a0, L_804178F4
    if (ctx->r3 != ctx->r4) {
        // 0x804172EC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804178F4;
    }
    // 0x804172EC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804172F0: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x804172F4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804172F8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804172FC: beq         $t0, $a0, L_80419158
    if (ctx->r8 == ctx->r4) {
        // 0x80417300: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419158;
    }
    // 0x80417300: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417304:
    // 0x80417304: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417308: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x8041730C: bne         $v1, $a0, L_80417918
    if (ctx->r3 != ctx->r4) {
        // 0x80417310: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417918;
    }
    // 0x80417310: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417314: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80417318: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041731C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417320: beq         $t0, $a0, L_80418F58
    if (ctx->r8 == ctx->r4) {
        // 0x80417324: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418F58;
    }
    // 0x80417324: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417328:
    // 0x80417328: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041732C: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80417330: bne         $v1, $a0, L_8041793C
    if (ctx->r3 != ctx->r4) {
        // 0x80417334: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041793C;
    }
    // 0x80417334: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417338: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x8041733C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417340: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417344: bne         $t0, $a0, L_80417370
    if (ctx->r8 != ctx->r4) {
        // 0x80417348: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417370;
    }
    // 0x80417348: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041734C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417350: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80417354: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417358: bne         $t0, $zero, L_80419944
    if (ctx->r8 != 0) {
        // 0x8041735C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419944;
    }
    // 0x8041735C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417360: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417364: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417368: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_8041736C:
    // 0x8041736C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417370:
    // 0x80417370: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x80417374: bne         $v1, $a0, L_80417964
    if (ctx->r3 != ctx->r4) {
        // 0x80417378: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417964;
    }
    // 0x80417378: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041737C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417380: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x80417384: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417388: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x8041738C: bne         $a0, $v1, L_80415B50
    if (ctx->r4 != ctx->r3) {
        // 0x80417390: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415B50;
    }
    // 0x80417390: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417394: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417398: lbu         $v1, -0x3383($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3383);
    // 0x8041739C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804173A0: bne         $a0, $zero, L_80419654
    if (ctx->r4 != 0) {
        // 0x804173A4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419654;
    }
    // 0x804173A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804173A8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804173AC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804173B0: b           L_80415B4C
    // 0x804173B4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_80415B4C;
    // 0x804173B4: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_804173B8:
    // 0x804173B8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804173BC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804173C0: lbu         $a1, 0x69E4($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
    // 0x804173C4: lw          $a2, 0x4EE8($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X4EE8);
    // 0x804173C8: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x804173CC: beq         $a2, $a3, L_80418CDC
    if (ctx->r6 == ctx->r7) {
        // 0x804173D0: andi        $t0, $a1, 0x1
        ctx->r8 = ctx->r5 & 0X1;
            goto L_80418CDC;
    }
    // 0x804173D0: andi        $t0, $a1, 0x1
    ctx->r8 = ctx->r5 & 0X1;
    // 0x804173D4: addiu       $a3, $zero, 0x13
    ctx->r7 = ADD32(0, 0X13);
    // 0x804173D8: bne         $a2, $a3, L_804174B0
    if (ctx->r6 != ctx->r7) {
        // 0x804173DC: lui         $a2, 0x8009
        ctx->r6 = S32(0X8009 << 16);
            goto L_804174B0;
    }
    // 0x804173DC: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
    // 0x804173E0: lw          $a3, 0x67C($a2)
    ctx->r7 = MEM_W(ctx->r6, 0X67C);
    // 0x804173E4: lui         $a2, 0xA0C0
    ctx->r6 = S32(0XA0C0 << 16);
    // 0x804173E8: ori         $a2, $a2, 0x8108
    ctx->r6 = ctx->r6 | 0X8108;
    // 0x804173EC: bne         $a3, $a2, L_80417404
    if (ctx->r7 != ctx->r6) {
        // 0x804173F0: andi        $a2, $a1, 0x8
        ctx->r6 = ctx->r5 & 0X8;
            goto L_80417404;
    }
    // 0x804173F0: andi        $a2, $a1, 0x8
    ctx->r6 = ctx->r5 & 0X8;
    // 0x804173F4: ori         $a1, $a1, 0x4
    ctx->r5 = ctx->r5 | 0X4;
    // 0x804173F8: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
    // 0x804173FC: lbu         $a1, 0x69E4($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
L_80417400:
    // 0x80417400: andi        $a2, $a1, 0x8
    ctx->r6 = ctx->r5 & 0X8;
L_80417404:
    // 0x80417404: beq         $a2, $zero, L_80419444
    if (ctx->r6 == 0) {
        // 0x80417408: andi        $a3, $a1, 0x10
        ctx->r7 = ctx->r5 & 0X10;
            goto L_80419444;
    }
    // 0x80417408: andi        $a3, $a1, 0x10
    ctx->r7 = ctx->r5 & 0X10;
    // 0x8041740C: bnel        $a3, $zero, L_80419430
    if (ctx->r7 != 0) {
        // 0x80417410: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80419430;
    }
    goto skip_36;
    // 0x80417410: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_36:
    // 0x80417414: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
    // 0x80417418: lw          $t0, 0x67C($a2)
    ctx->r8 = MEM_W(ctx->r6, 0X67C);
    // 0x8041741C: lui         $a3, 0xC081
    ctx->r7 = S32(0XC081 << 16);
    // 0x80417420: lui         $a2, 0x8018
    ctx->r6 = S32(0X8018 << 16);
    // 0x80417424: addiu       $t1, $zero, 0x486C
    ctx->r9 = ADD32(0, 0X486C);
    // 0x80417428: addiu       $a3, $a3, 0x1382
    ctx->r7 = ADD32(ctx->r7, 0X1382);
    // 0x8041742C: bne         $t0, $a3, L_804174B0
    if (ctx->r8 != ctx->r7) {
        // 0x80417430: sh          $t1, 0x3468($a2)
        MEM_H(0X3468, ctx->r6) = ctx->r9;
            goto L_804174B0;
    }
    // 0x80417430: sh          $t1, 0x3468($a2)
    MEM_H(0X3468, ctx->r6) = ctx->r9;
    // 0x80417434: lui         $ra, 0x8008
    ctx->r31 = S32(0X8008 << 16);
    // 0x80417438: lui         $t9, 0x8008
    ctx->r25 = S32(0X8008 << 16);
    // 0x8041743C: lui         $t8, 0x8008
    ctx->r24 = S32(0X8008 << 16);
    // 0x80417440: lui         $t7, 0x8008
    ctx->r15 = S32(0X8008 << 16);
    // 0x80417444: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80417448: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x8041744C: lhu         $t4, -0x4558($ra)
    ctx->r12 = MEM_HU(ctx->r31, -0X4558);
    // 0x80417450: lhu         $t3, -0x4556($t9)
    ctx->r11 = MEM_HU(ctx->r25, -0X4556);
    // 0x80417454: lhu         $t2, -0x4552($t8)
    ctx->r10 = MEM_HU(ctx->r24, -0X4552);
    // 0x80417458: lhu         $t1, -0x4554($t7)
    ctx->r9 = MEM_HU(ctx->r15, -0X4554);
    // 0x8041745C: lw          $t0, -0x4570($t6)
    ctx->r8 = MEM_W(ctx->r14, -0X4570);
    // 0x80417460: lw          $a3, -0x456C($t5)
    ctx->r7 = MEM_W(ctx->r13, -0X456C);
    // 0x80417464: ori         $a1, $a1, 0x10
    ctx->r5 = ctx->r5 | 0X10;
    // 0x80417468: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
    // 0x8041746C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417470: addiu       $a1, $zero, 0x1A
    ctx->r5 = ADD32(0, 0X1A);
    // 0x80417474: addiu       $t4, $t4, 0x32
    ctx->r12 = ADD32(ctx->r12, 0X32);
    // 0x80417478: addiu       $t3, $t3, 0x1E
    ctx->r11 = ADD32(ctx->r11, 0X1E);
    // 0x8041747C: addiu       $t2, $t2, 0x10
    ctx->r10 = ADD32(ctx->r10, 0X10);
    // 0x80417480: addiu       $t1, $t1, 0x10
    ctx->r9 = ADD32(ctx->r9, 0X10);
    // 0x80417484: addiu       $t0, $t0, 0x1C2
    ctx->r8 = ADD32(ctx->r8, 0X1C2);
    // 0x80417488: addiu       $a3, $a3, 0x12C
    ctx->r7 = ADD32(ctx->r7, 0X12C);
    // 0x8041748C: sb          $a1, -0x458D($v1)
    MEM_B(-0X458D, ctx->r3) = ctx->r5;
    // 0x80417490: addiu       $v1, $zero, 0x486C
    ctx->r3 = ADD32(0, 0X486C);
    // 0x80417494: sh          $t4, -0x4558($ra)
    MEM_H(-0X4558, ctx->r31) = ctx->r12;
    // 0x80417498: sh          $t3, -0x4556($t9)
    MEM_H(-0X4556, ctx->r25) = ctx->r11;
    // 0x8041749C: sh          $t2, -0x4552($t8)
    MEM_H(-0X4552, ctx->r24) = ctx->r10;
    // 0x804174A0: sh          $t1, -0x4554($t7)
    MEM_H(-0X4554, ctx->r15) = ctx->r9;
    // 0x804174A4: sw          $t0, -0x4570($t6)
    MEM_W(-0X4570, ctx->r14) = ctx->r8;
    // 0x804174A8: sw          $a3, -0x456C($t5)
    MEM_W(-0X456C, ctx->r13) = ctx->r7;
    // 0x804174AC: sh          $v1, 0x3468($a2)
    MEM_H(0X3468, ctx->r6) = ctx->r3;
L_804174B0:
    // 0x804174B0: lw          $a0, 0x4EE8($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X4EE8);
L_804174B4:
    // 0x804174B4: addiu       $v1, $zero, 0x6
    ctx->r3 = ADD32(0, 0X6);
    // 0x804174B8: beql        $a0, $v1, L_80418D30
    if (ctx->r4 == ctx->r3) {
        // 0x804174BC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80418D30;
    }
    goto skip_37;
    // 0x804174BC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    skip_37:
L_804174C0:
    // 0x804174C0: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
L_804174C4:
    // 0x804174C4: lbu         $a3, 0x69F0($t1)
    ctx->r7 = MEM_BU(ctx->r9, 0X69F0);
    // 0x804174C8: b           L_804157DC
    // 0x804174CC: lb          $v1, 0x69F0($t1)
    ctx->r3 = MEM_B(ctx->r9, 0X69F0);
        goto L_804157DC;
    // 0x804174CC: lb          $v1, 0x69F0($t1)
    ctx->r3 = MEM_B(ctx->r9, 0X69F0);
L_804174D0:
    // 0x804174D0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804174D4: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x804174D8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804174DC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804174E0: beq         $t0, $a0, L_80419098
    if (ctx->r8 == ctx->r4) {
        // 0x804174E4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419098;
    }
    // 0x804174E4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804174E8:
    // 0x804174E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804174EC:
    // 0x804174EC: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x804174F0: bne         $v1, $a0, L_80415974
    if (ctx->r3 != ctx->r4) {
        // 0x804174F4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415974;
    }
    // 0x804174F4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804174F8: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x804174FC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417500: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417504: beq         $t0, $a0, L_80419198
    if (ctx->r8 == ctx->r4) {
        // 0x80417508: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419198;
    }
    // 0x80417508: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041750C:
    // 0x8041750C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417510: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x80417514: bne         $v1, $a0, L_80415998
    if (ctx->r3 != ctx->r4) {
        // 0x80417518: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415998;
    }
    // 0x80417518: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041751C: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x80417520: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417524: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417528: beq         $t0, $a0, L_80418F98
    if (ctx->r8 == ctx->r4) {
        // 0x8041752C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418F98;
    }
    // 0x8041752C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417530:
    // 0x80417530: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417534: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80417538: bne         $v1, $a0, L_804159BC
    if (ctx->r3 != ctx->r4) {
        // 0x8041753C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804159BC;
    }
    // 0x8041753C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417540: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x80417544: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417548: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041754C: beq         $t0, $a0, L_80419238
    if (ctx->r8 == ctx->r4) {
        // 0x80417550: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419238;
    }
    // 0x80417550: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417554:
    // 0x80417554: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417558:
    // 0x80417558: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x8041755C: bne         $v1, $a0, L_804159E4
    if (ctx->r3 != ctx->r4) {
        // 0x80417560: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804159E4;
    }
    // 0x80417560: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417564: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x80417568: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x8041756C: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417570: bne         $a0, $v1, L_804159E8
    if (ctx->r4 != ctx->r3) {
        // 0x80417574: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804159E8;
    }
    // 0x80417574: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417578: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041757C: lbu         $v1, -0x35D3($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X35D3);
    // 0x80417580: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417584: bne         $a0, $zero, L_8041974C
    if (ctx->r4 != 0) {
        // 0x80417588: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041974C;
    }
    // 0x80417588: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041758C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417590: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80417594: b           L_804159E4
    // 0x80417598: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_804159E4;
    // 0x80417598: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_8041759C:
    // 0x8041759C: bne         $v1, $t0, L_80417700
    if (ctx->r3 != ctx->r8) {
        // 0x804175A0: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_80417700;
    }
    // 0x804175A0: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x804175A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804175A8: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x804175AC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804175B0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804175B4: beq         $t0, $a0, L_80418ED8
    if (ctx->r8 == ctx->r4) {
        // 0x804175B8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418ED8;
    }
    // 0x804175B8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804175BC:
    // 0x804175BC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804175C0: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x804175C4: bne         $v1, $t0, L_80417654
    if (ctx->r3 != ctx->r8) {
        // 0x804175C8: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_80417654;
    }
    // 0x804175C8: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x804175CC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804175D0: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x804175D4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804175D8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804175DC: beq         $t0, $a0, L_80418EB8
    if (ctx->r8 == ctx->r4) {
        // 0x804175E0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418EB8;
    }
    // 0x804175E0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804175E4:
    // 0x804175E4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804175E8: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x804175EC: bne         $v1, $a0, L_80417690
    if (ctx->r3 != ctx->r4) {
        // 0x804175F0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417690;
    }
    // 0x804175F0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804175F4: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x804175F8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804175FC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417600: beq         $t0, $a0, L_804192B8
    if (ctx->r8 == ctx->r4) {
        // 0x80417604: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804192B8;
    }
    // 0x80417604: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417608:
    // 0x80417608: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041760C:
    // 0x8041760C: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x80417610: bne         $v1, $a0, L_804176B8
    if (ctx->r3 != ctx->r4) {
        // 0x80417614: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804176B8;
    }
    // 0x80417614: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417618: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041761C: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x80417620: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417624: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417628: bne         $a0, $v1, L_80415A9C
    if (ctx->r4 != ctx->r3) {
        // 0x8041762C: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415A9C;
    }
    // 0x8041762C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417630: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417634: lbu         $v1, -0x34AB($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X34AB);
    // 0x80417638: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x8041763C: bne         $a0, $zero, L_804197BC
    if (ctx->r4 != 0) {
        // 0x80417640: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804197BC;
    }
    // 0x80417640: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417644: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417648: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041764C: b           L_80415A98
    // 0x80417650: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_80415A98;
    // 0x80417650: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_80417654:
    // 0x80417654: bne         $v1, $a0, L_8041773C
    if (ctx->r3 != ctx->r4) {
        // 0x80417658: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041773C;
    }
    // 0x80417658: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041765C: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80417660: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417664: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417668: bne         $t0, $a0, L_80417694
    if (ctx->r8 != ctx->r4) {
        // 0x8041766C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417694;
    }
    // 0x8041766C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417670: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417674: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80417678: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x8041767C: bne         $t0, $zero, L_80419824
    if (ctx->r8 != 0) {
        // 0x80417680: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419824;
    }
    // 0x80417680: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417684: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417688: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x8041768C: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80417690:
    // 0x80417690: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417694:
    // 0x80417694: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80417698: bne         $v1, $a0, L_80417780
    if (ctx->r3 != ctx->r4) {
        // 0x8041769C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417780;
    }
    // 0x8041769C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804176A0: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x804176A4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804176A8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804176AC: beq         $t0, $a0, L_804190D8
    if (ctx->r8 == ctx->r4) {
        // 0x804176B0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804190D8;
    }
    // 0x804176B0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804176B4:
    // 0x804176B4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804176B8:
    // 0x804176B8: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x804176BC: bne         $v1, $a0, L_804177A8
    if (ctx->r3 != ctx->r4) {
        // 0x804176C0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804177A8;
    }
    // 0x804176C0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804176C4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804176C8: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x804176CC: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804176D0: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804176D4: bne         $a0, $v1, L_80415A9C
    if (ctx->r4 != ctx->r3) {
        // 0x804176D8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415A9C;
    }
    // 0x804176D8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804176DC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804176E0: lbu         $v1, -0x34AB($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X34AB);
    // 0x804176E4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804176E8: bne         $a0, $zero, L_8041978C
    if (ctx->r4 != 0) {
        // 0x804176EC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041978C;
    }
    // 0x804176EC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804176F0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804176F4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804176F8: b           L_80415A98
    // 0x804176FC: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_80415A98;
    // 0x804176FC: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_80417700:
    // 0x80417700: bne         $v1, $a0, L_80417808
    if (ctx->r3 != ctx->r4) {
        // 0x80417704: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417808;
    }
    // 0x80417704: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417708: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x8041770C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417710: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417714: bne         $t0, $a0, L_80417740
    if (ctx->r8 != ctx->r4) {
        // 0x80417718: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417740;
    }
    // 0x80417718: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041771C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417720: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80417724: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417728: bne         $t0, $zero, L_804195DC
    if (ctx->r8 != 0) {
        // 0x8041772C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195DC;
    }
    // 0x8041772C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417730: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417734: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417738: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_8041773C:
    // 0x8041773C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417740:
    // 0x80417740: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x80417744: bne         $v1, $a0, L_8041782C
    if (ctx->r3 != ctx->r4) {
        // 0x80417748: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041782C;
    }
    // 0x80417748: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041774C: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80417750: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417754: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417758: bne         $t0, $a0, L_80417784
    if (ctx->r8 != ctx->r4) {
        // 0x8041775C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417784;
    }
    // 0x8041775C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417760: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417764: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80417768: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x8041776C: bne         $t0, $zero, L_804198E4
    if (ctx->r8 != 0) {
        // 0x80417770: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804198E4;
    }
    // 0x80417770: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417774: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417778: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x8041777C: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80417780:
    // 0x80417780: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417784:
    // 0x80417784: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80417788: bne         $v1, $a0, L_80417870
    if (ctx->r3 != ctx->r4) {
        // 0x8041778C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417870;
    }
    // 0x8041778C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417790: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x80417794: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417798: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041779C: beq         $t0, $a0, L_80419278
    if (ctx->r8 == ctx->r4) {
        // 0x804177A0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419278;
    }
    // 0x804177A0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804177A4:
    // 0x804177A4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804177A8:
    // 0x804177A8: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x804177AC: bne         $v1, $a0, L_80417898
    if (ctx->r3 != ctx->r4) {
        // 0x804177B0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417898;
    }
    // 0x804177B0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804177B4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804177B8: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x804177BC: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804177C0: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804177C4: bne         $a0, $v1, L_80415A9C
    if (ctx->r4 != ctx->r3) {
        // 0x804177C8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415A9C;
    }
    // 0x804177C8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804177CC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804177D0: lbu         $v1, -0x34AB($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X34AB);
    // 0x804177D4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804177D8: bne         $a0, $zero, L_80419504
    if (ctx->r4 != 0) {
        // 0x804177DC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419504;
    }
    // 0x804177DC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804177E0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804177E4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804177E8: b           L_80415A98
    // 0x804177EC: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_80415A98;
    // 0x804177EC: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_804177F0:
    // 0x804177F0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804177F4: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x804177F8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804177FC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417800: beq         $t0, $a0, L_80419138
    if (ctx->r8 == ctx->r4) {
        // 0x80417804: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419138;
    }
    // 0x80417804: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417808:
    // 0x80417808: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041780C:
    // 0x8041780C: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x80417810: bne         $v1, $a0, L_80415A28
    if (ctx->r3 != ctx->r4) {
        // 0x80417814: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415A28;
    }
    // 0x80417814: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417818: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x8041781C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417820: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417824: beq         $t0, $a0, L_80418FB8
    if (ctx->r8 == ctx->r4) {
        // 0x80417828: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418FB8;
    }
    // 0x80417828: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041782C:
    // 0x8041782C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417830: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x80417834: bne         $v1, $a0, L_80415A4C
    if (ctx->r3 != ctx->r4) {
        // 0x80417838: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415A4C;
    }
    // 0x80417838: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041783C: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x80417840: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417844: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417848: bne         $t0, $a0, L_80417874
    if (ctx->r8 != ctx->r4) {
        // 0x8041784C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417874;
    }
    // 0x8041784C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417850: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417854: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80417858: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x8041785C: bne         $t0, $zero, L_804194A4
    if (ctx->r8 != 0) {
        // 0x80417860: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194A4;
    }
    // 0x80417860: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417864: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417868: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x8041786C: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_80417870:
    // 0x80417870: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417874:
    // 0x80417874: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80417878: bne         $v1, $a0, L_80415A70
    if (ctx->r3 != ctx->r4) {
        // 0x8041787C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415A70;
    }
    // 0x8041787C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417880: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x80417884: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417888: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x8041788C: beq         $t0, $a0, L_80419058
    if (ctx->r8 == ctx->r4) {
        // 0x80417890: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419058;
    }
    // 0x80417890: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417894:
    // 0x80417894: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417898:
    // 0x80417898: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x8041789C: bne         $v1, $a0, L_80415A98
    if (ctx->r3 != ctx->r4) {
        // 0x804178A0: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415A98;
    }
    // 0x804178A0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804178A4: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x804178A8: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x804178AC: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x804178B0: bne         $a0, $v1, L_80415A9C
    if (ctx->r4 != ctx->r3) {
        // 0x804178B4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415A9C;
    }
    // 0x804178B4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804178B8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804178BC: lbu         $v1, -0x34AB($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X34AB);
    // 0x804178C0: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x804178C4: bne         $a0, $zero, L_80419854
    if (ctx->r4 != 0) {
        // 0x804178C8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419854;
    }
    // 0x804178C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804178CC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804178D0: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804178D4: b           L_80415A98
    // 0x804178D8: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_80415A98;
    // 0x804178D8: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_804178DC:
    // 0x804178DC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804178E0: lw          $t0, 0x75E4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X75E4);
    // 0x804178E4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804178E8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804178EC: beq         $t0, $a0, L_80419338
    if (ctx->r8 == ctx->r4) {
        // 0x804178F0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419338;
    }
    // 0x804178F0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804178F4:
    // 0x804178F4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_804178F8:
    // 0x804178F8: addiu       $a0, $a0, 0x7760
    ctx->r4 = ADD32(ctx->r4, 0X7760);
    // 0x804178FC: bne         $v1, $a0, L_80415ADC
    if (ctx->r3 != ctx->r4) {
        // 0x80417900: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415ADC;
    }
    // 0x80417900: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417904: lw          $t0, 0x779C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X779C);
    // 0x80417908: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x8041790C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417910: beq         $t0, $a0, L_804191F8
    if (ctx->r8 == ctx->r4) {
        // 0x80417914: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804191F8;
    }
    // 0x80417914: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417918:
    // 0x80417918: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041791C: addiu       $a0, $a0, 0x7918
    ctx->r4 = ADD32(ctx->r4, 0X7918);
    // 0x80417920: bne         $v1, $a0, L_80415B00
    if (ctx->r3 != ctx->r4) {
        // 0x80417924: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415B00;
    }
    // 0x80417924: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417928: lw          $t0, 0x7954($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7954);
    // 0x8041792C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417930: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417934: beq         $t0, $a0, L_804192F8
    if (ctx->r8 == ctx->r4) {
        // 0x80417938: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804192F8;
    }
    // 0x80417938: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_8041793C:
    // 0x8041793C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417940: addiu       $a0, $a0, 0x7AD0
    ctx->r4 = ADD32(ctx->r4, 0X7AD0);
    // 0x80417944: bne         $v1, $a0, L_80415B24
    if (ctx->r3 != ctx->r4) {
        // 0x80417948: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80415B24;
    }
    // 0x80417948: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041794C: lw          $t0, 0x7B0C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7B0C);
    // 0x80417950: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417954: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417958: beq         $t0, $a0, L_80418E0C
    if (ctx->r8 == ctx->r4) {
        // 0x8041795C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418E0C;
    }
    // 0x8041795C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417960:
    // 0x80417960: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417964:
    // 0x80417964: addiu       $a0, $a0, 0x7C88
    ctx->r4 = ADD32(ctx->r4, 0X7C88);
    // 0x80417968: bne         $v1, $a0, L_80415B4C
    if (ctx->r3 != ctx->r4) {
        // 0x8041796C: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415B4C;
    }
    // 0x8041796C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417970: lw          $a0, 0x7CC4($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7CC4);
    // 0x80417974: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417978: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x8041797C: bne         $a0, $v1, L_80415B50
    if (ctx->r4 != ctx->r3) {
        // 0x80417980: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415B50;
    }
    // 0x80417980: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417984: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417988: lbu         $v1, -0x3383($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3383);
    // 0x8041798C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417990: bne         $a0, $zero, L_80419904
    if (ctx->r4 != 0) {
        // 0x80417994: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419904;
    }
    // 0x80417994: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417998: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041799C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x804179A0: b           L_80415B4C
    // 0x804179A4: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
        goto L_80415B4C;
    // 0x804179A4: sh          $a0, 0x7CC0($v1)
    MEM_H(0X7CC0, ctx->r3) = ctx->r4;
L_804179A8:
    // 0x804179A8: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x804179AC: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804179B0: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804179B4: bne         $t0, $a0, L_804178F8
    if (ctx->r8 != ctx->r4) {
        // 0x804179B8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804178F8;
    }
    // 0x804179B8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804179BC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804179C0: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x804179C4: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804179C8: bne         $t0, $zero, L_80419964
    if (ctx->r8 != 0) {
        // 0x804179CC: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419964;
    }
    // 0x804179CC: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804179D0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804179D4: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804179D8: b           L_804178F4
    // 0x804179DC: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_804178F4;
    // 0x804179DC: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_804179E0:
    // 0x804179E0: bne         $v1, $t0, L_80417B64
    if (ctx->r3 != ctx->r8) {
        // 0x804179E4: addiu       $a0, $a0, 0x7708
        ctx->r4 = ADD32(ctx->r4, 0X7708);
            goto L_80417B64;
    }
    // 0x804179E4: addiu       $a0, $a0, 0x7708
    ctx->r4 = ADD32(ctx->r4, 0X7708);
    // 0x804179E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804179EC: lw          $t0, 0x76EC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X76EC);
    // 0x804179F0: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x804179F4: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x804179F8: beq         $t0, $a0, L_804190B8
    if (ctx->r8 == ctx->r4) {
        // 0x804179FC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804190B8;
    }
    // 0x804179FC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417A00:
    // 0x80417A00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A04: addiu       $t0, $a0, 0x7910
    ctx->r8 = ADD32(ctx->r4, 0X7910);
    // 0x80417A08: bne         $v1, $t0, L_80417AB8
    if (ctx->r3 != ctx->r8) {
        // 0x80417A0C: addiu       $a0, $a0, 0x7868
        ctx->r4 = ADD32(ctx->r4, 0X7868);
            goto L_80417AB8;
    }
    // 0x80417A0C: addiu       $a0, $a0, 0x7868
    ctx->r4 = ADD32(ctx->r4, 0X7868);
    // 0x80417A10: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A14: lw          $t0, 0x784C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X784C);
    // 0x80417A18: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417A1C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417A20: bne         $t0, $a0, L_80417A4C
    if (ctx->r8 != ctx->r4) {
        // 0x80417A24: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417A4C;
    }
    // 0x80417A24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A28: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A2C: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80417A30: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417A34: bne         $t0, $zero, L_804198EC
    if (ctx->r8 != 0) {
        // 0x80417A38: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804198EC;
    }
    // 0x80417A38: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417A3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A40: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417A44: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80417A48:
    // 0x80417A48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417A4C:
    // 0x80417A4C: addiu       $a0, $a0, 0x79C8
    ctx->r4 = ADD32(ctx->r4, 0X79C8);
    // 0x80417A50: bne         $v1, $a0, L_80417AF4
    if (ctx->r3 != ctx->r4) {
        // 0x80417A54: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417AF4;
    }
    // 0x80417A54: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A58: lw          $t0, 0x7A04($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A04);
    // 0x80417A5C: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417A60: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417A64: beq         $t0, $a0, L_80418FD8
    if (ctx->r8 == ctx->r4) {
        // 0x80417A68: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418FD8;
    }
    // 0x80417A68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417A6C:
    // 0x80417A6C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417A70:
    // 0x80417A70: addiu       $a0, $a0, 0x7B80
    ctx->r4 = ADD32(ctx->r4, 0X7B80);
    // 0x80417A74: bne         $v1, $a0, L_80417B1C
    if (ctx->r3 != ctx->r4) {
        // 0x80417A78: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417B1C;
    }
    // 0x80417A78: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417A7C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417A80: lw          $a0, 0x7BBC($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7BBC);
    // 0x80417A84: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417A88: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417A8C: bne         $a0, $v1, L_804159E8
    if (ctx->r4 != ctx->r3) {
        // 0x80417A90: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804159E8;
    }
    // 0x80417A90: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417A94: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417A98: lbu         $v1, -0x35D3($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X35D3);
    // 0x80417A9C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417AA0: bne         $a0, $zero, L_804195D4
    if (ctx->r4 != 0) {
        // 0x80417AA4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804195D4;
    }
    // 0x80417AA4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417AA8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417AAC: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80417AB0: b           L_804159E4
    // 0x80417AB4: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
        goto L_804159E4;
    // 0x80417AB4: sh          $a0, 0x7BB8($v1)
    MEM_H(0X7BB8, ctx->r3) = ctx->r4;
L_80417AB8:
    // 0x80417AB8: bne         $v1, $a0, L_80417B80
    if (ctx->r3 != ctx->r4) {
        // 0x80417ABC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417B80;
    }
    // 0x80417ABC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417AC0: lw          $t0, 0x78A4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78A4);
    // 0x80417AC4: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417AC8: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417ACC: bne         $t0, $a0, L_80417AF8
    if (ctx->r8 != ctx->r4) {
        // 0x80417AD0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417AF8;
    }
    // 0x80417AD0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417AD4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417AD8: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80417ADC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417AE0: bne         $t0, $zero, L_80419864
    if (ctx->r8 != 0) {
        // 0x80417AE4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419864;
    }
    // 0x80417AE4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417AE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417AEC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417AF0: sh          $t0, 0x78A0($a0)
    MEM_H(0X78A0, ctx->r4) = ctx->r8;
L_80417AF4:
    // 0x80417AF4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417AF8:
    // 0x80417AF8: addiu       $a0, $a0, 0x7A20
    ctx->r4 = ADD32(ctx->r4, 0X7A20);
    // 0x80417AFC: bne         $v1, $a0, L_80417BC4
    if (ctx->r3 != ctx->r4) {
        // 0x80417B00: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417BC4;
    }
    // 0x80417B00: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B04: lw          $t0, 0x7A5C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7A5C);
    // 0x80417B08: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417B0C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417B10: beq         $t0, $a0, L_80418E4C
    if (ctx->r8 == ctx->r4) {
        // 0x80417B14: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418E4C;
    }
    // 0x80417B14: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417B18:
    // 0x80417B18: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417B1C:
    // 0x80417B1C: addiu       $a0, $a0, 0x7BD8
    ctx->r4 = ADD32(ctx->r4, 0X7BD8);
    // 0x80417B20: bne         $v1, $a0, L_80417BEC
    if (ctx->r3 != ctx->r4) {
        // 0x80417B24: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417BEC;
    }
    // 0x80417B24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B28: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417B2C: lw          $a0, 0x7C14($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C14);
    // 0x80417B30: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417B34: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417B38: bne         $a0, $v1, L_804159E8
    if (ctx->r4 != ctx->r3) {
        // 0x80417B3C: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804159E8;
    }
    // 0x80417B3C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417B40: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417B44: lbu         $v1, -0x35D3($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X35D3);
    // 0x80417B48: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417B4C: bne         $a0, $zero, L_804195E4
    if (ctx->r4 != 0) {
        // 0x80417B50: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804195E4;
    }
    // 0x80417B50: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B54: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417B58: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80417B5C: b           L_804159E4
    // 0x80417B60: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
        goto L_804159E4;
    // 0x80417B60: sh          $a0, 0x7C10($v1)
    MEM_H(0X7C10, ctx->r3) = ctx->r4;
L_80417B64:
    // 0x80417B64: bne         $v1, $a0, L_804174E8
    if (ctx->r3 != ctx->r4) {
        // 0x80417B68: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804174E8;
    }
    // 0x80417B68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B6C: lw          $t0, 0x7744($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7744);
    // 0x80417B70: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417B74: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417B78: beq         $t0, $a0, L_80418CBC
    if (ctx->r8 == ctx->r4) {
        // 0x80417B7C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418CBC;
    }
    // 0x80417B7C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417B80:
    // 0x80417B80: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B84: addiu       $a0, $a0, 0x78C0
    ctx->r4 = ADD32(ctx->r4, 0X78C0);
    // 0x80417B88: bne         $v1, $a0, L_8041750C
    if (ctx->r3 != ctx->r4) {
        // 0x80417B8C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041750C;
    }
    // 0x80417B8C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417B90: lw          $t0, 0x78FC($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X78FC);
    // 0x80417B94: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417B98: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417B9C: bne         $t0, $a0, L_80417BC8
    if (ctx->r8 != ctx->r4) {
        // 0x80417BA0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417BC8;
    }
    // 0x80417BA0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417BA4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417BA8: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80417BAC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417BB0: bne         $t0, $zero, L_804194FC
    if (ctx->r8 != 0) {
        // 0x80417BB4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194FC;
    }
    // 0x80417BB4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417BB8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417BBC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417BC0: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80417BC4:
    // 0x80417BC4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417BC8:
    // 0x80417BC8: addiu       $a0, $a0, 0x7A78
    ctx->r4 = ADD32(ctx->r4, 0X7A78);
    // 0x80417BCC: bne         $v1, $a0, L_80417530
    if (ctx->r3 != ctx->r4) {
        // 0x80417BD0: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417530;
    }
    // 0x80417BD0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417BD4: lw          $t0, 0x7AB4($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X7AB4);
    // 0x80417BD8: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417BDC: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417BE0: beq         $t0, $a0, L_80418A1C
    if (ctx->r8 == ctx->r4) {
        // 0x80417BE4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80418A1C;
    }
    // 0x80417BE4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417BE8:
    // 0x80417BE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
L_80417BEC:
    // 0x80417BEC: addiu       $a0, $a0, 0x7C30
    ctx->r4 = ADD32(ctx->r4, 0X7C30);
    // 0x80417BF0: bne         $v1, $a0, L_80417558
    if (ctx->r3 != ctx->r4) {
        // 0x80417BF4: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417558;
    }
    // 0x80417BF4: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417BF8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417BFC: lw          $a0, 0x7C6C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X7C6C);
    // 0x80417C00: lui         $v1, 0x800E
    ctx->r3 = S32(0X800E << 16);
    // 0x80417C04: ori         $v1, $v1, 0xAEF0
    ctx->r3 = ctx->r3 | 0XAEF0;
    // 0x80417C08: bne         $a0, $v1, L_804159E8
    if (ctx->r4 != ctx->r3) {
        // 0x80417C0C: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_804159E8;
    }
    // 0x80417C0C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417C10: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417C14: lbu         $v1, -0x35D3($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X35D3);
    // 0x80417C18: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417C1C: bne         $a0, $zero, L_804194D4
    if (ctx->r4 != 0) {
        // 0x80417C20: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804194D4;
    }
    // 0x80417C20: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C24: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417C28: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80417C2C: b           L_804159E4
    // 0x80417C30: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
        goto L_804159E4;
    // 0x80417C30: sh          $a0, 0x7C68($v1)
    MEM_H(0X7C68, ctx->r3) = ctx->r4;
L_80417C34:
    // 0x80417C34: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x80417C38: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417C3C: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417C40: bne         $t0, $a0, L_8041780C
    if (ctx->r8 != ctx->r4) {
        // 0x80417C44: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041780C;
    }
    // 0x80417C44: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C4C: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80417C50: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417C54: bne         $t0, $zero, L_804198D4
    if (ctx->r8 != 0) {
        // 0x80417C58: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804198D4;
    }
    // 0x80417C58: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417C5C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C60: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417C64: b           L_80417808
    // 0x80417C68: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_80417808;
    // 0x80417C68: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_80417C6C:
    // 0x80417C6C: lw          $t0, 0x763C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X763C);
    // 0x80417C70: lui         $a0, 0x800E
    ctx->r4 = S32(0X800E << 16);
    // 0x80417C74: ori         $a0, $a0, 0xAEF0
    ctx->r4 = ctx->r4 | 0XAEF0;
    // 0x80417C78: bne         $t0, $a0, L_804174EC
    if (ctx->r8 != ctx->r4) {
        // 0x80417C7C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804174EC;
    }
    // 0x80417C7C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C80: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C84: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80417C88: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80417C8C: bne         $t0, $zero, L_804194F4
    if (ctx->r8 != 0) {
        // 0x80417C90: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194F4;
    }
    // 0x80417C90: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80417C94: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417C98: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417C9C: b           L_804174E8
    // 0x80417CA0: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_804174E8;
    // 0x80417CA0: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_80417CA4:
    // 0x80417CA4: lw          $a1, 0x4EE8($v0)
    ctx->r5 = MEM_W(ctx->r2, 0X4EE8);
    // 0x80417CA8: addiu       $v0, $zero, 0x15
    ctx->r2 = ADD32(0, 0X15);
    // 0x80417CAC: bne         $a1, $v0, L_80415DF4
    if (ctx->r5 != ctx->r2) {
        // 0x80417CB0: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80415DF4;
    }
    // 0x80417CB0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80417CB4: lbu         $v0, -0x2E64($v0)
    ctx->r2 = MEM_BU(ctx->r2, -0X2E64);
    // 0x80417CB8: sltiu       $v0, $v0, 0x5F
    ctx->r2 = ctx->r2 < 0X5F ? 1 : 0;
    // 0x80417CBC: bne         $v0, $zero, L_80415DF4
    if (ctx->r2 != 0) {
        // 0x80417CC0: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_80415DF4;
    }
    // 0x80417CC0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x80417CC4: lhu         $a1, 0x69FE($v0)
    ctx->r5 = MEM_HU(ctx->r2, 0X69FE);
    // 0x80417CC8: beql        $a1, $zero, L_80417CEC
    if (ctx->r5 == 0) {
        // 0x80417CCC: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_80417CEC;
    }
    goto skip_38;
    // 0x80417CCC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    skip_38:
    // 0x80417CD0: bnel        $v1, $zero, L_80417CEC
    if (ctx->r3 != 0) {
        // 0x80417CD4: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_80417CEC;
    }
    goto skip_39;
    // 0x80417CD4: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    skip_39:
    // 0x80417CD8: addiu       $a1, $a1, -0x1
    ctx->r5 = ADD32(ctx->r5, -0X1);
    // 0x80417CDC: addiu       $v1, $zero, 0x8
    ctx->r3 = ADD32(0, 0X8);
    // 0x80417CE0: sw          $v1, -0x4D1C($a0)
    MEM_W(-0X4D1C, ctx->r4) = ctx->r3;
    // 0x80417CE4: sh          $a1, 0x69FE($v0)
    MEM_H(0X69FE, ctx->r2) = ctx->r5;
    // 0x80417CE8: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
L_80417CEC:
    // 0x80417CEC: lw          $v1, -0x456C($a1)
    ctx->r3 = MEM_W(ctx->r5, -0X456C);
    // 0x80417CF0: sltiu       $a2, $v1, 0x2710
    ctx->r6 = ctx->r3 < 0X2710 ? 1 : 0;
    // 0x80417CF4: bne         $a2, $zero, L_80415DF4
    if (ctx->r6 != 0) {
        // 0x80417CF8: lhu         $a2, 0x69FE($v0)
        ctx->r6 = MEM_HU(ctx->r2, 0X69FE);
            goto L_80415DF4;
    }
    // 0x80417CF8: lhu         $a2, 0x69FE($v0)
    ctx->r6 = MEM_HU(ctx->r2, 0X69FE);
    // 0x80417CFC: lui         $a3, 0x8018
    ctx->r7 = S32(0X8018 << 16);
    // 0x80417D00: addiu       $t0, $zero, -0x1800
    ctx->r8 = ADD32(0, -0X1800);
    // 0x80417D04: bne         $a2, $zero, L_80415DF4
    if (ctx->r6 != 0) {
        // 0x80417D08: sh          $t0, 0x3202($a3)
        MEM_H(0X3202, ctx->r7) = ctx->r8;
            goto L_80415DF4;
    }
    // 0x80417D08: sh          $t0, 0x3202($a3)
    MEM_H(0X3202, ctx->r7) = ctx->r8;
    // 0x80417D0C: lw          $a0, -0x4D1C($a0)
    ctx->r4 = MEM_W(ctx->r4, -0X4D1C);
    // 0x80417D10: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x80417D14: bne         $a0, $a2, L_80415DF4
    if (ctx->r4 != ctx->r6) {
        // 0x80417D18: lui         $a2, 0x8009
        ctx->r6 = S32(0X8009 << 16);
            goto L_80415DF4;
    }
    // 0x80417D18: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
    // 0x80417D1C: lw          $a3, 0x67C($a2)
    ctx->r7 = MEM_W(ctx->r6, 0X67C);
    // 0x80417D20: lui         $a2, 0xA0C0
    ctx->r6 = S32(0XA0C0 << 16);
    // 0x80417D24: ori         $a2, $a2, 0x800E
    ctx->r6 = ctx->r6 | 0X800E;
    // 0x80417D28: bne         $a3, $a2, L_80415DF4
    if (ctx->r7 != ctx->r6) {
        // 0x80417D2C: lui         $a2, 0x8009
        ctx->r6 = S32(0X8009 << 16);
            goto L_80415DF4;
    }
    // 0x80417D2C: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
    // 0x80417D30: lhu         $a2, 0x2876($a2)
    ctx->r6 = MEM_HU(ctx->r6, 0X2876);
    // 0x80417D34: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80417D38: beq         $a2, $a3, L_8041A110
    if (ctx->r6 == ctx->r7) {
        // 0x80417D3C: addiu       $t0, $zero, 0x4
        ctx->r8 = ADD32(0, 0X4);
            goto L_8041A110;
    }
    // 0x80417D3C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80417D40: beql        $a2, $t0, L_8041A0F8
    if (ctx->r6 == ctx->r8) {
        // 0x80417D44: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_8041A0F8;
    }
    goto skip_40;
    // 0x80417D44: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    skip_40:
    // 0x80417D48: bnel        $a2, $a0, L_8041A0D8
    if (ctx->r6 != ctx->r4) {
        // 0x80417D4C: addiu       $a0, $zero, 0x8
        ctx->r4 = ADD32(0, 0X8);
            goto L_8041A0D8;
    }
    goto skip_41;
    // 0x80417D4C: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    skip_41:
    // 0x80417D50: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80417D54: lbu         $a0, -0x455B($a2)
    ctx->r4 = MEM_BU(ctx->r6, -0X455B);
    // 0x80417D58: addiu       $v1, $v1, -0x2710
    ctx->r3 = ADD32(ctx->r3, -0X2710);
    // 0x80417D5C: addiu       $a0, $a0, -0x1
    ctx->r4 = ADD32(ctx->r4, -0X1);
    // 0x80417D60: sh          $a0, 0x69FE($v0)
    MEM_H(0X69FE, ctx->r2) = ctx->r4;
    // 0x80417D64: sb          $a3, -0x455B($a2)
    MEM_B(-0X455B, ctx->r6) = ctx->r7;
L_80417D68:
    // 0x80417D68: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x80417D6C: sw          $zero, 0x5C0($v0)
    MEM_W(0X5C0, ctx->r2) = 0;
    // 0x80417D70: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x80417D74: sw          $v1, -0x456C($a1)
    MEM_W(-0X456C, ctx->r5) = ctx->r3;
    // 0x80417D78: b           L_80415DF4
    // 0x80417D7C: sw          $zero, 0x5E0($v0)
    MEM_W(0X5E0, ctx->r2) = 0;
        goto L_80415DF4;
    // 0x80417D7C: sw          $zero, 0x5E0($v0)
    MEM_W(0X5E0, ctx->r2) = 0;
L_80417D80:
    // 0x80417D80: bne         $v1, $zero, L_80415DE0
    if (ctx->r3 != 0) {
        // 0x80417D84: addiu       $a1, $zero, 0x5A0
        ctx->r5 = ADD32(0, 0X5A0);
            goto L_80415DE0;
    }
    // 0x80417D84: addiu       $a1, $zero, 0x5A0
    ctx->r5 = ADD32(0, 0X5A0);
    // 0x80417D88: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80417D8C: lhu         $a1, -0x3A6E($a1)
    ctx->r5 = MEM_HU(ctx->r5, -0X3A6E);
    // 0x80417D90: beql        $a1, $zero, L_80417DB0
    if (ctx->r5 == 0) {
        // 0x80417D94: addiu       $v1, $v0, -0xF
        ctx->r3 = ADD32(ctx->r2, -0XF);
            goto L_80417DB0;
    }
    goto skip_42;
    // 0x80417D94: addiu       $v1, $v0, -0xF
    ctx->r3 = ADD32(ctx->r2, -0XF);
    skip_42:
    // 0x80417D98: addiu       $a2, $zero, 0x16
    ctx->r6 = ADD32(0, 0X16);
    // 0x80417D9C: beq         $a1, $a2, L_80417DAC
    if (ctx->r5 == ctx->r6) {
        // 0x80417DA0: addiu       $a2, $zero, 0x406
        ctx->r6 = ADD32(0, 0X406);
            goto L_80417DAC;
    }
    // 0x80417DA0: addiu       $a2, $zero, 0x406
    ctx->r6 = ADD32(0, 0X406);
    // 0x80417DA4: bne         $a1, $a2, L_80415DEC
    if (ctx->r5 != ctx->r6) {
        // 0x80417DA8: addiu       $a1, $zero, 0x10
        ctx->r5 = ADD32(0, 0X10);
            goto L_80415DEC;
    }
    // 0x80417DA8: addiu       $a1, $zero, 0x10
    ctx->r5 = ADD32(0, 0X10);
L_80417DAC:
    // 0x80417DAC: addiu       $v1, $v0, -0xF
    ctx->r3 = ADD32(ctx->r2, -0XF);
L_80417DB0:
    // 0x80417DB0: sltiu       $v1, $v1, 0x4
    ctx->r3 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80417DB4: bne         $v1, $zero, L_80417DFC
    if (ctx->r3 != 0) {
        // 0x80417DB8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80417DFC;
    }
    // 0x80417DB8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417DBC: addiu       $v1, $zero, -0x3
    ctx->r3 = ADD32(0, -0X3);
    // 0x80417DC0: and         $v1, $v0, $v1
    ctx->r3 = ctx->r2 & ctx->r3;
    // 0x80417DC4: addiu       $a1, $zero, 0x14
    ctx->r5 = ADD32(0, 0X14);
    // 0x80417DC8: beq         $v1, $a1, L_80417DF8
    if (ctx->r3 == ctx->r5) {
        // 0x80417DCC: addiu       $v1, $zero, 0x17
        ctx->r3 = ADD32(0, 0X17);
            goto L_80417DF8;
    }
    // 0x80417DCC: addiu       $v1, $zero, 0x17
    ctx->r3 = ADD32(0, 0X17);
    // 0x80417DD0: beq         $v0, $v1, L_80419F40
    if (ctx->r2 == ctx->r3) {
        // 0x80417DD4: addiu       $v1, $zero, 0x19
        ctx->r3 = ADD32(0, 0X19);
            goto L_80419F40;
    }
    // 0x80417DD4: addiu       $v1, $zero, 0x19
    ctx->r3 = ADD32(0, 0X19);
    // 0x80417DD8: beq         $v0, $v1, L_80417DF8
    if (ctx->r2 == ctx->r3) {
        // 0x80417DDC: addiu       $v1, $zero, 0x1A
        ctx->r3 = ADD32(0, 0X1A);
            goto L_80417DF8;
    }
    // 0x80417DDC: addiu       $v1, $zero, 0x1A
    ctx->r3 = ADD32(0, 0X1A);
    // 0x80417DE0: bne         $v0, $v1, L_8041A0C4
    if (ctx->r2 != ctx->r3) {
        // 0x80417DE4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_8041A0C4;
    }
    // 0x80417DE4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417DE8: lw          $a1, 0x4EE8($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80417DEC: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x80417DF0: beq         $a1, $v1, L_80415DF4
    if (ctx->r5 == ctx->r3) {
        // 0x80417DF4: nop
    
            goto L_80415DF4;
    }
    // 0x80417DF4: nop

L_80417DF8:
    // 0x80417DF8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80417DFC:
    // 0x80417DFC: lui         $a1, 0x2
    ctx->r5 = S32(0X2 << 16);
L_80417E00:
    // 0x80417E00: sw          $a1, -0x4D20($v1)
    MEM_W(-0X4D20, ctx->r3) = ctx->r5;
    // 0x80417E04: addiu       $v1, $zero, 0x5A0
    ctx->r3 = ADD32(0, 0X5A0);
    // 0x80417E08: sw          $v1, -0x4D1C($a0)
    MEM_W(-0X4D1C, ctx->r4) = ctx->r3;
    // 0x80417E0C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_80417E10:
    // 0x80417E10: lhu         $a2, -0x340($v1)
    ctx->r6 = MEM_HU(ctx->r3, -0X340);
    // 0x80417E14: addiu       $a1, $zero, 0x1313
    ctx->r5 = ADD32(0, 0X1313);
    // 0x80417E18: beql        $a2, $a1, L_80415DE8
    if (ctx->r6 == ctx->r5) {
        // 0x80417E1C: addiu       $v1, $zero, 0x5A0
        ctx->r3 = ADD32(0, 0X5A0);
            goto L_80415DE8;
    }
    goto skip_43;
    // 0x80417E1C: addiu       $v1, $zero, 0x5A0
    ctx->r3 = ADD32(0, 0X5A0);
    skip_43:
    // 0x80417E20: sh          $a1, -0x340($v1)
    MEM_H(-0X340, ctx->r3) = ctx->r5;
    // 0x80417E24: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417E28: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x80417E2C: sh          $a1, -0x33E($v1)
    MEM_H(-0X33E, ctx->r3) = ctx->r5;
    // 0x80417E30: b           L_80415DE8
    // 0x80417E34: addiu       $v1, $zero, 0x5A0
    ctx->r3 = ADD32(0, 0X5A0);
        goto L_80415DE8;
    // 0x80417E34: addiu       $v1, $zero, 0x5A0
    ctx->r3 = ADD32(0, 0X5A0);
L_80417E38:
    // 0x80417E38: bne         $v0, $a0, L_80417F50
    if (ctx->r2 != ctx->r4) {
        // 0x80417E3C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80417F50;
    }
    // 0x80417E3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80417E40: lw          $a0, 0x4EE8($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X4EE8);
    // 0x80417E44: bne         $a0, $v1, L_80415DC8
    if (ctx->r4 != ctx->r3) {
        // 0x80417E48: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    // 0x80417E48: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417E4C: lui         $a0, 0x8018
    ctx->r4 = S32(0X8018 << 16);
    // 0x80417E50: lhu         $v1, 0x343C($a0)
    ctx->r3 = MEM_HU(ctx->r4, 0X343C);
    // 0x80417E54: addiu       $a1, $zero, 0x7570
    ctx->r5 = ADD32(0, 0X7570);
    // 0x80417E58: beq         $v1, $a1, L_80417E68
    if (ctx->r3 == ctx->r5) {
        // 0x80417E5C: ori         $a1, $zero, 0xE200
        ctx->r5 = 0 | 0XE200;
            goto L_80417E68;
    }
    // 0x80417E5C: ori         $a1, $zero, 0xE200
    ctx->r5 = 0 | 0XE200;
    // 0x80417E60: bnel        $v1, $a1, L_80415DC8
    if (ctx->r3 != ctx->r5) {
        // 0x80417E64: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    goto skip_44;
    // 0x80417E64: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_44:
L_80417E68:
    // 0x80417E68: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417E6C: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80417E70: lui         $a1, 0x80C0
    ctx->r5 = S32(0X80C0 << 16);
    // 0x80417E74: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80417E78: addiu       $a1, $a1, 0xE18
    ctx->r5 = ADD32(ctx->r5, 0XE18);
    // 0x80417E7C: beq         $a3, $a1, L_80419A6C
    if (ctx->r7 == ctx->r5) {
        // 0x80417E80: lw          $v1, -0x456C($a2)
        ctx->r3 = MEM_W(ctx->r6, -0X456C);
            goto L_80419A6C;
    }
    // 0x80417E80: lw          $v1, -0x456C($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X456C);
L_80417E84:
    // 0x80417E84: sltiu       $v1, $v1, 0x96
    ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
L_80417E88:
    // 0x80417E88: bnel        $v1, $zero, L_8041998C
    if (ctx->r3 != 0) {
        // 0x80417E8C: addiu       $v1, $zero, 0x7570
        ctx->r3 = ADD32(0, 0X7570);
            goto L_8041998C;
    }
    goto skip_45;
    // 0x80417E8C: addiu       $v1, $zero, 0x7570
    ctx->r3 = ADD32(0, 0X7570);
    skip_45:
    // 0x80417E90: addiu       $v1, $zero, -0x1E00
    ctx->r3 = ADD32(0, -0X1E00);
    // 0x80417E94: b           L_80415DC4
    // 0x80417E98: sh          $v1, 0x343C($a0)
    MEM_H(0X343C, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80417E98: sh          $v1, 0x343C($a0)
    MEM_H(0X343C, ctx->r4) = ctx->r3;
L_80417E9C:
    // 0x80417E9C: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80417EA0: bne         $v1, $zero, L_80415848
    if (ctx->r3 != 0) {
        // 0x80417EA4: lui         $a3, 0x8008
        ctx->r7 = S32(0X8008 << 16);
            goto L_80415848;
    }
    // 0x80417EA4: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x80417EA8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417EAC: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80417EB0: sltiu       $v1, $v1, 0x5F
    ctx->r3 = ctx->r3 < 0X5F ? 1 : 0;
    // 0x80417EB4: bne         $v1, $zero, L_8041584C
    if (ctx->r3 != 0) {
        // 0x80417EB8: lhu         $v1, -0x457A($a3)
        ctx->r3 = MEM_HU(ctx->r7, -0X457A);
            goto L_8041584C;
    }
    // 0x80417EB8: lhu         $v1, -0x457A($a3)
    ctx->r3 = MEM_HU(ctx->r7, -0X457A);
    // 0x80417EBC: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417EC0: lw          $a1, 0x67C($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X67C);
    // 0x80417EC4: lui         $v1, 0x80C0
    ctx->r3 = S32(0X80C0 << 16);
    // 0x80417EC8: ori         $v1, $v1, 0xE07F
    ctx->r3 = ctx->r3 | 0XE07F;
    // 0x80417ECC: beq         $a1, $v1, L_80419D6C
    if (ctx->r5 == ctx->r3) {
        // 0x80417ED0: lbu         $v1, 0x69F2($a2)
        ctx->r3 = MEM_BU(ctx->r6, 0X69F2);
            goto L_80419D6C;
    }
    // 0x80417ED0: lbu         $v1, 0x69F2($a2)
    ctx->r3 = MEM_BU(ctx->r6, 0X69F2);
    // 0x80417ED4: andi        $v1, $v1, 0x4
    ctx->r3 = ctx->r3 & 0X4;
    // 0x80417ED8: bne         $v1, $zero, L_80419DD4
    if (ctx->r3 != 0) {
        // 0x80417EDC: lui         $v1, 0x8017
        ctx->r3 = S32(0X8017 << 16);
            goto L_80419DD4;
    }
    // 0x80417EDC: lui         $v1, 0x8017
    ctx->r3 = S32(0X8017 << 16);
    // 0x80417EE0: addiu       $a0, $zero, -0x2E00
    ctx->r4 = ADD32(0, -0X2E00);
    // 0x80417EE4: b           L_80415848
    // 0x80417EE8: sh          $a0, 0x18AE($v1)
    MEM_H(0X18AE, ctx->r3) = ctx->r4;
        goto L_80415848;
    // 0x80417EE8: sh          $a0, 0x18AE($v1)
    MEM_H(0X18AE, ctx->r3) = ctx->r4;
L_80417EEC:
    // 0x80417EEC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417EF0: lw          $a3, 0x4EE8($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80417EF4: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x80417EF8: bne         $a3, $v1, L_804157F8
    if (ctx->r7 != ctx->r3) {
        // 0x80417EFC: nop
    
            goto L_804157F8;
    }
    // 0x80417EFC: nop

    // 0x80417F00: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417F04: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80417F08: sltiu       $v1, $v1, 0x5F
    ctx->r3 = ctx->r3 < 0X5F ? 1 : 0;
    // 0x80417F0C: bne         $v1, $zero, L_804157F8
    if (ctx->r3 != 0) {
        // 0x80417F10: nop
    
            goto L_804157F8;
    }
    // 0x80417F10: nop

    // 0x80417F14: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417F18: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80417F1C: lui         $v1, 0x80C0
    ctx->r3 = S32(0X80C0 << 16);
    // 0x80417F20: ori         $v1, $v1, 0xE00E
    ctx->r3 = ctx->r3 | 0XE00E;
    // 0x80417F24: beq         $a3, $v1, L_80418980
    if (ctx->r7 == ctx->r3) {
        // 0x80417F28: nop
    
            goto L_80418980;
    }
    // 0x80417F28: nop

    // 0x80417F2C: bne         $a1, $zero, L_804189E0
    if (ctx->r5 != 0) {
        // 0x80417F30: lui         $v1, 0x800C
        ctx->r3 = S32(0X800C << 16);
            goto L_804189E0;
    }
    // 0x80417F30: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80417F34: lhu         $a1, 0x1B28($v1)
    ctx->r5 = MEM_HU(ctx->r3, 0X1B28);
    // 0x80417F38: lui         $a3, 0x8014
    ctx->r7 = S32(0X8014 << 16);
    // 0x80417F3C: addiu       $t0, $zero, -0x2800
    ctx->r8 = ADD32(0, -0X2800);
    // 0x80417F40: beq         $a1, $zero, L_80416220
    if (ctx->r5 == 0) {
        // 0x80417F44: sh          $t0, -0x25E6($a3)
        MEM_H(-0X25E6, ctx->r7) = ctx->r8;
            goto L_80416220;
    }
    // 0x80417F44: sh          $t0, -0x25E6($a3)
    MEM_H(-0X25E6, ctx->r7) = ctx->r8;
    // 0x80417F48: b           L_80415848
    // 0x80417F4C: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
        goto L_80415848;
    // 0x80417F4C: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
L_80417F50:
    // 0x80417F50: addiu       $v1, $zero, 0x17
    ctx->r3 = ADD32(0, 0X17);
    // 0x80417F54: bne         $v0, $v1, L_80415DC8
    if (ctx->r2 != ctx->r3) {
        // 0x80417F58: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    // 0x80417F58: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417F5C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417F60: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80417F64: addiu       $v1, $zero, 0x5
    ctx->r3 = ADD32(0, 0X5);
    // 0x80417F68: bne         $a0, $v1, L_80415DC8
    if (ctx->r4 != ctx->r3) {
        // 0x80417F6C: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    // 0x80417F6C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417F70: lui         $a0, 0x8019
    ctx->r4 = S32(0X8019 << 16);
    // 0x80417F74: lhu         $v1, -0x30C2($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X30C2);
    // 0x80417F78: ori         $a1, $zero, 0xB490
    ctx->r5 = 0 | 0XB490;
    // 0x80417F7C: beq         $v1, $a1, L_80417F8C
    if (ctx->r3 == ctx->r5) {
        // 0x80417F80: ori         $a1, $zero, 0xE300
        ctx->r5 = 0 | 0XE300;
            goto L_80417F8C;
    }
    // 0x80417F80: ori         $a1, $zero, 0xE300
    ctx->r5 = 0 | 0XE300;
    // 0x80417F84: bnel        $v1, $a1, L_80415DC8
    if (ctx->r3 != ctx->r5) {
        // 0x80417F88: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    goto skip_46;
    // 0x80417F88: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_46:
L_80417F8C:
    // 0x80417F8C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80417F90: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80417F94: lui         $a1, 0x80C0
    ctx->r5 = S32(0X80C0 << 16);
    // 0x80417F98: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80417F9C: addiu       $a1, $a1, 0xE18
    ctx->r5 = ADD32(ctx->r5, 0XE18);
    // 0x80417FA0: beq         $a3, $a1, L_80419B20
    if (ctx->r7 == ctx->r5) {
        // 0x80417FA4: lw          $v1, -0x456C($a2)
        ctx->r3 = MEM_W(ctx->r6, -0X456C);
            goto L_80419B20;
    }
    // 0x80417FA4: lw          $v1, -0x456C($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X456C);
L_80417FA8:
    // 0x80417FA8: sltiu       $v1, $v1, 0xFA
    ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
L_80417FAC:
    // 0x80417FAC: bnel        $v1, $zero, L_8041999C
    if (ctx->r3 != 0) {
        // 0x80417FB0: addiu       $v1, $zero, -0x4B70
        ctx->r3 = ADD32(0, -0X4B70);
            goto L_8041999C;
    }
    goto skip_47;
    // 0x80417FB0: addiu       $v1, $zero, -0x4B70
    ctx->r3 = ADD32(0, -0X4B70);
    skip_47:
    // 0x80417FB4: addiu       $v1, $zero, -0x1D00
    ctx->r3 = ADD32(0, -0X1D00);
    // 0x80417FB8: b           L_80415DC4
    // 0x80417FBC: sh          $v1, -0x30C2($a0)
    MEM_H(-0X30C2, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80417FBC: sh          $v1, -0x30C2($a0)
    MEM_H(-0X30C2, ctx->r4) = ctx->r3;
L_80417FC0:
    // 0x80417FC0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417FC4: lw          $a1, -0x455C($v1)
    ctx->r5 = MEM_W(ctx->r3, -0X455C);
    // 0x80417FC8: lui         $v1, 0x9696
    ctx->r3 = S32(0X9696 << 16);
    // 0x80417FCC: ori         $v1, $v1, 0x9696
    ctx->r3 = ctx->r3 | 0X9696;
    // 0x80417FD0: beq         $a1, $v1, L_804158C8
    if (ctx->r5 == ctx->r3) {
        // 0x80417FD4: lui         $v1, 0xFE00
        ctx->r3 = S32(0XFE00 << 16);
            goto L_804158C8;
    }
    // 0x80417FD4: lui         $v1, 0xFE00
    ctx->r3 = S32(0XFE00 << 16);
    // 0x80417FD8: b           L_804158C8
    // 0x80417FDC: sw          $v1, -0x454C($a0)
    MEM_W(-0X454C, ctx->r4) = ctx->r3;
        goto L_804158C8;
    // 0x80417FDC: sw          $v1, -0x454C($a0)
    MEM_W(-0X454C, ctx->r4) = ctx->r3;
L_80417FE0:
    // 0x80417FE0: addiu       $v1, $zero, 0xD
    ctx->r3 = ADD32(0, 0XD);
    // 0x80417FE4: bne         $v0, $v1, L_80415844
    if (ctx->r2 != ctx->r3) {
        // 0x80417FE8: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415844;
    }
    // 0x80417FE8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80417FEC: lw          $a0, 0x4EE8($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80417FF0: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
    // 0x80417FF4: bne         $a0, $v1, L_80415848
    if (ctx->r4 != ctx->r3) {
        // 0x80417FF8: lui         $a3, 0x8008
        ctx->r7 = S32(0X8008 << 16);
            goto L_80415848;
    }
    // 0x80417FF8: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x80417FFC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418000: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80418004: sltiu       $v1, $v1, 0xF
    ctx->r3 = ctx->r3 < 0XF ? 1 : 0;
    // 0x80418008: bne         $v1, $zero, L_8041584C
    if (ctx->r3 != 0) {
        // 0x8041800C: lhu         $v1, -0x457A($a3)
        ctx->r3 = MEM_HU(ctx->r7, -0X457A);
            goto L_8041584C;
    }
    // 0x8041800C: lhu         $v1, -0x457A($a3)
    ctx->r3 = MEM_HU(ctx->r7, -0X457A);
    // 0x80418010: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80418014: lbu         $a0, 0x69F2($a2)
    ctx->r4 = MEM_BU(ctx->r6, 0X69F2);
    // 0x80418018: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x8041801C: lui         $v1, 0x80C0
    ctx->r3 = S32(0X80C0 << 16);
    // 0x80418020: ori         $v1, $v1, 0xE07F
    ctx->r3 = ctx->r3 | 0XE07F;
    // 0x80418024: beq         $a3, $v1, L_80419C00
    if (ctx->r7 == ctx->r3) {
        // 0x80418028: andi        $a1, $a0, 0x2
        ctx->r5 = ctx->r4 & 0X2;
            goto L_80419C00;
    }
    // 0x80418028: andi        $a1, $a0, 0x2
    ctx->r5 = ctx->r4 & 0X2;
    // 0x8041802C: bne         $a1, $zero, L_80419C60
    if (ctx->r5 != 0) {
        // 0x80418030: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419C60;
    }
    // 0x80418030: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80418034: addiu       $a0, $zero, -0x2C00
    ctx->r4 = ADD32(0, -0X2C00);
    // 0x80418038: b           L_80415844
    // 0x8041803C: sh          $a0, -0x6D4A($v1)
    MEM_H(-0X6D4A, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x8041803C: sh          $a0, -0x6D4A($v1)
    MEM_H(-0X6D4A, ctx->r3) = ctx->r4;
L_80418040:
    // 0x80418040: lbu         $t0, -0x44B5($v1)
    ctx->r8 = MEM_BU(ctx->r3, -0X44B5);
    // 0x80418044: addiu       $v1, $zero, 0x4
    ctx->r3 = ADD32(0, 0X4);
    // 0x80418048: beq         $t0, $v1, L_804158F8
    if (ctx->r8 == ctx->r3) {
        // 0x8041804C: lui         $v1, 0x3D8F
        ctx->r3 = S32(0X3D8F << 16);
            goto L_804158F8;
    }
    // 0x8041804C: lui         $v1, 0x3D8F
    ctx->r3 = S32(0X3D8F << 16);
    // 0x80418050: addiu       $v1, $v1, 0x5C29
    ctx->r3 = ADD32(ctx->r3, 0X5C29);
    // 0x80418054: sw          $v1, -0x4510($a0)
    MEM_W(-0X4510, ctx->r4) = ctx->r3;
    // 0x80418058: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041805C: lw          $v1, 0x73E4($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X73E4);
    // 0x80418060: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80418064: bne         $v1, $a0, L_80415910
    if (ctx->r3 != ctx->r4) {
        // 0x80418068: addiu       $a0, $zero, 0x2
        ctx->r4 = ADD32(0, 0X2);
            goto L_80415910;
    }
    // 0x80418068: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    // 0x8041806C: lui         $v1, 0x8005
    ctx->r3 = S32(0X8005 << 16);
L_80418070:
    // 0x80418070: addiu       $a0, $zero, -0x100
    ctx->r4 = ADD32(0, -0X100);
    // 0x80418074: b           L_80415930
    // 0x80418078: sw          $a0, -0x305C($v1)
    MEM_W(-0X305C, ctx->r3) = ctx->r4;
        goto L_80415930;
    // 0x80418078: sw          $a0, -0x305C($v1)
    MEM_W(-0X305C, ctx->r3) = ctx->r4;
L_8041807C:
    // 0x8041807C: lw          $v1, 0x4EE8($v0)
    ctx->r3 = MEM_W(ctx->r2, 0X4EE8);
    // 0x80418080: addiu       $v0, $zero, 0xD
    ctx->r2 = ADD32(0, 0XD);
    // 0x80418084: bne         $v1, $v0, L_80415F48
    if (ctx->r3 != ctx->r2) {
        // 0x80418088: lw          $ra, 0x1C($sp)
        ctx->r31 = MEM_W(ctx->r29, 0X1C);
            goto L_80415F48;
    }
    // 0x80418088: lw          $ra, 0x1C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X1C);
    // 0x8041808C: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x80418090: lbu         $v0, -0x3025($v0)
    ctx->r2 = MEM_BU(ctx->r2, -0X3025);
    // 0x80418094: addiu       $v1, $zero, 0x80
    ctx->r3 = ADD32(0, 0X80);
    // 0x80418098: beql        $v0, $v1, L_80419DE4
    if (ctx->r2 == ctx->r3) {
        // 0x8041809C: lui         $v0, 0xC7C3
        ctx->r2 = S32(0XC7C3 << 16);
            goto L_80419DE4;
    }
    goto skip_48;
    // 0x8041809C: lui         $v0, 0xC7C3
    ctx->r2 = S32(0XC7C3 << 16);
    skip_48:
    // 0x804180A0: bne         $v0, $zero, L_80415F4C
    if (ctx->r2 != 0) {
        // 0x804180A4: lw          $s1, 0x18($sp)
        ctx->r17 = MEM_W(ctx->r29, 0X18);
            goto L_80415F4C;
    }
    // 0x804180A4: lw          $s1, 0x18($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X18);
    // 0x804180A8: lui         $v1, 0xC188
    ctx->r3 = S32(0XC188 << 16);
    // 0x804180AC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x804180B0: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x804180B4: sw          $v1, -0x42BC($a1)
    MEM_W(-0X42BC, ctx->r5) = ctx->r3;
    // 0x804180B8: lhu         $v0, 0x59E0($v0)
    ctx->r2 = MEM_HU(ctx->r2, 0X59E0);
    // 0x804180BC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x804180C0: lui         $a2, 0xC2A8
    ctx->r6 = S32(0XC2A8 << 16);
    // 0x804180C4: sw          $a2, -0x42B4($a1)
    MEM_W(-0X42B4, ctx->r5) = ctx->r6;
    // 0x804180C8: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x804180CC: sw          $v1, -0x4238($a1)
    MEM_W(-0X4238, ctx->r5) = ctx->r3;
    // 0x804180D0: sltiu       $a0, $v0, 0x2
    ctx->r4 = ctx->r2 < 0X2 ? 1 : 0;
    // 0x804180D4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x804180D8: lui         $a1, 0xC2C8
    ctx->r5 = S32(0XC2C8 << 16);
    // 0x804180DC: bne         $a0, $zero, L_80419EC4
    if (ctx->r4 != 0) {
        // 0x804180E0: sw          $a1, -0x4230($v1)
        MEM_W(-0X4230, ctx->r3) = ctx->r5;
            goto L_80419EC4;
    }
    // 0x804180E0: sw          $a1, -0x4230($v1)
    MEM_W(-0X4230, ctx->r3) = ctx->r5;
    // 0x804180E4: addiu       $v1, $v0, -0x1
    ctx->r3 = ADD32(ctx->r2, -0X1);
    // 0x804180E8: sltiu       $v1, $v1, 0x7
    ctx->r3 = ctx->r3 < 0X7 ? 1 : 0;
    // 0x804180EC: beql        $v1, $zero, L_80419ED4
    if (ctx->r3 == 0) {
        // 0x804180F0: addiu       $v0, $v0, -0x7
        ctx->r2 = ADD32(ctx->r2, -0X7);
            goto L_80419ED4;
    }
    goto skip_49;
    // 0x804180F0: addiu       $v0, $v0, -0x7
    ctx->r2 = ADD32(ctx->r2, -0X7);
    skip_49:
    // 0x804180F4: lui         $v0, 0x8015
    ctx->r2 = S32(0X8015 << 16);
    // 0x804180F8: addiu       $v1, $zero, -0x200
    ctx->r3 = ADD32(0, -0X200);
    // 0x804180FC: b           L_80415F44
    // 0x80418100: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
        goto L_80415F44;
    // 0x80418100: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
L_80418104:
    // 0x80418104: lui         $a0, 0xFFFF
    ctx->r4 = S32(0XFFFF << 16);
    // 0x80418108: b           L_80415930
    // 0x8041810C: sw          $a0, -0x305C($v1)
    MEM_W(-0X305C, ctx->r3) = ctx->r4;
        goto L_80415930;
    // 0x8041810C: sw          $a0, -0x305C($v1)
    MEM_W(-0X305C, ctx->r3) = ctx->r4;
L_80418110:
    // 0x80418110: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80418114: bne         $v1, $a0, L_8041812C
    if (ctx->r3 != ctx->r4) {
        // 0x80418118: lui         $a0, 0x8005
        ctx->r4 = S32(0X8005 << 16);
            goto L_8041812C;
    }
    // 0x80418118: lui         $a0, 0x8005
    ctx->r4 = S32(0X8005 << 16);
    // 0x8041811C: lui         $v1, 0x8000
    ctx->r3 = S32(0X8000 << 16);
    // 0x80418120: ori         $v1, $v1, 0x8000
    ctx->r3 = ctx->r3 | 0X8000;
    // 0x80418124: b           L_80415930
    // 0x80418128: sw          $v1, -0x305C($a0)
    MEM_W(-0X305C, ctx->r4) = ctx->r3;
        goto L_80415930;
    // 0x80418128: sw          $v1, -0x305C($a0)
    MEM_W(-0X305C, ctx->r4) = ctx->r3;
L_8041812C:
    // 0x8041812C: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x80418130: bne         $v1, $a0, L_80415930
    if (ctx->r3 != ctx->r4) {
        // 0x80418134: lui         $v1, 0x4D4D
        ctx->r3 = S32(0X4D4D << 16);
            goto L_80415930;
    }
    // 0x80418134: lui         $v1, 0x4D4D
    ctx->r3 = S32(0X4D4D << 16);
    // 0x80418138: lui         $a0, 0x8005
    ctx->r4 = S32(0X8005 << 16);
    // 0x8041813C: addiu       $v1, $v1, 0x4D00
    ctx->r3 = ADD32(ctx->r3, 0X4D00);
    // 0x80418140: b           L_80415930
    // 0x80418144: sw          $v1, -0x305C($a0)
    MEM_W(-0X305C, ctx->r4) = ctx->r3;
        goto L_80415930;
    // 0x80418144: sw          $v1, -0x305C($a0)
    MEM_W(-0X305C, ctx->r4) = ctx->r3;
L_80418148:
    // 0x80418148: lw          $a2, -0x4D20($a1)
    ctx->r6 = MEM_W(ctx->r5, -0X4D20);
    // 0x8041814C: lui         $a1, 0x2
    ctx->r5 = S32(0X2 << 16);
    // 0x80418150: bne         $a2, $a1, L_80415DEC
    if (ctx->r6 != ctx->r5) {
        // 0x80418154: addiu       $a1, $zero, 0x10
        ctx->r5 = ADD32(0, 0X10);
            goto L_80415DEC;
    }
    // 0x80418154: addiu       $a1, $zero, 0x10
    ctx->r5 = ADD32(0, 0X10);
    // 0x80418158: b           L_80417E10
    // 0x8041815C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
        goto L_80417E10;
    // 0x8041815C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_80418160:
    // 0x80418160: addiu       $a2, $zero, -0x3
    ctx->r6 = ADD32(0, -0X3);
    // 0x80418164: and         $a2, $v1, $a2
    ctx->r6 = ctx->r3 & ctx->r6;
    // 0x80418168: addiu       $a1, $v1, -0x2
    ctx->r5 = ADD32(ctx->r3, -0X2);
    // 0x8041816C: xori        $a2, $a2, 0x5
    ctx->r6 = ctx->r6 ^ 0X5;
    // 0x80418170: sltiu       $a1, $a1, 0x2
    ctx->r5 = ctx->r5 < 0X2 ? 1 : 0;
    // 0x80418174: sltiu       $a2, $a2, 0x1
    ctx->r6 = ctx->r6 < 0X1 ? 1 : 0;
    // 0x80418178: or          $a1, $a1, $a2
    ctx->r5 = ctx->r5 | ctx->r6;
    // 0x8041817C: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    // 0x80418180: bne         $v1, $a0, L_80418910
    if (ctx->r3 != ctx->r4) {
        // 0x80418184: or          $ra, $a1, $zero
        ctx->r31 = ctx->r5 | 0;
            goto L_80418910;
    }
    // 0x80418184: or          $ra, $a1, $zero
    ctx->r31 = ctx->r5 | 0;
    // 0x80418188: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041818C: lw          $a0, 0x536C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X536C);
    // 0x80418190: bne         $a0, $zero, L_804188EC
    if (ctx->r4 != 0) {
        // 0x80418194: xori        $t3, $v1, 0x9
        ctx->r11 = ctx->r3 ^ 0X9;
            goto L_804188EC;
    }
    // 0x80418194: xori        $t3, $v1, 0x9
    ctx->r11 = ctx->r3 ^ 0X9;
    // 0x80418198: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041819C: or          $t0, $zero, $zero
    ctx->r8 = 0 | 0;
    // 0x804181A0: or          $t9, $zero, $zero
    ctx->r25 = 0 | 0;
    // 0x804181A4: or          $s1, $zero, $zero
    ctx->r17 = 0 | 0;
    // 0x804181A8: or          $t5, $zero, $zero
    ctx->r13 = 0 | 0;
    // 0x804181AC: or          $t6, $zero, $zero
    ctx->r14 = 0 | 0;
    // 0x804181B0: or          $t8, $zero, $zero
    ctx->r24 = 0 | 0;
    // 0x804181B4: or          $t7, $zero, $zero
    ctx->r15 = 0 | 0;
    // 0x804181B8: xori        $a2, $v1, 0x13
    ctx->r6 = ctx->r3 ^ 0X13;
L_804181BC:
    // 0x804181BC: xori        $a1, $v1, 0x17
    ctx->r5 = ctx->r3 ^ 0X17;
    // 0x804181C0: sltiu       $a2, $a2, 0x1
    ctx->r6 = ctx->r6 < 0X1 ? 1 : 0;
    // 0x804181C4: sltiu       $a1, $a1, 0x1
    ctx->r5 = ctx->r5 < 0X1 ? 1 : 0;
L_804181C8:
    // 0x804181C8: addiu       $t3, $zero, 0x20
    ctx->r11 = ADD32(0, 0X20);
    // 0x804181CC: bne         $v1, $t3, L_80416134
    if (ctx->r3 != ctx->r11) {
        // 0x804181D0: addiu       $t3, $v1, -0x21
        ctx->r11 = ADD32(ctx->r3, -0X21);
            goto L_80416134;
    }
    // 0x804181D0: addiu       $t3, $v1, -0x21
    ctx->r11 = ADD32(ctx->r3, -0X21);
L_804181D4:
    // 0x804181D4: ori         $a3, $a3, 0x40
    ctx->r7 = ctx->r7 | 0X40;
    // 0x804181D8: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
L_804181DC:
    // 0x804181DC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
L_804181E0:
    // 0x804181E0: lhu         $a1, -0x60DC($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X60DC);
    // 0x804181E4: addiu       $v1, $zero, 0x2EE0
    ctx->r3 = ADD32(0, 0X2EE0);
    // 0x804181E8: beq         $a1, $v1, L_80418294
    if (ctx->r5 == ctx->r3) {
        // 0x804181EC: lui         $a1, 0x8040
        ctx->r5 = S32(0X8040 << 16);
            goto L_80418294;
    }
    // 0x804181EC: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x804181F0: sh          $v1, -0x60DC($a0)
    MEM_H(-0X60DC, ctx->r4) = ctx->r3;
    // 0x804181F4: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804181F8: sh          $v1, -0x60DA($a0)
    MEM_H(-0X60DA, ctx->r4) = ctx->r3;
    // 0x804181FC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418200: addiu       $a0, $zero, 0x3E8
    ctx->r4 = ADD32(0, 0X3E8);
    // 0x80418204: sh          $a0, -0x60D2($v1)
    MEM_H(-0X60D2, ctx->r3) = ctx->r4;
    // 0x80418208: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041820C: addiu       $a0, $zero, 0x15E
    ctx->r4 = ADD32(0, 0X15E);
    // 0x80418210: sh          $a0, -0x60D4($v1)
    MEM_H(-0X60D4, ctx->r3) = ctx->r4;
    // 0x80418214: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418218: addiu       $a0, $zero, 0x78
    ctx->r4 = ADD32(0, 0X78);
    // 0x8041821C: sh          $a0, -0x60B6($v1)
    MEM_H(-0X60B6, ctx->r3) = ctx->r4;
    // 0x80418220: lui         $v1, 0x7
    ctx->r3 = S32(0X7 << 16);
    // 0x80418224: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418228: ori         $v1, $v1, 0xA120
    ctx->r3 = ctx->r3 | 0XA120;
    // 0x8041822C: sw          $v1, -0x60D0($a0)
    MEM_W(-0X60D0, ctx->r4) = ctx->r3;
    // 0x80418230: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418234: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x80418238: sw          $a0, -0x60CC($v1)
    MEM_W(-0X60CC, ctx->r3) = ctx->r4;
    // 0x8041823C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418240: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x80418244: sw          $a0, -0x60AC($v1)
    MEM_W(-0X60AC, ctx->r3) = ctx->r4;
    // 0x80418248: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041824C: sw          $zero, -0x6524($v1)
    MEM_W(-0X6524, ctx->r3) = 0;
    // 0x80418250: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418254: sw          $zero, -0x6520($v1)
    MEM_W(-0X6520, ctx->r3) = 0;
    // 0x80418258: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041825C: sw          $zero, -0x6518($v1)
    MEM_W(-0X6518, ctx->r3) = 0;
    // 0x80418260: lui         $v1, 0xE
    ctx->r3 = S32(0XE << 16);
    // 0x80418264: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x80418268: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x8041826C: ori         $a1, $a1, 0xEF0A
    ctx->r5 = ctx->r5 | 0XEF0A;
    // 0x80418270: sw          $v1, -0x6530($a0)
    MEM_W(-0X6530, ctx->r4) = ctx->r3;
    // 0x80418274: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418278: addiu       $a3, $a1, 0x56
    ctx->r7 = ADD32(ctx->r5, 0X56);
    // 0x8041827C: sw          $a1, -0x5428($v1)
    MEM_W(-0X5428, ctx->r3) = ctx->r5;
    // 0x80418280: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418284: addiu       $a2, $a1, 0xF6
    ctx->r6 = ADD32(ctx->r5, 0XF6);
    // 0x80418288: sw          $a3, -0x5448($v1)
    MEM_W(-0X5448, ctx->r3) = ctx->r7;
    // 0x8041828C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418290: sw          $a2, -0x5408($v1)
    MEM_W(-0X5408, ctx->r3) = ctx->r6;
L_80418294:
    // 0x80418294: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418298: lhu         $a1, -0x60A4($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X60A4);
    // 0x8041829C: addiu       $v1, $zero, 0x3A98
    ctx->r3 = ADD32(0, 0X3A98);
    // 0x804182A0: beq         $a1, $v1, L_80418384
    if (ctx->r5 == ctx->r3) {
        // 0x804182A4: lui         $a1, 0x8040
        ctx->r5 = S32(0X8040 << 16);
            goto L_80418384;
    }
    // 0x804182A4: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x804182A8: sh          $v1, -0x60A4($a0)
    MEM_H(-0X60A4, ctx->r4) = ctx->r3;
    // 0x804182AC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804182B0: sh          $v1, -0x60A2($a0)
    MEM_H(-0X60A2, ctx->r4) = ctx->r3;
    // 0x804182B4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804182B8: addiu       $a0, $zero, 0x3FC
    ctx->r4 = ADD32(0, 0X3FC);
    // 0x804182BC: sh          $a0, -0x609A($v1)
    MEM_H(-0X609A, ctx->r3) = ctx->r4;
    // 0x804182C0: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804182C4: addiu       $a0, $zero, 0x1C2
    ctx->r4 = ADD32(0, 0X1C2);
    // 0x804182C8: sh          $a0, -0x609C($v1)
    MEM_H(-0X609C, ctx->r3) = ctx->r4;
    // 0x804182CC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804182D0: addiu       $a0, $zero, 0x7D
    ctx->r4 = ADD32(0, 0X7D);
    // 0x804182D4: sh          $a0, -0x607E($v1)
    MEM_H(-0X607E, ctx->r3) = ctx->r4;
    // 0x804182D8: lui         $v1, 0x7
    ctx->r3 = S32(0X7 << 16);
    // 0x804182DC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804182E0: ori         $v1, $v1, 0xC830
    ctx->r3 = ctx->r3 | 0XC830;
    // 0x804182E4: sw          $v1, -0x6098($a0)
    MEM_W(-0X6098, ctx->r4) = ctx->r3;
    // 0x804182E8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804182EC: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x804182F0: sw          $a0, -0x6094($v1)
    MEM_W(-0X6094, ctx->r3) = ctx->r4;
    // 0x804182F4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804182F8: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x804182FC: sw          $a0, -0x6074($v1)
    MEM_W(-0X6074, ctx->r3) = ctx->r4;
    // 0x80418300: lui         $v1, 0xC31A
    ctx->r3 = S32(0XC31A << 16);
    // 0x80418304: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418308: ori         $v1, $v1, 0xC3A7
    ctx->r3 = ctx->r3 | 0XC3A7;
    // 0x8041830C: sw          $v1, -0x650C($a0)
    MEM_W(-0X650C, ctx->r4) = ctx->r3;
    // 0x80418310: lui         $v1, 0xC349
    ctx->r3 = S32(0XC349 << 16);
    // 0x80418314: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418318: addiu       $v1, $v1, 0x246B
    ctx->r3 = ADD32(ctx->r3, 0X246B);
    // 0x8041831C: sw          $v1, -0x6508($a0)
    MEM_W(-0X6508, ctx->r4) = ctx->r3;
    // 0x80418320: lui         $v1, 0x3BC5
    ctx->r3 = S32(0X3BC5 << 16);
    // 0x80418324: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418328: ori         $v1, $v1, 0x8D33
    ctx->r3 = ctx->r3 | 0X8D33;
    // 0x8041832C: sw          $v1, -0x6504($a0)
    MEM_W(-0X6504, ctx->r4) = ctx->r3;
    // 0x80418330: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418334: lui         $a0, 0x20
    ctx->r4 = S32(0X20 << 16);
    // 0x80418338: ori         $a1, $a1, 0xEF14
    ctx->r5 = ctx->r5 | 0XEF14;
    // 0x8041833C: sw          $a0, -0x6518($v1)
    MEM_W(-0X6518, ctx->r3) = ctx->r4;
    // 0x80418340: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418344: addiu       $a3, $a1, 0x54
    ctx->r7 = ADD32(ctx->r5, 0X54);
    // 0x80418348: sw          $a1, -0x5424($v1)
    MEM_W(-0X5424, ctx->r3) = ctx->r5;
    // 0x8041834C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418350: addiu       $a2, $a1, 0x1EC
    ctx->r6 = ADD32(ctx->r5, 0X1EC);
    // 0x80418354: sw          $a3, -0x5444($v1)
    MEM_W(-0X5444, ctx->r3) = ctx->r7;
    // 0x80418358: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041835C: addiu       $a0, $zero, -0x7CF6
    ctx->r4 = ADD32(0, -0X7CF6);
    // 0x80418360: sw          $a2, -0x5404($v1)
    MEM_W(-0X5404, ctx->r3) = ctx->r6;
    // 0x80418364: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418368: sh          $a0, -0x6260($v1)
    MEM_H(-0X6260, ctx->r3) = ctx->r4;
    // 0x8041836C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418370: addiu       $a0, $zero, -0x7CF5
    ctx->r4 = ADD32(0, -0X7CF5);
    // 0x80418374: sh          $a0, -0x6248($v1)
    MEM_H(-0X6248, ctx->r3) = ctx->r4;
    // 0x80418378: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041837C: addiu       $a0, $zero, -0x7CF9
    ctx->r4 = ADD32(0, -0X7CF9);
    // 0x80418380: sh          $a0, -0x6230($v1)
    MEM_H(-0X6230, ctx->r3) = ctx->r4;
L_80418384:
    // 0x80418384: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418388: lhu         $a1, -0x606C($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X606C);
    // 0x8041838C: addiu       $v1, $zero, 0x4650
    ctx->r3 = ADD32(0, 0X4650);
    // 0x80418390: beq         $a1, $v1, L_8041844C
    if (ctx->r5 == ctx->r3) {
        // 0x80418394: lui         $a1, 0x8040
        ctx->r5 = S32(0X8040 << 16);
            goto L_8041844C;
    }
    // 0x80418394: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x80418398: sh          $v1, -0x606C($a0)
    MEM_H(-0X606C, ctx->r4) = ctx->r3;
    // 0x8041839C: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804183A0: sh          $v1, -0x606A($a0)
    MEM_H(-0X606A, ctx->r4) = ctx->r3;
    // 0x804183A4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804183A8: addiu       $a0, $zero, 0x410
    ctx->r4 = ADD32(0, 0X410);
    // 0x804183AC: sh          $a0, -0x6062($v1)
    MEM_H(-0X6062, ctx->r3) = ctx->r4;
    // 0x804183B0: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804183B4: addiu       $a0, $zero, 0x1F4
    ctx->r4 = ADD32(0, 0X1F4);
    // 0x804183B8: sh          $a0, -0x6064($v1)
    MEM_H(-0X6064, ctx->r3) = ctx->r4;
    // 0x804183BC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804183C0: addiu       $a0, $zero, 0x82
    ctx->r4 = ADD32(0, 0X82);
    // 0x804183C4: sh          $a0, -0x6046($v1)
    MEM_H(-0X6046, ctx->r3) = ctx->r4;
    // 0x804183C8: lui         $v1, 0x7
    ctx->r3 = S32(0X7 << 16);
    // 0x804183CC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804183D0: ori         $v1, $v1, 0xEF40
    ctx->r3 = ctx->r3 | 0XEF40;
    // 0x804183D4: sw          $v1, -0x6060($a0)
    MEM_W(-0X6060, ctx->r4) = ctx->r3;
    // 0x804183D8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804183DC: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x804183E0: sw          $a0, -0x605C($v1)
    MEM_W(-0X605C, ctx->r3) = ctx->r4;
    // 0x804183E4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804183E8: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x804183EC: sw          $a0, -0x603C($v1)
    MEM_W(-0X603C, ctx->r3) = ctx->r4;
    // 0x804183F0: lui         $v1, 0xC3A9
    ctx->r3 = S32(0XC3A9 << 16);
    // 0x804183F4: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804183F8: ori         $v1, $v1, 0xEC04
    ctx->r3 = ctx->r3 | 0XEC04;
    // 0x804183FC: sw          $v1, -0x64F4($a0)
    MEM_W(-0X64F4, ctx->r4) = ctx->r3;
    // 0x80418400: lui         $v1, 0xC340
    ctx->r3 = S32(0XC340 << 16);
    // 0x80418404: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418408: addiu       $v1, $v1, 0x3C6D
    ctx->r3 = ADD32(ctx->r3, 0X3C6D);
    // 0x8041840C: sw          $v1, -0x64F0($a0)
    MEM_W(-0X64F0, ctx->r4) = ctx->r3;
    // 0x80418410: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418414: lui         $a0, 0x3FE0
    ctx->r4 = S32(0X3FE0 << 16);
    // 0x80418418: sw          $a0, -0x64EC($v1)
    MEM_W(-0X64EC, ctx->r3) = ctx->r4;
    // 0x8041841C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418420: lui         $a0, 0x2
    ctx->r4 = S32(0X2 << 16);
    // 0x80418424: ori         $a1, $a1, 0xEF1F
    ctx->r5 = ctx->r5 | 0XEF1F;
    // 0x80418428: sw          $a0, -0x6500($v1)
    MEM_W(-0X6500, ctx->r3) = ctx->r4;
    // 0x8041842C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418430: addiu       $a3, $a1, 0x51
    ctx->r7 = ADD32(ctx->r5, 0X51);
    // 0x80418434: sw          $a1, -0x5420($v1)
    MEM_W(-0X5420, ctx->r3) = ctx->r5;
    // 0x80418438: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041843C: addiu       $a2, $a1, 0x2E1
    ctx->r6 = ADD32(ctx->r5, 0X2E1);
    // 0x80418440: sw          $a3, -0x5440($v1)
    MEM_W(-0X5440, ctx->r3) = ctx->r7;
    // 0x80418444: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418448: sw          $a2, -0x5400($v1)
    MEM_W(-0X5400, ctx->r3) = ctx->r6;
L_8041844C:
    // 0x8041844C: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418450: lhu         $a1, -0x6034($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X6034);
    // 0x80418454: addiu       $v1, $zero, 0x5208
    ctx->r3 = ADD32(0, 0X5208);
    // 0x80418458: beq         $a1, $v1, L_80418508
    if (ctx->r5 == ctx->r3) {
        // 0x8041845C: lui         $a1, 0x8040
        ctx->r5 = S32(0X8040 << 16);
            goto L_80418508;
    }
    // 0x8041845C: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x80418460: sh          $v1, -0x6034($a0)
    MEM_H(-0X6034, ctx->r4) = ctx->r3;
    // 0x80418464: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418468: sh          $v1, -0x6032($a0)
    MEM_H(-0X6032, ctx->r4) = ctx->r3;
    // 0x8041846C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418470: addiu       $a0, $zero, 0x424
    ctx->r4 = ADD32(0, 0X424);
    // 0x80418474: sh          $a0, -0x602A($v1)
    MEM_H(-0X602A, ctx->r3) = ctx->r4;
    // 0x80418478: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041847C: addiu       $a0, $zero, 0x226
    ctx->r4 = ADD32(0, 0X226);
    // 0x80418480: sh          $a0, -0x602C($v1)
    MEM_H(-0X602C, ctx->r3) = ctx->r4;
    // 0x80418484: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418488: addiu       $a0, $zero, 0x87
    ctx->r4 = ADD32(0, 0X87);
    // 0x8041848C: sh          $a0, -0x600E($v1)
    MEM_H(-0X600E, ctx->r3) = ctx->r4;
    // 0x80418490: lui         $v1, 0x8
    ctx->r3 = S32(0X8 << 16);
    // 0x80418494: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418498: addiu       $v1, $v1, 0x1650
    ctx->r3 = ADD32(ctx->r3, 0X1650);
    // 0x8041849C: sw          $v1, -0x6028($a0)
    MEM_W(-0X6028, ctx->r4) = ctx->r3;
    // 0x804184A0: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184A4: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x804184A8: sw          $a0, -0x6024($v1)
    MEM_W(-0X6024, ctx->r3) = ctx->r4;
    // 0x804184AC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184B0: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x804184B4: sw          $a0, -0x6014($v1)
    MEM_W(-0X6014, ctx->r3) = ctx->r4;
    // 0x804184B8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184BC: lui         $a0, 0xC180
    ctx->r4 = S32(0XC180 << 16);
    // 0x804184C0: sw          $zero, -0x64DC($v1)
    MEM_W(-0X64DC, ctx->r3) = 0;
    // 0x804184C4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184C8: sw          $a0, -0x64D8($v1)
    MEM_W(-0X64D8, ctx->r3) = ctx->r4;
    // 0x804184CC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184D0: lui         $a0, 0x3FC0
    ctx->r4 = S32(0X3FC0 << 16);
    // 0x804184D4: sw          $a0, -0x64D4($v1)
    MEM_W(-0X64D4, ctx->r3) = ctx->r4;
    // 0x804184D8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184DC: addiu       $a0, $zero, 0x3
    ctx->r4 = ADD32(0, 0X3);
    // 0x804184E0: ori         $a1, $a1, 0xEF29
    ctx->r5 = ctx->r5 | 0XEF29;
    // 0x804184E4: sw          $a0, -0x64E8($v1)
    MEM_W(-0X64E8, ctx->r3) = ctx->r4;
    // 0x804184E8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184EC: addiu       $a3, $a1, 0x4F
    ctx->r7 = ADD32(ctx->r5, 0X4F);
    // 0x804184F0: sw          $a1, -0x541C($v1)
    MEM_W(-0X541C, ctx->r3) = ctx->r5;
    // 0x804184F4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804184F8: addiu       $a2, $a1, 0x3D7
    ctx->r6 = ADD32(ctx->r5, 0X3D7);
    // 0x804184FC: sw          $a3, -0x543C($v1)
    MEM_W(-0X543C, ctx->r3) = ctx->r7;
    // 0x80418500: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418504: sw          $a2, -0x53FC($v1)
    MEM_W(-0X53FC, ctx->r3) = ctx->r6;
L_80418508:
    // 0x80418508: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x8041850C: lhu         $a1, -0x5FFC($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X5FFC);
    // 0x80418510: addiu       $v1, $zero, 0x5DC0
    ctx->r3 = ADD32(0, 0X5DC0);
    // 0x80418514: beq         $a1, $v1, L_804185CC
    if (ctx->r5 == ctx->r3) {
        // 0x80418518: lui         $a2, 0x8
        ctx->r6 = S32(0X8 << 16);
            goto L_804185CC;
    }
    // 0x80418518: lui         $a2, 0x8
    ctx->r6 = S32(0X8 << 16);
    // 0x8041851C: sh          $v1, -0x5FFC($a0)
    MEM_H(-0X5FFC, ctx->r4) = ctx->r3;
    // 0x80418520: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418524: sh          $v1, -0x5FFA($a0)
    MEM_H(-0X5FFA, ctx->r4) = ctx->r3;
    // 0x80418528: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041852C: addiu       $a0, $zero, 0x438
    ctx->r4 = ADD32(0, 0X438);
    // 0x80418530: sh          $a0, -0x5FF2($v1)
    MEM_H(-0X5FF2, ctx->r3) = ctx->r4;
    // 0x80418534: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418538: addiu       $a0, $zero, 0x258
    ctx->r4 = ADD32(0, 0X258);
    // 0x8041853C: sh          $a0, -0x5FF4($v1)
    MEM_H(-0X5FF4, ctx->r3) = ctx->r4;
    // 0x80418540: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418544: addiu       $a0, $zero, 0x8C
    ctx->r4 = ADD32(0, 0X8C);
    // 0x80418548: sh          $a0, -0x5FD6($v1)
    MEM_H(-0X5FD6, ctx->r3) = ctx->r4;
    // 0x8041854C: addiu       $t1, $a2, 0x3D60
    ctx->r9 = ADD32(ctx->r6, 0X3D60);
    // 0x80418550: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418554: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x80418558: sw          $t1, -0x5FF0($v1)
    MEM_W(-0X5FF0, ctx->r3) = ctx->r9;
    // 0x8041855C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418560: sw          $a0, -0x5FEC($v1)
    MEM_W(-0X5FEC, ctx->r3) = ctx->r4;
    // 0x80418564: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418568: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x8041856C: sw          $a0, -0x5FCC($v1)
    MEM_W(-0X5FCC, ctx->r3) = ctx->r4;
    // 0x80418570: lui         $v1, 0xC440
    ctx->r3 = S32(0XC440 << 16);
    // 0x80418574: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418578: addiu       $v1, $v1, 0x7A70
    ctx->r3 = ADD32(ctx->r3, 0X7A70);
    // 0x8041857C: sw          $v1, -0x64C4($a0)
    MEM_W(-0X64C4, ctx->r4) = ctx->r3;
    // 0x80418580: lui         $v1, 0xC3D6
    ctx->r3 = S32(0XC3D6 << 16);
    // 0x80418584: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418588: addiu       $v1, $v1, 0x30E3
    ctx->r3 = ADD32(ctx->r3, 0X30E3);
    // 0x8041858C: sw          $v1, -0x64C0($a0)
    MEM_W(-0X64C0, ctx->r4) = ctx->r3;
    // 0x80418590: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418594: lui         $a0, 0x3F40
    ctx->r4 = S32(0X3F40 << 16);
    // 0x80418598: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x8041859C: sw          $a0, -0x64BC($v1)
    MEM_W(-0X64BC, ctx->r3) = ctx->r4;
    // 0x804185A0: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185A4: ori         $a1, $a1, 0xEF32
    ctx->r5 = ctx->r5 | 0XEF32;
    // 0x804185A8: sw          $a2, -0x64D0($v1)
    MEM_W(-0X64D0, ctx->r3) = ctx->r6;
    // 0x804185AC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185B0: addiu       $t0, $a1, 0x4E
    ctx->r8 = ADD32(ctx->r5, 0X4E);
    // 0x804185B4: sw          $a1, -0x5418($v1)
    MEM_W(-0X5418, ctx->r3) = ctx->r5;
    // 0x804185B8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185BC: addiu       $a3, $a1, 0x4CE
    ctx->r7 = ADD32(ctx->r5, 0X4CE);
    // 0x804185C0: sw          $t0, -0x5438($v1)
    MEM_W(-0X5438, ctx->r3) = ctx->r8;
    // 0x804185C4: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185C8: sw          $a3, -0x53F8($v1)
    MEM_W(-0X53F8, ctx->r3) = ctx->r7;
L_804185CC:
    // 0x804185CC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804185D0: lhu         $a1, -0x5FC4($a0)
    ctx->r5 = MEM_HU(ctx->r4, -0X5FC4);
    // 0x804185D4: addiu       $v1, $zero, 0x6978
    ctx->r3 = ADD32(0, 0X6978);
    // 0x804185D8: beq         $a1, $v1, L_80418698
    if (ctx->r5 == ctx->r3) {
        // 0x804185DC: lui         $a1, 0x8040
        ctx->r5 = S32(0X8040 << 16);
            goto L_80418698;
    }
    // 0x804185DC: lui         $a1, 0x8040
    ctx->r5 = S32(0X8040 << 16);
    // 0x804185E0: sh          $v1, -0x5FC4($a0)
    MEM_H(-0X5FC4, ctx->r4) = ctx->r3;
    // 0x804185E4: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804185E8: sh          $v1, -0x5FC2($a0)
    MEM_H(-0X5FC2, ctx->r4) = ctx->r3;
    // 0x804185EC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185F0: addiu       $a0, $zero, 0x44C
    ctx->r4 = ADD32(0, 0X44C);
    // 0x804185F4: sh          $a0, -0x5FBA($v1)
    MEM_H(-0X5FBA, ctx->r3) = ctx->r4;
    // 0x804185F8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804185FC: addiu       $a0, $zero, 0x28A
    ctx->r4 = ADD32(0, 0X28A);
    // 0x80418600: sh          $a0, -0x5FBC($v1)
    MEM_H(-0X5FBC, ctx->r3) = ctx->r4;
    // 0x80418604: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418608: addiu       $a0, $zero, 0x80
    ctx->r4 = ADD32(0, 0X80);
    // 0x8041860C: sh          $a0, -0x5F9E($v1)
    MEM_H(-0X5F9E, ctx->r3) = ctx->r4;
    // 0x80418610: lui         $v1, 0x8
    ctx->r3 = S32(0X8 << 16);
    // 0x80418614: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418618: addiu       $v1, $v1, 0x1650
    ctx->r3 = ADD32(ctx->r3, 0X1650);
    // 0x8041861C: sw          $v1, -0x5FB8($a0)
    MEM_W(-0X5FB8, ctx->r4) = ctx->r3;
    // 0x80418620: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418624: addiu       $a0, $zero, 0x61A8
    ctx->r4 = ADD32(0, 0X61A8);
    // 0x80418628: sw          $a0, -0x5FB4($v1)
    MEM_W(-0X5FB4, ctx->r3) = ctx->r4;
    // 0x8041862C: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418630: lui         $a0, 0x1A00
    ctx->r4 = S32(0X1A00 << 16);
    // 0x80418634: sw          $a0, -0x5F94($v1)
    MEM_W(-0X5F94, ctx->r3) = ctx->r4;
    // 0x80418638: lui         $v1, 0xC472
    ctx->r3 = S32(0XC472 << 16);
    // 0x8041863C: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418640: addiu       $v1, $v1, 0x5C31
    ctx->r3 = ADD32(ctx->r3, 0X5C31);
    // 0x80418644: sw          $v1, -0x64AC($a0)
    MEM_W(-0X64AC, ctx->r4) = ctx->r3;
    // 0x80418648: lui         $v1, 0x4234
    ctx->r3 = S32(0X4234 << 16);
    // 0x8041864C: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418650: ori         $v1, $v1, 0xE49F
    ctx->r3 = ctx->r3 | 0XE49F;
    // 0x80418654: sw          $v1, -0x64A8($a0)
    MEM_W(-0X64A8, ctx->r4) = ctx->r3;
    // 0x80418658: lui         $v1, 0x3FEA
    ctx->r3 = S32(0X3FEA << 16);
    // 0x8041865C: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80418660: addiu       $v1, $v1, 0x3752
    ctx->r3 = ADD32(ctx->r3, 0X3752);
    // 0x80418664: sw          $v1, -0x64A4($a0)
    MEM_W(-0X64A4, ctx->r4) = ctx->r3;
    // 0x80418668: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041866C: lui         $a0, 0x9
    ctx->r4 = S32(0X9 << 16);
    // 0x80418670: ori         $a1, $a1, 0xEF3C
    ctx->r5 = ctx->r5 | 0XEF3C;
    // 0x80418674: sw          $a0, -0x64B8($v1)
    MEM_W(-0X64B8, ctx->r3) = ctx->r4;
    // 0x80418678: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041867C: addiu       $a3, $a1, 0x4C
    ctx->r7 = ADD32(ctx->r5, 0X4C);
    // 0x80418680: sw          $a1, -0x5414($v1)
    MEM_W(-0X5414, ctx->r3) = ctx->r5;
    // 0x80418684: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418688: addiu       $a2, $a1, 0x5C4
    ctx->r6 = ADD32(ctx->r5, 0X5C4);
    // 0x8041868C: sw          $a3, -0x5434($v1)
    MEM_W(-0X5434, ctx->r3) = ctx->r7;
    // 0x80418690: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418694: sw          $a2, -0x53F4($v1)
    MEM_W(-0X53F4, ctx->r3) = ctx->r6;
L_80418698:
    // 0x80418698: lui         $a1, 0x803B
    ctx->r5 = S32(0X803B << 16);
    // 0x8041869C: lhu         $v1, -0x5F8C($a1)
    ctx->r3 = MEM_HU(ctx->r5, -0X5F8C);
    // 0x804186A0: addiu       $a0, $zero, 0x7530
    ctx->r4 = ADD32(0, 0X7530);
    // 0x804186A4: beq         $v1, $a0, L_80418728
    if (ctx->r3 == ctx->r4) {
        // 0x804186A8: lui         $v1, 0x8040
        ctx->r3 = S32(0X8040 << 16);
            goto L_80418728;
    }
    // 0x804186A8: lui         $v1, 0x8040
    ctx->r3 = S32(0X8040 << 16);
    // 0x804186AC: sh          $a0, -0x5F8C($a1)
    MEM_H(-0X5F8C, ctx->r5) = ctx->r4;
    // 0x804186B0: lui         $a1, 0x803B
    ctx->r5 = S32(0X803B << 16);
    // 0x804186B4: sh          $a0, -0x5F8A($a1)
    MEM_H(-0X5F8A, ctx->r5) = ctx->r4;
    // 0x804186B8: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804186BC: addiu       $a1, $zero, 0x460
    ctx->r5 = ADD32(0, 0X460);
    // 0x804186C0: sh          $a1, -0x5F82($a0)
    MEM_H(-0X5F82, ctx->r4) = ctx->r5;
    // 0x804186C4: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804186C8: addiu       $a1, $zero, 0x2BC
    ctx->r5 = ADD32(0, 0X2BC);
    // 0x804186CC: sh          $a1, -0x5F84($a0)
    MEM_H(-0X5F84, ctx->r4) = ctx->r5;
    // 0x804186D0: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804186D4: addiu       $a1, $zero, 0x96
    ctx->r5 = ADD32(0, 0X96);
    // 0x804186D8: sh          $a1, -0x5F66($a0)
    MEM_H(-0X5F66, ctx->r4) = ctx->r5;
    // 0x804186DC: lui         $a0, 0x8
    ctx->r4 = S32(0X8 << 16);
    // 0x804186E0: lui         $a1, 0x803B
    ctx->r5 = S32(0X803B << 16);
    // 0x804186E4: addiu       $a0, $a0, 0x1650
    ctx->r4 = ADD32(ctx->r4, 0X1650);
    // 0x804186E8: sw          $a0, -0x5F80($a1)
    MEM_W(-0X5F80, ctx->r5) = ctx->r4;
    // 0x804186EC: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804186F0: addiu       $a1, $zero, 0x61A8
    ctx->r5 = ADD32(0, 0X61A8);
    // 0x804186F4: sw          $a1, -0x5F7C($a0)
    MEM_W(-0X5F7C, ctx->r4) = ctx->r5;
    // 0x804186F8: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804186FC: lui         $a1, 0x1A00
    ctx->r5 = S32(0X1A00 << 16);
    // 0x80418700: ori         $v1, $v1, 0xEF49
    ctx->r3 = ctx->r3 | 0XEF49;
    // 0x80418704: sw          $a1, -0x5F5C($a0)
    MEM_W(-0X5F5C, ctx->r4) = ctx->r5;
    // 0x80418708: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x8041870C: addiu       $a3, $v1, 0x48
    ctx->r7 = ADD32(ctx->r3, 0X48);
    // 0x80418710: addiu       $a2, $v1, 0x6B7
    ctx->r6 = ADD32(ctx->r3, 0X6B7);
    // 0x80418714: sw          $v1, -0x5410($a0)
    MEM_W(-0X5410, ctx->r4) = ctx->r3;
    // 0x80418718: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x8041871C: sw          $a3, -0x5430($v1)
    MEM_W(-0X5430, ctx->r3) = ctx->r7;
    // 0x80418720: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x80418724: sw          $a2, -0x53F0($v1)
    MEM_W(-0X53F0, ctx->r3) = ctx->r6;
L_80418728:
    // 0x80418728: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041872C: lbu         $a0, 0x69F3($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X69F3);
    // 0x80418730: addiu       $v1, $zero, 0x7F
    ctx->r3 = ADD32(0, 0X7F);
    // 0x80418734: bne         $a0, $v1, L_804157E8
    if (ctx->r4 != ctx->r3) {
        // 0x80418738: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_804157E8;
    }
    // 0x80418738: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x8041873C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418740: lhu         $v1, 0x69F4($v1)
    ctx->r3 = MEM_HU(ctx->r3, 0X69F4);
    // 0x80418744: lui         $a3, 0x803B
    ctx->r7 = S32(0X803B << 16);
    // 0x80418748: srl         $a1, $v1, 1
    ctx->r5 = S32(U32(ctx->r3) >> 1);
    // 0x8041874C: lhu         $a0, -0x5F54($a3)
    ctx->r4 = MEM_HU(ctx->r7, -0X5F54);
    // 0x80418750: addiu       $a1, $a1, 0x7D00
    ctx->r5 = ADD32(ctx->r5, 0X7D00);
    // 0x80418754: beq         $a0, $a1, L_804157E8
    if (ctx->r4 == ctx->r5) {
        // 0x80418758: lui         $a0, 0x1062
        ctx->r4 = S32(0X1062 << 16);
            goto L_804157E8;
    }
    // 0x80418758: lui         $a0, 0x1062
    ctx->r4 = S32(0X1062 << 16);
    // 0x8041875C: addiu       $a0, $a0, 0x4DD3
    ctx->r4 = ADD32(ctx->r4, 0X4DD3);
    // 0x80418760: multu       $v1, $a0
    result = U64(U32(ctx->r3)) * U64(U32(ctx->r4)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80418764: lui         $t0, 0xF000
    ctx->r8 = S32(0XF000 << 16);
    // 0x80418768: ori         $t0, $t0, 0xF001
    ctx->r8 = ctx->r8 | 0XF001;
    // 0x8041876C: andi        $a1, $a1, 0xFFFF
    ctx->r5 = ctx->r5 & 0XFFFF;
    // 0x80418770: sh          $a1, -0x5F54($a3)
    MEM_H(-0X5F54, ctx->r7) = ctx->r5;
    // 0x80418774: mfhi        $a0
    ctx->r4 = hi;
    // 0x80418778: lui         $a3, 0x803B
    ctx->r7 = S32(0X803B << 16);
    // 0x8041877C: sh          $a1, -0x5F52($a3)
    MEM_H(-0X5F52, ctx->r7) = ctx->r5;
    // 0x80418780: multu       $v1, $t0
    result = U64(U32(ctx->r3)) * U64(U32(ctx->r8)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x80418784: srl         $t0, $a0, 5
    ctx->r8 = S32(U32(ctx->r4) >> 5);
    // 0x80418788: lui         $a1, 0x803B
    ctx->r5 = S32(0X803B << 16);
    // 0x8041878C: srl         $a0, $a0, 4
    ctx->r4 = S32(U32(ctx->r4) >> 4);
    // 0x80418790: addiu       $t0, $t0, 0x3E7
    ctx->r8 = ADD32(ctx->r8, 0X3E7);
    // 0x80418794: mfhi        $v1
    ctx->r3 = hi;
    // 0x80418798: addiu       $a0, $a0, 0x5DC
    ctx->r4 = ADD32(ctx->r4, 0X5DC);
    // 0x8041879C: srl         $v1, $v1, 12
    ctx->r3 = S32(U32(ctx->r3) >> 12);
    // 0x804187A0: sh          $t0, -0x5F4C($a1)
    MEM_H(-0X5F4C, ctx->r5) = ctx->r8;
    // 0x804187A4: lui         $a1, 0x803B
    ctx->r5 = S32(0X803B << 16);
    // 0x804187A8: addiu       $v1, $v1, 0x96
    ctx->r3 = ADD32(ctx->r3, 0X96);
    // 0x804187AC: lui         $a2, 0x8040
    ctx->r6 = S32(0X8040 << 16);
    // 0x804187B0: sh          $a0, -0x5F4A($a1)
    MEM_H(-0X5F4A, ctx->r5) = ctx->r4;
    // 0x804187B4: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x804187B8: ori         $a2, $a2, 0xEF52
    ctx->r6 = ctx->r6 | 0XEF52;
    // 0x804187BC: sh          $v1, -0x5F2E($a0)
    MEM_H(-0X5F2E, ctx->r4) = ctx->r3;
    // 0x804187C0: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804187C4: addiu       $t2, $a2, 0x46
    ctx->r10 = ADD32(ctx->r6, 0X46);
    // 0x804187C8: sw          $a2, -0x540C($v1)
    MEM_W(-0X540C, ctx->r3) = ctx->r6;
    // 0x804187CC: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804187D0: addiu       $t1, $a2, 0x8AE
    ctx->r9 = ADD32(ctx->r6, 0X8AE);
    // 0x804187D4: sw          $t2, -0x542C($v1)
    MEM_W(-0X542C, ctx->r3) = ctx->r10;
    // 0x804187D8: lui         $v1, 0x803B
    ctx->r3 = S32(0X803B << 16);
    // 0x804187DC: b           L_804157E4
    // 0x804187E0: sw          $t1, -0x53EC($v1)
    MEM_W(-0X53EC, ctx->r3) = ctx->r9;
        goto L_804157E4;
    // 0x804187E0: sw          $t1, -0x53EC($v1)
    MEM_W(-0X53EC, ctx->r3) = ctx->r9;
L_804187E4:
    // 0x804187E4: lbu         $a0, 0x69E5($v1)
    ctx->r4 = MEM_BU(ctx->r3, 0X69E5);
    // 0x804187E8: andi        $a1, $a0, 0x1
    ctx->r5 = ctx->r4 & 0X1;
    // 0x804187EC: beql        $a1, $zero, L_80418830
    if (ctx->r5 == 0) {
        // 0x804187F0: lbu         $v1, 0x69E5($v1)
        ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
            goto L_80418830;
    }
    goto skip_50;
    // 0x804187F0: lbu         $v1, 0x69E5($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
    skip_50:
    // 0x804187F4: lui         $a2, 0x8017
    ctx->r6 = S32(0X8017 << 16);
    // 0x804187F8: lhu         $t0, -0x54D6($a2)
    ctx->r8 = MEM_HU(ctx->r6, -0X54D6);
    // 0x804187FC: addiu       $a3, $zero, 0x5788
    ctx->r7 = ADD32(0, 0X5788);
    // 0x80418800: beq         $t0, $a3, L_80419E9C
    if (ctx->r8 == ctx->r7) {
        // 0x80418804: andi        $a1, $a0, 0x2
        ctx->r5 = ctx->r4 & 0X2;
            goto L_80419E9C;
    }
    // 0x80418804: andi        $a1, $a0, 0x2
    ctx->r5 = ctx->r4 & 0X2;
L_80418808:
    // 0x80418808: bne         $a1, $zero, L_8041882C
    if (ctx->r5 != 0) {
        // 0x8041880C: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041882C;
    }
    // 0x8041880C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80418810: lw          $a2, 0x67C($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X67C);
    // 0x80418814: lui         $a1, 0xA0C0
    ctx->r5 = S32(0XA0C0 << 16);
    // 0x80418818: ori         $a1, $a1, 0x8118
    ctx->r5 = ctx->r5 | 0X8118;
    // 0x8041881C: bnel        $a2, $a1, L_80418830
    if (ctx->r6 != ctx->r5) {
        // 0x80418820: lbu         $v1, 0x69E5($v1)
        ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
            goto L_80418830;
    }
    goto skip_51;
    // 0x80418820: lbu         $v1, 0x69E5($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
    skip_51:
    // 0x80418824: ori         $a0, $a0, 0x2
    ctx->r4 = ctx->r4 | 0X2;
    // 0x80418828: sb          $a0, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r4;
L_8041882C:
    // 0x8041882C: lbu         $v1, 0x69E5($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
L_80418830:
    // 0x80418830: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    // 0x80418834: andi        $v1, $v1, 0x6
    ctx->r3 = ctx->r3 & 0X6;
    // 0x80418838: bne         $v1, $a0, L_804157B8
    if (ctx->r3 != ctx->r4) {
        // 0x8041883C: lui         $t1, 0x8008
        ctx->r9 = S32(0X8008 << 16);
            goto L_804157B8;
    }
    // 0x8041883C: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    // 0x80418840: lui         $a0, 0x8017
    ctx->r4 = S32(0X8017 << 16);
    // 0x80418844: lhu         $v1, -0x54D6($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X54D6);
    // 0x80418848: addiu       $a1, $zero, 0x5788
    ctx->r5 = ADD32(0, 0X5788);
    // 0x8041884C: beq         $v1, $a1, L_8041885C
    if (ctx->r3 == ctx->r5) {
        // 0x80418850: ori         $a1, $zero, 0xBC00
        ctx->r5 = 0 | 0XBC00;
            goto L_8041885C;
    }
    // 0x80418850: ori         $a1, $zero, 0xBC00
    ctx->r5 = 0 | 0XBC00;
    // 0x80418854: bne         $v1, $a1, L_804157BC
    if (ctx->r3 != ctx->r5) {
        // 0x80418858: addiu       $v1, $zero, 0x14
        ctx->r3 = ADD32(0, 0X14);
            goto L_804157BC;
    }
    // 0x80418858: addiu       $v1, $zero, 0x14
    ctx->r3 = ADD32(0, 0X14);
L_8041885C:
    // 0x8041885C: addiu       $v1, $zero, -0x4100
    ctx->r3 = ADD32(0, -0X4100);
    // 0x80418860: b           L_804157B4
    // 0x80418864: sh          $v1, -0x54D6($a0)
    MEM_H(-0X54D6, ctx->r4) = ctx->r3;
        goto L_804157B4;
    // 0x80418864: sh          $v1, -0x54D6($a0)
    MEM_H(-0X54D6, ctx->r4) = ctx->r3;
L_80418868:
    // 0x80418868: lui         $v1, 0x3
    ctx->r3 = S32(0X3 << 16);
    // 0x8041886C: bne         $a2, $v1, L_80415D94
    if (ctx->r6 != ctx->r3) {
        // 0x80418870: addiu       $v1, $zero, 0x10
        ctx->r3 = ADD32(0, 0X10);
            goto L_80415D94;
    }
    // 0x80418870: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
    // 0x80418874: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x80418878: sb          $v1, 0x69F8($a1)
    MEM_B(0X69F8, ctx->r5) = ctx->r3;
    // 0x8041887C: lui         $v1, 0x40
    ctx->r3 = S32(0X40 << 16);
    // 0x80418880: addiu       $v1, $v1, 0x4
    ctx->r3 = ADD32(ctx->r3, 0X4);
    // 0x80418884: lui         $a1, 0x800C
    ctx->r5 = S32(0X800C << 16);
    // 0x80418888: sw          $v1, 0x2EA8($a1)
    MEM_W(0X2EA8, ctx->r5) = ctx->r3;
    // 0x8041888C: lui         $v1, 0x1
    ctx->r3 = S32(0X1 << 16);
    // 0x80418890: b           L_80415D90
    // 0x80418894: sw          $v1, -0x4428($a0)
    MEM_W(-0X4428, ctx->r4) = ctx->r3;
        goto L_80415D90;
    // 0x80418894: sw          $v1, -0x4428($a0)
    MEM_W(-0X4428, ctx->r4) = ctx->r3;
L_80418898:
    // 0x80418898: lbu         $v1, 0x69E5($a0)
    ctx->r3 = MEM_BU(ctx->r4, 0X69E5);
    // 0x8041889C: andi        $a1, $v1, 0x8
    ctx->r5 = ctx->r3 & 0X8;
    // 0x804188A0: beq         $a1, $zero, L_80416090
    if (ctx->r5 == 0) {
        // 0x804188A4: lui         $t1, 0x8008
        ctx->r9 = S32(0X8008 << 16);
            goto L_80416090;
    }
    // 0x804188A4: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    // 0x804188A8: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x804188AC: lui         $a3, 0x8016
    ctx->r7 = S32(0X8016 << 16);
    // 0x804188B0: addiu       $t0, $zero, -0x3E00
    ctx->r8 = ADD32(0, -0X3E00);
    // 0x804188B4: lw          $a2, 0x67C($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X67C);
    // 0x804188B8: lui         $a1, 0xA0C0
    ctx->r5 = S32(0XA0C0 << 16);
    // 0x804188BC: sh          $t0, 0x433E($a3)
    MEM_H(0X433E, ctx->r7) = ctx->r8;
    // 0x804188C0: ori         $a1, $a1, 0x8113
    ctx->r5 = ctx->r5 | 0X8113;
    // 0x804188C4: lui         $a3, 0x8016
    ctx->r7 = S32(0X8016 << 16);
    // 0x804188C8: addiu       $t0, $zero, -0x3F00
    ctx->r8 = ADD32(0, -0X3F00);
    // 0x804188CC: bne         $a2, $a1, L_80416090
    if (ctx->r6 != ctx->r5) {
        // 0x804188D0: sh          $t0, 0x436A($a3)
        MEM_H(0X436A, ctx->r7) = ctx->r8;
            goto L_80416090;
    }
    // 0x804188D0: sh          $t0, 0x436A($a3)
    MEM_H(0X436A, ctx->r7) = ctx->r8;
    // 0x804188D4: ori         $v1, $v1, 0x10
    ctx->r3 = ctx->r3 | 0X10;
    // 0x804188D8: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    // 0x804188DC: lbu         $a3, 0x69F0($t1)
    ctx->r7 = MEM_BU(ctx->r9, 0X69F0);
    // 0x804188E0: b           L_80416094
    // 0x804188E4: sb          $v1, 0x69E5($a0)
    MEM_B(0X69E5, ctx->r4) = ctx->r3;
        goto L_80416094;
    // 0x804188E4: sb          $v1, 0x69E5($a0)
    MEM_B(0X69E5, ctx->r4) = ctx->r3;
L_804188E8:
    // 0x804188E8: xori        $t3, $v1, 0x9
    ctx->r11 = ctx->r3 ^ 0X9;
L_804188EC:
    // 0x804188EC: xori        $t9, $v1, 0xA
    ctx->r25 = ctx->r3 ^ 0XA;
    // 0x804188F0: sltiu       $s1, $t3, 0x1
    ctx->r17 = ctx->r11 < 0X1 ? 1 : 0;
    // 0x804188F4: sltiu       $t9, $t9, 0x1
    ctx->r25 = ctx->r25 < 0X1 ? 1 : 0;
    // 0x804188F8: or          $t0, $zero, $zero
    ctx->r8 = 0 | 0;
    // 0x804188FC: or          $t5, $zero, $zero
    ctx->r13 = 0 | 0;
    // 0x80418900: or          $t6, $zero, $zero
    ctx->r14 = 0 | 0;
    // 0x80418904: or          $t8, $zero, $zero
    ctx->r24 = 0 | 0;
    // 0x80418908: b           L_80416100
    // 0x8041890C: or          $t7, $zero, $zero
    ctx->r15 = 0 | 0;
        goto L_80416100;
    // 0x8041890C: or          $t7, $zero, $zero
    ctx->r15 = 0 | 0;
L_80418910:
    // 0x80418910: xori        $t0, $v1, 0xC
    ctx->r8 = ctx->r3 ^ 0XC;
    // 0x80418914: addiu       $a0, $v1, -0x9
    ctx->r4 = ADD32(ctx->r3, -0X9);
    // 0x80418918: sltiu       $t0, $t0, 0x1
    ctx->r8 = ctx->r8 < 0X1 ? 1 : 0;
    // 0x8041891C: sltiu       $a0, $a0, 0x2
    ctx->r4 = ctx->r4 < 0X2 ? 1 : 0;
    // 0x80418920: or          $a0, $t0, $a0
    ctx->r4 = ctx->r8 | ctx->r4;
    // 0x80418924: xori        $t7, $v1, 0x2
    ctx->r15 = ctx->r3 ^ 0X2;
    // 0x80418928: xori        $t8, $v1, 0x3
    ctx->r24 = ctx->r3 ^ 0X3;
    // 0x8041892C: xori        $t6, $v1, 0x5
    ctx->r14 = ctx->r3 ^ 0X5;
    // 0x80418930: xori        $t5, $v1, 0x7
    ctx->r13 = ctx->r3 ^ 0X7;
    // 0x80418934: xori        $t3, $v1, 0x9
    ctx->r11 = ctx->r3 ^ 0X9;
    // 0x80418938: xori        $t9, $v1, 0xA
    ctx->r25 = ctx->r3 ^ 0XA;
    // 0x8041893C: or          $a1, $a1, $a0
    ctx->r5 = ctx->r5 | ctx->r4;
    // 0x80418940: addiu       $a2, $zero, 0xE
    ctx->r6 = ADD32(0, 0XE);
    // 0x80418944: sltiu       $t7, $t7, 0x1
    ctx->r15 = ctx->r15 < 0X1 ? 1 : 0;
    // 0x80418948: sltiu       $t8, $t8, 0x1
    ctx->r24 = ctx->r24 < 0X1 ? 1 : 0;
    // 0x8041894C: sltiu       $t6, $t6, 0x1
    ctx->r14 = ctx->r14 < 0X1 ? 1 : 0;
    // 0x80418950: sltiu       $t5, $t5, 0x1
    ctx->r13 = ctx->r13 < 0X1 ? 1 : 0;
    // 0x80418954: sltiu       $s1, $t3, 0x1
    ctx->r17 = ctx->r11 < 0X1 ? 1 : 0;
    // 0x80418958: sltiu       $t9, $t9, 0x1
    ctx->r25 = ctx->r25 < 0X1 ? 1 : 0;
    // 0x8041895C: bne         $v1, $a2, L_804160EC
    if (ctx->r3 != ctx->r6) {
        // 0x80418960: or          $ra, $a1, $zero
        ctx->r31 = ctx->r5 | 0;
            goto L_804160EC;
    }
    // 0x80418960: or          $ra, $a1, $zero
    ctx->r31 = ctx->r5 | 0;
    // 0x80418964: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418968: lw          $a2, 0x536C($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X536C);
    // 0x8041896C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80418970: beq         $a2, $a0, L_804181D4
    if (ctx->r6 == ctx->r4) {
        // 0x80418974: or          $a0, $a1, $t4
        ctx->r4 = ctx->r5 | ctx->r12;
            goto L_804181D4;
    }
    // 0x80418974: or          $a0, $a1, $t4
    ctx->r4 = ctx->r5 | ctx->r12;
    // 0x80418978: b           L_804181BC
    // 0x8041897C: xori        $a2, $v1, 0x13
    ctx->r6 = ctx->r3 ^ 0X13;
        goto L_804181BC;
    // 0x8041897C: xori        $a2, $v1, 0x13
    ctx->r6 = ctx->r3 ^ 0X13;
L_80418980:
    // 0x80418980: bne         $a1, $zero, L_804189E4
    if (ctx->r5 != 0) {
        // 0x80418984: lui         $v1, 0x800C
        ctx->r3 = S32(0X800C << 16);
            goto L_804189E4;
    }
    // 0x80418984: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80418988: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x8041898C: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80418990: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80418994: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80418998: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x8041899C: lhu         $t1, -0x4558($t6)
    ctx->r9 = MEM_HU(ctx->r14, -0X4558);
    // 0x804189A0: lhu         $t0, -0x4556($t5)
    ctx->r8 = MEM_HU(ctx->r13, -0X4556);
    // 0x804189A4: lhu         $a3, -0x4552($t4)
    ctx->r7 = MEM_HU(ctx->r12, -0X4552);
    // 0x804189A8: lhu         $a1, -0x4554($t3)
    ctx->r5 = MEM_HU(ctx->r11, -0X4554);
    // 0x804189AC: lw          $v1, -0x4570($t2)
    ctx->r3 = MEM_W(ctx->r10, -0X4570);
    // 0x804189B0: addiu       $t1, $t1, 0x10
    ctx->r9 = ADD32(ctx->r9, 0X10);
    // 0x804189B4: addiu       $t0, $t0, 0x10
    ctx->r8 = ADD32(ctx->r8, 0X10);
    // 0x804189B8: addiu       $a3, $a3, 0x10
    ctx->r7 = ADD32(ctx->r7, 0X10);
    // 0x804189BC: addiu       $a1, $a1, 0x10
    ctx->r5 = ADD32(ctx->r5, 0X10);
    // 0x804189C0: addiu       $v1, $v1, 0x3A98
    ctx->r3 = ADD32(ctx->r3, 0X3A98);
    // 0x804189C4: ori         $a0, $a0, 0x10
    ctx->r4 = ctx->r4 | 0X10;
    // 0x804189C8: sh          $t1, -0x4558($t6)
    MEM_H(-0X4558, ctx->r14) = ctx->r9;
    // 0x804189CC: sh          $t0, -0x4556($t5)
    MEM_H(-0X4556, ctx->r13) = ctx->r8;
    // 0x804189D0: sh          $a3, -0x4552($t4)
    MEM_H(-0X4552, ctx->r12) = ctx->r7;
    // 0x804189D4: sh          $a1, -0x4554($t3)
    MEM_H(-0X4554, ctx->r11) = ctx->r5;
    // 0x804189D8: sw          $v1, -0x4570($t2)
    MEM_W(-0X4570, ctx->r10) = ctx->r3;
    // 0x804189DC: sb          $a0, 0x69F2($a2)
    MEM_B(0X69F2, ctx->r6) = ctx->r4;
L_804189E0:
    // 0x804189E0: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
L_804189E4:
    // 0x804189E4: lhu         $a3, 0x1B28($v1)
    ctx->r7 = MEM_HU(ctx->r3, 0X1B28);
    // 0x804189E8: lui         $t0, 0x8014
    ctx->r8 = S32(0X8014 << 16);
    // 0x804189EC: addiu       $t1, $zero, -0x6A00
    ctx->r9 = ADD32(0, -0X6A00);
    // 0x804189F0: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x804189F4: bne         $a3, $a1, L_80415844
    if (ctx->r7 != ctx->r5) {
        // 0x804189F8: sh          $t1, -0x25E6($t0)
        MEM_H(-0X25E6, ctx->r8) = ctx->r9;
            goto L_80415844;
    }
    // 0x804189F8: sh          $t1, -0x25E6($t0)
    MEM_H(-0X25E6, ctx->r8) = ctx->r9;
    // 0x804189FC: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_80418A00:
    // 0x80418A00: sh          $zero, 0x1B28($v1)
    MEM_H(0X1B28, ctx->r3) = 0;
    // 0x80418A04: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80418A08: sh          $a1, 0x1B2A($v1)
    MEM_H(0X1B2A, ctx->r3) = ctx->r5;
    // 0x80418A0C: lui         $v1, 0x800C
    ctx->r3 = S32(0X800C << 16);
    // 0x80418A10: addiu       $a1, $zero, 0x2
    ctx->r5 = ADD32(0, 0X2);
    // 0x80418A14: b           L_80415810
    // 0x80418A18: sh          $a1, 0x1B2C($v1)
    MEM_H(0X1B2C, ctx->r3) = ctx->r5;
        goto L_80415810;
    // 0x80418A18: sh          $a1, 0x1B2C($v1)
    MEM_H(0X1B2C, ctx->r3) = ctx->r5;
L_80418A1C:
    // 0x80418A1C: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418A20: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418A24: bne         $t0, $zero, L_804196D4
    if (ctx->r8 != 0) {
        // 0x80418A28: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196D4;
    }
    // 0x80418A28: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418A2C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418A30: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418A34: b           L_80417BE8
    // 0x80418A38: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
        goto L_80417BE8;
    // 0x80418A38: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_80418A3C:
    // 0x80418A3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418A40: lbu         $a0, -0x2E64($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X2E64);
    // 0x80418A44: andi        $a0, $a0, 0x20
    ctx->r4 = ctx->r4 & 0X20;
    // 0x80418A48: beql        $a0, $zero, L_804196B4
    if (ctx->r4 == 0) {
        // 0x80418A4C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804196B4;
    }
    goto skip_52;
    // 0x80418A4C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_52:
    // 0x80418A50: bltz        $v1, L_80418A60
    if (SIGNED(ctx->r3) < 0) {
        // 0x80418A54: addiu       $a1, $zero, 0x6798
        ctx->r5 = ADD32(0, 0X6798);
            goto L_80418A60;
    }
    // 0x80418A54: addiu       $a1, $zero, 0x6798
    ctx->r5 = ADD32(0, 0X6798);
    // 0x80418A58: lui         $a0, 0x8017
    ctx->r4 = S32(0X8017 << 16);
    // 0x80418A5C: sh          $a1, -0x1FEA($a0)
    MEM_H(-0X1FEA, ctx->r4) = ctx->r5;
L_80418A60:
    // 0x80418A60: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x80418A64: lw          $t0, 0x67C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X67C);
    // 0x80418A68: lui         $a1, 0xA0C0
    ctx->r5 = S32(0XA0C0 << 16);
    // 0x80418A6C: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80418A70: ori         $a1, $a1, 0x8118
    ctx->r5 = ctx->r5 | 0X8118;
    // 0x80418A74: beq         $t0, $a1, L_80419BF4
    if (ctx->r8 == ctx->r5) {
        // 0x80418A78: lbu         $a0, 0x69F1($a2)
        ctx->r4 = MEM_BU(ctx->r6, 0X69F1);
            goto L_80419BF4;
    }
    // 0x80418A78: lbu         $a0, 0x69F1($a2)
    ctx->r4 = MEM_BU(ctx->r6, 0X69F1);
    // 0x80418A7C: b           L_804196BC
    // 0x80418A80: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
        goto L_804196BC;
    // 0x80418A80: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
L_80418A84:
    // 0x80418A84: lw          $t0, -0x455C($v1)
    ctx->r8 = MEM_W(ctx->r3, -0X455C);
    // 0x80418A88: lui         $v1, 0x9696
    ctx->r3 = S32(0X9696 << 16);
    // 0x80418A8C: ori         $v1, $v1, 0x9696
    ctx->r3 = ctx->r3 | 0X9696;
    // 0x80418A90: beql        $t0, $v1, L_804158C8
    if (ctx->r8 == ctx->r3) {
        // 0x80418A94: sw          $a1, -0x454C($a0)
        MEM_W(-0X454C, ctx->r4) = ctx->r5;
            goto L_804158C8;
    }
    goto skip_53;
    // 0x80418A94: sw          $a1, -0x454C($a0)
    MEM_W(-0X454C, ctx->r4) = ctx->r5;
    skip_53:
    // 0x80418A98: b           L_804158CC
    // 0x80418A9C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
        goto L_804158CC;
    // 0x80418A9C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80418AA0:
    // 0x80418AA0: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80418AA4: sltiu       $v1, $v1, 0x1F
    ctx->r3 = ctx->r3 < 0X1F ? 1 : 0;
    // 0x80418AA8: bne         $v1, $zero, L_80415848
    if (ctx->r3 != 0) {
        // 0x80418AAC: lui         $a3, 0x8008
        ctx->r7 = S32(0X8008 << 16);
            goto L_80415848;
    }
    // 0x80418AAC: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x80418AB0: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80418AB4: lbu         $a0, 0x69F2($a2)
    ctx->r4 = MEM_BU(ctx->r6, 0X69F2);
    // 0x80418AB8: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80418ABC: lui         $v1, 0x80C0
    ctx->r3 = S32(0X80C0 << 16);
    // 0x80418AC0: ori         $v1, $v1, 0xE00E
    ctx->r3 = ctx->r3 | 0XE00E;
    // 0x80418AC4: beq         $a3, $v1, L_80419B84
    if (ctx->r7 == ctx->r3) {
        // 0x80418AC8: andi        $a1, $a0, 0x1
        ctx->r5 = ctx->r4 & 0X1;
            goto L_80419B84;
    }
    // 0x80418AC8: andi        $a1, $a0, 0x1
    ctx->r5 = ctx->r4 & 0X1;
    // 0x80418ACC: bne         $a1, $zero, L_80419BE4
    if (ctx->r5 != 0) {
        // 0x80418AD0: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419BE4;
    }
    // 0x80418AD0: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80418AD4: addiu       $a0, $zero, -0x2A00
    ctx->r4 = ADD32(0, -0X2A00);
    // 0x80418AD8: b           L_80415844
    // 0x80418ADC: sh          $a0, 0x6CE6($v1)
    MEM_H(0X6CE6, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x80418ADC: sh          $a0, 0x6CE6($v1)
    MEM_H(0X6CE6, ctx->r3) = ctx->r4;
L_80418AE0:
    // 0x80418AE0: lbu         $v1, -0x34AB($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X34AB);
    // 0x80418AE4: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80418AE8: bne         $a0, $zero, L_804195F4
    if (ctx->r4 != 0) {
        // 0x80418AEC: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804195F4;
    }
    // 0x80418AEC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418AF0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418AF4: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80418AF8: b           L_80415A98
    // 0x80418AFC: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_80415A98;
    // 0x80418AFC: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80418B00:
    // 0x80418B00: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418B04: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418B08: bne         $t0, $zero, L_804196DC
    if (ctx->r8 != 0) {
        // 0x80418B0C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196DC;
    }
    // 0x80418B0C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418B10: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418B14: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418B18: b           L_80415A70
    // 0x80418B1C: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_80415A70;
    // 0x80418B1C: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_80418B20:
    // 0x80418B20: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418B24: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418B28: bne         $t0, $zero, L_804196EC
    if (ctx->r8 != 0) {
        // 0x80418B2C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196EC;
    }
    // 0x80418B2C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418B30: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418B34: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418B38: b           L_80415A4C
    // 0x80418B3C: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415A4C;
    // 0x80418B3C: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_80418B40:
    // 0x80418B40: lbu         $v1, -0x35D3($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X35D3);
    // 0x80418B44: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80418B48: bne         $a0, $zero, L_804196E4
    if (ctx->r4 != 0) {
        // 0x80418B4C: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804196E4;
    }
    // 0x80418B4C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418B50: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418B54: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80418B58: b           L_804159E4
    // 0x80418B5C: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_804159E4;
    // 0x80418B5C: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80418B60:
    // 0x80418B60: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418B64: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418B68: bne         $t0, $zero, L_804196FC
    if (ctx->r8 != 0) {
        // 0x80418B6C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196FC;
    }
    // 0x80418B6C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418B70: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418B74: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418B78: b           L_80415A28
    // 0x80418B7C: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415A28;
    // 0x80418B7C: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_80418B80:
    // 0x80418B80: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418B84: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418B88: bne         $t0, $zero, L_804196F4
    if (ctx->r8 != 0) {
        // 0x80418B8C: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196F4;
    }
    // 0x80418B8C: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418B90: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418B94: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418B98: b           L_80415998
    // 0x80418B9C: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415998;
    // 0x80418B9C: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_80418BA0:
    // 0x80418BA0: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418BA4: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418BA8: bne         $t0, $zero, L_80419704
    if (ctx->r8 != 0) {
        // 0x80418BAC: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419704;
    }
    // 0x80418BAC: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418BB0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418BB4: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418BB8: b           L_804159BC
    // 0x80418BBC: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_804159BC;
    // 0x80418BBC: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_80418BC0:
    // 0x80418BC0: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418BC4: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418BC8: bne         $t0, $zero, L_80419514
    if (ctx->r8 != 0) {
        // 0x80418BCC: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419514;
    }
    // 0x80418BCC: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418BD0: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418BD4: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418BD8: b           L_80415974
    // 0x80418BDC: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415974;
    // 0x80418BDC: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_80418BE0:
    // 0x80418BE0: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x80418BE4: lui         $v1, 0xA0C0
    ctx->r3 = S32(0XA0C0 << 16);
    // 0x80418BE8: lw          $a1, 0x67C($a0)
    ctx->r5 = MEM_W(ctx->r4, 0X67C);
    // 0x80418BEC: ori         $v1, $v1, 0x8116
    ctx->r3 = ctx->r3 | 0X8116;
    // 0x80418BF0: beq         $a1, $v1, L_80419E1C
    if (ctx->r5 == ctx->r3) {
        // 0x80418BF4: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80419E1C;
    }
    // 0x80418BF4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80418BF8:
    // 0x80418BF8: lbu         $a3, 0x69E4($v1)
    ctx->r7 = MEM_BU(ctx->r3, 0X69E4);
    // 0x80418BFC: andi        $a2, $a3, 0x40
    ctx->r6 = ctx->r7 & 0X40;
    // 0x80418C00: beq         $a2, $zero, L_8041986C
    if (ctx->r6 == 0) {
        // 0x80418C04: lb          $a1, 0x69E4($v1)
        ctx->r5 = MEM_B(ctx->r3, 0X69E4);
            goto L_8041986C;
    }
    // 0x80418C04: lb          $a1, 0x69E4($v1)
    ctx->r5 = MEM_B(ctx->r3, 0X69E4);
    // 0x80418C08: bltz        $a1, L_8041989C
    if (SIGNED(ctx->r5) < 0) {
        // 0x80418C0C: lui         $t1, 0x8019
        ctx->r9 = S32(0X8019 << 16);
            goto L_8041989C;
    }
    // 0x80418C0C: lui         $t1, 0x8019
    ctx->r9 = S32(0X8019 << 16);
    // 0x80418C10: addiu       $t0, $zero, -0x73B4
    ctx->r8 = ADD32(0, -0X73B4);
    // 0x80418C14: andi        $a2, $a3, 0x8
    ctx->r6 = ctx->r7 & 0X8;
    // 0x80418C18: beq         $a2, $zero, L_80416070
    if (ctx->r6 == 0) {
        // 0x80418C1C: sh          $t0, -0x3250($t1)
        MEM_H(-0X3250, ctx->r9) = ctx->r8;
            goto L_80416070;
    }
    // 0x80418C1C: sh          $t0, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r8;
    // 0x80418C20: lw          $a2, 0x67C($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X67C);
    // 0x80418C24: lui         $a0, 0xA0C0
    ctx->r4 = S32(0XA0C0 << 16);
    // 0x80418C28: ori         $a0, $a0, 0x8118
    ctx->r4 = ctx->r4 | 0X8118;
    // 0x80418C2C: bne         $a2, $a0, L_804198BC
    if (ctx->r6 != ctx->r4) {
        // 0x80418C30: nop
    
            goto L_804198BC;
    }
    // 0x80418C30: nop

L_80418C34:
    // 0x80418C34: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80418C38: lw          $a0, -0x456C($t5)
    ctx->r4 = MEM_W(ctx->r13, -0X456C);
    // 0x80418C3C: lui         $t8, 0x8008
    ctx->r24 = S32(0X8008 << 16);
    // 0x80418C40: lhu         $t0, -0x4552($t8)
    ctx->r8 = MEM_HU(ctx->r24, -0X4552);
    // 0x80418C44: lui         $ra, 0x8008
    ctx->r31 = S32(0X8008 << 16);
    // 0x80418C48: lui         $t9, 0x8008
    ctx->r25 = S32(0X8008 << 16);
    // 0x80418C4C: lui         $t7, 0x8008
    ctx->r15 = S32(0X8008 << 16);
    // 0x80418C50: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80418C54: addiu       $t4, $zero, -0x80
    ctx->r12 = ADD32(0, -0X80);
    // 0x80418C58: lhu         $t3, -0x4558($ra)
    ctx->r11 = MEM_HU(ctx->r31, -0X4558);
    // 0x80418C5C: lhu         $t2, -0x4556($t9)
    ctx->r10 = MEM_HU(ctx->r25, -0X4556);
    // 0x80418C60: lhu         $a2, -0x4554($t7)
    ctx->r6 = MEM_HU(ctx->r15, -0X4554);
    // 0x80418C64: lw          $a1, -0x4570($t6)
    ctx->r5 = MEM_W(ctx->r14, -0X4570);
    // 0x80418C68: or          $t4, $a3, $t4
    ctx->r12 = ctx->r7 | ctx->r12;
    // 0x80418C6C: addiu       $a0, $a0, 0x7530
    ctx->r4 = ADD32(ctx->r4, 0X7530);
    // 0x80418C70: addiu       $a3, $t0, 0x32
    ctx->r7 = ADD32(ctx->r8, 0X32);
    // 0x80418C74: sb          $t4, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r12;
    // 0x80418C78: ori         $t0, $zero, 0xAFC8
    ctx->r8 = 0 | 0XAFC8;
    // 0x80418C7C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418C80: sw          $a0, -0x456C($t5)
    MEM_W(-0X456C, ctx->r13) = ctx->r4;
    // 0x80418C84: addiu       $a0, $zero, 0x1A
    ctx->r4 = ADD32(0, 0X1A);
    // 0x80418C88: addiu       $t3, $t3, 0x64
    ctx->r11 = ADD32(ctx->r11, 0X64);
    // 0x80418C8C: addiu       $t2, $t2, 0x3C
    ctx->r10 = ADD32(ctx->r10, 0X3C);
    // 0x80418C90: addiu       $a2, $a2, 0x32
    ctx->r6 = ADD32(ctx->r6, 0X32);
    // 0x80418C94: addu        $a1, $a1, $t0
    ctx->r5 = ADD32(ctx->r5, ctx->r8);
    // 0x80418C98: sb          $a0, -0x458D($v1)
    MEM_B(-0X458D, ctx->r3) = ctx->r4;
    // 0x80418C9C: addiu       $v1, $zero, -0x74BE
    ctx->r3 = ADD32(0, -0X74BE);
    // 0x80418CA0: sh          $t3, -0x4558($ra)
    MEM_H(-0X4558, ctx->r31) = ctx->r11;
    // 0x80418CA4: sh          $t2, -0x4556($t9)
    MEM_H(-0X4556, ctx->r25) = ctx->r10;
    // 0x80418CA8: sh          $a3, -0x4552($t8)
    MEM_H(-0X4552, ctx->r24) = ctx->r7;
    // 0x80418CAC: sh          $a2, -0x4554($t7)
    MEM_H(-0X4554, ctx->r15) = ctx->r6;
    // 0x80418CB0: sw          $a1, -0x4570($t6)
    MEM_W(-0X4570, ctx->r14) = ctx->r5;
    // 0x80418CB4: b           L_80416070
    // 0x80418CB8: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
        goto L_80416070;
    // 0x80418CB8: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
L_80418CBC:
    // 0x80418CBC: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418CC0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418CC4: bne         $t0, $zero, L_804195BC
    if (ctx->r8 != 0) {
        // 0x80418CC8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195BC;
    }
    // 0x80418CC8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418CCC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418CD0: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418CD4: b           L_80417B80
    // 0x80418CD8: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
        goto L_80417B80;
    // 0x80418CD8: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_80418CDC:
    // 0x80418CDC: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80418CE0: lbu         $a2, 0x6AF0($a2)
    ctx->r6 = MEM_BU(ctx->r6, 0X6AF0);
    // 0x80418CE4: addiu       $a3, $zero, 0xF
    ctx->r7 = ADD32(0, 0XF);
    // 0x80418CE8: andi        $a2, $a2, 0xF
    ctx->r6 = ctx->r6 & 0XF;
    // 0x80418CEC: beq         $a2, $a3, L_804197C4
    if (ctx->r6 == ctx->r7) {
        // 0x80418CF0: nop
    
            goto L_804197C4;
    }
    // 0x80418CF0: nop

    // 0x80418CF4: bne         $t0, $zero, L_804197D4
    if (ctx->r8 != 0) {
        // 0x80418CF8: lui         $a2, 0x8009
        ctx->r6 = S32(0X8009 << 16);
            goto L_804197D4;
    }
    // 0x80418CF8: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
L_80418CFC:
    // 0x80418CFC: lbu         $a1, 0x69E4($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
    // 0x80418D00: andi        $a1, $a1, 0x2
    ctx->r5 = ctx->r5 & 0X2;
    // 0x80418D04: bne         $a1, $zero, L_804197F8
    if (ctx->r5 != 0) {
        // 0x80418D08: lui         $a3, 0x8018
        ctx->r7 = S32(0X8018 << 16);
            goto L_804197F8;
    }
    // 0x80418D08: lui         $a3, 0x8018
    ctx->r7 = S32(0X8018 << 16);
L_80418D0C:
    // 0x80418D0C: lw          $a2, 0x4EE8($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X4EE8);
    // 0x80418D10: addiu       $a1, $zero, 0x13
    ctx->r5 = ADD32(0, 0X13);
    // 0x80418D14: beql        $a2, $a1, L_80417400
    if (ctx->r6 == ctx->r5) {
        // 0x80418D18: lbu         $a1, 0x69E4($v1)
        ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
            goto L_80417400;
    }
    goto skip_54;
    // 0x80418D18: lbu         $a1, 0x69E4($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
    skip_54:
    // 0x80418D1C: lw          $a0, 0x4EE8($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X4EE8);
    // 0x80418D20: addiu       $v1, $zero, 0x6
    ctx->r3 = ADD32(0, 0X6);
    // 0x80418D24: bne         $a0, $v1, L_804174C4
    if (ctx->r4 != ctx->r3) {
        // 0x80418D28: lui         $t1, 0x8008
        ctx->r9 = S32(0X8008 << 16);
            goto L_804174C4;
    }
    // 0x80418D28: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    // 0x80418D2C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80418D30:
    // 0x80418D30: lbu         $v1, -0x2E64($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X2E64);
    // 0x80418D34: andi        $v1, $v1, 0x10
    ctx->r3 = ctx->r3 & 0X10;
    // 0x80418D38: beq         $v1, $zero, L_80418D50
    if (ctx->r3 == 0) {
        // 0x80418D3C: lui         $a0, 0x8009
        ctx->r4 = S32(0X8009 << 16);
            goto L_80418D50;
    }
    // 0x80418D3C: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x80418D40: lui         $a0, 0x8018
    ctx->r4 = S32(0X8018 << 16);
    // 0x80418D44: addiu       $v1, $zero, -0x4000
    ctx->r3 = ADD32(0, -0X4000);
    // 0x80418D48: sh          $v1, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r3;
    // 0x80418D4C: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
L_80418D50:
    // 0x80418D50: lui         $v1, 0xA0C0
    ctx->r3 = S32(0XA0C0 << 16);
    // 0x80418D54: lw          $a1, 0x67C($a0)
    ctx->r5 = MEM_W(ctx->r4, 0X67C);
    // 0x80418D58: ori         $v1, $v1, 0x8113
    ctx->r3 = ctx->r3 | 0X8113;
    // 0x80418D5C: beq         $a1, $v1, L_80419E0C
    if (ctx->r5 == ctx->r3) {
        // 0x80418D60: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80419E0C;
    }
    // 0x80418D60: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_80418D64:
    // 0x80418D64: lbu         $a1, 0x69E5($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E5);
    // 0x80418D68: andi        $a2, $a1, 0x10
    ctx->r6 = ctx->r5 & 0X10;
    // 0x80418D6C: beql        $a2, $zero, L_80418D94
    if (ctx->r6 == 0) {
        // 0x80418D70: andi        $a2, $a1, 0x20
        ctx->r6 = ctx->r5 & 0X20;
            goto L_80418D94;
    }
    goto skip_55;
    // 0x80418D70: andi        $a2, $a1, 0x20
    ctx->r6 = ctx->r5 & 0X20;
    skip_55:
    // 0x80418D74: lw          $t0, 0x67C($a0)
    ctx->r8 = MEM_W(ctx->r4, 0X67C);
    // 0x80418D78: lui         $a3, 0xA0C0
    ctx->r7 = S32(0XA0C0 << 16);
    // 0x80418D7C: addiu       $a2, $zero, -0x3D00
    ctx->r6 = ADD32(0, -0X3D00);
    // 0x80418D80: lui         $a0, 0x8018
    ctx->r4 = S32(0X8018 << 16);
    // 0x80418D84: ori         $a3, $a3, 0x8101
    ctx->r7 = ctx->r7 | 0X8101;
    // 0x80418D88: sh          $a2, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r6;
    // 0x80418D8C: beq         $t0, $a3, L_80419CE4
    if (ctx->r8 == ctx->r7) {
        // 0x80418D90: andi        $a2, $a1, 0x20
        ctx->r6 = ctx->r5 & 0X20;
            goto L_80419CE4;
    }
    // 0x80418D90: andi        $a2, $a1, 0x20
    ctx->r6 = ctx->r5 & 0X20;
L_80418D94:
    // 0x80418D94: beql        $a2, $zero, L_804174C4
    if (ctx->r6 == 0) {
        // 0x80418D98: lui         $t1, 0x8008
        ctx->r9 = S32(0X8008 << 16);
            goto L_804174C4;
    }
    goto skip_56;
    // 0x80418D98: lui         $t1, 0x8008
    ctx->r9 = S32(0X8008 << 16);
    skip_56:
    // 0x80418D9C: lui         $a0, 0x8018
    ctx->r4 = S32(0X8018 << 16);
    // 0x80418DA0: addiu       $v1, $zero, -0x3C00
    ctx->r3 = ADD32(0, -0X3C00);
L_80418DA4:
    // 0x80418DA4: b           L_804174C0
    // 0x80418DA8: sh          $v1, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r3;
        goto L_804174C0;
    // 0x80418DA8: sh          $v1, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r3;
L_80418DAC:
    // 0x80418DAC: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x80418DB0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418DB4: bne         $t0, $zero, L_80419544
    if (ctx->r8 != 0) {
        // 0x80418DB8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419544;
    }
    // 0x80418DB8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418DBC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418DC0: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418DC4: b           L_80416CEC
    // 0x80418DC8: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_80416CEC;
    // 0x80418DC8: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_80418DCC:
    // 0x80418DCC: lbu         $v1, -0x300B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X300B);
    // 0x80418DD0: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80418DD4: bnel        $a0, $zero, L_80419974
    if (ctx->r4 != 0) {
        // 0x80418DD8: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419974;
    }
    goto skip_57;
    // 0x80418DD8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    skip_57:
    // 0x80418DDC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418DE0: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80418DE4: b           L_80415D68
    // 0x80418DE8: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_80415D68;
    // 0x80418DE8: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80418DEC:
    // 0x80418DEC: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80418DF0: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418DF4: bne         $t0, $zero, L_80419694
    if (ctx->r8 != 0) {
        // 0x80418DF8: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419694;
    }
    // 0x80418DF8: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418DFC: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418E00: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418E04: b           L_80416978
    // 0x80418E08: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
        goto L_80416978;
    // 0x80418E08: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_80418E0C:
    // 0x80418E0C: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80418E10: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418E14: bne         $t0, $zero, L_80419734
    if (ctx->r8 != 0) {
        // 0x80418E18: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419734;
    }
    // 0x80418E18: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418E1C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418E20: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418E24: b           L_80417960
    // 0x80418E28: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
        goto L_80417960;
    // 0x80418E28: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_80418E2C:
    // 0x80418E2C: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80418E30: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418E34: bne         $t0, $zero, L_804194DC
    if (ctx->r8 != 0) {
        // 0x80418E38: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194DC;
    }
    // 0x80418E38: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418E3C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418E40: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418E44: b           L_8041708C
    // 0x80418E48: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
        goto L_8041708C;
    // 0x80418E48: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_80418E4C:
    // 0x80418E4C: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418E50: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418E54: bne         $t0, $zero, L_8041985C
    if (ctx->r8 != 0) {
        // 0x80418E58: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041985C;
    }
    // 0x80418E58: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418E5C: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418E60: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418E64: b           L_80417B18
    // 0x80418E68: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
        goto L_80417B18;
    // 0x80418E68: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_80418E6C:
    // 0x80418E6C: lhu         $v1, -0x2016($a0)
    ctx->r3 = MEM_HU(ctx->r4, -0X2016);
    // 0x80418E70: addiu       $a1, $zero, 0x61D0
    ctx->r5 = ADD32(0, 0X61D0);
    // 0x80418E74: beq         $v1, $a1, L_80418E84
    if (ctx->r3 == ctx->r5) {
        // 0x80418E78: ori         $a1, $zero, 0xE100
        ctx->r5 = 0 | 0XE100;
            goto L_80418E84;
    }
    // 0x80418E78: ori         $a1, $zero, 0xE100
    ctx->r5 = 0 | 0XE100;
    // 0x80418E7C: bnel        $v1, $a1, L_80415DC8
    if (ctx->r3 != ctx->r5) {
        // 0x80418E80: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_80415DC8;
    }
    goto skip_58;
    // 0x80418E80: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_58:
L_80418E84:
    // 0x80418E84: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x80418E88: lw          $a3, 0x67C($v1)
    ctx->r7 = MEM_W(ctx->r3, 0X67C);
    // 0x80418E8C: lui         $a1, 0x80C0
    ctx->r5 = S32(0X80C0 << 16);
    // 0x80418E90: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80418E94: addiu       $a1, $a1, 0xE18
    ctx->r5 = ADD32(ctx->r5, 0XE18);
    // 0x80418E98: beq         $a3, $a1, L_80419A10
    if (ctx->r7 == ctx->r5) {
        // 0x80418E9C: lw          $v1, -0x456C($a2)
        ctx->r3 = MEM_W(ctx->r6, -0X456C);
            goto L_80419A10;
    }
    // 0x80418E9C: lw          $v1, -0x456C($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X456C);
L_80418EA0:
    // 0x80418EA0: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
L_80418EA4:
    // 0x80418EA4: bnel        $v1, $zero, L_804199A4
    if (ctx->r3 != 0) {
        // 0x80418EA8: addiu       $v1, $zero, 0x61D0
        ctx->r3 = ADD32(0, 0X61D0);
            goto L_804199A4;
    }
    goto skip_59;
    // 0x80418EA8: addiu       $v1, $zero, 0x61D0
    ctx->r3 = ADD32(0, 0X61D0);
    skip_59:
    // 0x80418EAC: addiu       $v1, $zero, -0x1F00
    ctx->r3 = ADD32(0, -0X1F00);
    // 0x80418EB0: b           L_80415DC4
    // 0x80418EB4: sh          $v1, -0x2016($a0)
    MEM_H(-0X2016, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80418EB4: sh          $v1, -0x2016($a0)
    MEM_H(-0X2016, ctx->r4) = ctx->r3;
L_80418EB8:
    // 0x80418EB8: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418EBC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418EC0: bne         $t0, $zero, L_804195FC
    if (ctx->r8 != 0) {
        // 0x80418EC4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195FC;
    }
    // 0x80418EC4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418EC8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418ECC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418ED0: b           L_804175E4
    // 0x80418ED4: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
        goto L_804175E4;
    // 0x80418ED4: sh          $t0, 0x7848($a0)
    MEM_H(0X7848, ctx->r4) = ctx->r8;
L_80418ED8:
    // 0x80418ED8: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418EDC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418EE0: bne         $t0, $zero, L_804194AC
    if (ctx->r8 != 0) {
        // 0x80418EE4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194AC;
    }
    // 0x80418EE4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418EE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418EEC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418EF0: b           L_804175BC
    // 0x80418EF4: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
        goto L_804175BC;
    // 0x80418EF4: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_80418EF8:
    // 0x80418EF8: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80418EFC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418F00: bne         $t0, $zero, L_80419574
    if (ctx->r8 != 0) {
        // 0x80418F04: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419574;
    }
    // 0x80418F04: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418F08: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418F0C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418F10: b           L_804170F4
    // 0x80418F14: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
        goto L_804170F4;
    // 0x80418F14: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_80418F18:
    // 0x80418F18: lbu         $v1, -0x325B($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X325B);
    // 0x80418F1C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80418F20: bne         $a0, $zero, L_8041993C
    if (ctx->r4 != 0) {
        // 0x80418F24: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041993C;
    }
    // 0x80418F24: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418F28: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80418F2C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80418F30: b           L_80415C00
    // 0x80418F34: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_80415C00;
    // 0x80418F34: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80418F38:
    // 0x80418F38: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x80418F3C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418F40: bne         $t0, $zero, L_8041965C
    if (ctx->r8 != 0) {
        // 0x80418F44: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041965C;
    }
    // 0x80418F44: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418F48: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418F4C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418F50: b           L_80416804
    // 0x80418F54: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
        goto L_80416804;
    // 0x80418F54: sh          $t0, 0x7638($a0)
    MEM_H(0X7638, ctx->r4) = ctx->r8;
L_80418F58:
    // 0x80418F58: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x80418F5C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418F60: bne         $t0, $zero, L_80419724
    if (ctx->r8 != 0) {
        // 0x80418F64: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419724;
    }
    // 0x80418F64: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418F68: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418F6C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418F70: b           L_80417328
    // 0x80418F74: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
        goto L_80417328;
    // 0x80418F74: sh          $t0, 0x78F8($a0)
    MEM_H(0X78F8, ctx->r4) = ctx->r8;
L_80418F78:
    // 0x80418F78: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80418F7C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418F80: bne         $t0, $zero, L_8041952C
    if (ctx->r8 != 0) {
        // 0x80418F84: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041952C;
    }
    // 0x80418F84: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418F88: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418F8C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418F90: b           L_8041715C
    // 0x80418F94: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
        goto L_8041715C;
    // 0x80418F94: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_80418F98:
    // 0x80418F98: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418F9C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418FA0: bne         $t0, $zero, L_804198F4
    if (ctx->r8 != 0) {
        // 0x80418FA4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804198F4;
    }
    // 0x80418FA4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418FA8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418FAC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418FB0: b           L_80417530
    // 0x80418FB4: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
        goto L_80417530;
    // 0x80418FB4: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_80418FB8:
    // 0x80418FB8: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x80418FBC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418FC0: bne         $t0, $zero, L_804195EC
    if (ctx->r8 != 0) {
        // 0x80418FC4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195EC;
    }
    // 0x80418FC4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418FC8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418FCC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418FD0: b           L_8041782C
    // 0x80418FD4: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
        goto L_8041782C;
    // 0x80418FD4: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_80418FD8:
    // 0x80418FD8: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x80418FDC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80418FE0: bne         $t0, $zero, L_804194BC
    if (ctx->r8 != 0) {
        // 0x80418FE4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194BC;
    }
    // 0x80418FE4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80418FE8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80418FEC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80418FF0: b           L_80417A6C
    // 0x80418FF4: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
        goto L_80417A6C;
    // 0x80418FF4: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_80418FF8:
    // 0x80418FF8: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x80418FFC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419000: bne         $t0, $zero, L_80419554
    if (ctx->r8 != 0) {
        // 0x80419004: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419554;
    }
    // 0x80419004: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419008: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041900C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419010: b           L_80416F74
    // 0x80419014: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
        goto L_80416F74;
    // 0x80419014: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_80419018:
    // 0x80419018: lbu         $v1, -0x3133($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3133);
    // 0x8041901C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80419020: bne         $a0, $zero, L_8041995C
    if (ctx->r4 != 0) {
        // 0x80419024: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_8041995C;
    }
    // 0x80419024: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80419028: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041902C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80419030: b           L_80415CB4
    // 0x80419034: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_80415CB4;
    // 0x80419034: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80419038:
    // 0x80419038: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041903C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419040: bne         $t0, $zero, L_8041967C
    if (ctx->r8 != 0) {
        // 0x80419044: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041967C;
    }
    // 0x80419044: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419048: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041904C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419050: b           L_8041699C
    // 0x80419054: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
        goto L_8041699C;
    // 0x80419054: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_80419058:
    // 0x80419058: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x8041905C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419060: bne         $t0, $zero, L_80419744
    if (ctx->r8 != 0) {
        // 0x80419064: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419744;
    }
    // 0x80419064: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419068: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041906C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419070: b           L_80417894
    // 0x80419074: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
        goto L_80417894;
    // 0x80419074: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_80419078:
    // 0x80419078: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041907C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419080: bne         $t0, $zero, L_80419524
    if (ctx->r8 != 0) {
        // 0x80419084: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419524;
    }
    // 0x80419084: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419088: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041908C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419090: b           L_80416FE0
    // 0x80419094: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
        goto L_80416FE0;
    // 0x80419094: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_80419098:
    // 0x80419098: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x8041909C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804190A0: bne         $t0, $zero, L_804198DC
    if (ctx->r8 != 0) {
        // 0x804190A4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804198DC;
    }
    // 0x804190A4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804190A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804190AC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804190B0: b           L_804174E8
    // 0x804190B4: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_804174E8;
    // 0x804190B4: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_804190B8:
    // 0x804190B8: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x804190BC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804190C0: bne         $t0, $zero, L_8041961C
    if (ctx->r8 != 0) {
        // 0x804190C4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041961C;
    }
    // 0x804190C4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804190C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804190CC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804190D0: b           L_80417A00
    // 0x804190D4: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
        goto L_80417A00;
    // 0x804190D4: sh          $t0, 0x76E8($a0)
    MEM_H(0X76E8, ctx->r4) = ctx->r8;
L_804190D8:
    // 0x804190D8: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x804190DC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804190E0: bne         $t0, $zero, L_8041996C
    if (ctx->r8 != 0) {
        // 0x804190E4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041996C;
    }
    // 0x804190E4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804190E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804190EC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804190F0: b           L_804176B4
    // 0x804190F4: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
        goto L_804176B4;
    // 0x804190F4: sh          $t0, 0x7A58($a0)
    MEM_H(0X7A58, ctx->r4) = ctx->r8;
L_804190F8:
    // 0x804190F8: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x804190FC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419100: bne         $t0, $zero, L_80419594
    if (ctx->r8 != 0) {
        // 0x80419104: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419594;
    }
    // 0x80419104: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419108: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041910C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419110: b           L_80416930
    // 0x80419114: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_80416930;
    // 0x80419114: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_80419118:
    // 0x80419118: lbu         $v1, -0x3383($v1)
    ctx->r3 = MEM_BU(ctx->r3, -0X3383);
    // 0x8041911C: sltiu       $a0, $v1, 0x4
    ctx->r4 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x80419120: bne         $a0, $zero, L_804198FC
    if (ctx->r4 != 0) {
        // 0x80419124: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_804198FC;
    }
    // 0x80419124: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80419128: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041912C: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x80419130: b           L_80415B4C
    // 0x80419134: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
        goto L_80415B4C;
    // 0x80419134: sh          $a0, 0x7B60($v1)
    MEM_H(0X7B60, ctx->r3) = ctx->r4;
L_80419138:
    // 0x80419138: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x8041913C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419140: bne         $t0, $zero, L_8041966C
    if (ctx->r8 != 0) {
        // 0x80419144: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041966C;
    }
    // 0x80419144: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419148: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041914C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419150: b           L_80417808
    // 0x80419154: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_80417808;
    // 0x80419154: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_80419158:
    // 0x80419158: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x8041915C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419160: bne         $t0, $zero, L_8041997C
    if (ctx->r8 != 0) {
        // 0x80419164: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041997C;
    }
    // 0x80419164: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419168: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041916C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419170: b           L_80417304
    // 0x80419174: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
        goto L_80417304;
    // 0x80419174: sh          $t0, 0x7740($a0)
    MEM_H(0X7740, ctx->r4) = ctx->r8;
L_80419178:
    // 0x80419178: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041917C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419180: bne         $t0, $zero, L_80419534
    if (ctx->r8 != 0) {
        // 0x80419184: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419534;
    }
    // 0x80419184: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419188: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041918C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419190: b           L_80416954
    // 0x80419194: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
        goto L_80416954;
    // 0x80419194: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_80419198:
    // 0x80419198: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x8041919C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804191A0: bne         $t0, $zero, L_804195CC
    if (ctx->r8 != 0) {
        // 0x804191A4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195CC;
    }
    // 0x804191A4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804191A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804191AC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804191B0: b           L_8041750C
    // 0x804191B4: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
        goto L_8041750C;
    // 0x804191B4: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_804191B8:
    // 0x804191B8: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804191BC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804191C0: bnel        $t0, $zero, L_8041953C
    if (ctx->r8 != 0) {
        // 0x804191C4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041953C;
    }
    goto skip_60;
    // 0x804191C4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    skip_60:
    // 0x804191C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804191CC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804191D0: b           L_80415D1C
    // 0x804191D4: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415D1C;
    // 0x804191D4: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_804191D8:
    // 0x804191D8: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804191DC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804191E0: bne         $t0, $zero, L_804196A4
    if (ctx->r8 != 0) {
        // 0x804191E4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196A4;
    }
    // 0x804191E4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804191E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804191EC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804191F0: b           L_80415CF8
    // 0x804191F4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415CF8;
    // 0x804191F4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_804191F8:
    // 0x804191F8: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x804191FC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419200: bne         $t0, $zero, L_80419584
    if (ctx->r8 != 0) {
        // 0x80419204: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419584;
    }
    // 0x80419204: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419208: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041920C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419210: b           L_80417918
    // 0x80419214: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
        goto L_80417918;
    // 0x80419214: sh          $t0, 0x7798($a0)
    MEM_H(0X7798, ctx->r4) = ctx->r8;
L_80419218:
    // 0x80419218: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x8041921C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419220: bne         $t0, $zero, L_8041960C
    if (ctx->r8 != 0) {
        // 0x80419224: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041960C;
    }
    // 0x80419224: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419228: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041922C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419230: b           L_80415BB4
    // 0x80419234: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415BB4;
    // 0x80419234: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_80419238:
    // 0x80419238: lbu         $a0, -0x35D3($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X35D3);
    // 0x8041923C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419240: bne         $t0, $zero, L_80419564
    if (ctx->r8 != 0) {
        // 0x80419244: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_80419564;
    }
    // 0x80419244: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419248: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041924C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419250: b           L_80417554
    // 0x80419254: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
        goto L_80417554;
    // 0x80419254: sh          $t0, 0x7B08($a0)
    MEM_H(0X7B08, ctx->r4) = ctx->r8;
L_80419258:
    // 0x80419258: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041925C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419260: bne         $t0, $zero, L_8041968C
    if (ctx->r8 != 0) {
        // 0x80419264: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041968C;
    }
    // 0x80419264: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419268: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041926C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419270: b           L_80415C68
    // 0x80419274: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415C68;
    // 0x80419274: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_80419278:
    // 0x80419278: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x8041927C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419280: bne         $t0, $zero, L_804195A4
    if (ctx->r8 != 0) {
        // 0x80419284: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195A4;
    }
    // 0x80419284: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419288: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041928C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419290: b           L_804177A4
    // 0x80419294: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
        goto L_804177A4;
    // 0x80419294: sh          $t0, 0x7AB0($a0)
    MEM_H(0X7AB0, ctx->r4) = ctx->r8;
L_80419298:
    // 0x80419298: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x8041929C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804192A0: bne         $t0, $zero, L_8041962C
    if (ctx->r8 != 0) {
        // 0x804192A4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041962C;
    }
    // 0x804192A4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804192A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804192AC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804192B0: b           L_80415B00
    // 0x804192B4: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
        goto L_80415B00;
    // 0x804192B4: sh          $t0, 0x77F0($a0)
    MEM_H(0X77F0, ctx->r4) = ctx->r8;
L_804192B8:
    // 0x804192B8: lbu         $a0, -0x34AB($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X34AB);
    // 0x804192BC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804192C0: bne         $t0, $zero, L_8041954C
    if (ctx->r8 != 0) {
        // 0x804192C4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041954C;
    }
    // 0x804192C4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804192C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804192CC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804192D0: b           L_80417608
    // 0x804192D4: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
        goto L_80417608;
    // 0x804192D4: sh          $t0, 0x7A00($a0)
    MEM_H(0X7A00, ctx->r4) = ctx->r8;
L_804192D8:
    // 0x804192D8: lbu         $a0, -0x300B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X300B);
    // 0x804192DC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804192E0: bnel        $t0, $zero, L_8041969C
    if (ctx->r8 != 0) {
        // 0x804192E4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041969C;
    }
    goto skip_61;
    // 0x804192E4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    skip_61:
    // 0x804192E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804192EC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804192F0: b           L_80415D40
    // 0x804192F4: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_80415D40;
    // 0x804192F4: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_804192F8:
    // 0x804192F8: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x804192FC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419300: bne         $t0, $zero, L_8041957C
    if (ctx->r8 != 0) {
        // 0x80419304: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041957C;
    }
    // 0x80419304: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419308: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041930C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419310: b           L_8041793C
    // 0x80419314: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
        goto L_8041793C;
    // 0x80419314: sh          $t0, 0x7950($a0)
    MEM_H(0X7950, ctx->r4) = ctx->r8;
L_80419318:
    // 0x80419318: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x8041931C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419320: bne         $t0, $zero, L_804196AC
    if (ctx->r8 != 0) {
        // 0x80419324: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804196AC;
    }
    // 0x80419324: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419328: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041932C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419330: b           L_80415BD8
    // 0x80419334: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_80415BD8;
    // 0x80419334: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_80419338:
    // 0x80419338: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x8041933C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419340: bne         $t0, $zero, L_8041955C
    if (ctx->r8 != 0) {
        // 0x80419344: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041955C;
    }
    // 0x80419344: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419348: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041934C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419350: b           L_804178F4
    // 0x80419354: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
        goto L_804178F4;
    // 0x80419354: sh          $t0, 0x75E0($a0)
    MEM_H(0X75E0, ctx->r4) = ctx->r8;
L_80419358:
    // 0x80419358: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041935C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419360: bnel        $t0, $zero, L_8041959C
    if (ctx->r8 != 0) {
        // 0x80419364: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041959C;
    }
    goto skip_62;
    // 0x80419364: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    skip_62:
    // 0x80419368: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041936C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419370: b           L_80415C8C
    // 0x80419374: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_80415C8C;
    // 0x80419374: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_80419378:
    // 0x80419378: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x8041937C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x80419380: bne         $t0, $zero, L_8041956C
    if (ctx->r8 != 0) {
        // 0x80419384: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041956C;
    }
    // 0x80419384: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x80419388: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041938C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419390: b           L_80415B24
    // 0x80419394: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
        goto L_80415B24;
    // 0x80419394: sh          $t0, 0x79A8($a0)
    MEM_H(0X79A8, ctx->r4) = ctx->r8;
L_80419398:
    // 0x80419398: lbu         $a0, -0x3133($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3133);
    // 0x8041939C: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804193A0: bne         $t0, $zero, L_8041958C
    if (ctx->r8 != 0) {
        // 0x804193A4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_8041958C;
    }
    // 0x804193A4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804193A8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804193AC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804193B0: b           L_80415C44
    // 0x804193B4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415C44;
    // 0x804193B4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_804193B8:
    // 0x804193B8: lbu         $a0, -0x325B($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X325B);
    // 0x804193BC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804193C0: bne         $t0, $zero, L_804195AC
    if (ctx->r8 != 0) {
        // 0x804193C4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804195AC;
    }
    // 0x804193C4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804193C8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804193CC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804193D0: b           L_80415B90
    // 0x804193D4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415B90;
    // 0x804193D4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_804193D8:
    // 0x804193D8: lbu         $a0, -0x3383($a0)
    ctx->r4 = MEM_BU(ctx->r4, -0X3383);
    // 0x804193DC: sltiu       $t0, $a0, 0x4
    ctx->r8 = ctx->r4 < 0X4 ? 1 : 0;
    // 0x804193E0: bne         $t0, $zero, L_804194C4
    if (ctx->r8 != 0) {
        // 0x804193E4: lui         $t0, 0x8008
        ctx->r8 = S32(0X8008 << 16);
            goto L_804194C4;
    }
    // 0x804193E4: lui         $t0, 0x8008
    ctx->r8 = S32(0X8008 << 16);
    // 0x804193E8: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x804193EC: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x804193F0: b           L_80415ADC
    // 0x804193F4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
        goto L_80415ADC;
    // 0x804193F4: sh          $t0, 0x7690($a0)
    MEM_H(0X7690, ctx->r4) = ctx->r8;
L_804193F8:
    // 0x804193F8: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    // 0x804193FC: bne         $v1, $a0, L_80419470
    if (ctx->r3 != ctx->r4) {
        // 0x80419400: lui         $a0, 0x8008
        ctx->r4 = S32(0X8008 << 16);
            goto L_80419470;
    }
    // 0x80419400: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x80419404: lw          $a0, 0x536C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X536C);
    // 0x80419408: beq         $a0, $zero, L_8041616C
    if (ctx->r4 == 0) {
        // 0x8041940C: addiu       $a0, $zero, 0x1
        ctx->r4 = ADD32(0, 0X1);
            goto L_8041616C;
    }
    // 0x8041940C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x80419410: sltiu       $a0, $t2, 0x5
    ctx->r4 = ctx->r10 < 0X5 ? 1 : 0;
    // 0x80419414: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_80419418:
    // 0x80419418: or          $a1, $a0, $a1
    ctx->r5 = ctx->r4 | ctx->r5;
L_8041941C:
    // 0x8041941C: beq         $a1, $zero, L_804181E0
    if (ctx->r5 == 0) {
        // 0x80419420: lui         $a0, 0x803B
        ctx->r4 = S32(0X803B << 16);
            goto L_804181E0;
    }
    // 0x80419420: lui         $a0, 0x803B
    ctx->r4 = S32(0X803B << 16);
    // 0x80419424: andi        $a3, $a3, 0xBF
    ctx->r7 = ctx->r7 & 0XBF;
    // 0x80419428: b           L_804181E0
    // 0x8041942C: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
        goto L_804181E0;
    // 0x8041942C: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
L_80419430:
    // 0x80419430: lw          $a1, 0x67C($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X67C);
    // 0x80419434: lui         $v1, 0xC081
    ctx->r3 = S32(0XC081 << 16);
    // 0x80419438: addiu       $v1, $v1, 0x1382
    ctx->r3 = ADD32(ctx->r3, 0X1382);
    // 0x8041943C: beq         $a1, $v1, L_80419450
    if (ctx->r5 == ctx->r3) {
        // 0x80419440: lui         $a2, 0x8018
        ctx->r6 = S32(0X8018 << 16);
            goto L_80419450;
    }
    // 0x80419440: lui         $a2, 0x8018
    ctx->r6 = S32(0X8018 << 16);
L_80419444:
    // 0x80419444: beql        $a3, $zero, L_804174B4
    if (ctx->r7 == 0) {
        // 0x80419448: lw          $a0, 0x4EE8($a0)
        ctx->r4 = MEM_W(ctx->r4, 0X4EE8);
            goto L_804174B4;
    }
    goto skip_63;
    // 0x80419448: lw          $a0, 0x4EE8($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X4EE8);
    skip_63:
    // 0x8041944C: lui         $a2, 0x8018
    ctx->r6 = S32(0X8018 << 16);
L_80419450:
    // 0x80419450: addiu       $v1, $zero, 0x486C
    ctx->r3 = ADD32(0, 0X486C);
    // 0x80419454: b           L_804174B0
    // 0x80419458: sh          $v1, 0x3468($a2)
    MEM_H(0X3468, ctx->r6) = ctx->r3;
        goto L_804174B0;
    // 0x80419458: sh          $v1, 0x3468($a2)
    MEM_H(0X3468, ctx->r6) = ctx->r3;
L_8041945C:
    // 0x8041945C: lui         $a3, 0x8000
    ctx->r7 = S32(0X8000 << 16);
    // 0x80419460: trunc.w.d   $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.u32l = TRUNC_W_D(ctx->f0.d);
    // 0x80419464: mfc1        $a0, $f0
    ctx->r4 = (int32_t)ctx->f0.u32l;
    // 0x80419468: b           L_804164B4
    // 0x8041946C: or          $a0, $a0, $a3
    ctx->r4 = ctx->r4 | ctx->r7;
        goto L_804164B4;
    // 0x8041946C: or          $a0, $a0, $a3
    ctx->r4 = ctx->r4 | ctx->r7;
L_80419470:
    // 0x80419470: addiu       $a0, $zero, 0xE
    ctx->r4 = ADD32(0, 0XE);
    // 0x80419474: bne         $v1, $a0, L_80416178
    if (ctx->r3 != ctx->r4) {
        // 0x80419478: addiu       $a1, $zero, 0x1
        ctx->r5 = ADD32(0, 0X1);
            goto L_80416178;
    }
    // 0x80419478: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041947C: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419480: lw          $a0, 0x536C($v1)
    ctx->r4 = MEM_W(ctx->r3, 0X536C);
    // 0x80419484: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x80419488: beq         $a0, $v1, L_80419418
    if (ctx->r4 == ctx->r3) {
        // 0x8041948C: sltiu       $a0, $t2, 0x5
        ctx->r4 = ctx->r10 < 0X5 ? 1 : 0;
            goto L_80419418;
    }
    // 0x8041948C: sltiu       $a0, $t2, 0x5
    ctx->r4 = ctx->r10 < 0X5 ? 1 : 0;
    // 0x80419490: andi        $a3, $a3, 0xBF
    ctx->r7 = ctx->r7 & 0XBF;
    // 0x80419494: b           L_804181DC
    // 0x80419498: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
        goto L_804181DC;
    // 0x80419498: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
L_8041949C:
    // 0x8041949C: b           L_804181C8
    // 0x804194A0: or          $a0, $t4, $ra
    ctx->r4 = ctx->r12 | ctx->r31;
        goto L_804181C8;
    // 0x804194A0: or          $a0, $t4, $ra
    ctx->r4 = ctx->r12 | ctx->r31;
L_804194A4:
    // 0x804194A4: b           L_80417870
    // 0x804194A8: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_80417870;
    // 0x804194A8: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_804194AC:
    // 0x804194AC: b           L_804175BC
    // 0x804194B0: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_804175BC;
    // 0x804194B0: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_804194B4:
    // 0x804194B4: b           L_80415D68
    // 0x804194B8: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_80415D68;
    // 0x804194B8: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_804194BC:
    // 0x804194BC: b           L_80417A6C
    // 0x804194C0: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_80417A6C;
    // 0x804194C0: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_804194C4:
    // 0x804194C4: b           L_80415ADC
    // 0x804194C8: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415ADC;
    // 0x804194C8: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_804194CC:
    // 0x804194CC: b           L_80417258
    // 0x804194D0: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80417258;
    // 0x804194D0: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_804194D4:
    // 0x804194D4: b           L_804159E4
    // 0x804194D8: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_804159E4;
    // 0x804194D8: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_804194DC:
    // 0x804194DC: b           L_8041708C
    // 0x804194E0: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_8041708C;
    // 0x804194E0: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_804194E4:
    // 0x804194E4: b           L_804165C8
    // 0x804194E8: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_804165C8;
    // 0x804194E8: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_804194EC:
    // 0x804194EC: b           L_80416C48
    // 0x804194F0: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80416C48;
    // 0x804194F0: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_804194F4:
    // 0x804194F4: b           L_804174E8
    // 0x804194F8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_804174E8;
    // 0x804194F8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_804194FC:
    // 0x804194FC: b           L_80417BC4
    // 0x80419500: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80417BC4;
    // 0x80419500: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_80419504:
    // 0x80419504: b           L_80415A98
    // 0x80419508: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_80415A98;
    // 0x80419508: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_8041950C:
    // 0x8041950C: b           L_804162E8
    // 0x80419510: andi        $a0, $a0, 0x20
    ctx->r4 = ctx->r4 & 0X20;
        goto L_804162E8;
    // 0x80419510: andi        $a0, $a0, 0x20
    ctx->r4 = ctx->r4 & 0X20;
L_80419514:
    // 0x80419514: b           L_80415974
    // 0x80419518: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415974;
    // 0x80419518: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_8041951C:
    // 0x8041951C: b           L_80416A6C
    // 0x80419520: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_80416A6C;
    // 0x80419520: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_80419524:
    // 0x80419524: b           L_80416FE0
    // 0x80419528: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_80416FE0;
    // 0x80419528: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_8041952C:
    // 0x8041952C: b           L_8041715C
    // 0x80419530: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_8041715C;
    // 0x80419530: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_80419534:
    // 0x80419534: b           L_80416954
    // 0x80419538: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_80416954;
    // 0x80419538: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_8041953C:
    // 0x8041953C: b           L_80415D1C
    // 0x80419540: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415D1C;
    // 0x80419540: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_80419544:
    // 0x80419544: b           L_80416CEC
    // 0x80419548: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_80416CEC;
    // 0x80419548: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_8041954C:
    // 0x8041954C: b           L_80417608
    // 0x80419550: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_80417608;
    // 0x80419550: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_80419554:
    // 0x80419554: b           L_80416F74
    // 0x80419558: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_80416F74;
    // 0x80419558: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_8041955C:
    // 0x8041955C: b           L_804178F4
    // 0x80419560: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_804178F4;
    // 0x80419560: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_80419564:
    // 0x80419564: b           L_80417554
    // 0x80419568: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_80417554;
    // 0x80419568: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_8041956C:
    // 0x8041956C: b           L_80415B24
    // 0x80419570: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_80415B24;
    // 0x80419570: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_80419574:
    // 0x80419574: b           L_804170F4
    // 0x80419578: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_804170F4;
    // 0x80419578: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_8041957C:
    // 0x8041957C: b           L_8041793C
    // 0x80419580: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_8041793C;
    // 0x80419580: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_80419584:
    // 0x80419584: b           L_80417918
    // 0x80419588: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_80417918;
    // 0x80419588: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_8041958C:
    // 0x8041958C: b           L_80415C44
    // 0x80419590: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415C44;
    // 0x80419590: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_80419594:
    // 0x80419594: b           L_80416930
    // 0x80419598: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_80416930;
    // 0x80419598: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_8041959C:
    // 0x8041959C: b           L_80415C8C
    // 0x804195A0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_80415C8C;
    // 0x804195A0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_804195A4:
    // 0x804195A4: b           L_804177A4
    // 0x804195A8: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_804177A4;
    // 0x804195A8: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_804195AC:
    // 0x804195AC: b           L_80415B90
    // 0x804195B0: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415B90;
    // 0x804195B0: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_804195B4:
    // 0x804195B4: b           L_80416584
    // 0x804195B8: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_80416584;
    // 0x804195B8: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_804195BC:
    // 0x804195BC: b           L_80417B80
    // 0x804195C0: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_80417B80;
    // 0x804195C0: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_804195C4:
    // 0x804195C4: b           L_80416EC4
    // 0x804195C8: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_80416EC4;
    // 0x804195C8: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_804195CC:
    // 0x804195CC: b           L_8041750C
    // 0x804195D0: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_8041750C;
    // 0x804195D0: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_804195D4:
    // 0x804195D4: b           L_804159E4
    // 0x804195D8: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_804159E4;
    // 0x804195D8: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_804195DC:
    // 0x804195DC: b           L_8041773C
    // 0x804195E0: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_8041773C;
    // 0x804195E0: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_804195E4:
    // 0x804195E4: b           L_804159E4
    // 0x804195E8: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_804159E4;
    // 0x804195E8: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_804195EC:
    // 0x804195EC: b           L_8041782C
    // 0x804195F0: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_8041782C;
    // 0x804195F0: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_804195F4:
    // 0x804195F4: b           L_80415A98
    // 0x804195F8: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_80415A98;
    // 0x804195F8: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_804195FC:
    // 0x804195FC: b           L_804175E4
    // 0x80419600: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_804175E4;
    // 0x80419600: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_80419604:
    // 0x80419604: b           L_8041653C
    // 0x80419608: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_8041653C;
    // 0x80419608: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_8041960C:
    // 0x8041960C: b           L_80415BB4
    // 0x80419610: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415BB4;
    // 0x80419610: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_80419614:
    // 0x80419614: b           L_80416AB0
    // 0x80419618: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_80416AB0;
    // 0x80419618: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_8041961C:
    // 0x8041961C: b           L_80417A00
    // 0x80419620: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_80417A00;
    // 0x80419620: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_80419624:
    // 0x80419624: b           L_80416848
    // 0x80419628: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_80416848;
    // 0x80419628: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_8041962C:
    // 0x8041962C: b           L_80415B00
    // 0x80419630: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415B00;
    // 0x80419630: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_80419634:
    // 0x80419634: b           L_8041671C
    // 0x80419638: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_8041671C;
    // 0x80419638: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_8041963C:
    // 0x8041963C: b           L_80416650
    // 0x80419640: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80416650;
    // 0x80419640: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_80419644:
    // 0x80419644: b           L_80416A24
    // 0x80419648: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_80416A24;
    // 0x80419648: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_8041964C:
    // 0x8041964C: b           L_80415CB4
    // 0x80419650: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_80415CB4;
    // 0x80419650: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_80419654:
    // 0x80419654: b           L_80415B4C
    // 0x80419658: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_80415B4C;
    // 0x80419658: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_8041965C:
    // 0x8041965C: b           L_80416804
    // 0x80419660: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_80416804;
    // 0x80419660: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_80419664:
    // 0x80419664: b           L_80417138
    // 0x80419668: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80417138;
    // 0x80419668: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_8041966C:
    // 0x8041966C: b           L_80417808
    // 0x80419670: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_80417808;
    // 0x80419670: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_80419674:
    // 0x80419674: b           L_80415B4C
    // 0x80419678: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_80415B4C;
    // 0x80419678: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_8041967C:
    // 0x8041967C: b           L_8041699C
    // 0x80419680: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_8041699C;
    // 0x80419680: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_80419684:
    // 0x80419684: b           L_80415C00
    // 0x80419688: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_80415C00;
    // 0x80419688: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_8041968C:
    // 0x8041968C: b           L_80415C68
    // 0x80419690: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415C68;
    // 0x80419690: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_80419694:
    // 0x80419694: b           L_80416978
    // 0x80419698: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_80416978;
    // 0x80419698: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_8041969C:
    // 0x8041969C: b           L_80415D40
    // 0x804196A0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_80415D40;
    // 0x804196A0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_804196A4:
    // 0x804196A4: b           L_80415CF8
    // 0x804196A8: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415CF8;
    // 0x804196A8: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_804196AC:
    // 0x804196AC: b           L_80415BD8
    // 0x804196B0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_80415BD8;
    // 0x804196B0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_804196B4:
    // 0x804196B4: lbu         $a0, 0x69F1($a0)
    ctx->r4 = MEM_BU(ctx->r4, 0X69F1);
    // 0x804196B8: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
L_804196BC:
    // 0x804196BC: beq         $a0, $zero, L_804157DC
    if (ctx->r4 == 0) {
        // 0x804196C0: nop
    
            goto L_804157DC;
    }
    // 0x804196C0: nop

L_804196C4:
    // 0x804196C4: lui         $a0, 0x8017
    ctx->r4 = S32(0X8017 << 16);
    // 0x804196C8: addiu       $a1, $zero, 0x67FA
    ctx->r5 = ADD32(0, 0X67FA);
    // 0x804196CC: b           L_804157DC
    // 0x804196D0: sh          $a1, -0x1FEA($a0)
    MEM_H(-0X1FEA, ctx->r4) = ctx->r5;
        goto L_804157DC;
    // 0x804196D0: sh          $a1, -0x1FEA($a0)
    MEM_H(-0X1FEA, ctx->r4) = ctx->r5;
L_804196D4:
    // 0x804196D4: b           L_80417BE8
    // 0x804196D8: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_80417BE8;
    // 0x804196D8: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_804196DC:
    // 0x804196DC: b           L_80415A70
    // 0x804196E0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_80415A70;
    // 0x804196E0: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_804196E4:
    // 0x804196E4: b           L_804159E4
    // 0x804196E8: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_804159E4;
    // 0x804196E8: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_804196EC:
    // 0x804196EC: b           L_80415A4C
    // 0x804196F0: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415A4C;
    // 0x804196F0: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_804196F4:
    // 0x804196F4: b           L_80415998
    // 0x804196F8: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
        goto L_80415998;
    // 0x804196F8: sh          $a0, 0x77F0($t0)
    MEM_H(0X77F0, ctx->r8) = ctx->r4;
L_804196FC:
    // 0x804196FC: b           L_80415A28
    // 0x80419700: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
        goto L_80415A28;
    // 0x80419700: sh          $a0, 0x7690($t0)
    MEM_H(0X7690, ctx->r8) = ctx->r4;
L_80419704:
    // 0x80419704: b           L_804159BC
    // 0x80419708: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
        goto L_804159BC;
    // 0x80419708: sh          $a0, 0x79A8($t0)
    MEM_H(0X79A8, ctx->r8) = ctx->r4;
L_8041970C:
    // 0x8041970C: b           L_80416F08
    // 0x80419710: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
        goto L_80416F08;
    // 0x80419710: sh          $a0, 0x7A00($t0)
    MEM_H(0X7A00, ctx->r8) = ctx->r4;
L_80419714:
    // 0x80419714: b           L_80416760
    // 0x80419718: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80416760;
    // 0x80419718: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_8041971C:
    // 0x8041971C: b           L_80415D68
    // 0x80419720: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_80415D68;
    // 0x80419720: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_80419724:
    // 0x80419724: b           L_80417328
    // 0x80419728: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80417328;
    // 0x80419728: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_8041972C:
    // 0x8041972C: b           L_80416E7C
    // 0x80419730: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
        goto L_80416E7C;
    // 0x80419730: sh          $a0, 0x76E8($t0)
    MEM_H(0X76E8, ctx->r8) = ctx->r4;
L_80419734:
    // 0x80419734: b           L_80417960
    // 0x80419738: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_80417960;
    // 0x80419738: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_8041973C:
    // 0x8041973C: b           L_80416694
    // 0x80419740: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_80416694;
    // 0x80419740: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_80419744:
    // 0x80419744: b           L_80417894
    // 0x80419748: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_80417894;
    // 0x80419748: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_8041974C:
    // 0x8041974C: b           L_804159E4
    // 0x80419750: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_804159E4;
    // 0x80419750: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_80419754:
    // 0x80419754: b           L_80415D68
    // 0x80419758: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_80415D68;
    // 0x80419758: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_8041975C:
    // 0x8041975C: b           L_8041688C
    // 0x80419760: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_8041688C;
    // 0x80419760: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_80419764:
    // 0x80419764: b           L_80416D30
    // 0x80419768: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
        goto L_80416D30;
    // 0x80419768: sh          $a0, 0x7798($t0)
    MEM_H(0X7798, ctx->r8) = ctx->r4;
L_8041976C:
    // 0x8041976C: b           L_80415D68
    // 0x80419770: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_80415D68;
    // 0x80419770: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_80419774:
    // 0x80419774: b           L_80416DB8
    // 0x80419778: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_80416DB8;
    // 0x80419778: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_8041977C:
    // 0x8041977C: b           L_804167A4
    // 0x80419780: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_804167A4;
    // 0x80419780: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_80419784:
    // 0x80419784: b           L_80415C00
    // 0x80419788: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_80415C00;
    // 0x80419788: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_8041978C:
    // 0x8041978C: b           L_80415A98
    // 0x80419790: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_80415A98;
    // 0x80419790: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_80419794:
    // 0x80419794: b           L_804168D0
    // 0x80419798: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
        goto L_804168D0;
    // 0x80419798: sh          $a0, 0x7B08($t0)
    MEM_H(0X7B08, ctx->r8) = ctx->r4;
L_8041979C:
    // 0x8041979C: b           L_80415CB4
    // 0x804197A0: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_80415CB4;
    // 0x804197A0: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_804197A4:
    // 0x804197A4: b           L_80416B7C
    // 0x804197A8: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_80416B7C;
    // 0x804197A8: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_804197AC:
    // 0x804197AC: b           L_80415C00
    // 0x804197B0: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_80415C00;
    // 0x804197B0: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_804197B4:
    // 0x804197B4: b           L_80416930
    // 0x804197B8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_80416930;
    // 0x804197B8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_804197BC:
    // 0x804197BC: b           L_80415A98
    // 0x804197C0: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_80415A98;
    // 0x804197C0: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_804197C4:
    // 0x804197C4: bne         $t0, $zero, L_804197D4
    if (ctx->r8 != 0) {
        // 0x804197C8: lui         $a2, 0x8009
        ctx->r6 = S32(0X8009 << 16);
            goto L_804197D4;
    }
    // 0x804197C8: lui         $a2, 0x8009
    ctx->r6 = S32(0X8009 << 16);
    // 0x804197CC: ori         $a1, $a1, 0x1
    ctx->r5 = ctx->r5 | 0X1;
    // 0x804197D0: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
L_804197D4:
    // 0x804197D4: lw          $t0, 0x67C($a2)
    ctx->r8 = MEM_W(ctx->r6, 0X67C);
    // 0x804197D8: lui         $a2, 0xC081
    ctx->r6 = S32(0XC081 << 16);
    // 0x804197DC: lui         $a3, 0x8018
    ctx->r7 = S32(0X8018 << 16);
    // 0x804197E0: addiu       $t1, $zero, 0x346E
    ctx->r9 = ADD32(0, 0X346E);
    // 0x804197E4: addiu       $a2, $a2, 0x1882
    ctx->r6 = ADD32(ctx->r6, 0X1882);
    // 0x804197E8: bne         $t0, $a2, L_80418CFC
    if (ctx->r8 != ctx->r6) {
        // 0x804197EC: sh          $t1, 0x3336($a3)
        MEM_H(0X3336, ctx->r7) = ctx->r9;
            goto L_80418CFC;
    }
    // 0x804197EC: sh          $t1, 0x3336($a3)
    MEM_H(0X3336, ctx->r7) = ctx->r9;
    // 0x804197F0: ori         $a1, $a1, 0x2
    ctx->r5 = ctx->r5 | 0X2;
    // 0x804197F4: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
L_804197F8:
    // 0x804197F8: addiu       $a1, $zero, 0x378B
    ctx->r5 = ADD32(0, 0X378B);
    // 0x804197FC: sh          $a1, 0x3336($a3)
    MEM_H(0X3336, ctx->r7) = ctx->r5;
    // 0x80419800: lui         $a1, 0xC0AB
    ctx->r5 = S32(0XC0AB << 16);
    // 0x80419804: lui         $a2, 0x8040
    ctx->r6 = S32(0X8040 << 16);
    // 0x80419808: addiu       $a1, $a1, 0x6890
    ctx->r5 = ADD32(ctx->r5, 0X6890);
    // 0x8041980C: sw          $a1, 0x564C($a2)
    MEM_W(0X564C, ctx->r6) = ctx->r5;
    // 0x80419810: lui         $a1, 0x40AB
    ctx->r5 = S32(0X40AB << 16);
    // 0x80419814: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x80419818: addiu       $a1, $a1, 0x6890
    ctx->r5 = ADD32(ctx->r5, 0X6890);
    // 0x8041981C: b           L_80418D0C
    // 0x80419820: sw          $a1, 0x6A10($a2)
    MEM_W(0X6A10, ctx->r6) = ctx->r5;
        goto L_80418D0C;
    // 0x80419820: sw          $a1, 0x6A10($a2)
    MEM_W(0X6A10, ctx->r6) = ctx->r5;
L_80419824:
    // 0x80419824: b           L_80417690
    // 0x80419828: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80417690;
    // 0x80419828: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_8041982C:
    // 0x8041982C: b           L_80416D74
    // 0x80419830: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_80416D74;
    // 0x80419830: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_80419834:
    // 0x80419834: b           L_80416C04
    // 0x80419838: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_80416C04;
    // 0x80419838: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_8041983C:
    // 0x8041983C: b           L_80415C00
    // 0x80419840: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_80415C00;
    // 0x80419840: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_80419844:
    // 0x80419844: b           L_80416B38
    // 0x80419848: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80416B38;
    // 0x80419848: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_8041984C:
    // 0x8041984C: b           L_80416C8C
    // 0x80419850: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_80416C8C;
    // 0x80419850: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_80419854:
    // 0x80419854: b           L_80415A98
    // 0x80419858: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_80415A98;
    // 0x80419858: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_8041985C:
    // 0x8041985C: b           L_80417B18
    // 0x80419860: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_80417B18;
    // 0x80419860: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_80419864:
    // 0x80419864: b           L_80417AF4
    // 0x80419868: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80417AF4;
    // 0x80419868: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_8041986C:
    // 0x8041986C: andi        $a2, $a3, 0x8
    ctx->r6 = ctx->r7 & 0X8;
    // 0x80419870: beq         $a2, $zero, L_804198BC
    if (ctx->r6 == 0) {
        // 0x80419874: lw          $a2, 0x67C($a0)
        ctx->r6 = MEM_W(ctx->r4, 0X67C);
            goto L_804198BC;
    }
    // 0x80419874: lw          $a2, 0x67C($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X67C);
    // 0x80419878: lui         $a0, 0xA0C0
    ctx->r4 = S32(0XA0C0 << 16);
    // 0x8041987C: ori         $a0, $a0, 0x8118
    ctx->r4 = ctx->r4 | 0X8118;
    // 0x80419880: bne         $a2, $a0, L_804198BC
    if (ctx->r6 != ctx->r4) {
        // 0x80419884: nop
    
            goto L_804198BC;
    }
    // 0x80419884: nop

    // 0x80419888: bgez        $a1, L_80418C34
    if (SIGNED(ctx->r5) >= 0) {
        // 0x8041988C: lui         $t1, 0x8019
        ctx->r9 = S32(0X8019 << 16);
            goto L_80418C34;
    }
    // 0x8041988C: lui         $t1, 0x8019
    ctx->r9 = S32(0X8019 << 16);
    // 0x80419890: addiu       $v1, $zero, -0x74BE
    ctx->r3 = ADD32(0, -0X74BE);
    // 0x80419894: b           L_80416070
    // 0x80419898: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
        goto L_80416070;
    // 0x80419898: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
L_8041989C:
    // 0x8041989C: andi        $a3, $a3, 0x8
    ctx->r7 = ctx->r7 & 0X8;
    // 0x804198A0: beq         $a3, $zero, L_804198CC
    if (ctx->r7 == 0) {
        // 0x804198A4: addiu       $v1, $zero, -0x74BE
        ctx->r3 = ADD32(0, -0X74BE);
            goto L_804198CC;
    }
    // 0x804198A4: addiu       $v1, $zero, -0x74BE
    ctx->r3 = ADD32(0, -0X74BE);
    // 0x804198A8: lui         $v1, 0xA0C0
    ctx->r3 = S32(0XA0C0 << 16);
    // 0x804198AC: lw          $a0, 0x67C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X67C);
    // 0x804198B0: ori         $v1, $v1, 0x8118
    ctx->r3 = ctx->r3 | 0X8118;
    // 0x804198B4: beq         $a0, $v1, L_804198CC
    if (ctx->r4 == ctx->r3) {
        // 0x804198B8: addiu       $v1, $zero, -0x74BE
        ctx->r3 = ADD32(0, -0X74BE);
            goto L_804198CC;
    }
    // 0x804198B8: addiu       $v1, $zero, -0x74BE
    ctx->r3 = ADD32(0, -0X74BE);
L_804198BC:
    // 0x804198BC: bgez        $a1, L_80416074
    if (SIGNED(ctx->r5) >= 0) {
        // 0x804198C0: addiu       $v1, $zero, 0x12
        ctx->r3 = ADD32(0, 0X12);
            goto L_80416074;
    }
    // 0x804198C0: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
    // 0x804198C4: lui         $t1, 0x8019
    ctx->r9 = S32(0X8019 << 16);
    // 0x804198C8: addiu       $v1, $zero, -0x74BE
    ctx->r3 = ADD32(0, -0X74BE);
L_804198CC:
    // 0x804198CC: b           L_80416070
    // 0x804198D0: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
        goto L_80416070;
    // 0x804198D0: sh          $v1, -0x3250($t1)
    MEM_H(-0X3250, ctx->r9) = ctx->r3;
L_804198D4:
    // 0x804198D4: b           L_80417808
    // 0x804198D8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_80417808;
    // 0x804198D8: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_804198DC:
    // 0x804198DC: b           L_804174E8
    // 0x804198E0: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_804174E8;
    // 0x804198E0: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_804198E4:
    // 0x804198E4: b           L_80417780
    // 0x804198E8: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
        goto L_80417780;
    // 0x804198E8: sh          $a0, 0x78F8($t0)
    MEM_H(0X78F8, ctx->r8) = ctx->r4;
L_804198EC:
    // 0x804198EC: b           L_80417A48
    // 0x804198F0: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_80417A48;
    // 0x804198F0: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_804198F4:
    // 0x804198F4: b           L_80417530
    // 0x804198F8: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
        goto L_80417530;
    // 0x804198F8: sh          $a0, 0x7950($t0)
    MEM_H(0X7950, ctx->r8) = ctx->r4;
L_804198FC:
    // 0x804198FC: b           L_80415B4C
    // 0x80419900: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_80415B4C;
    // 0x80419900: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_80419904:
    // 0x80419904: b           L_80415B4C
    // 0x80419908: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
        goto L_80415B4C;
    // 0x80419908: sh          $v1, 0x7CC0($a0)
    MEM_H(0X7CC0, ctx->r4) = ctx->r3;
L_8041990C:
    // 0x8041990C: b           L_80416CEC
    // 0x80419910: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_80416CEC;
    // 0x80419910: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_80419914:
    // 0x80419914: b           L_80415CB4
    // 0x80419918: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
        goto L_80415CB4;
    // 0x80419918: sh          $v1, 0x7BB8($a0)
    MEM_H(0X7BB8, ctx->r4) = ctx->r3;
L_8041991C:
    // 0x8041991C: b           L_80416804
    // 0x80419920: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
        goto L_80416804;
    // 0x80419920: sh          $a0, 0x75E0($t0)
    MEM_H(0X75E0, ctx->r8) = ctx->r4;
L_80419924:
    // 0x80419924: b           L_80415CB4
    // 0x80419928: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
        goto L_80415CB4;
    // 0x80419928: sh          $v1, 0x7C68($a0)
    MEM_H(0X7C68, ctx->r4) = ctx->r3;
L_8041992C:
    // 0x8041992C: b           L_80417068
    // 0x80419930: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
        goto L_80417068;
    // 0x80419930: sh          $a0, 0x78A0($t0)
    MEM_H(0X78A0, ctx->r8) = ctx->r4;
L_80419934:
    // 0x80419934: b           L_8041729C
    // 0x80419938: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_8041729C;
    // 0x80419938: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_8041993C:
    // 0x8041993C: b           L_80415C00
    // 0x80419940: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_80415C00;
    // 0x80419940: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_80419944:
    // 0x80419944: b           L_8041736C
    // 0x80419948: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
        goto L_8041736C;
    // 0x80419948: sh          $a0, 0x7AB0($t0)
    MEM_H(0X7AB0, ctx->r8) = ctx->r4;
L_8041994C:
    // 0x8041994C: b           L_80416FBC
    // 0x80419950: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
        goto L_80416FBC;
    // 0x80419950: sh          $a0, 0x7848($t0)
    MEM_H(0X7848, ctx->r8) = ctx->r4;
L_80419954:
    // 0x80419954: b           L_80415B4C
    // 0x80419958: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
        goto L_80415B4C;
    // 0x80419958: sh          $v1, 0x7C10($a0)
    MEM_H(0X7C10, ctx->r4) = ctx->r3;
L_8041995C:
    // 0x8041995C: b           L_80415CB4
    // 0x80419960: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_80415CB4;
    // 0x80419960: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_80419964:
    // 0x80419964: b           L_804178F4
    // 0x80419968: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
        goto L_804178F4;
    // 0x80419968: sh          $a0, 0x7638($t0)
    MEM_H(0X7638, ctx->r8) = ctx->r4;
L_8041996C:
    // 0x8041996C: b           L_804176B4
    // 0x80419970: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
        goto L_804176B4;
    // 0x80419970: sh          $a0, 0x7A58($t0)
    MEM_H(0X7A58, ctx->r8) = ctx->r4;
L_80419974:
    // 0x80419974: b           L_80415D68
    // 0x80419978: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
        goto L_80415D68;
    // 0x80419978: sh          $v1, 0x7B60($a0)
    MEM_H(0X7B60, ctx->r4) = ctx->r3;
L_8041997C:
    // 0x8041997C: b           L_80417304
    // 0x80419980: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
        goto L_80417304;
    // 0x80419980: sh          $a0, 0x7740($t0)
    MEM_H(0X7740, ctx->r8) = ctx->r4;
L_80419984:
    // 0x80419984: b           L_80415DC4
    // 0x80419988: sh          $v1, -0x547E($a0)
    MEM_H(-0X547E, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80419988: sh          $v1, -0x547E($a0)
    MEM_H(-0X547E, ctx->r4) = ctx->r3;
L_8041998C:
    // 0x8041998C: b           L_80415DC4
    // 0x80419990: sh          $v1, 0x343C($a0)
    MEM_H(0X343C, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80419990: sh          $v1, 0x343C($a0)
    MEM_H(0X343C, ctx->r4) = ctx->r3;
L_80419994:
    // 0x80419994: b           L_80415DC4
    // 0x80419998: sh          $v1, 0x3412($a0)
    MEM_H(0X3412, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x80419998: sh          $v1, 0x3412($a0)
    MEM_H(0X3412, ctx->r4) = ctx->r3;
L_8041999C:
    // 0x8041999C: b           L_80415DC4
    // 0x804199A0: sh          $v1, -0x30C2($a0)
    MEM_H(-0X30C2, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x804199A0: sh          $v1, -0x30C2($a0)
    MEM_H(-0X30C2, ctx->r4) = ctx->r3;
L_804199A4:
    // 0x804199A4: b           L_80415DC4
    // 0x804199A8: sh          $v1, -0x2016($a0)
    MEM_H(-0X2016, ctx->r4) = ctx->r3;
        goto L_80415DC4;
    // 0x804199A8: sh          $v1, -0x2016($a0)
    MEM_H(-0X2016, ctx->r4) = ctx->r3;
L_804199AC:
    // 0x804199AC: addiu       $a0, $zero, -0x4200
    ctx->r4 = ADD32(0, -0X4200);
    // 0x804199B0: b           L_80416070
    // 0x804199B4: sh          $a0, -0x3146($v1)
    MEM_H(-0X3146, ctx->r3) = ctx->r4;
        goto L_80416070;
    // 0x804199B4: sh          $a0, -0x3146($v1)
    MEM_H(-0X3146, ctx->r3) = ctx->r4;
L_804199B8:
    // 0x804199B8: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x804199BC: lw          $t3, 0x67C($v1)
    ctx->r11 = MEM_W(ctx->r3, 0X67C);
    // 0x804199C0: lui         $v1, 0xA0C0
    ctx->r3 = S32(0XA0C0 << 16);
    // 0x804199C4: ori         $v1, $v1, 0x8118
    ctx->r3 = ctx->r3 | 0X8118;
    // 0x804199C8: bne         $t3, $v1, L_804162EC
    if (ctx->r11 != ctx->r3) {
        // 0x804199CC: sll         $v1, $a3, 24
        ctx->r3 = S32(ctx->r7 << 24);
            goto L_804162EC;
    }
    // 0x804199CC: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
    // 0x804199D0: ori         $a3, $a3, 0x80
    ctx->r7 = ctx->r7 | 0X80;
    // 0x804199D4: sll         $v1, $a3, 24
    ctx->r3 = S32(ctx->r7 << 24);
    // 0x804199D8: addiu       $a0, $zero, -0x44A0
    ctx->r4 = ADD32(0, -0X44A0);
    // 0x804199DC: sb          $a3, 0x69F0($t1)
    MEM_B(0X69F0, ctx->r9) = ctx->r7;
    // 0x804199E0: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x804199E4: sh          $a0, -0x24B6($t2)
    MEM_H(-0X24B6, ctx->r10) = ctx->r4;
L_804199E8:
    // 0x804199E8: bne         $a1, $zero, L_8041630C
    if (ctx->r5 != 0) {
        // 0x804199EC: lui         $a0, 0x8009
        ctx->r4 = S32(0X8009 << 16);
            goto L_8041630C;
    }
    // 0x804199EC: lui         $a0, 0x8009
    ctx->r4 = S32(0X8009 << 16);
    // 0x804199F0: b           L_804160A8
    // 0x804199F4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
        goto L_804160A8;
    // 0x804199F4: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
L_804199F8:
    // 0x804199F8: lhu         $a1, -0x365E($a1)
    ctx->r5 = MEM_HU(ctx->r5, -0X365E);
    // 0x804199FC: bnel        $a1, $zero, L_804157A0
    if (ctx->r5 != 0) {
        // 0x80419A00: addiu       $v1, $zero, 0x11
        ctx->r3 = ADD32(0, 0X11);
            goto L_804157A0;
    }
    goto skip_64;
    // 0x80419A00: addiu       $v1, $zero, 0x11
    ctx->r3 = ADD32(0, 0X11);
    skip_64:
    // 0x80419A04: ori         $v1, $v1, 0x8
    ctx->r3 = ctx->r3 | 0X8;
    // 0x80419A08: b           L_8041579C
    // 0x80419A0C: sb          $v1, 0x69E4($a0)
    MEM_B(0X69E4, ctx->r4) = ctx->r3;
        goto L_8041579C;
    // 0x80419A0C: sb          $v1, 0x69E4($a0)
    MEM_B(0X69E4, ctx->r4) = ctx->r3;
L_80419A10:
    // 0x80419A10: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419A14: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80419A18: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80419A1C: beq         $a1, $a3, L_80419F84
    if (ctx->r5 == ctx->r7) {
        // 0x80419A20: addiu       $t0, $zero, 0x2
        ctx->r8 = ADD32(0, 0X2);
            goto L_80419F84;
    }
    // 0x80419A20: addiu       $t0, $zero, 0x2
    ctx->r8 = ADD32(0, 0X2);
    // 0x80419A24: beql        $a1, $t0, L_80419F6C
    if (ctx->r5 == ctx->r8) {
        // 0x80419A28: sltiu       $a3, $v1, 0x96
        ctx->r7 = ctx->r3 < 0X96 ? 1 : 0;
            goto L_80419F6C;
    }
    goto skip_65;
    // 0x80419A28: sltiu       $a3, $v1, 0x96
    ctx->r7 = ctx->r3 < 0X96 ? 1 : 0;
    skip_65:
    // 0x80419A2C: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419A30: bnel        $a1, $t0, L_8041A048
    if (ctx->r5 != ctx->r8) {
        // 0x80419A34: addiu       $t0, $zero, 0x1
        ctx->r8 = ADD32(0, 0X1);
            goto L_8041A048;
    }
    goto skip_66;
    // 0x80419A34: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    skip_66:
    // 0x80419A38: sltiu       $a1, $v1, 0x4B
    ctx->r5 = ctx->r3 < 0X4B ? 1 : 0;
    // 0x80419A3C: bnel        $a1, $zero, L_80418EA4
    if (ctx->r5 != 0) {
        // 0x80419A40: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80418EA4;
    }
    goto skip_67;
    // 0x80419A40: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_67:
    // 0x80419A44: addiu       $v1, $v1, -0x4B
    ctx->r3 = ADD32(ctx->r3, -0X4B);
    // 0x80419A48: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419A4C: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
L_80419A50:
    // 0x80419A50: sb          $a3, -0x458D($a1)
    MEM_B(-0X458D, ctx->r5) = ctx->r7;
L_80419A54:
    // 0x80419A54: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419A58: sw          $zero, 0x5C0($a1)
    MEM_W(0X5C0, ctx->r5) = 0;
    // 0x80419A5C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419A60: sw          $zero, 0x5E0($a1)
    MEM_W(0X5E0, ctx->r5) = 0;
    // 0x80419A64: b           L_80418EA0
    // 0x80419A68: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
        goto L_80418EA0;
    // 0x80419A68: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
L_80419A6C:
    // 0x80419A6C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419A70: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80419A74: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80419A78: beq         $a1, $a3, L_80419FD8
    if (ctx->r5 == ctx->r7) {
        // 0x80419A7C: addiu       $t0, $zero, 0x2
        ctx->r8 = ADD32(0, 0X2);
            goto L_80419FD8;
    }
    // 0x80419A7C: addiu       $t0, $zero, 0x2
    ctx->r8 = ADD32(0, 0X2);
    // 0x80419A80: beql        $a1, $t0, L_80419FC0
    if (ctx->r5 == ctx->r8) {
        // 0x80419A84: sltiu       $a1, $v1, 0xAF
        ctx->r5 = ctx->r3 < 0XAF ? 1 : 0;
            goto L_80419FC0;
    }
    goto skip_68;
    // 0x80419A84: sltiu       $a1, $v1, 0xAF
    ctx->r5 = ctx->r3 < 0XAF ? 1 : 0;
    skip_68:
    // 0x80419A88: addiu       $t0, $zero, 0x4
    ctx->r8 = ADD32(0, 0X4);
    // 0x80419A8C: bnel        $a1, $t0, L_80419FA0
    if (ctx->r5 != ctx->r8) {
        // 0x80419A90: addiu       $a3, $zero, 0x1
        ctx->r7 = ADD32(0, 0X1);
            goto L_80419FA0;
    }
    goto skip_69;
    // 0x80419A90: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    skip_69:
    // 0x80419A94: sltiu       $a1, $v1, 0x113
    ctx->r5 = ctx->r3 < 0X113 ? 1 : 0;
    // 0x80419A98: bnel        $a1, $zero, L_80417E88
    if (ctx->r5 != 0) {
        // 0x80419A9C: sltiu       $v1, $v1, 0x96
        ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
            goto L_80417E88;
    }
    goto skip_70;
    // 0x80419A9C: sltiu       $v1, $v1, 0x96
    ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
    skip_70:
    // 0x80419AA0: addiu       $v1, $v1, -0x113
    ctx->r3 = ADD32(ctx->r3, -0X113);
    // 0x80419AA4: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
L_80419AA8:
    // 0x80419AA8: sb          $a3, -0x458D($a1)
    MEM_B(-0X458D, ctx->r5) = ctx->r7;
    // 0x80419AAC: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419AB0: sw          $zero, 0x5C0($a1)
    MEM_W(0X5C0, ctx->r5) = 0;
    // 0x80419AB4: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419AB8: sw          $zero, 0x5E0($a1)
    MEM_W(0X5E0, ctx->r5) = 0;
    // 0x80419ABC: b           L_80417E84
    // 0x80419AC0: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
        goto L_80417E84;
    // 0x80419AC0: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
L_80419AC4:
    // 0x80419AC4: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419AC8: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80419ACC: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80419AD0: beq         $a1, $a3, L_8041A02C
    if (ctx->r5 == ctx->r7) {
        // 0x80419AD4: addiu       $a3, $zero, 0x2
        ctx->r7 = ADD32(0, 0X2);
            goto L_8041A02C;
    }
    // 0x80419AD4: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x80419AD8: beql        $a1, $a3, L_8041A014
    if (ctx->r5 == ctx->r7) {
        // 0x80419ADC: sltiu       $a1, $v1, 0x4B
        ctx->r5 = ctx->r3 < 0X4B ? 1 : 0;
            goto L_8041A014;
    }
    goto skip_71;
    // 0x80419ADC: sltiu       $a1, $v1, 0x4B
    ctx->r5 = ctx->r3 < 0X4B ? 1 : 0;
    skip_71:
    // 0x80419AE0: addiu       $a3, $zero, 0x4
    ctx->r7 = ADD32(0, 0X4);
    // 0x80419AE4: bne         $a1, $a3, L_80419FF4
    if (ctx->r5 != ctx->r7) {
        // 0x80419AE8: addiu       $a3, $zero, 0x1
        ctx->r7 = ADD32(0, 0X1);
            goto L_80419FF4;
    }
    // 0x80419AE8: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80419AEC: sltiu       $a1, $v1, 0x64
    ctx->r5 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x80419AF0: bnel        $a1, $zero, L_804163D4
    if (ctx->r5 != 0) {
        // 0x80419AF4: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_804163D4;
    }
    goto skip_72;
    // 0x80419AF4: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_72:
    // 0x80419AF8: addiu       $v1, $v1, -0x64
    ctx->r3 = ADD32(ctx->r3, -0X64);
    // 0x80419AFC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419B00: addiu       $a3, $zero, 0xC
    ctx->r7 = ADD32(0, 0XC);
L_80419B04:
    // 0x80419B04: sb          $a3, -0x458D($a1)
    MEM_B(-0X458D, ctx->r5) = ctx->r7;
    // 0x80419B08: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419B0C: sw          $zero, 0x5C0($a1)
    MEM_W(0X5C0, ctx->r5) = 0;
    // 0x80419B10: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419B14: sw          $zero, 0x5E0($a1)
    MEM_W(0X5E0, ctx->r5) = 0;
    // 0x80419B18: b           L_804163D0
    // 0x80419B1C: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
        goto L_804163D0;
    // 0x80419B1C: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
L_80419B20:
    // 0x80419B20: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419B24: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80419B28: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80419B2C: beq         $a1, $a3, L_8041A09C
    if (ctx->r5 == ctx->r7) {
        // 0x80419B30: addiu       $a3, $zero, 0x2
        ctx->r7 = ADD32(0, 0X2);
            goto L_8041A09C;
    }
    // 0x80419B30: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x80419B34: beql        $a1, $a3, L_8041A084
    if (ctx->r5 == ctx->r7) {
        // 0x80419B38: sltiu       $a1, $v1, 0x15E
        ctx->r5 = ctx->r3 < 0X15E ? 1 : 0;
            goto L_8041A084;
    }
    goto skip_73;
    // 0x80419B38: sltiu       $a1, $v1, 0x15E
    ctx->r5 = ctx->r3 < 0X15E ? 1 : 0;
    skip_73:
    // 0x80419B3C: addiu       $a3, $zero, 0x4
    ctx->r7 = ADD32(0, 0X4);
    // 0x80419B40: bne         $a1, $a3, L_8041A064
    if (ctx->r5 != ctx->r7) {
        // 0x80419B44: addiu       $a3, $zero, 0x1
        ctx->r7 = ADD32(0, 0X1);
            goto L_8041A064;
    }
    // 0x80419B44: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x80419B48: sltiu       $a1, $v1, 0x15E
    ctx->r5 = ctx->r3 < 0X15E ? 1 : 0;
    // 0x80419B4C: bnel        $a1, $zero, L_80417FAC
    if (ctx->r5 != 0) {
        // 0x80419B50: sltiu       $v1, $v1, 0xFA
        ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
            goto L_80417FAC;
    }
    goto skip_74;
    // 0x80419B50: sltiu       $v1, $v1, 0xFA
    ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
    skip_74:
    // 0x80419B54: addiu       $v1, $v1, -0x15E
    ctx->r3 = ADD32(ctx->r3, -0X15E);
    // 0x80419B58: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419B5C: addiu       $a3, $zero, 0x9
    ctx->r7 = ADD32(0, 0X9);
L_80419B60:
    // 0x80419B60: sb          $a3, -0x458D($a1)
    MEM_B(-0X458D, ctx->r5) = ctx->r7;
    // 0x80419B64: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419B68: sw          $zero, 0x5C0($a1)
    MEM_W(0X5C0, ctx->r5) = 0;
    // 0x80419B6C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419B70: sw          $zero, 0x5E0($a1)
    MEM_W(0X5E0, ctx->r5) = 0;
    // 0x80419B74: b           L_80417FA8
    // 0x80419B78: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
        goto L_80417FA8;
    // 0x80419B78: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
L_80419B7C:
    // 0x80419B7C: b           L_80415FA4
    // 0x80419B80: sb          $a0, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r4;
        goto L_80415FA4;
    // 0x80419B80: sb          $a0, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r4;
L_80419B84:
    // 0x80419B84: bne         $a1, $zero, L_80419BE8
    if (ctx->r5 != 0) {
        // 0x80419B88: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419BE8;
    }
    // 0x80419B88: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80419B8C: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80419B90: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80419B94: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80419B98: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80419B9C: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x80419BA0: lhu         $t1, -0x4558($t6)
    ctx->r9 = MEM_HU(ctx->r14, -0X4558);
    // 0x80419BA4: lhu         $t0, -0x4556($t5)
    ctx->r8 = MEM_HU(ctx->r13, -0X4556);
    // 0x80419BA8: lhu         $a3, -0x4552($t4)
    ctx->r7 = MEM_HU(ctx->r12, -0X4552);
    // 0x80419BAC: lhu         $a1, -0x4554($t3)
    ctx->r5 = MEM_HU(ctx->r11, -0X4554);
    // 0x80419BB0: lw          $v1, -0x4570($t2)
    ctx->r3 = MEM_W(ctx->r10, -0X4570);
    // 0x80419BB4: addiu       $t1, $t1, 0x32
    ctx->r9 = ADD32(ctx->r9, 0X32);
    // 0x80419BB8: addiu       $t0, $t0, 0xA
    ctx->r8 = ADD32(ctx->r8, 0XA);
    // 0x80419BBC: addiu       $a3, $a3, 0xA
    ctx->r7 = ADD32(ctx->r7, 0XA);
    // 0x80419BC0: addiu       $a1, $a1, 0xA
    ctx->r5 = ADD32(ctx->r5, 0XA);
    // 0x80419BC4: addiu       $v1, $v1, 0x2710
    ctx->r3 = ADD32(ctx->r3, 0X2710);
    // 0x80419BC8: ori         $a0, $a0, 0x1
    ctx->r4 = ctx->r4 | 0X1;
    // 0x80419BCC: sh          $t1, -0x4558($t6)
    MEM_H(-0X4558, ctx->r14) = ctx->r9;
    // 0x80419BD0: sh          $t0, -0x4556($t5)
    MEM_H(-0X4556, ctx->r13) = ctx->r8;
    // 0x80419BD4: sh          $a3, -0x4552($t4)
    MEM_H(-0X4552, ctx->r12) = ctx->r7;
    // 0x80419BD8: sh          $a1, -0x4554($t3)
    MEM_H(-0X4554, ctx->r11) = ctx->r5;
    // 0x80419BDC: sw          $v1, -0x4570($t2)
    MEM_W(-0X4570, ctx->r10) = ctx->r3;
    // 0x80419BE0: sb          $a0, 0x69F2($a2)
    MEM_B(0X69F2, ctx->r6) = ctx->r4;
L_80419BE4:
    // 0x80419BE4: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
L_80419BE8:
    // 0x80419BE8: addiu       $a0, $zero, 0x7E40
    ctx->r4 = ADD32(0, 0X7E40);
    // 0x80419BEC: b           L_80415844
    // 0x80419BF0: sh          $a0, 0x6CE6($v1)
    MEM_H(0X6CE6, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x80419BF0: sh          $a0, 0x6CE6($v1)
    MEM_H(0X6CE6, ctx->r3) = ctx->r4;
L_80419BF4:
    // 0x80419BF4: ori         $a0, $a0, 0x1
    ctx->r4 = ctx->r4 | 0X1;
    // 0x80419BF8: b           L_804196C4
    // 0x80419BFC: sb          $a0, 0x69F1($a2)
    MEM_B(0X69F1, ctx->r6) = ctx->r4;
        goto L_804196C4;
    // 0x80419BFC: sb          $a0, 0x69F1($a2)
    MEM_B(0X69F1, ctx->r6) = ctx->r4;
L_80419C00:
    // 0x80419C00: bne         $a1, $zero, L_80419C64
    if (ctx->r5 != 0) {
        // 0x80419C04: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419C64;
    }
    // 0x80419C04: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80419C08: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80419C0C: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80419C10: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80419C14: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80419C18: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x80419C1C: lhu         $t1, -0x4558($t6)
    ctx->r9 = MEM_HU(ctx->r14, -0X4558);
    // 0x80419C20: lhu         $t0, -0x4556($t5)
    ctx->r8 = MEM_HU(ctx->r13, -0X4556);
    // 0x80419C24: lhu         $a3, -0x4552($t4)
    ctx->r7 = MEM_HU(ctx->r12, -0X4552);
    // 0x80419C28: lhu         $a1, -0x4554($t3)
    ctx->r5 = MEM_HU(ctx->r11, -0X4554);
    // 0x80419C2C: lw          $v1, -0x4570($t2)
    ctx->r3 = MEM_W(ctx->r10, -0X4570);
    // 0x80419C30: addiu       $t1, $t1, 0x19
    ctx->r9 = ADD32(ctx->r9, 0X19);
    // 0x80419C34: addiu       $t0, $t0, 0xA
    ctx->r8 = ADD32(ctx->r8, 0XA);
    // 0x80419C38: addiu       $a3, $a3, 0xA
    ctx->r7 = ADD32(ctx->r7, 0XA);
    // 0x80419C3C: addiu       $a1, $a1, 0xA
    ctx->r5 = ADD32(ctx->r5, 0XA);
    // 0x80419C40: addiu       $v1, $v1, 0x7D0
    ctx->r3 = ADD32(ctx->r3, 0X7D0);
    // 0x80419C44: ori         $a0, $a0, 0x2
    ctx->r4 = ctx->r4 | 0X2;
    // 0x80419C48: sh          $t1, -0x4558($t6)
    MEM_H(-0X4558, ctx->r14) = ctx->r9;
    // 0x80419C4C: sh          $t0, -0x4556($t5)
    MEM_H(-0X4556, ctx->r13) = ctx->r8;
    // 0x80419C50: sh          $a3, -0x4552($t4)
    MEM_H(-0X4552, ctx->r12) = ctx->r7;
    // 0x80419C54: sh          $a1, -0x4554($t3)
    MEM_H(-0X4554, ctx->r11) = ctx->r5;
    // 0x80419C58: sw          $v1, -0x4570($t2)
    MEM_W(-0X4570, ctx->r10) = ctx->r3;
    // 0x80419C5C: sb          $a0, 0x69F2($a2)
    MEM_B(0X69F2, ctx->r6) = ctx->r4;
L_80419C60:
    // 0x80419C60: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
L_80419C64:
    // 0x80419C64: addiu       $a0, $zero, 0x2620
    ctx->r4 = ADD32(0, 0X2620);
    // 0x80419C68: b           L_80415844
    // 0x80419C6C: sh          $a0, -0x6D4A($v1)
    MEM_H(-0X6D4A, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x80419C6C: sh          $a0, -0x6D4A($v1)
    MEM_H(-0X6D4A, ctx->r3) = ctx->r4;
L_80419C70:
    // 0x80419C70: andi        $v1, $a0, 0x8
    ctx->r3 = ctx->r4 & 0X8;
    // 0x80419C74: bne         $v1, $zero, L_80419CD8
    if (ctx->r3 != 0) {
        // 0x80419C78: lui         $v1, 0x801A
        ctx->r3 = S32(0X801A << 16);
            goto L_80419CD8;
    }
    // 0x80419C78: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
    // 0x80419C7C: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80419C80: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80419C84: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80419C88: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80419C8C: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x80419C90: lhu         $t1, -0x4558($t6)
    ctx->r9 = MEM_HU(ctx->r14, -0X4558);
    // 0x80419C94: lhu         $t0, -0x4556($t5)
    ctx->r8 = MEM_HU(ctx->r13, -0X4556);
    // 0x80419C98: lhu         $a3, -0x4552($t4)
    ctx->r7 = MEM_HU(ctx->r12, -0X4552);
    // 0x80419C9C: lhu         $a1, -0x4554($t3)
    ctx->r5 = MEM_HU(ctx->r11, -0X4554);
    // 0x80419CA0: lw          $v1, -0x4570($t2)
    ctx->r3 = MEM_W(ctx->r10, -0X4570);
    // 0x80419CA4: addiu       $t1, $t1, 0x20
    ctx->r9 = ADD32(ctx->r9, 0X20);
    // 0x80419CA8: addiu       $t0, $t0, 0xF
    ctx->r8 = ADD32(ctx->r8, 0XF);
    // 0x80419CAC: addiu       $a3, $a3, 0x10
    ctx->r7 = ADD32(ctx->r7, 0X10);
    // 0x80419CB0: addiu       $a1, $a1, 0x20
    ctx->r5 = ADD32(ctx->r5, 0X20);
    // 0x80419CB4: addiu       $v1, $v1, 0x1388
    ctx->r3 = ADD32(ctx->r3, 0X1388);
    // 0x80419CB8: ori         $a0, $a0, 0x8
    ctx->r4 = ctx->r4 | 0X8;
    // 0x80419CBC: sh          $t1, -0x4558($t6)
    MEM_H(-0X4558, ctx->r14) = ctx->r9;
    // 0x80419CC0: sh          $t0, -0x4556($t5)
    MEM_H(-0X4556, ctx->r13) = ctx->r8;
    // 0x80419CC4: sh          $a3, -0x4552($t4)
    MEM_H(-0X4552, ctx->r12) = ctx->r7;
    // 0x80419CC8: sh          $a1, -0x4554($t3)
    MEM_H(-0X4554, ctx->r11) = ctx->r5;
    // 0x80419CCC: sw          $v1, -0x4570($t2)
    MEM_W(-0X4570, ctx->r10) = ctx->r3;
    // 0x80419CD0: sb          $a0, 0x69F2($a2)
    MEM_B(0X69F2, ctx->r6) = ctx->r4;
L_80419CD4:
    // 0x80419CD4: lui         $v1, 0x801A
    ctx->r3 = S32(0X801A << 16);
L_80419CD8:
    // 0x80419CD8: addiu       $a0, $zero, 0x4700
    ctx->r4 = ADD32(0, 0X4700);
    // 0x80419CDC: b           L_80415844
    // 0x80419CE0: sh          $a0, 0x3D5A($v1)
    MEM_H(0X3D5A, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x80419CE0: sh          $a0, 0x3D5A($v1)
    MEM_H(0X3D5A, ctx->r3) = ctx->r4;
L_80419CE4:
    // 0x80419CE4: bnel        $a2, $zero, L_80418DA4
    if (ctx->r6 != 0) {
        // 0x80419CE8: addiu       $v1, $zero, -0x3C00
        ctx->r3 = ADD32(0, -0X3C00);
            goto L_80418DA4;
    }
    goto skip_75;
    // 0x80419CE8: addiu       $v1, $zero, -0x3C00
    ctx->r3 = ADD32(0, -0X3C00);
    skip_75:
    // 0x80419CEC: lui         $t9, 0x8008
    ctx->r25 = S32(0X8008 << 16);
    // 0x80419CF0: lui         $t8, 0x8008
    ctx->r24 = S32(0X8008 << 16);
    // 0x80419CF4: lui         $t7, 0x8008
    ctx->r15 = S32(0X8008 << 16);
    // 0x80419CF8: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80419CFC: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80419D00: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80419D04: lhu         $t3, -0x4558($t9)
    ctx->r11 = MEM_HU(ctx->r25, -0X4558);
    // 0x80419D08: lhu         $t2, -0x4556($t8)
    ctx->r10 = MEM_HU(ctx->r24, -0X4556);
    // 0x80419D0C: lhu         $t1, -0x4552($t7)
    ctx->r9 = MEM_HU(ctx->r15, -0X4552);
    // 0x80419D10: lhu         $t0, -0x4554($t6)
    ctx->r8 = MEM_HU(ctx->r14, -0X4554);
    // 0x80419D14: lw          $a3, -0x4570($t5)
    ctx->r7 = MEM_W(ctx->r13, -0X4570);
    // 0x80419D18: lw          $a2, -0x456C($t4)
    ctx->r6 = MEM_W(ctx->r12, -0X456C);
    // 0x80419D1C: ori         $a1, $a1, 0x20
    ctx->r5 = ctx->r5 | 0X20;
    // 0x80419D20: sb          $a1, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r5;
    // 0x80419D24: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419D28: addiu       $a1, $zero, 0x1A
    ctx->r5 = ADD32(0, 0X1A);
    // 0x80419D2C: addiu       $t3, $t3, 0x28
    ctx->r11 = ADD32(ctx->r11, 0X28);
    // 0x80419D30: addiu       $t2, $t2, 0x28
    ctx->r10 = ADD32(ctx->r10, 0X28);
    // 0x80419D34: addiu       $t1, $t1, 0x28
    ctx->r9 = ADD32(ctx->r9, 0X28);
    // 0x80419D38: addiu       $t0, $t0, 0x28
    ctx->r8 = ADD32(ctx->r8, 0X28);
    // 0x80419D3C: addiu       $a3, $a3, 0x3A98
    ctx->r7 = ADD32(ctx->r7, 0X3A98);
    // 0x80419D40: addiu       $a2, $a2, 0x3E8
    ctx->r6 = ADD32(ctx->r6, 0X3E8);
    // 0x80419D44: sb          $a1, -0x458D($v1)
    MEM_B(-0X458D, ctx->r3) = ctx->r5;
    // 0x80419D48: addiu       $v1, $zero, -0x3C00
    ctx->r3 = ADD32(0, -0X3C00);
    // 0x80419D4C: sh          $t3, -0x4558($t9)
    MEM_H(-0X4558, ctx->r25) = ctx->r11;
    // 0x80419D50: sh          $t2, -0x4556($t8)
    MEM_H(-0X4556, ctx->r24) = ctx->r10;
    // 0x80419D54: sh          $t1, -0x4552($t7)
    MEM_H(-0X4552, ctx->r15) = ctx->r9;
    // 0x80419D58: sh          $t0, -0x4554($t6)
    MEM_H(-0X4554, ctx->r14) = ctx->r8;
    // 0x80419D5C: sw          $a3, -0x4570($t5)
    MEM_W(-0X4570, ctx->r13) = ctx->r7;
    // 0x80419D60: sw          $a2, -0x456C($t4)
    MEM_W(-0X456C, ctx->r12) = ctx->r6;
    // 0x80419D64: b           L_804174C0
    // 0x80419D68: sh          $v1, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r3;
        goto L_804174C0;
    // 0x80419D68: sh          $v1, 0x32B2($a0)
    MEM_H(0X32B2, ctx->r4) = ctx->r3;
L_80419D6C:
    // 0x80419D6C: andi        $v1, $a0, 0x4
    ctx->r3 = ctx->r4 & 0X4;
    // 0x80419D70: bne         $v1, $zero, L_80419DD8
    if (ctx->r3 != 0) {
        // 0x80419D74: lui         $v1, 0x8017
        ctx->r3 = S32(0X8017 << 16);
            goto L_80419DD8;
    }
    // 0x80419D74: lui         $v1, 0x8017
    ctx->r3 = S32(0X8017 << 16);
    // 0x80419D78: lui         $t6, 0x8008
    ctx->r14 = S32(0X8008 << 16);
    // 0x80419D7C: lui         $t5, 0x8008
    ctx->r13 = S32(0X8008 << 16);
    // 0x80419D80: lui         $t4, 0x8008
    ctx->r12 = S32(0X8008 << 16);
    // 0x80419D84: lui         $t3, 0x8008
    ctx->r11 = S32(0X8008 << 16);
    // 0x80419D88: lui         $t2, 0x8008
    ctx->r10 = S32(0X8008 << 16);
    // 0x80419D8C: lhu         $t1, -0x4558($t6)
    ctx->r9 = MEM_HU(ctx->r14, -0X4558);
    // 0x80419D90: lhu         $t0, -0x4556($t5)
    ctx->r8 = MEM_HU(ctx->r13, -0X4556);
    // 0x80419D94: lhu         $a3, -0x4552($t4)
    ctx->r7 = MEM_HU(ctx->r12, -0X4552);
    // 0x80419D98: lhu         $a1, -0x4554($t3)
    ctx->r5 = MEM_HU(ctx->r11, -0X4554);
    // 0x80419D9C: lw          $v1, -0x4570($t2)
    ctx->r3 = MEM_W(ctx->r10, -0X4570);
    // 0x80419DA0: ori         $t7, $zero, 0x9C40
    ctx->r15 = 0 | 0X9C40;
    // 0x80419DA4: addiu       $t1, $t1, 0x32
    ctx->r9 = ADD32(ctx->r9, 0X32);
    // 0x80419DA8: addiu       $t0, $t0, 0x1E
    ctx->r8 = ADD32(ctx->r8, 0X1E);
    // 0x80419DAC: addiu       $a3, $a3, 0x40
    ctx->r7 = ADD32(ctx->r7, 0X40);
    // 0x80419DB0: addiu       $a1, $a1, 0x20
    ctx->r5 = ADD32(ctx->r5, 0X20);
    // 0x80419DB4: addu        $v1, $v1, $t7
    ctx->r3 = ADD32(ctx->r3, ctx->r15);
    // 0x80419DB8: ori         $a0, $a0, 0x4
    ctx->r4 = ctx->r4 | 0X4;
    // 0x80419DBC: sh          $t1, -0x4558($t6)
    MEM_H(-0X4558, ctx->r14) = ctx->r9;
    // 0x80419DC0: sh          $t0, -0x4556($t5)
    MEM_H(-0X4556, ctx->r13) = ctx->r8;
    // 0x80419DC4: sh          $a3, -0x4552($t4)
    MEM_H(-0X4552, ctx->r12) = ctx->r7;
    // 0x80419DC8: sh          $a1, -0x4554($t3)
    MEM_H(-0X4554, ctx->r11) = ctx->r5;
    // 0x80419DCC: sw          $v1, -0x4570($t2)
    MEM_W(-0X4570, ctx->r10) = ctx->r3;
    // 0x80419DD0: sb          $a0, 0x69F2($a2)
    MEM_B(0X69F2, ctx->r6) = ctx->r4;
L_80419DD4:
    // 0x80419DD4: lui         $v1, 0x8017
    ctx->r3 = S32(0X8017 << 16);
L_80419DD8:
    // 0x80419DD8: addiu       $a0, $zero, -0x5F88
    ctx->r4 = ADD32(0, -0X5F88);
    // 0x80419DDC: b           L_80415844
    // 0x80419DE0: sh          $a0, 0x18AE($v1)
    MEM_H(0X18AE, ctx->r3) = ctx->r4;
        goto L_80415844;
    // 0x80419DE0: sh          $a0, 0x18AE($v1)
    MEM_H(0X18AE, ctx->r3) = ctx->r4;
L_80419DE4:
    // 0x80419DE4: addiu       $v0, $v0, 0x5000
    ctx->r2 = ADD32(ctx->r2, 0X5000);
    // 0x80419DE8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419DEC: sw          $v0, -0x42BC($v1)
    MEM_W(-0X42BC, ctx->r3) = ctx->r2;
    // 0x80419DF0: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419DF4: sw          $v0, -0x42B4($v1)
    MEM_W(-0X42B4, ctx->r3) = ctx->r2;
    // 0x80419DF8: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419DFC: sw          $v0, -0x4238($v1)
    MEM_W(-0X4238, ctx->r3) = ctx->r2;
    // 0x80419E00: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419E04: b           L_80415F44
    // 0x80419E08: sw          $v0, -0x4230($v1)
    MEM_W(-0X4230, ctx->r3) = ctx->r2;
        goto L_80415F44;
    // 0x80419E08: sw          $v0, -0x4230($v1)
    MEM_W(-0X4230, ctx->r3) = ctx->r2;
L_80419E0C:
    // 0x80419E0C: lbu         $a1, 0x69E5($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E5);
    // 0x80419E10: ori         $a1, $a1, 0x8
    ctx->r5 = ctx->r5 | 0X8;
    // 0x80419E14: b           L_80418D64
    // 0x80419E18: sb          $a1, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r5;
        goto L_80418D64;
    // 0x80419E18: sb          $a1, 0x69E5($v1)
    MEM_B(0X69E5, ctx->r3) = ctx->r5;
L_80419E1C:
    // 0x80419E1C: lbu         $a1, 0x69E4($v1)
    ctx->r5 = MEM_BU(ctx->r3, 0X69E4);
    // 0x80419E20: ori         $a1, $a1, 0x20
    ctx->r5 = ctx->r5 | 0X20;
    // 0x80419E24: b           L_80418BF8
    // 0x80419E28: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
        goto L_80418BF8;
    // 0x80419E28: sb          $a1, 0x69E4($v1)
    MEM_B(0X69E4, ctx->r3) = ctx->r5;
L_80419E2C:
    // 0x80419E2C: addiu       $t3, $zero, -0x44A0
    ctx->r11 = ADD32(0, -0X44A0);
    // 0x80419E30: beq         $a0, $zero, L_804160A4
    if (ctx->r4 == 0) {
        // 0x80419E34: sh          $t3, -0x24B6($t2)
        MEM_H(-0X24B6, ctx->r10) = ctx->r11;
            goto L_804160A4;
    }
    // 0x80419E34: sh          $t3, -0x24B6($t2)
    MEM_H(-0X24B6, ctx->r10) = ctx->r11;
    // 0x80419E38: b           L_804199E8
    // 0x80419E3C: nop

        goto L_804199E8;
    // 0x80419E3C: nop

L_80419E40:
    // 0x80419E40: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419E44: lhu         $a1, 0x2876($a1)
    ctx->r5 = MEM_HU(ctx->r5, 0X2876);
    // 0x80419E48: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x80419E4C: beq         $a1, $a3, L_80419F24
    if (ctx->r5 == ctx->r7) {
        // 0x80419E50: addiu       $a3, $zero, 0x2
        ctx->r7 = ADD32(0, 0X2);
            goto L_80419F24;
    }
    // 0x80419E50: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x80419E54: beql        $a1, $a3, L_80419F0C
    if (ctx->r5 == ctx->r7) {
        // 0x80419E58: sltiu       $a1, $v1, 0x4B
        ctx->r5 = ctx->r3 < 0X4B ? 1 : 0;
            goto L_80419F0C;
    }
    goto skip_76;
    // 0x80419E58: sltiu       $a1, $v1, 0x4B
    ctx->r5 = ctx->r3 < 0X4B ? 1 : 0;
    skip_76:
    // 0x80419E5C: addiu       $a3, $zero, 0x4
    ctx->r7 = ADD32(0, 0X4);
    // 0x80419E60: bnel        $a1, $a3, L_80419EEC
    if (ctx->r5 != ctx->r7) {
        // 0x80419E64: addiu       $a3, $zero, 0x1
        ctx->r7 = ADD32(0, 0X1);
            goto L_80419EEC;
    }
    goto skip_77;
    // 0x80419E64: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    skip_77:
    // 0x80419E68: sltiu       $a1, $v1, 0xAF
    ctx->r5 = ctx->r3 < 0XAF ? 1 : 0;
    // 0x80419E6C: bnel        $a1, $zero, L_80416370
    if (ctx->r5 != 0) {
        // 0x80419E70: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80416370;
    }
    goto skip_78;
    // 0x80419E70: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_78:
    // 0x80419E74: addiu       $v1, $v1, -0xAF
    ctx->r3 = ADD32(ctx->r3, -0XAF);
    // 0x80419E78: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419E7C: addiu       $a3, $zero, 0x6
    ctx->r7 = ADD32(0, 0X6);
L_80419E80:
    // 0x80419E80: sb          $a3, -0x458D($a1)
    MEM_B(-0X458D, ctx->r5) = ctx->r7;
    // 0x80419E84: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419E88: sw          $zero, 0x5C0($a1)
    MEM_W(0X5C0, ctx->r5) = 0;
    // 0x80419E8C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x80419E90: sw          $zero, 0x5E0($a1)
    MEM_W(0X5E0, ctx->r5) = 0;
    // 0x80419E94: b           L_8041636C
    // 0x80419E98: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
        goto L_8041636C;
    // 0x80419E98: sw          $v1, -0x456C($a2)
    MEM_W(-0X456C, ctx->r6) = ctx->r3;
L_80419E9C:
    // 0x80419E9C: bnel        $a1, $zero, L_80418830
    if (ctx->r5 != 0) {
        // 0x80419EA0: lbu         $v1, 0x69E5($v1)
        ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
            goto L_80418830;
    }
    goto skip_79;
    // 0x80419EA0: lbu         $v1, 0x69E5($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X69E5);
    skip_79:
    // 0x80419EA4: addiu       $a3, $zero, -0x4400
    ctx->r7 = ADD32(0, -0X4400);
    // 0x80419EA8: b           L_80418808
    // 0x80419EAC: sh          $a3, -0x54D6($a2)
    MEM_H(-0X54D6, ctx->r6) = ctx->r7;
        goto L_80418808;
    // 0x80419EAC: sh          $a3, -0x54D6($a2)
    MEM_H(-0X54D6, ctx->r6) = ctx->r7;
L_80419EB0:
    // 0x80419EB0: lui         $t1, 0x8000
    ctx->r9 = S32(0X8000 << 16);
    // 0x80419EB4: trunc.w.d   $f0, $f0
    CHECK_FR(ctx, 0);
    CHECK_FR(ctx, 0);
    ctx->f0.u32l = TRUNC_W_D(ctx->f0.d);
    // 0x80419EB8: mfc1        $a0, $f0
    ctx->r4 = (int32_t)ctx->f0.u32l;
    // 0x80419EBC: b           L_80416204
    // 0x80419EC0: or          $a0, $a0, $t1
    ctx->r4 = ctx->r4 | ctx->r9;
        goto L_80416204;
    // 0x80419EC0: or          $a0, $a0, $t1
    ctx->r4 = ctx->r4 | ctx->r9;
L_80419EC4:
    // 0x80419EC4: lui         $v0, 0x8015
    ctx->r2 = S32(0X8015 << 16);
    // 0x80419EC8: addiu       $v1, $zero, -0x100
    ctx->r3 = ADD32(0, -0X100);
    // 0x80419ECC: b           L_80415F44
    // 0x80419ED0: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
        goto L_80415F44;
    // 0x80419ED0: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
L_80419ED4:
    // 0x80419ED4: sltiu       $v0, $v0, 0x18
    ctx->r2 = ctx->r2 < 0X18 ? 1 : 0;
    // 0x80419ED8: beq         $v0, $zero, L_8041A0B8
    if (ctx->r2 == 0) {
        // 0x80419EDC: lui         $v0, 0x8015
        ctx->r2 = S32(0X8015 << 16);
            goto L_8041A0B8;
    }
    // 0x80419EDC: lui         $v0, 0x8015
    ctx->r2 = S32(0X8015 << 16);
    // 0x80419EE0: addiu       $v1, $zero, 0xA40
    ctx->r3 = ADD32(0, 0XA40);
    // 0x80419EE4: b           L_80415F44
    // 0x80419EE8: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
        goto L_80415F44;
    // 0x80419EE8: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
L_80419EEC:
    // 0x80419EEC: bne         $a1, $a3, L_8041636C
    if (ctx->r5 != ctx->r7) {
        // 0x80419EF0: sltiu       $a1, $v1, 0xFA
        ctx->r5 = ctx->r3 < 0XFA ? 1 : 0;
            goto L_8041636C;
    }
    // 0x80419EF0: sltiu       $a1, $v1, 0xFA
    ctx->r5 = ctx->r3 < 0XFA ? 1 : 0;
    // 0x80419EF4: bnel        $a1, $zero, L_80416370
    if (ctx->r5 != 0) {
        // 0x80419EF8: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80416370;
    }
    goto skip_80;
    // 0x80419EF8: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_80:
    // 0x80419EFC: addiu       $v1, $v1, -0xFA
    ctx->r3 = ADD32(ctx->r3, -0XFA);
    // 0x80419F00: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419F04: b           L_80419E80
    // 0x80419F08: addiu       $a3, $zero, 0xB
    ctx->r7 = ADD32(0, 0XB);
        goto L_80419E80;
    // 0x80419F08: addiu       $a3, $zero, 0xB
    ctx->r7 = ADD32(0, 0XB);
L_80419F0C:
    // 0x80419F0C: bnel        $a1, $zero, L_80416370
    if (ctx->r5 != 0) {
        // 0x80419F10: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80416370;
    }
    goto skip_81;
    // 0x80419F10: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_81:
    // 0x80419F14: addiu       $v1, $v1, -0x4B
    ctx->r3 = ADD32(ctx->r3, -0X4B);
    // 0x80419F18: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419F1C: b           L_80419E80
    // 0x80419F20: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
        goto L_80419E80;
    // 0x80419F20: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
L_80419F24:
    // 0x80419F24: sltiu       $a1, $v1, 0x32
    ctx->r5 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x80419F28: bnel        $a1, $zero, L_80419984
    if (ctx->r5 != 0) {
        // 0x80419F2C: addiu       $v1, $zero, 0x5AD0
        ctx->r3 = ADD32(0, 0X5AD0);
            goto L_80419984;
    }
    goto skip_82;
    // 0x80419F2C: addiu       $v1, $zero, 0x5AD0
    ctx->r3 = ADD32(0, 0X5AD0);
    skip_82:
    // 0x80419F30: addiu       $v1, $v1, -0x32
    ctx->r3 = ADD32(ctx->r3, -0X32);
    // 0x80419F34: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419F38: b           L_80419E80
    // 0x80419F3C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
        goto L_80419E80;
    // 0x80419F3C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
L_80419F40:
    // 0x80419F40: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419F44: lw          $a1, 0x4EE8($v1)
    ctx->r5 = MEM_W(ctx->r3, 0X4EE8);
    // 0x80419F48: addiu       $v1, $zero, -0x9
    ctx->r3 = ADD32(0, -0X9);
    // 0x80419F4C: and         $v1, $a1, $v1
    ctx->r3 = ctx->r5 & ctx->r3;
    // 0x80419F50: beq         $v1, $zero, L_80417DFC
    if (ctx->r3 == 0) {
        // 0x80419F54: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80417DFC;
    }
    // 0x80419F54: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419F58: addiu       $v1, $zero, 0x16
    ctx->r3 = ADD32(0, 0X16);
    // 0x80419F5C: bne         $a1, $v1, L_80415DF4
    if (ctx->r5 != ctx->r3) {
        // 0x80419F60: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80415DF4;
    }
    // 0x80419F60: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x80419F64: b           L_80417E00
    // 0x80419F68: lui         $a1, 0x2
    ctx->r5 = S32(0X2 << 16);
        goto L_80417E00;
    // 0x80419F68: lui         $a1, 0x2
    ctx->r5 = S32(0X2 << 16);
L_80419F6C:
    // 0x80419F6C: bnel        $a3, $zero, L_80418EA4
    if (ctx->r7 != 0) {
        // 0x80419F70: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80418EA4;
    }
    goto skip_83;
    // 0x80419F70: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_83:
    // 0x80419F74: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x80419F78: addiu       $v1, $v1, -0x96
    ctx->r3 = ADD32(ctx->r3, -0X96);
    // 0x80419F7C: b           L_80419A54
    // 0x80419F80: sb          $a1, -0x458D($a3)
    MEM_B(-0X458D, ctx->r7) = ctx->r5;
        goto L_80419A54;
    // 0x80419F80: sb          $a1, -0x458D($a3)
    MEM_B(-0X458D, ctx->r7) = ctx->r5;
L_80419F84:
    // 0x80419F84: sltiu       $a1, $v1, 0x32
    ctx->r5 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x80419F88: bnel        $a1, $zero, L_804199A4
    if (ctx->r5 != 0) {
        // 0x80419F8C: addiu       $v1, $zero, 0x61D0
        ctx->r3 = ADD32(0, 0X61D0);
            goto L_804199A4;
    }
    goto skip_84;
    // 0x80419F8C: addiu       $v1, $zero, 0x61D0
    ctx->r3 = ADD32(0, 0X61D0);
    skip_84:
    // 0x80419F90: addiu       $v1, $v1, -0x32
    ctx->r3 = ADD32(ctx->r3, -0X32);
    // 0x80419F94: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419F98: b           L_80419A50
    // 0x80419F9C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
        goto L_80419A50;
    // 0x80419F9C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
L_80419FA0:
    // 0x80419FA0: bne         $a1, $a3, L_80417E84
    if (ctx->r5 != ctx->r7) {
        // 0x80419FA4: sltiu       $a1, $v1, 0xC8
        ctx->r5 = ctx->r3 < 0XC8 ? 1 : 0;
            goto L_80417E84;
    }
    // 0x80419FA4: sltiu       $a1, $v1, 0xC8
    ctx->r5 = ctx->r3 < 0XC8 ? 1 : 0;
    // 0x80419FA8: bnel        $a1, $zero, L_80417E88
    if (ctx->r5 != 0) {
        // 0x80419FAC: sltiu       $v1, $v1, 0x96
        ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
            goto L_80417E88;
    }
    goto skip_85;
    // 0x80419FAC: sltiu       $v1, $v1, 0x96
    ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
    skip_85:
    // 0x80419FB0: addiu       $v1, $v1, -0xC8
    ctx->r3 = ADD32(ctx->r3, -0XC8);
    // 0x80419FB4: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419FB8: b           L_80419AA8
    // 0x80419FBC: addiu       $a3, $zero, 0xA
    ctx->r7 = ADD32(0, 0XA);
        goto L_80419AA8;
    // 0x80419FBC: addiu       $a3, $zero, 0xA
    ctx->r7 = ADD32(0, 0XA);
L_80419FC0:
    // 0x80419FC0: bnel        $a1, $zero, L_80417E88
    if (ctx->r5 != 0) {
        // 0x80419FC4: sltiu       $v1, $v1, 0x96
        ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
            goto L_80417E88;
    }
    goto skip_86;
    // 0x80419FC4: sltiu       $v1, $v1, 0x96
    ctx->r3 = ctx->r3 < 0X96 ? 1 : 0;
    skip_86:
    // 0x80419FC8: addiu       $v1, $v1, -0xAF
    ctx->r3 = ADD32(ctx->r3, -0XAF);
    // 0x80419FCC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419FD0: b           L_80419AA8
    // 0x80419FD4: addiu       $a3, $zero, 0x6
    ctx->r7 = ADD32(0, 0X6);
        goto L_80419AA8;
    // 0x80419FD4: addiu       $a3, $zero, 0x6
    ctx->r7 = ADD32(0, 0X6);
L_80419FD8:
    // 0x80419FD8: sltiu       $a1, $v1, 0x96
    ctx->r5 = ctx->r3 < 0X96 ? 1 : 0;
    // 0x80419FDC: bnel        $a1, $zero, L_8041998C
    if (ctx->r5 != 0) {
        // 0x80419FE0: addiu       $v1, $zero, 0x7570
        ctx->r3 = ADD32(0, 0X7570);
            goto L_8041998C;
    }
    goto skip_87;
    // 0x80419FE0: addiu       $v1, $zero, 0x7570
    ctx->r3 = ADD32(0, 0X7570);
    skip_87:
    // 0x80419FE4: addiu       $v1, $v1, -0x96
    ctx->r3 = ADD32(ctx->r3, -0X96);
    // 0x80419FE8: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x80419FEC: b           L_80419AA8
    // 0x80419FF0: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
        goto L_80419AA8;
    // 0x80419FF0: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
L_80419FF4:
    // 0x80419FF4: bne         $a1, $a3, L_804163D0
    if (ctx->r5 != ctx->r7) {
        // 0x80419FF8: sltiu       $a1, $v1, 0xC8
        ctx->r5 = ctx->r3 < 0XC8 ? 1 : 0;
            goto L_804163D0;
    }
    // 0x80419FF8: sltiu       $a1, $v1, 0xC8
    ctx->r5 = ctx->r3 < 0XC8 ? 1 : 0;
    // 0x80419FFC: bnel        $a1, $zero, L_804163D4
    if (ctx->r5 != 0) {
        // 0x8041A000: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_804163D4;
    }
    goto skip_88;
    // 0x8041A000: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_88:
    // 0x8041A004: addiu       $v1, $v1, -0xC8
    ctx->r3 = ADD32(ctx->r3, -0XC8);
    // 0x8041A008: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A00C: b           L_80419B04
    // 0x8041A010: addiu       $a3, $zero, 0xA
    ctx->r7 = ADD32(0, 0XA);
        goto L_80419B04;
    // 0x8041A010: addiu       $a3, $zero, 0xA
    ctx->r7 = ADD32(0, 0XA);
L_8041A014:
    // 0x8041A014: bnel        $a1, $zero, L_804163D4
    if (ctx->r5 != 0) {
        // 0x8041A018: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_804163D4;
    }
    goto skip_89;
    // 0x8041A018: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_89:
    // 0x8041A01C: addiu       $v1, $v1, -0x4B
    ctx->r3 = ADD32(ctx->r3, -0X4B);
    // 0x8041A020: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A024: b           L_80419B04
    // 0x8041A028: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
        goto L_80419B04;
    // 0x8041A028: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
L_8041A02C:
    // 0x8041A02C: sltiu       $a1, $v1, 0x32
    ctx->r5 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x8041A030: bnel        $a1, $zero, L_80419994
    if (ctx->r5 != 0) {
        // 0x8041A034: addiu       $v1, $zero, 0x3A68
        ctx->r3 = ADD32(0, 0X3A68);
            goto L_80419994;
    }
    goto skip_90;
    // 0x8041A034: addiu       $v1, $zero, 0x3A68
    ctx->r3 = ADD32(0, 0X3A68);
    skip_90:
    // 0x8041A038: addiu       $v1, $v1, -0x32
    ctx->r3 = ADD32(ctx->r3, -0X32);
    // 0x8041A03C: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A040: b           L_80419B04
    // 0x8041A044: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
        goto L_80419B04;
    // 0x8041A044: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
L_8041A048:
    // 0x8041A048: bne         $a1, $t0, L_80418EA0
    if (ctx->r5 != ctx->r8) {
        // 0x8041A04C: sltiu       $a1, $v1, 0x113
        ctx->r5 = ctx->r3 < 0X113 ? 1 : 0;
            goto L_80418EA0;
    }
    // 0x8041A04C: sltiu       $a1, $v1, 0x113
    ctx->r5 = ctx->r3 < 0X113 ? 1 : 0;
    // 0x8041A050: bnel        $a1, $zero, L_80418EA4
    if (ctx->r5 != 0) {
        // 0x8041A054: sltiu       $v1, $v1, 0x32
        ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
            goto L_80418EA4;
    }
    goto skip_91;
    // 0x8041A054: sltiu       $v1, $v1, 0x32
    ctx->r3 = ctx->r3 < 0X32 ? 1 : 0;
    skip_91:
    // 0x8041A058: addiu       $v1, $v1, -0x113
    ctx->r3 = ADD32(ctx->r3, -0X113);
    // 0x8041A05C: b           L_80419A50
    // 0x8041A060: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
        goto L_80419A50;
    // 0x8041A060: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
L_8041A064:
    // 0x8041A064: bne         $a1, $a3, L_80417FA8
    if (ctx->r5 != ctx->r7) {
        // 0x8041A068: sltiu       $a1, $v1, 0xFA
        ctx->r5 = ctx->r3 < 0XFA ? 1 : 0;
            goto L_80417FA8;
    }
    // 0x8041A068: sltiu       $a1, $v1, 0xFA
    ctx->r5 = ctx->r3 < 0XFA ? 1 : 0;
    // 0x8041A06C: bnel        $a1, $zero, L_8041999C
    if (ctx->r5 != 0) {
        // 0x8041A070: addiu       $v1, $zero, -0x4B70
        ctx->r3 = ADD32(0, -0X4B70);
            goto L_8041999C;
    }
    goto skip_92;
    // 0x8041A070: addiu       $v1, $zero, -0x4B70
    ctx->r3 = ADD32(0, -0X4B70);
    skip_92:
    // 0x8041A074: addiu       $v1, $v1, -0xFA
    ctx->r3 = ADD32(ctx->r3, -0XFA);
    // 0x8041A078: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A07C: b           L_80419B60
    // 0x8041A080: addiu       $a3, $zero, 0xB
    ctx->r7 = ADD32(0, 0XB);
        goto L_80419B60;
    // 0x8041A080: addiu       $a3, $zero, 0xB
    ctx->r7 = ADD32(0, 0XB);
L_8041A084:
    // 0x8041A084: bnel        $a1, $zero, L_80417FAC
    if (ctx->r5 != 0) {
        // 0x8041A088: sltiu       $v1, $v1, 0xFA
        ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
            goto L_80417FAC;
    }
    goto skip_93;
    // 0x8041A088: sltiu       $v1, $v1, 0xFA
    ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
    skip_93:
    // 0x8041A08C: addiu       $v1, $v1, -0x15E
    ctx->r3 = ADD32(ctx->r3, -0X15E);
    // 0x8041A090: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A094: b           L_80419B60
    // 0x8041A098: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
        goto L_80419B60;
    // 0x8041A098: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
L_8041A09C:
    // 0x8041A09C: sltiu       $a1, $v1, 0x12C
    ctx->r5 = ctx->r3 < 0X12C ? 1 : 0;
    // 0x8041A0A0: bnel        $a1, $zero, L_80417FAC
    if (ctx->r5 != 0) {
        // 0x8041A0A4: sltiu       $v1, $v1, 0xFA
        ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
            goto L_80417FAC;
    }
    goto skip_94;
    // 0x8041A0A4: sltiu       $v1, $v1, 0xFA
    ctx->r3 = ctx->r3 < 0XFA ? 1 : 0;
    skip_94:
    // 0x8041A0A8: addiu       $v1, $v1, -0x12C
    ctx->r3 = ADD32(ctx->r3, -0X12C);
    // 0x8041A0AC: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A0B0: b           L_80419B60
    // 0x8041A0B4: addiu       $a3, $zero, 0x3
    ctx->r7 = ADD32(0, 0X3);
        goto L_80419B60;
    // 0x8041A0B4: addiu       $a3, $zero, 0x3
    ctx->r7 = ADD32(0, 0X3);
L_8041A0B8:
    // 0x8041A0B8: addiu       $v1, $zero, -0x300
    ctx->r3 = ADD32(0, -0X300);
    // 0x8041A0BC: b           L_80415F44
    // 0x8041A0C0: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
        goto L_80415F44;
    // 0x8041A0C0: sh          $v1, 0x2B88($v0)
    MEM_H(0X2B88, ctx->r2) = ctx->r3;
L_8041A0C4:
    // 0x8041A0C4: addiu       $v1, $zero, 0x1E
    ctx->r3 = ADD32(0, 0X1E);
    // 0x8041A0C8: beq         $v0, $v1, L_80417DFC
    if (ctx->r2 == ctx->r3) {
        // 0x8041A0CC: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_80417DFC;
    }
    // 0x8041A0CC: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041A0D0: b           L_80415DF4
    // 0x8041A0D4: nop

        goto L_80415DF4;
    // 0x8041A0D4: nop

L_8041A0D8:
    // 0x8041A0D8: bne         $a2, $a0, L_80415DF4
    if (ctx->r6 != ctx->r4) {
        // 0x8041A0DC: lui         $a2, 0x8008
        ctx->r6 = S32(0X8008 << 16);
            goto L_80415DF4;
    }
    // 0x8041A0DC: lui         $a2, 0x8008
    ctx->r6 = S32(0X8008 << 16);
    // 0x8041A0E0: lbu         $a0, -0x455C($a2)
    ctx->r4 = MEM_BU(ctx->r6, -0X455C);
    // 0x8041A0E4: addiu       $v1, $v1, -0x2710
    ctx->r3 = ADD32(ctx->r3, -0X2710);
    // 0x8041A0E8: addiu       $a0, $a0, -0x1
    ctx->r4 = ADD32(ctx->r4, -0X1);
    // 0x8041A0EC: sh          $a0, 0x69FE($v0)
    MEM_H(0X69FE, ctx->r2) = ctx->r4;
    // 0x8041A0F0: b           L_80417D68
    // 0x8041A0F4: sb          $a3, -0x455C($a2)
    MEM_B(-0X455C, ctx->r6) = ctx->r7;
        goto L_80417D68;
    // 0x8041A0F4: sb          $a3, -0x455C($a2)
    MEM_B(-0X455C, ctx->r6) = ctx->r7;
L_8041A0F8:
    // 0x8041A0F8: lbu         $a0, -0x455A($a2)
    ctx->r4 = MEM_BU(ctx->r6, -0X455A);
    // 0x8041A0FC: addiu       $v1, $v1, -0x2710
    ctx->r3 = ADD32(ctx->r3, -0X2710);
    // 0x8041A100: addiu       $a0, $a0, -0x1
    ctx->r4 = ADD32(ctx->r4, -0X1);
    // 0x8041A104: sh          $a0, 0x69FE($v0)
    MEM_H(0X69FE, ctx->r2) = ctx->r4;
    // 0x8041A108: b           L_80417D68
    // 0x8041A10C: sb          $a3, -0x455A($a2)
    MEM_B(-0X455A, ctx->r6) = ctx->r7;
        goto L_80417D68;
    // 0x8041A10C: sb          $a3, -0x455A($a2)
    MEM_B(-0X455A, ctx->r6) = ctx->r7;
L_8041A110:
    // 0x8041A110: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x8041A114: lbu         $a0, -0x4559($a3)
    ctx->r4 = MEM_BU(ctx->r7, -0X4559);
    // 0x8041A118: addiu       $v1, $v1, -0x2710
    ctx->r3 = ADD32(ctx->r3, -0X2710);
    // 0x8041A11C: addiu       $a0, $a0, -0x1
    ctx->r4 = ADD32(ctx->r4, -0X1);
    // 0x8041A120: sh          $a0, 0x69FE($v0)
    MEM_H(0X69FE, ctx->r2) = ctx->r4;
    // 0x8041A124: b           L_80417D68
    // 0x8041A128: sb          $a2, -0x4559($a3)
    MEM_B(-0X4559, ctx->r7) = ctx->r6;
        goto L_80417D68;
    // 0x8041A128: sb          $a2, -0x4559($a3)
    MEM_B(-0X4559, ctx->r7) = ctx->r6;
L_8041A12C:
    // 0x8041A12C: addiu       $v1, $zero, 0x1D
    ctx->r3 = ADD32(0, 0X1D);
    // 0x8041A130: bne         $v0, $v1, L_8041579C
    if (ctx->r2 != ctx->r3) {
        // 0x8041A134: lui         $v1, 0x8008
        ctx->r3 = S32(0X8008 << 16);
            goto L_8041579C;
    }
    // 0x8041A134: lui         $v1, 0x8008
    ctx->r3 = S32(0X8008 << 16);
    // 0x8041A138: lw          $v1, 0x4EE8($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X4EE8);
    // 0x8041A13C: bne         $v1, $zero, L_80416074
    if (ctx->r3 != 0) {
        // 0x8041A140: addiu       $v1, $zero, 0x12
        ctx->r3 = ADD32(0, 0X12);
            goto L_80416074;
    }
    // 0x8041A140: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
    // 0x8041A144: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041A148: lbu         $v1, 0x69E4($a0)
    ctx->r3 = MEM_BU(ctx->r4, 0X69E4);
    // 0x8041A14C: andi        $a1, $v1, 0x20
    ctx->r5 = ctx->r3 & 0X20;
    // 0x8041A150: beql        $a1, $zero, L_80416074
    if (ctx->r5 == 0) {
        // 0x8041A154: addiu       $v1, $zero, 0x12
        ctx->r3 = ADD32(0, 0X12);
            goto L_80416074;
    }
    goto skip_95;
    // 0x8041A154: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
    skip_95:
    // 0x8041A158: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A15C: lbu         $a2, -0x35F3($a1)
    ctx->r6 = MEM_BU(ctx->r5, -0X35F3);
    // 0x8041A160: addiu       $a1, $zero, 0x9
    ctx->r5 = ADD32(0, 0X9);
    // 0x8041A164: bnel        $a2, $a1, L_80416074
    if (ctx->r6 != ctx->r5) {
        // 0x8041A168: addiu       $v1, $zero, 0x12
        ctx->r3 = ADD32(0, 0X12);
            goto L_80416074;
    }
    goto skip_96;
    // 0x8041A168: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
    skip_96:
    // 0x8041A16C: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A170: lhu         $a1, -0x365E($a1)
    ctx->r5 = MEM_HU(ctx->r5, -0X365E);
    // 0x8041A174: bnel        $a1, $zero, L_80416074
    if (ctx->r5 != 0) {
        // 0x8041A178: addiu       $v1, $zero, 0x12
        ctx->r3 = ADD32(0, 0X12);
            goto L_80416074;
    }
    goto skip_97;
    // 0x8041A178: addiu       $v1, $zero, 0x12
    ctx->r3 = ADD32(0, 0X12);
    skip_97:
    // 0x8041A17C: ori         $v1, $v1, 0x40
    ctx->r3 = ctx->r3 | 0X40;
    // 0x8041A180: b           L_80416070
    // 0x8041A184: sb          $v1, 0x69E4($a0)
    MEM_B(0X69E4, ctx->r4) = ctx->r3;
        goto L_80416070;
    // 0x8041A184: sb          $v1, 0x69E4($a0)
    MEM_B(0X69E4, ctx->r4) = ctx->r3;
    // 0x8041A188: jr          $ra
    // 0x8041A18C: nop

    return;
    // 0x8041A18C: nop

;}
RECOMP_FUNC void hm_func_8041A190(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041A190: bne         $a0, $zero, L_8041A1A0
    if (ctx->r4 != 0) {
        // 0x8041A194: or          $v0, $a0, $zero
        ctx->r2 = ctx->r4 | 0;
            goto L_8041A1A0;
    }
    // 0x8041A194: or          $v0, $a0, $zero
    ctx->r2 = ctx->r4 | 0;
    // 0x8041A198: jr          $ra
    // 0x8041A19C: nop

    return;
    // 0x8041A19C: nop

L_8041A1A0:
    // 0x8041A1A0: lui         $a2, 0x8005
    ctx->r6 = S32(0X8005 << 16);
    // 0x8041A1A4: lw          $v1, -0x28B8($a2)
    ctx->r3 = MEM_W(ctx->r6, -0X28B8);
    // 0x8041A1A8: lui         $a0, 0x41C6
    ctx->r4 = S32(0X41C6 << 16);
    // 0x8041A1AC: addiu       $a0, $a0, 0x4E6D
    ctx->r4 = ADD32(ctx->r4, 0X4E6D);
    // 0x8041A1B0: mult        $v1, $a0
    result = S64(S32(ctx->r3)) * S64(S32(ctx->r4)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x8041A1B4: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x8041A1B8: lw          $a0, -0x5C2C($a3)
    ctx->r4 = MEM_W(ctx->r7, -0X5C2C);
    // 0x8041A1BC: addiu       $a0, $a0, 0x1
    ctx->r4 = ADD32(ctx->r4, 0X1);
    // 0x8041A1C0: mflo        $v1
    ctx->r3 = lo;
    // 0x8041A1C4: addiu       $v1, $v1, 0x3039
    ctx->r3 = ADD32(ctx->r3, 0X3039);
    // 0x8041A1C8: srl         $a1, $v1, 16
    ctx->r5 = S32(U32(ctx->r3) >> 16);
    // 0x8041A1CC: divu        $zero, $a1, $v0
    lo = S32(U32(ctx->r5) / U32(ctx->r2)); hi = S32(U32(ctx->r5) % U32(ctx->r2));
    // 0x8041A1D0: nop

    // 0x8041A1D4: sw          $a0, -0x5C2C($a3)
    MEM_W(-0X5C2C, ctx->r7) = ctx->r4;
    // 0x8041A1D8: mfhi        $v0
    ctx->r2 = hi;
    // 0x8041A1DC: jr          $ra
    // 0x8041A1E0: sw          $v1, -0x28B8($a2)
    MEM_W(-0X28B8, ctx->r6) = ctx->r3;
    return;
    // 0x8041A1E0: sw          $v1, -0x28B8($a2)
    MEM_W(-0X28B8, ctx->r6) = ctx->r3;
;}
RECOMP_FUNC void hm_func_8041A204(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041A204: addiu       $sp, $sp, -0x18
    ctx->r29 = ADD32(ctx->r29, -0X18);
    // 0x8041A208: lui         $v0, 0x8040
    ctx->r2 = S32(0X8040 << 16);
    // 0x8041A20C: addiu       $v0, $v0, 0x0
    ctx->r2 = ADD32(ctx->r2, 0X0);
    // 0x8041A210: sw          $s0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r16;
    // 0x8041A214: or          $s0, $a0, $zero
    ctx->r16 = ctx->r4 | 0;
    // 0x8041A218: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041A21C: addiu       $a2, $zero, 0x400
    ctx->r6 = ADD32(0, 0X400);
    // 0x8041A220: addu        $a1, $v0, $a1
    ctx->r5 = ADD32(ctx->r2, ctx->r5);
    // 0x8041A224: sw          $ra, 0x14($sp)
    MEM_W(0X14, ctx->r29) = ctx->r31;
    // 0x8041A228: jal         0x8003FB20
    // 0x8041A22C: addiu       $a0, $a0, -0x3A90
    ctx->r4 = ADD32(ctx->r4, -0X3A90);
    memcpy_recomp(rdram, ctx);
        goto after_0;
    // 0x8041A22C: addiu       $a0, $a0, -0x3A90
    ctx->r4 = ADD32(ctx->r4, -0X3A90);
    after_0:
    // 0x8041A230: lw          $v0, 0x80($s0)
    ctx->r2 = MEM_W(ctx->r16, 0X80);
    // 0x8041A234: lw          $v0, 0x14($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X14);
    // 0x8041A238: beq         $v0, $zero, L_8041A280
    if (ctx->r2 == 0) {
        // 0x8041A23C: lui         $a1, 0x8008
        ctx->r5 = S32(0X8008 << 16);
            goto L_8041A280;
    }
    // 0x8041A23C: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A240: addiu       $a1, $a1, -0x3690
    ctx->r5 = ADD32(ctx->r5, -0X3690);
    // 0x8041A244: or          $v1, $zero, $zero
    ctx->r3 = 0 | 0;
    // 0x8041A248: b           L_8041A258
    // 0x8041A24C: addiu       $a0, $zero, 0x20
    ctx->r4 = ADD32(0, 0X20);
        goto L_8041A258;
    // 0x8041A24C: addiu       $a0, $zero, 0x20
    ctx->r4 = ADD32(0, 0X20);
L_8041A250:
    // 0x8041A250: lw          $v0, 0x80($s0)
    ctx->r2 = MEM_W(ctx->r16, 0X80);
    // 0x8041A254: lw          $v0, 0x14($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X14);
L_8041A258:
    // 0x8041A258: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x8041A25C: lbu         $v0, 0x0($v0)
    ctx->r2 = MEM_BU(ctx->r2, 0X0);
    // 0x8041A260: addiu       $v1, $v1, 0x1
    ctx->r3 = ADD32(ctx->r3, 0X1);
    // 0x8041A264: sb          $v0, 0x0($a1)
    MEM_B(0X0, ctx->r5) = ctx->r2;
    // 0x8041A268: bne         $v1, $a0, L_8041A250
    if (ctx->r3 != ctx->r4) {
        // 0x8041A26C: addiu       $a1, $a1, 0x1
        ctx->r5 = ADD32(ctx->r5, 0X1);
            goto L_8041A250;
    }
    // 0x8041A26C: addiu       $a1, $a1, 0x1
    ctx->r5 = ADD32(ctx->r5, 0X1);
    // 0x8041A270: lw          $ra, 0x14($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X14);
    // 0x8041A274: lw          $s0, 0x10($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X10);
    // 0x8041A278: jr          $ra
    // 0x8041A27C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
    return;
    // 0x8041A27C: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
L_8041A280:
    // 0x8041A280: lw          $ra, 0x14($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X14);
    // 0x8041A284: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041A288: addiu       $v1, $zero, -0x1
    ctx->r3 = ADD32(0, -0X1);
    // 0x8041A28C: lw          $s0, 0x10($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X10);
    // 0x8041A290: sb          $v1, -0x3690($v0)
    MEM_B(-0X3690, ctx->r2) = ctx->r3;
    // 0x8041A294: jr          $ra
    // 0x8041A298: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
    return;
    // 0x8041A298: addiu       $sp, $sp, 0x18
    ctx->r29 = ADD32(ctx->r29, 0X18);
;}
RECOMP_FUNC void hm_func_8041A5C0(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041A5C0: lui         $a1, 0x8008
    ctx->r5 = S32(0X8008 << 16);
    // 0x8041A5C4: addiu       $a1, $a1, -0x4580
    ctx->r5 = ADD32(ctx->r5, -0X4580);
    // 0x8041A5C8: lbu         $v1, 0x24($a1)
    ctx->r3 = MEM_BU(ctx->r5, 0X24);
    // 0x8041A5CC: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A5D0: beq         $v1, $v0, L_8041AAC8
    if (ctx->r3 == ctx->r2) {
        // 0x8041A5D4: sltiu       $v0, $v1, 0x4
        ctx->r2 = ctx->r3 < 0X4 ? 1 : 0;
            goto L_8041AAC8;
    }
    // 0x8041A5D4: sltiu       $v0, $v1, 0x4
    ctx->r2 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x8041A5D8: bne         $v0, $zero, L_8041ABEC
    if (ctx->r2 != 0) {
        // 0x8041A5DC: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041ABEC;
    }
    // 0x8041A5DC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A5E0: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A5E4: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041A5E8: sh          $a2, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r6;
    // 0x8041A5EC: sltiu       $a0, $v1, 0x10
    ctx->r4 = ctx->r3 < 0X10 ? 1 : 0;
    // 0x8041A5F0: addiu       $a2, $zero, 0xBE
    ctx->r6 = ADD32(0, 0XBE);
    // 0x8041A5F4: bne         $a0, $zero, L_8041ABEC
    if (ctx->r4 != 0) {
        // 0x8041A5F8: sh          $a2, 0xC($v0)
        MEM_H(0XC, ctx->r2) = ctx->r6;
            goto L_8041ABEC;
    }
    // 0x8041A5F8: sh          $a2, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r6;
    // 0x8041A5FC: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041A600: sh          $a2, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r6;
    // 0x8041A604: sltiu       $a0, $v1, 0x20
    ctx->r4 = ctx->r3 < 0X20 ? 1 : 0;
    // 0x8041A608: addiu       $a2, $zero, 0xA8
    ctx->r6 = ADD32(0, 0XA8);
    // 0x8041A60C: sltiu       $a3, $v1, 0x32
    ctx->r7 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x8041A610: bne         $a0, $zero, L_8041A624
    if (ctx->r4 != 0) {
        // 0x8041A614: sh          $a2, 0xC($v0)
        MEM_H(0XC, ctx->r2) = ctx->r6;
            goto L_8041A624;
    }
    // 0x8041A614: sh          $a2, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r6;
    // 0x8041A618: bne         $a3, $zero, L_8041AC34
    if (ctx->r7 != 0) {
        // 0x8041A61C: addiu       $a0, $zero, 0x5
        ctx->r4 = ADD32(0, 0X5);
            goto L_8041AC34;
    }
    // 0x8041A61C: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x8041A620: sh          $a0, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r4;
L_8041A624:
    // 0x8041A624: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041A628: lw          $v0, 0x4EE4($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X4EE4);
    // 0x8041A62C: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    // 0x8041A630: beq         $v0, $a0, L_8041A9F8
    if (ctx->r2 == ctx->r4) {
        // 0x8041A634: addiu       $a0, $zero, 0x1F
        ctx->r4 = ADD32(0, 0X1F);
            goto L_8041A9F8;
    }
    // 0x8041A634: addiu       $a0, $zero, 0x1F
    ctx->r4 = ADD32(0, 0X1F);
    // 0x8041A638: beq         $v0, $a0, L_8041A9FC
    if (ctx->r2 == ctx->r4) {
        // 0x8041A63C: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041A9FC;
    }
    // 0x8041A63C: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
L_8041A640:
    // 0x8041A640: addiu       $a2, $v0, -0x3
    ctx->r6 = ADD32(ctx->r2, -0X3);
    // 0x8041A644: sltiu       $a0, $a2, 0x1E
    ctx->r4 = ctx->r6 < 0X1E ? 1 : 0;
    // 0x8041A648: beq         $a0, $zero, L_8041A678
    if (ctx->r4 == 0) {
        // 0x8041A64C: addiu       $a0, $v0, -0x7
        ctx->r4 = ADD32(ctx->r2, -0X7);
            goto L_8041A678;
    }
    // 0x8041A64C: addiu       $a0, $v0, -0x7
    ctx->r4 = ADD32(ctx->r2, -0X7);
    // 0x8041A650: lui         $a0, 0x2100
    ctx->r4 = S32(0X2100 << 16);
    // 0x8041A654: addiu       $a0, $a0, 0x5
    ctx->r4 = ADD32(ctx->r4, 0X5);
    // 0x8041A658: srlv        $a0, $a0, $a2
    ctx->r4 = S32(U32(ctx->r4) >> (ctx->r6 & 31));
    // 0x8041A65C: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
    // 0x8041A660: beq         $a0, $zero, L_8041A674
    if (ctx->r4 == 0) {
        // 0x8041A664: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041A674;
    }
    // 0x8041A664: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041A668: lw          $a0, 0x167C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X167C);
    // 0x8041A66C: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041A670: sh          $a2, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r6;
L_8041A674:
    // 0x8041A674: addiu       $a0, $v0, -0x7
    ctx->r4 = ADD32(ctx->r2, -0X7);
L_8041A678:
    // 0x8041A678: sltiu       $a2, $a0, 0x1B
    ctx->r6 = ctx->r4 < 0X1B ? 1 : 0;
    // 0x8041A67C: beql        $a2, $zero, L_8041A6A0
    if (ctx->r6 == 0) {
        // 0x8041A680: addiu       $v0, $v0, -0x9
        ctx->r2 = ADD32(ctx->r2, -0X9);
            goto L_8041A6A0;
    }
    goto skip_0;
    // 0x8041A680: addiu       $v0, $v0, -0x9
    ctx->r2 = ADD32(ctx->r2, -0X9);
    skip_0:
    // 0x8041A684: lui         $a2, 0x428
    ctx->r6 = S32(0X428 << 16);
    // 0x8041A688: addiu       $a2, $a2, 0x21
    ctx->r6 = ADD32(ctx->r6, 0X21);
    // 0x8041A68C: srlv        $a0, $a2, $a0
    ctx->r4 = S32(U32(ctx->r6) >> (ctx->r4 & 31));
    // 0x8041A690: andi        $a0, $a0, 0x1
    ctx->r4 = ctx->r4 & 0X1;
    // 0x8041A694: bne         $a0, $zero, L_8041A9E0
    if (ctx->r4 != 0) {
        // 0x8041A698: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041A9E0;
    }
    // 0x8041A698: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
L_8041A69C:
    // 0x8041A69C: addiu       $v0, $v0, -0x9
    ctx->r2 = ADD32(ctx->r2, -0X9);
L_8041A6A0:
    // 0x8041A6A0: sltiu       $a0, $v0, 0x1A
    ctx->r4 = ctx->r2 < 0X1A ? 1 : 0;
    // 0x8041A6A4: beql        $a0, $zero, L_8041A6D4
    if (ctx->r4 == 0) {
        // 0x8041A6A8: lbu         $a0, 0x25($a1)
        ctx->r4 = MEM_BU(ctx->r5, 0X25);
            goto L_8041A6D4;
    }
    goto skip_1;
    // 0x8041A6A8: lbu         $a0, 0x25($a1)
    ctx->r4 = MEM_BU(ctx->r5, 0X25);
    skip_1:
    // 0x8041A6AC: lui         $a0, 0x230
    ctx->r4 = S32(0X230 << 16);
    // 0x8041A6B0: addiu       $a0, $a0, 0x5
    ctx->r4 = ADD32(ctx->r4, 0X5);
    // 0x8041A6B4: srlv        $v0, $a0, $v0
    ctx->r2 = S32(U32(ctx->r4) >> (ctx->r2 & 31));
    // 0x8041A6B8: andi        $v0, $v0, 0x1
    ctx->r2 = ctx->r2 & 0X1;
    // 0x8041A6BC: beq         $v0, $zero, L_8041A6D0
    if (ctx->r2 == 0) {
        // 0x8041A6C0: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A6D0;
    }
    // 0x8041A6C0: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A6C4: lw          $v0, 0x167C($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X167C);
    // 0x8041A6C8: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041A6CC: sh          $a0, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r4;
L_8041A6D0:
    // 0x8041A6D0: lbu         $a0, 0x25($a1)
    ctx->r4 = MEM_BU(ctx->r5, 0X25);
L_8041A6D4:
    // 0x8041A6D4: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A6D8: beql        $a0, $v0, L_8041AB20
    if (ctx->r4 == ctx->r2) {
        // 0x8041A6DC: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AB20;
    }
    goto skip_2;
    // 0x8041A6DC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    skip_2:
    // 0x8041A6E0: beq         $v1, $v0, L_8041AB48
    if (ctx->r3 == ctx->r2) {
        // 0x8041A6E4: sltiu       $v0, $a0, 0x7
        ctx->r2 = ctx->r4 < 0X7 ? 1 : 0;
            goto L_8041AB48;
    }
    // 0x8041A6E4: sltiu       $v0, $a0, 0x7
    ctx->r2 = ctx->r4 < 0X7 ? 1 : 0;
    // 0x8041A6E8: bne         $v0, $zero, L_8041A6FC
    if (ctx->r2 != 0) {
        // 0x8041A6EC: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A6FC;
    }
    // 0x8041A6EC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A6F0: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041A6F4:
    // 0x8041A6F4: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041A6F8: sb          $a2, 0xBB($v0)
    MEM_B(0XBB, ctx->r2) = ctx->r6;
L_8041A6FC:
    // 0x8041A6FC: sltiu       $v0, $v1, 0x16
    ctx->r2 = ctx->r3 < 0X16 ? 1 : 0;
    // 0x8041A700: bne         $v0, $zero, L_8041A714
    if (ctx->r2 != 0) {
        // 0x8041A704: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A714;
    }
    // 0x8041A704: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A708: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A70C: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041A710: sb          $a2, 0xBC($v0)
    MEM_B(0XBC, ctx->r2) = ctx->r6;
L_8041A714:
    // 0x8041A714: lbu         $a2, 0x27($a1)
    ctx->r6 = MEM_BU(ctx->r5, 0X27);
L_8041A718:
    // 0x8041A718: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A71C: beq         $a2, $v0, L_8041AB0C
    if (ctx->r6 == ctx->r2) {
        // 0x8041A720: sltiu       $v0, $a2, 0x9
        ctx->r2 = ctx->r6 < 0X9 ? 1 : 0;
            goto L_8041AB0C;
    }
    // 0x8041A720: sltiu       $v0, $a2, 0x9
    ctx->r2 = ctx->r6 < 0X9 ? 1 : 0;
    // 0x8041A724: bne         $v0, $zero, L_8041A740
    if (ctx->r2 != 0) {
        // 0x8041A728: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A740;
    }
    // 0x8041A728: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A72C: sltiu       $t0, $a2, 0x12
    ctx->r8 = ctx->r6 < 0X12 ? 1 : 0;
    // 0x8041A730: bne         $t0, $zero, L_8041AC40
    if (ctx->r8 != 0) {
        // 0x8041A734: lw          $t1, 0x1668($v0)
        ctx->r9 = MEM_W(ctx->r2, 0X1668);
            goto L_8041AC40;
    }
    // 0x8041A734: lw          $t1, 0x1668($v0)
    ctx->r9 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A738: addiu       $v0, $zero, 0x7
    ctx->r2 = ADD32(0, 0X7);
    // 0x8041A73C: sh          $v0, 0xEC($t1)
    MEM_H(0XEC, ctx->r9) = ctx->r2;
L_8041A740:
    // 0x8041A740: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A744: beq         $v1, $v0, L_8041AAE4
    if (ctx->r3 == ctx->r2) {
        // 0x8041A748: lbu         $t0, 0x26($a1)
        ctx->r8 = MEM_BU(ctx->r5, 0X26);
            goto L_8041AAE4;
    }
    // 0x8041A748: lbu         $t0, 0x26($a1)
    ctx->r8 = MEM_BU(ctx->r5, 0X26);
    // 0x8041A74C: beq         $t0, $v0, L_8041AB64
    if (ctx->r8 == ctx->r2) {
        // 0x8041A750: sltiu       $v0, $v1, 0x26
        ctx->r2 = ctx->r3 < 0X26 ? 1 : 0;
            goto L_8041AB64;
    }
    // 0x8041A750: sltiu       $v0, $v1, 0x26
    ctx->r2 = ctx->r3 < 0X26 ? 1 : 0;
    // 0x8041A754: bne         $v0, $zero, L_8041A768
    if (ctx->r2 != 0) {
        // 0x8041A758: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A768;
    }
    // 0x8041A758: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A75C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041A760:
    // 0x8041A760: addiu       $a1, $zero, 0x1C2
    ctx->r5 = ADD32(0, 0X1C2);
    // 0x8041A764: sh          $a1, 0x11C($v0)
    MEM_H(0X11C, ctx->r2) = ctx->r5;
L_8041A768:
    // 0x8041A768: sltiu       $a1, $t0, 0xF
    ctx->r5 = ctx->r8 < 0XF ? 1 : 0;
    // 0x8041A76C: bne         $a1, $zero, L_8041A784
    if (ctx->r5 != 0) {
        // 0x8041A770: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A784;
    }
    // 0x8041A770: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A774: lui         $t1, 0x8042
    ctx->r9 = S32(0X8042 << 16);
    // 0x8041A778: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A77C: lwc1        $f0, -0x3F4C($t1)
    ctx->f0.u32l = MEM_W(ctx->r9, -0X3F4C);
    // 0x8041A780: swc1        $f0, 0x12C($v0)
    MEM_W(0X12C, ctx->r2) = ctx->f0.u32l;
L_8041A784:
    // 0x8041A784: sltiu       $v0, $v1, 0x2C
    ctx->r2 = ctx->r3 < 0X2C ? 1 : 0;
    // 0x8041A788: bne         $v0, $zero, L_8041AA94
    if (ctx->r2 != 0) {
        // 0x8041A78C: addiu       $v0, $zero, 0x1
        ctx->r2 = ADD32(0, 0X1);
            goto L_8041AA94;
    }
    // 0x8041A78C: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A790: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A794: lw          $t1, 0x1668($v0)
    ctx->r9 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A798: addiu       $t2, $zero, 0x208
    ctx->r10 = ADD32(0, 0X208);
    // 0x8041A79C: sltiu       $v0, $v1, 0x42
    ctx->r2 = ctx->r3 < 0X42 ? 1 : 0;
    // 0x8041A7A0: bne         $v0, $zero, L_8041A7B8
    if (ctx->r2 != 0) {
        // 0x8041A7A4: sh          $t2, 0x11C($t1)
        MEM_H(0X11C, ctx->r9) = ctx->r10;
            goto L_8041A7B8;
    }
    // 0x8041A7A4: sh          $t2, 0x11C($t1)
    MEM_H(0X11C, ctx->r9) = ctx->r10;
    // 0x8041A7A8: sltiu       $v0, $v1, 0x58
    ctx->r2 = ctx->r3 < 0X58 ? 1 : 0;
    // 0x8041A7AC: bne         $v0, $zero, L_8041AC00
    if (ctx->r2 != 0) {
        // 0x8041A7B0: addiu       $v0, $zero, 0x20
        ctx->r2 = ADD32(0, 0X20);
            goto L_8041AC00;
    }
    // 0x8041A7B0: addiu       $v0, $zero, 0x20
    ctx->r2 = ADD32(0, 0X20);
    // 0x8041A7B4: sh          $v0, 0x174($t1)
    MEM_H(0X174, ctx->r9) = ctx->r2;
L_8041A7B8:
    // 0x8041A7B8: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A7BC: beq         $a0, $v0, L_8041AAB8
    if (ctx->r4 == ctx->r2) {
        // 0x8041A7C0: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AAB8;
    }
    // 0x8041A7C0: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A7C4: sltiu       $v0, $a0, 0x7
    ctx->r2 = ctx->r4 < 0X7 ? 1 : 0;
L_8041A7C8:
    // 0x8041A7C8: bne         $v0, $zero, L_8041A7E4
    if (ctx->r2 != 0) {
        // 0x8041A7CC: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A7E4;
    }
    // 0x8041A7CC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A7D0: sltiu       $t1, $a0, 0x15
    ctx->r9 = ctx->r4 < 0X15 ? 1 : 0;
    // 0x8041A7D4: bne         $t1, $zero, L_8041AC0C
    if (ctx->r9 != 0) {
        // 0x8041A7D8: lw          $t2, 0x1668($v0)
        ctx->r10 = MEM_W(ctx->r2, 0X1668);
            goto L_8041AC0C;
    }
    // 0x8041A7D8: lw          $t2, 0x1668($v0)
    ctx->r10 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A7DC: addiu       $v0, $zero, 0xC
    ctx->r2 = ADD32(0, 0XC);
    // 0x8041A7E0: sh          $v0, 0x1E8($t2)
    MEM_H(0X1E8, ctx->r10) = ctx->r2;
L_8041A7E4:
    // 0x8041A7E4: bne         $a1, $zero, L_8041A804
    if (ctx->r5 != 0) {
        // 0x8041A7E8: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A804;
    }
    // 0x8041A7E8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A7EC: sltiu       $a1, $t0, 0x1E
    ctx->r5 = ctx->r8 < 0X1E ? 1 : 0;
    // 0x8041A7F0: bne         $a1, $zero, L_8041AC18
    if (ctx->r5 != 0) {
        // 0x8041A7F4: lw          $t1, 0x1668($v0)
        ctx->r9 = MEM_W(ctx->r2, 0X1668);
            goto L_8041AC18;
    }
    // 0x8041A7F4: lw          $t1, 0x1668($v0)
    ctx->r9 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A7F8: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041A7FC: lwc1        $f0, -0x3F44($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F44);
    // 0x8041A800: swc1        $f0, 0x23C($t1)
    MEM_W(0X23C, ctx->r9) = ctx->f0.u32l;
L_8041A804:
    // 0x8041A804: bne         $a3, $zero, L_8041AA0C
    if (ctx->r7 != 0) {
        // 0x8041A808: addiu       $v0, $zero, 0x1
        ctx->r2 = ADD32(0, 0X1);
            goto L_8041AA0C;
    }
    // 0x8041A808: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A80C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A810: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A814: addiu       $t1, $zero, 0x3E8
    ctx->r9 = ADD32(0, 0X3E8);
    // 0x8041A818: sltiu       $a1, $v1, 0x46
    ctx->r5 = ctx->r3 < 0X46 ? 1 : 0;
    // 0x8041A81C: bne         $a1, $zero, L_8041A854
    if (ctx->r5 != 0) {
        // 0x8041A820: sh          $t1, 0x22C($v0)
        MEM_H(0X22C, ctx->r2) = ctx->r9;
            goto L_8041A854;
    }
    // 0x8041A820: sh          $t1, 0x22C($v0)
    MEM_H(0X22C, ctx->r2) = ctx->r9;
    // 0x8041A824: addiu       $t1, $zero, 0x2
    ctx->r9 = ADD32(0, 0X2);
    // 0x8041A828: sltiu       $a1, $v1, 0x64
    ctx->r5 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041A82C: bne         $a1, $zero, L_8041A854
    if (ctx->r5 != 0) {
        // 0x8041A830: sh          $t1, 0x240($v0)
        MEM_H(0X240, ctx->r2) = ctx->r9;
            goto L_8041A854;
    }
    // 0x8041A830: sh          $t1, 0x240($v0)
    MEM_H(0X240, ctx->r2) = ctx->r9;
    // 0x8041A834: addiu       $t1, $zero, 0x3
    ctx->r9 = ADD32(0, 0X3);
    // 0x8041A838: sltiu       $a1, $v1, 0x82
    ctx->r5 = ctx->r3 < 0X82 ? 1 : 0;
    // 0x8041A83C: bne         $a1, $zero, L_8041A854
    if (ctx->r5 != 0) {
        // 0x8041A840: sh          $t1, 0x240($v0)
        MEM_H(0X240, ctx->r2) = ctx->r9;
            goto L_8041A854;
    }
    // 0x8041A840: sh          $t1, 0x240($v0)
    MEM_H(0X240, ctx->r2) = ctx->r9;
    // 0x8041A844: sltiu       $a1, $v1, 0x96
    ctx->r5 = ctx->r3 < 0X96 ? 1 : 0;
    // 0x8041A848: bne         $a1, $zero, L_8041AC28
    if (ctx->r5 != 0) {
        // 0x8041A84C: addiu       $a1, $zero, 0x578
        ctx->r5 = ADD32(0, 0X578);
            goto L_8041AC28;
    }
    // 0x8041A84C: addiu       $a1, $zero, 0x578
    ctx->r5 = ADD32(0, 0X578);
    // 0x8041A850: sh          $a1, 0x22C($v0)
    MEM_H(0X22C, ctx->r2) = ctx->r5;
L_8041A854:
    // 0x8041A854: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041A858: beq         $a2, $a1, L_8041ABE4
    if (ctx->r6 == ctx->r5) {
        // 0x8041A85C: addiu       $a1, $zero, 0xA
        ctx->r5 = ADD32(0, 0XA);
            goto L_8041ABE4;
    }
    // 0x8041A85C: addiu       $a1, $zero, 0xA
    ctx->r5 = ADD32(0, 0XA);
L_8041A860:
    // 0x8041A860: sltiu       $v0, $a2, 0x15
    ctx->r2 = ctx->r6 < 0X15 ? 1 : 0;
    // 0x8041A864: bne         $v0, $zero, L_8041A880
    if (ctx->r2 != 0) {
        // 0x8041A868: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A880;
    }
    // 0x8041A868: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A86C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041A870:
    // 0x8041A870: sltiu       $a2, $a2, 0x1F
    ctx->r6 = ctx->r6 < 0X1F ? 1 : 0;
    // 0x8041A874: bne         $a2, $zero, L_8041ABF4
    if (ctx->r6 != 0) {
        // 0x8041A878: addiu       $a1, $zero, 0x17
        ctx->r5 = ADD32(0, 0X17);
            goto L_8041ABF4;
    }
    // 0x8041A878: addiu       $a1, $zero, 0x17
    ctx->r5 = ADD32(0, 0X17);
    // 0x8041A87C: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
L_8041A880:
    // 0x8041A880: sltiu       $v0, $v1, 0x23
    ctx->r2 = ctx->r3 < 0X23 ? 1 : 0;
    // 0x8041A884: bne         $v0, $zero, L_8041AB34
    if (ctx->r2 != 0) {
        // 0x8041A888: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AB34;
    }
    // 0x8041A888: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A88C: lw          $a1, 0x1668($v0)
    ctx->r5 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A890: addiu       $t1, $zero, 0x2
    ctx->r9 = ADD32(0, 0X2);
    // 0x8041A894: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041A898: beq         $a0, $a2, L_8041ABD0
    if (ctx->r4 == ctx->r6) {
        // 0x8041A89C: sb          $t1, 0x2A0($a1)
        MEM_B(0X2A0, ctx->r5) = ctx->r9;
            goto L_8041ABD0;
    }
    // 0x8041A89C: sb          $t1, 0x2A0($a1)
    MEM_B(0X2A0, ctx->r5) = ctx->r9;
    // 0x8041A8A0: sltiu       $a2, $v1, 0x64
    ctx->r6 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041A8A4: bnel        $a2, $zero, L_8041AC88
    if (ctx->r6 != 0) {
        // 0x8041A8A8: sltiu       $a1, $a0, 0x33
        ctx->r5 = ctx->r4 < 0X33 ? 1 : 0;
            goto L_8041AC88;
    }
    goto skip_3;
    // 0x8041A8A8: sltiu       $a1, $a0, 0x33
    ctx->r5 = ctx->r4 < 0X33 ? 1 : 0;
    skip_3:
L_8041A8AC:
    // 0x8041A8AC: sltiu       $v0, $v1, 0x6E
    ctx->r2 = ctx->r3 < 0X6E ? 1 : 0;
    // 0x8041A8B0: bne         $v0, $zero, L_8041A9C4
    if (ctx->r2 != 0) {
        // 0x8041A8B4: addiu       $a2, $zero, 0x3
        ctx->r6 = ADD32(0, 0X3);
            goto L_8041A9C4;
    }
    // 0x8041A8B4: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041A8B8: sltiu       $v0, $a0, 0x33
    ctx->r2 = ctx->r4 < 0X33 ? 1 : 0;
    // 0x8041A8BC: beq         $v0, $zero, L_8041A9D4
    if (ctx->r2 == 0) {
        // 0x8041A8C0: sh          $a2, 0x2C8($a1)
        MEM_H(0X2C8, ctx->r5) = ctx->r6;
            goto L_8041A9D4;
    }
    // 0x8041A8C0: sh          $a2, 0x2C8($a1)
    MEM_H(0X2C8, ctx->r5) = ctx->r6;
L_8041A8C4:
    // 0x8041A8C4: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A8C8: beq         $a0, $v0, L_8041AA74
    if (ctx->r4 == ctx->r2) {
        // 0x8041A8CC: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AA74;
    }
    // 0x8041A8CC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041A8D0:
    // 0x8041A8D0: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
L_8041A8D4:
    // 0x8041A8D4: bne         $t0, $v0, L_8041A8E8
    if (ctx->r8 != ctx->r2) {
        // 0x8041A8D8: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A8E8;
    }
    // 0x8041A8D8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A8DC: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A8E0: addiu       $a1, $zero, 0x3C
    ctx->r5 = ADD32(0, 0X3C);
    // 0x8041A8E4: sh          $a1, 0x2FA($v0)
    MEM_H(0X2FA, ctx->r2) = ctx->r5;
L_8041A8E8:
    // 0x8041A8E8: bnel        $a3, $zero, L_8041A90C
    if (ctx->r7 != 0) {
        // 0x8041A8EC: sltiu       $a0, $a0, 0x2C
        ctx->r4 = ctx->r4 < 0X2C ? 1 : 0;
            goto L_8041A90C;
    }
    goto skip_4;
    // 0x8041A8EC: sltiu       $a0, $a0, 0x2C
    ctx->r4 = ctx->r4 < 0X2C ? 1 : 0;
    skip_4:
    // 0x8041A8F0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041A8F4: sltiu       $v0, $v1, 0x3C
    ctx->r2 = ctx->r3 < 0X3C ? 1 : 0;
    // 0x8041A8F8: bne         $v0, $zero, L_8041AC64
    if (ctx->r2 != 0) {
        // 0x8041A8FC: lw          $a1, 0x1668($a1)
        ctx->r5 = MEM_W(ctx->r5, 0X1668);
            goto L_8041AC64;
    }
    // 0x8041A8FC: lw          $a1, 0x1668($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1668);
    // 0x8041A900: addiu       $v0, $zero, 0x320
    ctx->r2 = ADD32(0, 0X320);
    // 0x8041A904: sh          $v0, 0x2F8($a1)
    MEM_H(0X2F8, ctx->r5) = ctx->r2;
L_8041A908:
    // 0x8041A908: sltiu       $a0, $a0, 0x2C
    ctx->r4 = ctx->r4 < 0X2C ? 1 : 0;
L_8041A90C:
    // 0x8041A90C: bne         $a0, $zero, L_8041A93C
    if (ctx->r4 != 0) {
        // 0x8041A910: sltiu       $v0, $t0, 0x2C
        ctx->r2 = ctx->r8 < 0X2C ? 1 : 0;
            goto L_8041A93C;
    }
    // 0x8041A910: sltiu       $v0, $t0, 0x2C
    ctx->r2 = ctx->r8 < 0X2C ? 1 : 0;
    // 0x8041A914: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041A918: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041A91C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041A920: sh          $a0, 0x302($v0)
    MEM_H(0X302, ctx->r2) = ctx->r4;
    // 0x8041A924: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x8041A928: sh          $a0, 0x2EE($v0)
    MEM_H(0X2EE, ctx->r2) = ctx->r4;
    // 0x8041A92C: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    // 0x8041A930: sh          $zero, 0x304($v0)
    MEM_H(0X304, ctx->r2) = 0;
    // 0x8041A934: sh          $a0, 0x2F6($v0)
    MEM_H(0X2F6, ctx->r2) = ctx->r4;
    // 0x8041A938: sltiu       $v0, $t0, 0x2C
    ctx->r2 = ctx->r8 < 0X2C ? 1 : 0;
L_8041A93C:
    // 0x8041A93C: bne         $v0, $zero, L_8041A970
    if (ctx->r2 != 0) {
        // 0x8041A940: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041A970;
    }
    // 0x8041A940: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041A944: sltiu       $v0, $t0, 0x30
    ctx->r2 = ctx->r8 < 0X30 ? 1 : 0;
    // 0x8041A948: bne         $v0, $zero, L_8041AC70
    if (ctx->r2 != 0) {
        // 0x8041A94C: lw          $a0, 0x1668($a0)
        ctx->r4 = MEM_W(ctx->r4, 0X1668);
            goto L_8041AC70;
    }
    // 0x8041A94C: lw          $a0, 0x1668($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X1668);
    // 0x8041A950: sltiu       $v0, $t0, 0x34
    ctx->r2 = ctx->r8 < 0X34 ? 1 : 0;
    // 0x8041A954: bne         $v0, $zero, L_8041AC7C
    if (ctx->r2 != 0) {
        // 0x8041A958: sltiu       $v0, $t0, 0x38
        ctx->r2 = ctx->r8 < 0X38 ? 1 : 0;
            goto L_8041AC7C;
    }
    // 0x8041A958: sltiu       $v0, $t0, 0x38
    ctx->r2 = ctx->r8 < 0X38 ? 1 : 0;
    // 0x8041A95C: bne         $v0, $zero, L_8041AC4C
    if (ctx->r2 != 0) {
        // 0x8041A960: sltiu       $t0, $t0, 0x3C
        ctx->r8 = ctx->r8 < 0X3C ? 1 : 0;
            goto L_8041AC4C;
    }
    // 0x8041A960: sltiu       $t0, $t0, 0x3C
    ctx->r8 = ctx->r8 < 0X3C ? 1 : 0;
    // 0x8041A964: bne         $t0, $zero, L_8041AC58
    if (ctx->r8 != 0) {
        // 0x8041A968: addiu       $v0, $zero, 0x64
        ctx->r2 = ADD32(0, 0X64);
            goto L_8041AC58;
    }
    // 0x8041A968: addiu       $v0, $zero, 0x64
    ctx->r2 = ADD32(0, 0X64);
    // 0x8041A96C: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
L_8041A970:
    // 0x8041A970: beq         $v1, $zero, L_8041A9F0
    if (ctx->r3 == 0) {
        // 0x8041A974: lui         $v0, 0x8008
        ctx->r2 = S32(0X8008 << 16);
            goto L_8041A9F0;
    }
    // 0x8041A974: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041A978: lhu         $v0, 0x69F4($v0)
    ctx->r2 = MEM_HU(ctx->r2, 0X69F4);
    // 0x8041A97C: sltiu       $a0, $v0, 0x1773
    ctx->r4 = ctx->r2 < 0X1773 ? 1 : 0;
    // 0x8041A980: beq         $a0, $zero, L_8041A99C
    if (ctx->r4 == 0) {
        // 0x8041A984: addiu       $v1, $zero, 0x7D0
        ctx->r3 = ADD32(0, 0X7D0);
            goto L_8041A99C;
    }
    // 0x8041A984: addiu       $v1, $zero, 0x7D0
    ctx->r3 = ADD32(0, 0X7D0);
    // 0x8041A988: lui         $v1, 0xAAAA
    ctx->r3 = S32(0XAAAA << 16);
    // 0x8041A98C: ori         $v1, $v1, 0xAAAB
    ctx->r3 = ctx->r3 | 0XAAAB;
    // 0x8041A990: multu       $v0, $v1
    result = U64(U32(ctx->r2)) * U64(U32(ctx->r3)); lo = S32(result >> 0); hi = S32(result >> 32);
    // 0x8041A994: mfhi        $v1
    ctx->r3 = hi;
    // 0x8041A998: srl         $v1, $v1, 1
    ctx->r3 = S32(U32(ctx->r3) >> 1);
L_8041A99C:
    // 0x8041A99C: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041A9A0: lw          $a2, 0x1668($a0)
    ctx->r6 = MEM_W(ctx->r4, 0X1668);
    // 0x8041A9A4: sltiu       $a1, $v0, 0x7D1
    ctx->r5 = ctx->r2 < 0X7D1 ? 1 : 0;
    // 0x8041A9A8: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041A9AC: lw          $a0, 0x167C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X167C);
    // 0x8041A9B0: bne         $a1, $zero, L_8041A9BC
    if (ctx->r5 != 0) {
        // 0x8041A9B4: sh          $v1, 0x3C4($a2)
        MEM_H(0X3C4, ctx->r6) = ctx->r3;
            goto L_8041A9BC;
    }
    // 0x8041A9B4: sh          $v1, 0x3C4($a2)
    MEM_H(0X3C4, ctx->r6) = ctx->r3;
    // 0x8041A9B8: addiu       $v0, $zero, 0x7D0
    ctx->r2 = ADD32(0, 0X7D0);
L_8041A9BC:
    // 0x8041A9BC: jr          $ra
    // 0x8041A9C0: sh          $v0, 0x3C4($a0)
    MEM_H(0X3C4, ctx->r4) = ctx->r2;
    return;
    // 0x8041A9C0: sh          $v0, 0x3C4($a0)
    MEM_H(0X3C4, ctx->r4) = ctx->r2;
L_8041A9C4:
    // 0x8041A9C4: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041A9C8: sltiu       $v0, $a0, 0x33
    ctx->r2 = ctx->r4 < 0X33 ? 1 : 0;
    // 0x8041A9CC: bne         $v0, $zero, L_8041A8C4
    if (ctx->r2 != 0) {
        // 0x8041A9D0: sh          $a2, 0x2C8($a1)
        MEM_H(0X2C8, ctx->r5) = ctx->r6;
            goto L_8041A8C4;
    }
    // 0x8041A9D0: sh          $a2, 0x2C8($a1)
    MEM_H(0X2C8, ctx->r5) = ctx->r6;
L_8041A9D4:
    // 0x8041A9D4: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041A9D8: b           L_8041A8D0
    // 0x8041A9DC: sb          $v0, 0x2E4($a1)
    MEM_B(0X2E4, ctx->r5) = ctx->r2;
        goto L_8041A8D0;
    // 0x8041A9DC: sb          $v0, 0x2E4($a1)
    MEM_B(0X2E4, ctx->r5) = ctx->r2;
L_8041A9E0:
    // 0x8041A9E0: lw          $a0, 0x167C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X167C);
    // 0x8041A9E4: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041A9E8: b           L_8041A69C
    // 0x8041A9EC: sh          $a2, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r6;
        goto L_8041A69C;
    // 0x8041A9EC: sh          $a2, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r6;
L_8041A9F0:
    // 0x8041A9F0: jr          $ra
    // 0x8041A9F4: nop

    return;
    // 0x8041A9F4: nop

L_8041A9F8:
    // 0x8041A9F8: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
L_8041A9FC:
    // 0x8041A9FC: lw          $a0, 0x167C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X167C);
    // 0x8041AA00: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041AA04: b           L_8041A640
    // 0x8041AA08: sh          $a2, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r6;
        goto L_8041A640;
    // 0x8041AA08: sh          $a2, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r6;
L_8041AA0C:
    // 0x8041AA0C: beql        $a2, $v0, L_8041ABB8
    if (ctx->r6 == ctx->r2) {
        // 0x8041AA10: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041ABB8;
    }
    goto skip_5;
    // 0x8041AA10: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    skip_5:
    // 0x8041AA14: bne         $v1, $v0, L_8041A860
    if (ctx->r3 != ctx->r2) {
        // 0x8041AA18: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A860;
    }
    // 0x8041AA18: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AA1C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AA20: sltiu       $a1, $a2, 0x15
    ctx->r5 = ctx->r6 < 0X15 ? 1 : 0;
    // 0x8041AA24: beq         $a1, $zero, L_8041A870
    if (ctx->r5 == 0) {
        // 0x8041AA28: sb          $zero, 0x2A0($v0)
        MEM_B(0X2A0, ctx->r2) = 0;
            goto L_8041A870;
    }
    // 0x8041AA28: sb          $zero, 0x2A0($v0)
    MEM_B(0X2A0, ctx->r2) = 0;
L_8041AA2C:
    // 0x8041AA2C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AA30: sh          $a1, 0x2C8($v0)
    MEM_H(0X2C8, ctx->r2) = ctx->r5;
L_8041AA34:
    // 0x8041AA34: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041AA38: beq         $a0, $v0, L_8041AB88
    if (ctx->r4 == ctx->r2) {
        // 0x8041AA3C: sltiu       $v0, $a0, 0x33
        ctx->r2 = ctx->r4 < 0X33 ? 1 : 0;
            goto L_8041AB88;
    }
    // 0x8041AA3C: sltiu       $v0, $a0, 0x33
    ctx->r2 = ctx->r4 < 0X33 ? 1 : 0;
    // 0x8041AA40: bne         $v0, $zero, L_8041AB9C
    if (ctx->r2 != 0) {
        // 0x8041AA44: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AB9C;
    }
    // 0x8041AA44: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AA48: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041AA4C:
    // 0x8041AA4C: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041AA50: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AA54: bne         $v1, $a1, L_8041A8D0
    if (ctx->r3 != ctx->r5) {
        // 0x8041AA58: sb          $a2, 0x2E4($v0)
        MEM_B(0X2E4, ctx->r2) = ctx->r6;
            goto L_8041A8D0;
    }
    // 0x8041AA58: sb          $a2, 0x2E4($v0)
    MEM_B(0X2E4, ctx->r2) = ctx->r6;
    // 0x8041AA5C: addiu       $a1, $zero, 0x12C
    ctx->r5 = ADD32(0, 0X12C);
    // 0x8041AA60: sh          $a1, 0x2F8($v0)
    MEM_H(0X2F8, ctx->r2) = ctx->r5;
L_8041AA64:
    // 0x8041AA64: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041AA68: bne         $a0, $v0, L_8041A8D4
    if (ctx->r4 != ctx->r2) {
        // 0x8041AA6C: nop
    
            goto L_8041A8D4;
    }
    // 0x8041AA6C: nop

    // 0x8041AA70: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041AA74:
    // 0x8041AA74: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041AA78:
    // 0x8041AA78: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AA7C: addiu       $a2, $zero, 0x11
    ctx->r6 = ADD32(0, 0X11);
    // 0x8041AA80: sh          $a1, 0x302($v0)
    MEM_H(0X302, ctx->r2) = ctx->r5;
    // 0x8041AA84: sh          $a2, 0x2EE($v0)
    MEM_H(0X2EE, ctx->r2) = ctx->r6;
    // 0x8041AA88: sh          $zero, 0x304($v0)
    MEM_H(0X304, ctx->r2) = 0;
    // 0x8041AA8C: b           L_8041A8D0
    // 0x8041AA90: sh          $a1, 0x2F6($v0)
    MEM_H(0X2F6, ctx->r2) = ctx->r5;
        goto L_8041A8D0;
    // 0x8041AA90: sh          $a1, 0x2F6($v0)
    MEM_H(0X2F6, ctx->r2) = ctx->r5;
L_8041AA94:
    // 0x8041AA94: bne         $v1, $v0, L_8041A7B8
    if (ctx->r3 != ctx->r2) {
        // 0x8041AA98: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A7B8;
    }
    // 0x8041AA98: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AA9C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AAA0: addiu       $t1, $zero, 0x10
    ctx->r9 = ADD32(0, 0X10);
    // 0x8041AAA4: sh          $t1, 0x174($v0)
    MEM_H(0X174, ctx->r2) = ctx->r9;
    // 0x8041AAA8: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041AAAC: bne         $a0, $v0, L_8041A7C8
    if (ctx->r4 != ctx->r2) {
        // 0x8041AAB0: sltiu       $v0, $a0, 0x7
        ctx->r2 = ctx->r4 < 0X7 ? 1 : 0;
            goto L_8041A7C8;
    }
    // 0x8041AAB0: sltiu       $v0, $a0, 0x7
    ctx->r2 = ctx->r4 < 0X7 ? 1 : 0;
    // 0x8041AAB4: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041AAB8:
    // 0x8041AAB8: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AABC: addiu       $t1, $zero, 0x5
    ctx->r9 = ADD32(0, 0X5);
    // 0x8041AAC0: b           L_8041A7E4
    // 0x8041AAC4: sh          $t1, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r9;
        goto L_8041A7E4;
    // 0x8041AAC4: sh          $t1, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r9;
L_8041AAC8:
    // 0x8041AAC8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AACC: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AAD0: addiu       $a0, $zero, 0xF0
    ctx->r4 = ADD32(0, 0XF0);
    // 0x8041AAD4: sh          $v1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r3;
    // 0x8041AAD8: sh          $a0, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r4;
    // 0x8041AADC: b           L_8041A624
    // 0x8041AAE0: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
        goto L_8041A624;
    // 0x8041AAE0: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
L_8041AAE4:
    // 0x8041AAE4: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AAE8: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AAEC: addiu       $a1, $zero, 0x140
    ctx->r5 = ADD32(0, 0X140);
    // 0x8041AAF0: bne         $t0, $v1, L_8041A768
    if (ctx->r8 != ctx->r3) {
        // 0x8041AAF4: sh          $a1, 0x11C($v0)
        MEM_H(0X11C, ctx->r2) = ctx->r5;
            goto L_8041A768;
    }
    // 0x8041AAF4: sh          $a1, 0x11C($v0)
    MEM_H(0X11C, ctx->r2) = ctx->r5;
    // 0x8041AAF8: lui         $t1, 0x8042
    ctx->r9 = S32(0X8042 << 16);
    // 0x8041AAFC: lwc1        $f0, -0x3F50($t1)
    ctx->f0.u32l = MEM_W(ctx->r9, -0X3F50);
    // 0x8041AB00: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AB04: b           L_8041A784
    // 0x8041AB08: swc1        $f0, 0x12C($v0)
    MEM_W(0X12C, ctx->r2) = ctx->f0.u32l;
        goto L_8041A784;
    // 0x8041AB08: swc1        $f0, 0x12C($v0)
    MEM_W(0X12C, ctx->r2) = ctx->f0.u32l;
L_8041AB0C:
    // 0x8041AB0C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AB10: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AB14: addiu       $t0, $zero, 0x3
    ctx->r8 = ADD32(0, 0X3);
    // 0x8041AB18: b           L_8041A740
    // 0x8041AB1C: sh          $t0, 0xEC($v0)
    MEM_H(0XEC, ctx->r2) = ctx->r8;
        goto L_8041A740;
    // 0x8041AB1C: sh          $t0, 0xEC($v0)
    MEM_H(0XEC, ctx->r2) = ctx->r8;
L_8041AB20:
    // 0x8041AB20: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AB24: bne         $v1, $a0, L_8041A6FC
    if (ctx->r3 != ctx->r4) {
        // 0x8041AB28: sb          $zero, 0xBB($v0)
        MEM_B(0XBB, ctx->r2) = 0;
            goto L_8041A6FC;
    }
    // 0x8041AB28: sb          $zero, 0xBB($v0)
    MEM_B(0XBB, ctx->r2) = 0;
    // 0x8041AB2C: b           L_8041A714
    // 0x8041AB30: sb          $v1, 0xBC($v0)
    MEM_B(0XBC, ctx->r2) = ctx->r3;
        goto L_8041A714;
    // 0x8041AB30: sb          $v1, 0xBC($v0)
    MEM_B(0XBC, ctx->r2) = ctx->r3;
L_8041AB34:
    // 0x8041AB34: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041AB38: bne         $v1, $v0, L_8041AA34
    if (ctx->r3 != ctx->r2) {
        // 0x8041AB3C: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041AA34;
    }
    // 0x8041AB3C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AB40: b           L_8041AA2C
    // 0x8041AB44: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
        goto L_8041AA2C;
    // 0x8041AB44: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041AB48:
    // 0x8041AB48: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AB4C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AB50: sltiu       $a2, $a0, 0x7
    ctx->r6 = ctx->r4 < 0X7 ? 1 : 0;
    // 0x8041AB54: beq         $a2, $zero, L_8041A6F4
    if (ctx->r6 == 0) {
        // 0x8041AB58: sb          $v1, 0xBC($v0)
        MEM_B(0XBC, ctx->r2) = ctx->r3;
            goto L_8041A6F4;
    }
    // 0x8041AB58: sb          $v1, 0xBC($v0)
    MEM_B(0XBC, ctx->r2) = ctx->r3;
    // 0x8041AB5C: b           L_8041A718
    // 0x8041AB60: lbu         $a2, 0x27($a1)
    ctx->r6 = MEM_BU(ctx->r5, 0X27);
        goto L_8041A718;
    // 0x8041AB60: lbu         $a2, 0x27($a1)
    ctx->r6 = MEM_BU(ctx->r5, 0X27);
L_8041AB64:
    // 0x8041AB64: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AB68: lui         $t1, 0x8042
    ctx->r9 = S32(0X8042 << 16);
    // 0x8041AB6C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AB70: lwc1        $f0, -0x3F50($t1)
    ctx->f0.u32l = MEM_W(ctx->r9, -0X3F50);
    // 0x8041AB74: sltiu       $a1, $v1, 0x26
    ctx->r5 = ctx->r3 < 0X26 ? 1 : 0;
    // 0x8041AB78: beq         $a1, $zero, L_8041A760
    if (ctx->r5 == 0) {
        // 0x8041AB7C: swc1        $f0, 0x12C($v0)
        MEM_W(0X12C, ctx->r2) = ctx->f0.u32l;
            goto L_8041A760;
    }
    // 0x8041AB7C: swc1        $f0, 0x12C($v0)
    MEM_W(0X12C, ctx->r2) = ctx->f0.u32l;
    // 0x8041AB80: b           L_8041A784
    // 0x8041AB84: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
        goto L_8041A784;
    // 0x8041AB84: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_8041AB88:
    // 0x8041AB88: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041AB8C: lw          $a1, 0x1668($v0)
    ctx->r5 = MEM_W(ctx->r2, 0X1668);
    // 0x8041AB90: sltiu       $v0, $v1, 0x64
    ctx->r2 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041AB94: beq         $v0, $zero, L_8041A8AC
    if (ctx->r2 == 0) {
        // 0x8041AB98: sb          $zero, 0x2E4($a1)
        MEM_B(0X2E4, ctx->r5) = 0;
            goto L_8041A8AC;
    }
    // 0x8041AB98: sb          $zero, 0x2E4($a1)
    MEM_B(0X2E4, ctx->r5) = 0;
L_8041AB9C:
    // 0x8041AB9C: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041ABA0: bne         $v1, $v0, L_8041A8C4
    if (ctx->r3 != ctx->r2) {
        // 0x8041ABA4: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041A8C4;
    }
    // 0x8041ABA4: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041ABA8: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041ABAC: addiu       $a1, $zero, 0x12C
    ctx->r5 = ADD32(0, 0X12C);
    // 0x8041ABB0: b           L_8041AA64
    // 0x8041ABB4: sh          $a1, 0x2F8($v0)
    MEM_H(0X2F8, ctx->r2) = ctx->r5;
        goto L_8041AA64;
    // 0x8041ABB4: sh          $a1, 0x2F8($v0)
    MEM_H(0X2F8, ctx->r2) = ctx->r5;
L_8041ABB8:
    // 0x8041ABB8: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    // 0x8041ABBC: addiu       $a1, $zero, 0xA
    ctx->r5 = ADD32(0, 0XA);
    // 0x8041ABC0: bne         $v1, $a2, L_8041A880
    if (ctx->r3 != ctx->r6) {
        // 0x8041ABC4: sh          $a1, 0x284($v0)
        MEM_H(0X284, ctx->r2) = ctx->r5;
            goto L_8041A880;
    }
    // 0x8041ABC4: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
    // 0x8041ABC8: b           L_8041AA2C
    // 0x8041ABCC: sb          $zero, 0x2A0($v0)
    MEM_B(0X2A0, ctx->r2) = 0;
        goto L_8041AA2C;
    // 0x8041ABCC: sb          $zero, 0x2A0($v0)
    MEM_B(0X2A0, ctx->r2) = 0;
L_8041ABD0:
    // 0x8041ABD0: sltiu       $a2, $v1, 0x64
    ctx->r6 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041ABD4: beq         $a2, $zero, L_8041A8AC
    if (ctx->r6 == 0) {
        // 0x8041ABD8: sb          $zero, 0x2E4($a1)
        MEM_B(0X2E4, ctx->r5) = 0;
            goto L_8041A8AC;
    }
    // 0x8041ABD8: sb          $zero, 0x2E4($a1)
    MEM_B(0X2E4, ctx->r5) = 0;
    // 0x8041ABDC: b           L_8041AA78
    // 0x8041ABE0: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
        goto L_8041AA78;
    // 0x8041ABE0: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
L_8041ABE4:
    // 0x8041ABE4: b           L_8041A880
    // 0x8041ABE8: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
        goto L_8041A880;
    // 0x8041ABE8: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
L_8041ABEC:
    // 0x8041ABEC: b           L_8041A624
    // 0x8041ABF0: sltiu       $a3, $v1, 0x32
    ctx->r7 = ctx->r3 < 0X32 ? 1 : 0;
        goto L_8041A624;
    // 0x8041ABF0: sltiu       $a3, $v1, 0x32
    ctx->r7 = ctx->r3 < 0X32 ? 1 : 0;
L_8041ABF4:
    // 0x8041ABF4: addiu       $a1, $zero, 0xF
    ctx->r5 = ADD32(0, 0XF);
    // 0x8041ABF8: b           L_8041A880
    // 0x8041ABFC: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
        goto L_8041A880;
    // 0x8041ABFC: sh          $a1, 0x284($v0)
    MEM_H(0X284, ctx->r2) = ctx->r5;
L_8041AC00:
    // 0x8041AC00: addiu       $v0, $zero, 0x18
    ctx->r2 = ADD32(0, 0X18);
    // 0x8041AC04: b           L_8041A7B8
    // 0x8041AC08: sh          $v0, 0x174($t1)
    MEM_H(0X174, ctx->r9) = ctx->r2;
        goto L_8041A7B8;
    // 0x8041AC08: sh          $v0, 0x174($t1)
    MEM_H(0X174, ctx->r9) = ctx->r2;
L_8041AC0C:
    // 0x8041AC0C: addiu       $v0, $zero, 0x8
    ctx->r2 = ADD32(0, 0X8);
    // 0x8041AC10: b           L_8041A7E4
    // 0x8041AC14: sh          $v0, 0x1E8($t2)
    MEM_H(0X1E8, ctx->r10) = ctx->r2;
        goto L_8041A7E4;
    // 0x8041AC14: sh          $v0, 0x1E8($t2)
    MEM_H(0X1E8, ctx->r10) = ctx->r2;
L_8041AC18:
    // 0x8041AC18: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041AC1C: lwc1        $f0, -0x3F48($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F48);
    // 0x8041AC20: b           L_8041A804
    // 0x8041AC24: swc1        $f0, 0x23C($t1)
    MEM_W(0X23C, ctx->r9) = ctx->f0.u32l;
        goto L_8041A804;
    // 0x8041AC24: swc1        $f0, 0x23C($t1)
    MEM_W(0X23C, ctx->r9) = ctx->f0.u32l;
L_8041AC28:
    // 0x8041AC28: addiu       $a1, $zero, 0x4B0
    ctx->r5 = ADD32(0, 0X4B0);
    // 0x8041AC2C: b           L_8041A854
    // 0x8041AC30: sh          $a1, 0x22C($v0)
    MEM_H(0X22C, ctx->r2) = ctx->r5;
        goto L_8041A854;
    // 0x8041AC30: sh          $a1, 0x22C($v0)
    MEM_H(0X22C, ctx->r2) = ctx->r5;
L_8041AC34:
    // 0x8041AC34: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041AC38: b           L_8041A624
    // 0x8041AC3C: sh          $a0, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r4;
        goto L_8041A624;
    // 0x8041AC3C: sh          $a0, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r4;
L_8041AC40:
    // 0x8041AC40: addiu       $v0, $zero, 0x5
    ctx->r2 = ADD32(0, 0X5);
    // 0x8041AC44: b           L_8041A740
    // 0x8041AC48: sh          $v0, 0xEC($t1)
    MEM_H(0XEC, ctx->r9) = ctx->r2;
        goto L_8041A740;
    // 0x8041AC48: sh          $v0, 0xEC($t1)
    MEM_H(0XEC, ctx->r9) = ctx->r2;
L_8041AC4C:
    // 0x8041AC4C: addiu       $v0, $zero, 0x50
    ctx->r2 = ADD32(0, 0X50);
    // 0x8041AC50: b           L_8041A970
    // 0x8041AC54: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
        goto L_8041A970;
    // 0x8041AC54: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
L_8041AC58:
    // 0x8041AC58: addiu       $v0, $zero, 0x5A
    ctx->r2 = ADD32(0, 0X5A);
    // 0x8041AC5C: b           L_8041A970
    // 0x8041AC60: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
        goto L_8041A970;
    // 0x8041AC60: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
L_8041AC64:
    // 0x8041AC64: addiu       $v0, $zero, 0x1F4
    ctx->r2 = ADD32(0, 0X1F4);
    // 0x8041AC68: b           L_8041A908
    // 0x8041AC6C: sh          $v0, 0x2F8($a1)
    MEM_H(0X2F8, ctx->r5) = ctx->r2;
        goto L_8041A908;
    // 0x8041AC6C: sh          $v0, 0x2F8($a1)
    MEM_H(0X2F8, ctx->r5) = ctx->r2;
L_8041AC70:
    // 0x8041AC70: addiu       $v0, $zero, 0x41
    ctx->r2 = ADD32(0, 0X41);
    // 0x8041AC74: b           L_8041A970
    // 0x8041AC78: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
        goto L_8041A970;
    // 0x8041AC78: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
L_8041AC7C:
    // 0x8041AC7C: addiu       $v0, $zero, 0x46
    ctx->r2 = ADD32(0, 0X46);
    // 0x8041AC80: b           L_8041A970
    // 0x8041AC84: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
        goto L_8041A970;
    // 0x8041AC84: sh          $v0, 0x2FA($a0)
    MEM_H(0X2FA, ctx->r4) = ctx->r2;
L_8041AC88:
    // 0x8041AC88: beql        $a1, $zero, L_8041AA4C
    if (ctx->r5 == 0) {
        // 0x8041AC8C: lw          $v0, 0x1668($v0)
        ctx->r2 = MEM_W(ctx->r2, 0X1668);
            goto L_8041AA4C;
    }
    goto skip_6;
    // 0x8041AC8C: lw          $v0, 0x1668($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1668);
    skip_6:
    // 0x8041AC90: b           L_8041A8D4
    // 0x8041AC94: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
        goto L_8041A8D4;
    // 0x8041AC94: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
;}
RECOMP_FUNC void hm_func_8041AC98(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041AC98: lui         $a3, 0x8008
    ctx->r7 = S32(0X8008 << 16);
    // 0x8041AC9C: addiu       $a3, $a3, -0x4580
    ctx->r7 = ADD32(ctx->r7, -0X4580);
    // 0x8041ACA0: lbu         $v0, 0x25($a3)
    ctx->r2 = MEM_BU(ctx->r7, 0X25);
    // 0x8041ACA4: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041ACA8: beq         $v0, $a2, L_8041B084
    if (ctx->r2 == ctx->r6) {
        // 0x8041ACAC: lbu         $v1, 0x26($a3)
        ctx->r3 = MEM_BU(ctx->r7, 0X26);
            goto L_8041B084;
    }
    // 0x8041ACAC: lbu         $v1, 0x26($a3)
    ctx->r3 = MEM_BU(ctx->r7, 0X26);
    // 0x8041ACB0: sltiu       $a0, $v0, 0x4
    ctx->r4 = ctx->r2 < 0X4 ? 1 : 0;
    // 0x8041ACB4: bne         $a0, $zero, L_8041B28C
    if (ctx->r4 != 0) {
        // 0x8041ACB8: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B28C;
    }
    // 0x8041ACB8: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041ACBC: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041ACC0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041ACC4: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041ACC8: lwc1        $f0, -0x3F3C($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F3C);
    // 0x8041ACCC: sltiu       $t0, $v0, 0x10
    ctx->r8 = ctx->r2 < 0X10 ? 1 : 0;
    // 0x8041ACD0: addiu       $t2, $zero, 0x136
    ctx->r10 = ADD32(0, 0X136);
    // 0x8041ACD4: or          $t1, $a0, $zero
    ctx->r9 = ctx->r4 | 0;
    // 0x8041ACD8: swc1        $f0, 0x1C($a0)
    MEM_W(0X1C, ctx->r4) = ctx->f0.u32l;
    // 0x8041ACDC: bne         $t0, $zero, L_8041B28C
    if (ctx->r8 != 0) {
        // 0x8041ACE0: sh          $t2, 0xC($a0)
        MEM_H(0XC, ctx->r4) = ctx->r10;
            goto L_8041B28C;
    }
    // 0x8041ACE0: sh          $t2, 0xC($a0)
    MEM_H(0XC, ctx->r4) = ctx->r10;
    // 0x8041ACE4: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041ACE8: lwc1        $f0, -0x3F50($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F50);
    // 0x8041ACEC: sltiu       $t0, $v0, 0x1F
    ctx->r8 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041ACF0: addiu       $t2, $zero, 0x168
    ctx->r10 = ADD32(0, 0X168);
    // 0x8041ACF4: swc1        $f0, 0x1C($a0)
    MEM_W(0X1C, ctx->r4) = ctx->f0.u32l;
    // 0x8041ACF8: bne         $t0, $zero, L_8041B28C
    if (ctx->r8 != 0) {
        // 0x8041ACFC: sh          $t2, 0xC($a0)
        MEM_H(0XC, ctx->r4) = ctx->r10;
            goto L_8041B28C;
    }
    // 0x8041ACFC: sh          $t2, 0xC($a0)
    MEM_H(0XC, ctx->r4) = ctx->r10;
    // 0x8041AD00: addiu       $t0, $zero, 0x3
    ctx->r8 = ADD32(0, 0X3);
    // 0x8041AD04: sh          $t0, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r8;
    // 0x8041AD08: addiu       $t0, $zero, 0x8D
    ctx->r8 = ADD32(0, 0X8D);
    // 0x8041AD0C: beq         $v1, $a2, L_8041B228
    if (ctx->r3 == ctx->r6) {
        // 0x8041AD10: sh          $t0, 0xC($a0)
        MEM_H(0XC, ctx->r4) = ctx->r8;
            goto L_8041B228;
    }
    // 0x8041AD10: sh          $t0, 0xC($a0)
    MEM_H(0XC, ctx->r4) = ctx->r8;
    // 0x8041AD14: addiu       $a2, $zero, 0x4
    ctx->r6 = ADD32(0, 0X4);
    // 0x8041AD18: sltiu       $t0, $v1, 0xF
    ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041AD1C: bne         $t0, $zero, L_8041ADAC
    if (ctx->r8 != 0) {
        // 0x8041AD20: sb          $a2, 0xC4($a0)
        MEM_B(0XC4, ctx->r4) = ctx->r6;
            goto L_8041ADAC;
    }
    // 0x8041AD20: sb          $a2, 0xC4($a0)
    MEM_B(0XC4, ctx->r4) = ctx->r6;
L_8041AD24:
    // 0x8041AD24: sltiu       $a0, $v1, 0x1D
    ctx->r4 = ctx->r3 < 0X1D ? 1 : 0;
    // 0x8041AD28: bne         $a0, $zero, L_8041B3B0
    if (ctx->r4 != 0) {
        // 0x8041AD2C: addiu       $a0, $zero, 0x64
        ctx->r4 = ADD32(0, 0X64);
            goto L_8041B3B0;
    }
    // 0x8041AD2C: addiu       $a0, $zero, 0x64
    ctx->r4 = ADD32(0, 0X64);
    // 0x8041AD30: sh          $a0, 0x96($t1)
    MEM_H(0X96, ctx->r9) = ctx->r4;
L_8041AD34:
    // 0x8041AD34: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
L_8041AD38:
    // 0x8041AD38: beql        $v0, $a0, L_8041B190
    if (ctx->r2 == ctx->r4) {
        // 0x8041AD3C: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B190;
    }
    goto skip_0;
    // 0x8041AD3C: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    skip_0:
L_8041AD40:
    // 0x8041AD40: sltiu       $a0, $v0, 0x1D
    ctx->r4 = ctx->r2 < 0X1D ? 1 : 0;
L_8041AD44:
    // 0x8041AD44: bne         $a0, $zero, L_8041B248
    if (ctx->r4 != 0) {
        // 0x8041AD48: addiu       $a0, $zero, 0x1
        ctx->r4 = ADD32(0, 0X1);
            goto L_8041B248;
    }
    // 0x8041AD48: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041AD4C: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
L_8041AD50:
    // 0x8041AD50: lui         $t0, 0x8042
    ctx->r8 = S32(0X8042 << 16);
    // 0x8041AD54: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041AD58: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041AD5C: lwc1        $f0, -0x3F48($t0)
    ctx->f0.u32l = MEM_W(ctx->r8, -0X3F48);
    // 0x8041AD60: sltiu       $a2, $v0, 0x27
    ctx->r6 = ctx->r2 < 0X27 ? 1 : 0;
    // 0x8041AD64: addiu       $t0, $zero, 0x12C
    ctx->r8 = ADD32(0, 0X12C);
    // 0x8041AD68: swc1        $f0, 0xE8($a0)
    MEM_W(0XE8, ctx->r4) = ctx->f0.u32l;
    // 0x8041AD6C: bne         $a2, $zero, L_8041B360
    if (ctx->r6 != 0) {
        // 0x8041AD70: sh          $t0, 0xD8($a0)
        MEM_H(0XD8, ctx->r4) = ctx->r8;
            goto L_8041B360;
    }
    // 0x8041AD70: sh          $t0, 0xD8($a0)
    MEM_H(0XD8, ctx->r4) = ctx->r8;
    // 0x8041AD74: lui         $t0, 0x8042
    ctx->r8 = S32(0X8042 << 16);
    // 0x8041AD78: lwc1        $f0, -0x3F34($t0)
    ctx->f0.u32l = MEM_W(ctx->r8, -0X3F34);
    // 0x8041AD7C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AD80: addiu       $t0, $zero, 0x190
    ctx->r8 = ADD32(0, 0X190);
    // 0x8041AD84: sh          $t0, 0xD8($a0)
    MEM_H(0XD8, ctx->r4) = ctx->r8;
    // 0x8041AD88: lbu         $a2, 0x27($a3)
    ctx->r6 = MEM_BU(ctx->r7, 0X27);
    // 0x8041AD8C: swc1        $f0, 0xE8($a0)
    MEM_W(0XE8, ctx->r4) = ctx->f0.u32l;
    // 0x8041AD90: bne         $v1, $a1, L_8041ADE0
    if (ctx->r3 != ctx->r5) {
        // 0x8041AD94: sltiu       $t0, $v1, 0xF
        ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
            goto L_8041ADE0;
    }
    // 0x8041AD94: sltiu       $t0, $v1, 0xF
    ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041AD98: addiu       $a1, $zero, 0x5A
    ctx->r5 = ADD32(0, 0X5A);
    // 0x8041AD9C: sh          $a1, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r5;
    // 0x8041ADA0: sb          $v1, 0x14C($a0)
    MEM_B(0X14C, ctx->r4) = ctx->r3;
    // 0x8041ADA4: b           L_8041ADE8
    // 0x8041ADA8: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
        goto L_8041ADE8;
    // 0x8041ADA8: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041ADAC:
    // 0x8041ADAC: lui         $t1, 0x8042
    ctx->r9 = S32(0X8042 << 16);
    // 0x8041ADB0: lwc1        $f0, -0x3F48($t1)
    ctx->f0.u32l = MEM_W(ctx->r9, -0X3F48);
    // 0x8041ADB4: sltiu       $a2, $v0, 0x27
    ctx->r6 = ctx->r2 < 0X27 ? 1 : 0;
    // 0x8041ADB8: addiu       $t1, $zero, 0x12C
    ctx->r9 = ADD32(0, 0X12C);
    // 0x8041ADBC: swc1        $f0, 0xE8($a0)
    MEM_W(0XE8, ctx->r4) = ctx->f0.u32l;
    // 0x8041ADC0: bne         $a2, $zero, L_8041B360
    if (ctx->r6 != 0) {
        // 0x8041ADC4: sh          $t1, 0xD8($a0)
        MEM_H(0XD8, ctx->r4) = ctx->r9;
            goto L_8041B360;
    }
    // 0x8041ADC4: sh          $t1, 0xD8($a0)
    MEM_H(0XD8, ctx->r4) = ctx->r9;
    // 0x8041ADC8: lui         $a1, 0x8042
    ctx->r5 = S32(0X8042 << 16);
    // 0x8041ADCC: lwc1        $f0, -0x3F34($a1)
    ctx->f0.u32l = MEM_W(ctx->r5, -0X3F34);
    // 0x8041ADD0: lbu         $a2, 0x27($a3)
    ctx->r6 = MEM_BU(ctx->r7, 0X27);
    // 0x8041ADD4: addiu       $a1, $zero, 0x190
    ctx->r5 = ADD32(0, 0X190);
    // 0x8041ADD8: swc1        $f0, 0xE8($a0)
    MEM_W(0XE8, ctx->r4) = ctx->f0.u32l;
    // 0x8041ADDC: sh          $a1, 0xD8($a0)
    MEM_H(0XD8, ctx->r4) = ctx->r5;
L_8041ADE0:
    // 0x8041ADE0: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041ADE4: sb          $a1, 0x14C($a0)
    MEM_B(0X14C, ctx->r4) = ctx->r5;
L_8041ADE8:
    // 0x8041ADE8: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041ADEC: sh          $a1, 0x126($a0)
    MEM_H(0X126, ctx->r4) = ctx->r5;
    // 0x8041ADF0: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041ADF4: sh          $a1, 0x112($a0)
    MEM_H(0X112, ctx->r4) = ctx->r5;
    // 0x8041ADF8: addiu       $a1, $zero, 0x8
    ctx->r5 = ADD32(0, 0X8);
    // 0x8041ADFC: sh          $zero, 0x128($a0)
    MEM_H(0X128, ctx->r4) = 0;
    // 0x8041AE00: sh          $a1, 0x11A($a0)
    MEM_H(0X11A, ctx->r4) = ctx->r5;
L_8041AE04:
    // 0x8041AE04: bne         $t0, $zero, L_8041AE28
    if (ctx->r8 != 0) {
        // 0x8041AE08: addiu       $t0, $zero, 0x1
        ctx->r8 = ADD32(0, 0X1);
            goto L_8041AE28;
    }
    // 0x8041AE08: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041AE0C:
    // 0x8041AE0C: sltiu       $a1, $v1, 0x23
    ctx->r5 = ctx->r3 < 0X23 ? 1 : 0;
L_8041AE10:
    // 0x8041AE10: bne         $a1, $zero, L_8041B3A4
    if (ctx->r5 != 0) {
        // 0x8041AE14: addiu       $a1, $zero, 0x64
        ctx->r5 = ADD32(0, 0X64);
            goto L_8041B3A4;
    }
    // 0x8041AE14: addiu       $a1, $zero, 0x64
    ctx->r5 = ADD32(0, 0X64);
    // 0x8041AE18: sh          $a1, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r5;
L_8041AE1C:
    // 0x8041AE1C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041AE20: beq         $v0, $a0, L_8041B1EC
    if (ctx->r2 == ctx->r4) {
        // 0x8041AE24: addiu       $t0, $zero, 0x1
        ctx->r8 = ADD32(0, 0X1);
            goto L_8041B1EC;
    }
    // 0x8041AE24: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041AE28:
    // 0x8041AE28: beq         $a2, $t0, L_8041B114
    if (ctx->r6 == ctx->r8) {
        // 0x8041AE2C: sltiu       $a0, $v0, 0x19
        ctx->r4 = ctx->r2 < 0X19 ? 1 : 0;
            goto L_8041B114;
    }
    // 0x8041AE2C: sltiu       $a0, $v0, 0x19
    ctx->r4 = ctx->r2 < 0X19 ? 1 : 0;
    // 0x8041AE30: bne         $a0, $zero, L_8041B2E4
    if (ctx->r4 != 0) {
        // 0x8041AE34: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B2E4;
    }
    // 0x8041AE34: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041AE38: lw          $a0, 0x166C($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X166C);
    // 0x8041AE3C: addiu       $t1, $zero, 0x3
    ctx->r9 = ADD32(0, 0X3);
    // 0x8041AE40: sltiu       $a1, $v0, 0x1F
    ctx->r5 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041AE44: bne         $a1, $zero, L_8041AE50
    if (ctx->r5 != 0) {
        // 0x8041AE48: sb          $t1, 0x1CF($a0)
        MEM_B(0X1CF, ctx->r4) = ctx->r9;
            goto L_8041AE50;
    }
    // 0x8041AE48: sb          $t1, 0x1CF($a0)
    MEM_B(0X1CF, ctx->r4) = ctx->r9;
    // 0x8041AE4C: sb          $t0, 0x1CE($a0)
    MEM_B(0X1CE, ctx->r4) = ctx->r8;
L_8041AE50:
    // 0x8041AE50: sltiu       $t0, $a2, 0x14
    ctx->r8 = ctx->r6 < 0X14 ? 1 : 0;
    // 0x8041AE54: bnel        $t0, $zero, L_8041AE84
    if (ctx->r8 != 0) {
        // 0x8041AE58: sltiu       $a0, $v0, 0x33
        ctx->r4 = ctx->r2 < 0X33 ? 1 : 0;
            goto L_8041AE84;
    }
    goto skip_1;
    // 0x8041AE58: sltiu       $a0, $v0, 0x33
    ctx->r4 = ctx->r2 < 0X33 ? 1 : 0;
    skip_1:
L_8041AE5C:
    // 0x8041AE5C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_8041AE60:
    // 0x8041AE60: sh          $a1, 0x1AE($a0)
    MEM_H(0X1AE, ctx->r4) = ctx->r5;
    // 0x8041AE64: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041AE68: sh          $a1, 0x19A($a0)
    MEM_H(0X19A, ctx->r4) = ctx->r5;
    // 0x8041AE6C: addiu       $a1, $zero, 0x8
    ctx->r5 = ADD32(0, 0X8);
    // 0x8041AE70: sh          $zero, 0x1B0($a0)
    MEM_H(0X1B0, ctx->r4) = 0;
    // 0x8041AE74: sh          $a1, 0x1A2($a0)
    MEM_H(0X1A2, ctx->r4) = ctx->r5;
L_8041AE78:
    // 0x8041AE78: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041AE7C: beq         $v0, $a0, L_8041B0CC
    if (ctx->r2 == ctx->r4) {
        // 0x8041AE80: sltiu       $a0, $v0, 0x33
        ctx->r4 = ctx->r2 < 0X33 ? 1 : 0;
            goto L_8041B0CC;
    }
    // 0x8041AE80: sltiu       $a0, $v0, 0x33
    ctx->r4 = ctx->r2 < 0X33 ? 1 : 0;
L_8041AE84:
    // 0x8041AE84: bne         $a0, $zero, L_8041B160
    if (ctx->r4 != 0) {
        // 0x8041AE88: addiu       $a0, $zero, 0x1
        ctx->r4 = ADD32(0, 0X1);
            goto L_8041B160;
    }
    // 0x8041AE88: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
L_8041AE8C:
    // 0x8041AE8C: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041AE90: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
    // 0x8041AE94: addiu       $t1, $zero, 0x28
    ctx->r9 = ADD32(0, 0X28);
    // 0x8041AE98: sh          $t1, 0x240($a0)
    MEM_H(0X240, ctx->r4) = ctx->r9;
    // 0x8041AE9C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041AEA0: addiu       $t1, $zero, 0x64
    ctx->r9 = ADD32(0, 0X64);
    // 0x8041AEA4: beq         $v1, $a1, L_8041B218
    if (ctx->r3 == ctx->r5) {
        // 0x8041AEA8: sh          $t1, 0x22C($a0)
        MEM_H(0X22C, ctx->r4) = ctx->r9;
            goto L_8041B218;
    }
    // 0x8041AEA8: sh          $t1, 0x22C($a0)
    MEM_H(0X22C, ctx->r4) = ctx->r9;
    // 0x8041AEAC: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
L_8041AEB0:
    // 0x8041AEB0: addiu       $t1, $zero, 0x4
    ctx->r9 = ADD32(0, 0X4);
L_8041AEB4:
    // 0x8041AEB4: sb          $t1, 0x2A0($a0)
    MEM_B(0X2A0, ctx->r4) = ctx->r9;
L_8041AEB8:
    // 0x8041AEB8: sltiu       $a0, $v1, 0xF
    ctx->r4 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041AEBC: bne         $a0, $zero, L_8041AED8
    if (ctx->r4 != 0) {
        // 0x8041AEC0: lui         $t1, 0x8041
        ctx->r9 = S32(0X8041 << 16);
            goto L_8041AED8;
    }
    // 0x8041AEC0: lui         $t1, 0x8041
    ctx->r9 = S32(0X8041 << 16);
    // 0x8041AEC4: sltiu       $a0, $v1, 0x23
    ctx->r4 = ctx->r3 < 0X23 ? 1 : 0;
    // 0x8041AEC8: bne         $a0, $zero, L_8041B38C
    if (ctx->r4 != 0) {
        // 0x8041AECC: lw          $t1, 0x166C($t1)
        ctx->r9 = MEM_W(ctx->r9, 0X166C);
            goto L_8041B38C;
    }
    // 0x8041AECC: lw          $t1, 0x166C($t1)
    ctx->r9 = MEM_W(ctx->r9, 0X166C);
    // 0x8041AED0: addiu       $a0, $zero, 0x64
    ctx->r4 = ADD32(0, 0X64);
    // 0x8041AED4: sh          $a0, 0x272($t1)
    MEM_H(0X272, ctx->r9) = ctx->r4;
L_8041AED8:
    // 0x8041AED8: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041AEDC: beql        $a2, $a0, L_8041B068
    if (ctx->r6 == ctx->r4) {
        // 0x8041AEE0: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041B068;
    }
    goto skip_2;
    // 0x8041AEE0: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    skip_2:
    // 0x8041AEE4: beq         $v0, $a0, L_8041B0FC
    if (ctx->r2 == ctx->r4) {
        // 0x8041AEE8: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041B0FC;
    }
    // 0x8041AEE8: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041AEEC: bne         $t0, $zero, L_8041AF04
    if (ctx->r8 != 0) {
        // 0x8041AEF0: lw          $a0, 0x166C($a0)
        ctx->r4 = MEM_W(ctx->r4, 0X166C);
            goto L_8041AF04;
    }
    // 0x8041AEF0: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
L_8041AEF4:
    // 0x8041AEF4: addiu       $t1, $zero, 0x4
    ctx->r9 = ADD32(0, 0X4);
    // 0x8041AEF8: sh          $t1, 0x30C($a0)
    MEM_H(0X30C, ctx->r4) = ctx->r9;
    // 0x8041AEFC: addiu       $t1, $zero, 0x5B
    ctx->r9 = ADD32(0, 0X5B);
    // 0x8041AF00: sh          $t1, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r9;
L_8041AF04:
    // 0x8041AF04: sltiu       $a0, $v0, 0x28
    ctx->r4 = ctx->r2 < 0X28 ? 1 : 0;
    // 0x8041AF08: bne         $a0, $zero, L_8041AF1C
    if (ctx->r4 != 0) {
        // 0x8041AF0C: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041AF1C;
    }
    // 0x8041AF0C: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041AF10: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
    // 0x8041AF14: addiu       $t1, $zero, 0x69
    ctx->r9 = ADD32(0, 0X69);
    // 0x8041AF18: sh          $t1, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r9;
L_8041AF1C:
    // 0x8041AF1C: lbu         $a0, 0x24($a3)
    ctx->r4 = MEM_BU(ctx->r7, 0X24);
L_8041AF20:
    // 0x8041AF20: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041AF24: bne         $a0, $a3, L_8041AF40
    if (ctx->r4 != ctx->r7) {
        // 0x8041AF28: nop
    
            goto L_8041AF40;
    }
    // 0x8041AF28: nop

    // 0x8041AF2C: lui         $a3, 0x8041
    ctx->r7 = S32(0X8041 << 16);
    // 0x8041AF30: lw          $a3, 0x166C($a3)
    ctx->r7 = MEM_W(ctx->r7, 0X166C);
    // 0x8041AF34: sb          $zero, 0x36C($a3)
    MEM_B(0X36C, ctx->r7) = 0;
    // 0x8041AF38: sb          $zero, 0x364($a3)
    MEM_B(0X364, ctx->r7) = 0;
    // 0x8041AF3C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
L_8041AF40:
    // 0x8041AF40: bne         $a2, $a3, L_8041AF58
    if (ctx->r6 != ctx->r7) {
        // 0x8041AF44: addiu       $a2, $zero, 0x1
        ctx->r6 = ADD32(0, 0X1);
            goto L_8041AF58;
    }
    // 0x8041AF44: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041AF48: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AF4C: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AF50: sh          $zero, 0x366($a2)
    MEM_H(0X366, ctx->r6) = 0;
    // 0x8041AF54: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
L_8041AF58:
    // 0x8041AF58: bne         $v1, $a2, L_8041AF74
    if (ctx->r3 != ctx->r6) {
        // 0x8041AF5C: addiu       $a2, $zero, 0x1
        ctx->r6 = ADD32(0, 0X1);
            goto L_8041AF74;
    }
    // 0x8041AF5C: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041AF60: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AF64: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AF68: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041AF6C: sh          $a3, 0x33C($a2)
    MEM_H(0X33C, ctx->r6) = ctx->r7;
    // 0x8041AF70: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
L_8041AF74:
    // 0x8041AF74: bne         $v0, $a2, L_8041AF94
    if (ctx->r2 != ctx->r6) {
        // 0x8041AF78: sltiu       $a2, $a0, 0x14
        ctx->r6 = ctx->r4 < 0X14 ? 1 : 0;
            goto L_8041AF94;
    }
    // 0x8041AF78: sltiu       $a2, $a0, 0x14
    ctx->r6 = ctx->r4 < 0X14 ? 1 : 0;
    // 0x8041AF7C: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AF80: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AF84: addiu       $a3, $zero, 0x5A
    ctx->r7 = ADD32(0, 0X5A);
    // 0x8041AF88: sh          $a3, 0x33E($a2)
    MEM_H(0X33E, ctx->r6) = ctx->r7;
    // 0x8041AF8C: sb          $zero, 0x36A($a2)
    MEM_B(0X36A, ctx->r6) = 0;
    // 0x8041AF90: sltiu       $a2, $a0, 0x14
    ctx->r6 = ctx->r4 < 0X14 ? 1 : 0;
L_8041AF94:
    // 0x8041AF94: bne         $a2, $zero, L_8041AFB0
    if (ctx->r6 != 0) {
        // 0x8041AF98: lui         $a2, 0x8041
        ctx->r6 = S32(0X8041 << 16);
            goto L_8041AFB0;
    }
    // 0x8041AF98: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AF9C: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AFA0: addiu       $a3, $zero, 0x3
    ctx->r7 = ADD32(0, 0X3);
    // 0x8041AFA4: sb          $a3, 0x36C($a2)
    MEM_B(0X36C, ctx->r6) = ctx->r7;
    // 0x8041AFA8: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041AFAC: sb          $a3, 0x364($a2)
    MEM_B(0X364, ctx->r6) = ctx->r7;
L_8041AFB0:
    // 0x8041AFB0: bne         $t0, $zero, L_8041AFCC
    if (ctx->r8 != 0) {
        // 0x8041AFB4: sltiu       $a2, $v1, 0x19
        ctx->r6 = ctx->r3 < 0X19 ? 1 : 0;
            goto L_8041AFCC;
    }
    // 0x8041AFB4: sltiu       $a2, $v1, 0x19
    ctx->r6 = ctx->r3 < 0X19 ? 1 : 0;
    // 0x8041AFB8: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AFBC: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AFC0: addiu       $a3, $zero, 0x201
    ctx->r7 = ADD32(0, 0X201);
    // 0x8041AFC4: sh          $a3, 0x366($a2)
    MEM_H(0X366, ctx->r6) = ctx->r7;
    // 0x8041AFC8: sltiu       $a2, $v1, 0x19
    ctx->r6 = ctx->r3 < 0X19 ? 1 : 0;
L_8041AFCC:
    // 0x8041AFCC: bne         $a2, $zero, L_8041AFE8
    if (ctx->r6 != 0) {
        // 0x8041AFD0: lui         $a2, 0x8041
        ctx->r6 = S32(0X8041 << 16);
            goto L_8041AFE8;
    }
    // 0x8041AFD0: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041AFD4: sltiu       $v1, $v1, 0x33
    ctx->r3 = ctx->r3 < 0X33 ? 1 : 0;
    // 0x8041AFD8: bne         $v1, $zero, L_8041B3BC
    if (ctx->r3 != 0) {
        // 0x8041AFDC: lw          $a2, 0x166C($a2)
        ctx->r6 = MEM_W(ctx->r6, 0X166C);
            goto L_8041B3BC;
    }
    // 0x8041AFDC: lw          $a2, 0x166C($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X166C);
    // 0x8041AFE0: addiu       $v1, $zero, 0x14
    ctx->r3 = ADD32(0, 0X14);
    // 0x8041AFE4: sh          $v1, 0x33C($a2)
    MEM_H(0X33C, ctx->r6) = ctx->r3;
L_8041AFE8:
    // 0x8041AFE8: bnel        $a1, $zero, L_8041B01C
    if (ctx->r5 != 0) {
        // 0x8041AFEC: addiu       $v0, $zero, 0x1
        ctx->r2 = ADD32(0, 0X1);
            goto L_8041B01C;
    }
    goto skip_3;
    // 0x8041AFEC: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    skip_3:
    // 0x8041AFF0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041AFF4: sltiu       $v1, $v0, 0x32
    ctx->r3 = ctx->r2 < 0X32 ? 1 : 0;
    // 0x8041AFF8: bne         $v1, $zero, L_8041B3C8
    if (ctx->r3 != 0) {
        // 0x8041AFFC: lw          $a1, 0x166C($a1)
        ctx->r5 = MEM_W(ctx->r5, 0X166C);
            goto L_8041B3C8;
    }
    // 0x8041AFFC: lw          $a1, 0x166C($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X166C);
    // 0x8041B000: addiu       $v1, $zero, 0x64
    ctx->r3 = ADD32(0, 0X64);
    // 0x8041B004: sltiu       $v0, $v0, 0x78
    ctx->r2 = ctx->r2 < 0X78 ? 1 : 0;
    // 0x8041B008: bne         $v0, $zero, L_8041B018
    if (ctx->r2 != 0) {
        // 0x8041B00C: sh          $v1, 0x33E($a1)
        MEM_H(0X33E, ctx->r5) = ctx->r3;
            goto L_8041B018;
    }
    // 0x8041B00C: sh          $v1, 0x33E($a1)
    MEM_H(0X33E, ctx->r5) = ctx->r3;
    // 0x8041B010: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041B014: sb          $v0, 0x36A($a1)
    MEM_B(0X36A, ctx->r5) = ctx->r2;
L_8041B018:
    // 0x8041B018: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
L_8041B01C:
    // 0x8041B01C: beq         $a0, $v0, L_8041B04C
    if (ctx->r4 == ctx->r2) {
        // 0x8041B020: sltiu       $v0, $a0, 0x2D
        ctx->r2 = ctx->r4 < 0X2D ? 1 : 0;
            goto L_8041B04C;
    }
    // 0x8041B020: sltiu       $v0, $a0, 0x2D
    ctx->r2 = ctx->r4 < 0X2D ? 1 : 0;
    // 0x8041B024: bne         $v0, $zero, L_8041B044
    if (ctx->r2 != 0) {
        // 0x8041B028: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041B044;
    }
    // 0x8041B028: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041B02C: lw          $v0, 0x166C($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X166C);
    // 0x8041B030: addiu       $v1, $zero, 0x8
    ctx->r3 = ADD32(0, 0X8);
    // 0x8041B034: sh          $v1, 0x394($v0)
    MEM_H(0X394, ctx->r2) = ctx->r3;
    // 0x8041B038: addiu       $v1, $zero, 0xA5
    ctx->r3 = ADD32(0, 0XA5);
    // 0x8041B03C: jr          $ra
    // 0x8041B040: sh          $v1, 0x380($v0)
    MEM_H(0X380, ctx->r2) = ctx->r3;
    return;
    // 0x8041B040: sh          $v1, 0x380($v0)
    MEM_H(0X380, ctx->r2) = ctx->r3;
L_8041B044:
    // 0x8041B044: jr          $ra
    // 0x8041B048: nop

    return;
    // 0x8041B048: nop

L_8041B04C:
    // 0x8041B04C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041B050: lw          $v0, 0x166C($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X166C);
    // 0x8041B054: addiu       $v1, $zero, 0x4
    ctx->r3 = ADD32(0, 0X4);
    // 0x8041B058: sh          $v1, 0x394($v0)
    MEM_H(0X394, ctx->r2) = ctx->r3;
    // 0x8041B05C: addiu       $v1, $zero, 0xED
    ctx->r3 = ADD32(0, 0XED);
    // 0x8041B060: jr          $ra
    // 0x8041B064: sh          $v1, 0x380($v0)
    MEM_H(0X380, ctx->r2) = ctx->r3;
    return;
    // 0x8041B064: sh          $v1, 0x380($v0)
    MEM_H(0X380, ctx->r2) = ctx->r3;
L_8041B068:
    // 0x8041B068: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
    // 0x8041B06C: addiu       $t1, $zero, 0x78
    ctx->r9 = ADD32(0, 0X78);
    // 0x8041B070: sh          $a2, 0x30C($a0)
    MEM_H(0X30C, ctx->r4) = ctx->r6;
    // 0x8041B074: bne         $v0, $a2, L_8041AF04
    if (ctx->r2 != ctx->r6) {
        // 0x8041B078: sh          $t1, 0x2F8($a0)
        MEM_H(0X2F8, ctx->r4) = ctx->r9;
            goto L_8041AF04;
    }
    // 0x8041B078: sh          $t1, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r9;
    // 0x8041B07C: b           L_8041AF20
    // 0x8041B080: lbu         $a0, 0x24($a3)
    ctx->r4 = MEM_BU(ctx->r7, 0X24);
        goto L_8041AF20;
    // 0x8041B080: lbu         $a0, 0x24($a3)
    ctx->r4 = MEM_BU(ctx->r7, 0X24);
L_8041B084:
    // 0x8041B084: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B088: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B08C: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B090: lui         $a2, 0x8042
    ctx->r6 = S32(0X8042 << 16);
    // 0x8041B094: lwc1        $f0, -0x3F40($a2)
    ctx->f0.u32l = MEM_W(ctx->r6, -0X3F40);
    // 0x8041B098: addiu       $a2, $zero, 0x122
    ctx->r6 = ADD32(0, 0X122);
    // 0x8041B09C: sh          $a2, 0xC($a0)
    MEM_H(0XC, ctx->r4) = ctx->r6;
    // 0x8041B0A0: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B0A4: swc1        $f0, 0x1C($a0)
    MEM_W(0X1C, ctx->r4) = ctx->f0.u32l;
    // 0x8041B0A8: sh          $v0, 0x20($a0)
    MEM_H(0X20, ctx->r4) = ctx->r2;
    // 0x8041B0AC: beq         $v1, $v0, L_8041B2D8
    if (ctx->r3 == ctx->r2) {
        // 0x8041B0B0: sb          $a2, 0xC4($a0)
        MEM_B(0XC4, ctx->r4) = ctx->r6;
            goto L_8041B2D8;
    }
    // 0x8041B0B0: sb          $a2, 0xC4($a0)
    MEM_B(0XC4, ctx->r4) = ctx->r6;
    // 0x8041B0B4: sltiu       $t0, $v1, 0xF
    ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
L_8041B0B8:
    // 0x8041B0B8: bne         $t0, $zero, L_8041AD38
    if (ctx->r8 != 0) {
        // 0x8041B0BC: addiu       $a0, $zero, 0x1
        ctx->r4 = ADD32(0, 0X1);
            goto L_8041AD38;
    }
    // 0x8041B0BC: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041B0C0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B0C4: b           L_8041AD24
    // 0x8041B0C8: lw          $t1, 0x166C($a1)
    ctx->r9 = MEM_W(ctx->r5, 0X166C);
        goto L_8041AD24;
    // 0x8041B0C8: lw          $t1, 0x166C($a1)
    ctx->r9 = MEM_W(ctx->r5, 0X166C);
L_8041B0CC:
    // 0x8041B0CC: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B0D0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B0D4: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
L_8041B0D8:
    // 0x8041B0D8: addiu       $t1, $zero, 0x1F
    ctx->r9 = ADD32(0, 0X1F);
    // 0x8041B0DC: sh          $t1, 0x240($a0)
    MEM_H(0X240, ctx->r4) = ctx->r9;
    // 0x8041B0E0: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B0E4: addiu       $t1, $zero, 0x50
    ctx->r9 = ADD32(0, 0X50);
    // 0x8041B0E8: sh          $t1, 0x22C($a0)
    MEM_H(0X22C, ctx->r4) = ctx->r9;
    // 0x8041B0EC: beq         $v1, $a1, L_8041B2C8
    if (ctx->r3 == ctx->r5) {
        // 0x8041B0F0: sb          $zero, 0x2A0($a0)
        MEM_B(0X2A0, ctx->r4) = 0;
            goto L_8041B2C8;
    }
    // 0x8041B0F0: sb          $zero, 0x2A0($a0)
    MEM_B(0X2A0, ctx->r4) = 0;
    // 0x8041B0F4: b           L_8041AEB8
    // 0x8041B0F8: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
        goto L_8041AEB8;
    // 0x8041B0F8: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
L_8041B0FC:
    // 0x8041B0FC: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
    // 0x8041B100: addiu       $t1, $zero, 0x78
    ctx->r9 = ADD32(0, 0X78);
    // 0x8041B104: beq         $t0, $zero, L_8041AEF4
    if (ctx->r8 == 0) {
        // 0x8041B108: sh          $t1, 0x2F8($a0)
        MEM_H(0X2F8, ctx->r4) = ctx->r9;
            goto L_8041AEF4;
    }
    // 0x8041B108: sh          $t1, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r9;
    // 0x8041B10C: b           L_8041AF20
    // 0x8041B110: lbu         $a0, 0x24($a3)
    ctx->r4 = MEM_BU(ctx->r7, 0X24);
        goto L_8041AF20;
    // 0x8041B110: lbu         $a0, 0x24($a3)
    ctx->r4 = MEM_BU(ctx->r7, 0X24);
L_8041B114:
    // 0x8041B114: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B118: lw          $a0, 0x166C($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X166C);
    // 0x8041B11C: addiu       $t0, $zero, 0x5
    ctx->r8 = ADD32(0, 0X5);
    // 0x8041B120: sltiu       $a1, $v0, 0x19
    ctx->r5 = ctx->r2 < 0X19 ? 1 : 0;
    // 0x8041B124: sh          $zero, 0x1AE($a0)
    MEM_H(0X1AE, ctx->r4) = 0;
    // 0x8041B128: sh          $t0, 0x19A($a0)
    MEM_H(0X19A, ctx->r4) = ctx->r8;
    // 0x8041B12C: sh          $zero, 0x1B0($a0)
    MEM_H(0X1B0, ctx->r4) = 0;
    // 0x8041B130: bne         $a1, $zero, L_8041B284
    if (ctx->r5 != 0) {
        // 0x8041B134: sh          $a2, 0x1A2($a0)
        MEM_H(0X1A2, ctx->r4) = ctx->r6;
            goto L_8041B284;
    }
    // 0x8041B134: sh          $a2, 0x1A2($a0)
    MEM_H(0X1A2, ctx->r4) = ctx->r6;
    // 0x8041B138: addiu       $t0, $zero, 0x3
    ctx->r8 = ADD32(0, 0X3);
    // 0x8041B13C: sltiu       $a1, $v0, 0x1F
    ctx->r5 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041B140: bne         $a1, $zero, L_8041B150
    if (ctx->r5 != 0) {
        // 0x8041B144: sb          $t0, 0x1CF($a0)
        MEM_B(0X1CF, ctx->r4) = ctx->r8;
            goto L_8041B150;
    }
    // 0x8041B144: sb          $t0, 0x1CF($a0)
    MEM_B(0X1CF, ctx->r4) = ctx->r8;
    // 0x8041B148: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B14C: sb          $a1, 0x1CE($a0)
    MEM_B(0X1CE, ctx->r4) = ctx->r5;
L_8041B150:
    // 0x8041B150: sltiu       $a0, $v0, 0x33
    ctx->r4 = ctx->r2 < 0X33 ? 1 : 0;
    // 0x8041B154: beq         $a0, $zero, L_8041AE8C
    if (ctx->r4 == 0) {
        // 0x8041B158: addiu       $t0, $zero, 0x1
        ctx->r8 = ADD32(0, 0X1);
            goto L_8041AE8C;
    }
    // 0x8041B158: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B15C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
L_8041B160:
    // 0x8041B160: beq         $v1, $a0, L_8041B31C
    if (ctx->r3 == ctx->r4) {
        // 0x8041B164: sltiu       $a0, $v0, 0x1E
        ctx->r4 = ctx->r2 < 0X1E ? 1 : 0;
            goto L_8041B31C;
    }
    // 0x8041B164: sltiu       $a0, $v0, 0x1E
    ctx->r4 = ctx->r2 < 0X1E ? 1 : 0;
    // 0x8041B168: bne         $a0, $zero, L_8041AEB8
    if (ctx->r4 != 0) {
        // 0x8041B16C: sltiu       $a1, $v0, 0x26
        ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
            goto L_8041AEB8;
    }
    // 0x8041B16C: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
    // 0x8041B170: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041B174: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
L_8041B178:
    // 0x8041B178: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
    // 0x8041B17C: beq         $a1, $zero, L_8041AEB4
    if (ctx->r5 == 0) {
        // 0x8041B180: addiu       $t1, $zero, 0x4
        ctx->r9 = ADD32(0, 0X4);
            goto L_8041AEB4;
    }
    // 0x8041B180: addiu       $t1, $zero, 0x4
    ctx->r9 = ADD32(0, 0X4);
    // 0x8041B184: addiu       $t1, $zero, 0x3
    ctx->r9 = ADD32(0, 0X3);
    // 0x8041B188: b           L_8041AEB8
    // 0x8041B18C: sb          $t1, 0x2A0($a0)
    MEM_B(0X2A0, ctx->r4) = ctx->r9;
        goto L_8041AEB8;
    // 0x8041B18C: sb          $t1, 0x2A0($a0)
    MEM_B(0X2A0, ctx->r4) = ctx->r9;
L_8041B190:
    // 0x8041B190: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
L_8041B194:
    // 0x8041B194: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B198: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041B19C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B1A0: lwc1        $f0, -0x3F38($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F38);
    // 0x8041B1A4: addiu       $t2, $zero, 0x118
    ctx->r10 = ADD32(0, 0X118);
    // 0x8041B1A8: sh          $t2, 0xD8($a0)
    MEM_H(0XD8, ctx->r4) = ctx->r10;
    // 0x8041B1AC: sh          $t0, 0x126($a0)
    MEM_H(0X126, ctx->r4) = ctx->r8;
    // 0x8041B1B0: addiu       $t2, $zero, 0x11
    ctx->r10 = ADD32(0, 0X11);
    // 0x8041B1B4: sh          $t0, 0x11A($a0)
    MEM_H(0X11A, ctx->r4) = ctx->r8;
    // 0x8041B1B8: addiu       $t1, $zero, 0x1
    ctx->r9 = ADD32(0, 0X1);
    // 0x8041B1BC: addiu       $t0, $zero, 0x2
    ctx->r8 = ADD32(0, 0X2);
    // 0x8041B1C0: lbu         $a2, 0x27($a3)
    ctx->r6 = MEM_BU(ctx->r7, 0X27);
    // 0x8041B1C4: swc1        $f0, 0xE8($a0)
    MEM_W(0XE8, ctx->r4) = ctx->f0.u32l;
    // 0x8041B1C8: sh          $t2, 0x112($a0)
    MEM_H(0X112, ctx->r4) = ctx->r10;
    // 0x8041B1CC: sh          $zero, 0x128($a0)
    MEM_H(0X128, ctx->r4) = 0;
    // 0x8041B1D0: beq         $v1, $t1, L_8041B2BC
    if (ctx->r3 == ctx->r9) {
        // 0x8041B1D4: sb          $t0, 0x14C($a0)
        MEM_B(0X14C, ctx->r4) = ctx->r8;
            goto L_8041B2BC;
    }
    // 0x8041B1D4: sb          $t0, 0x14C($a0)
    MEM_B(0X14C, ctx->r4) = ctx->r8;
    // 0x8041B1D8: sltiu       $a0, $v1, 0xF
    ctx->r4 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041B1DC: bne         $a0, $zero, L_8041B1F8
    if (ctx->r4 != 0) {
        // 0x8041B1E0: lw          $a0, 0x4($a1)
        ctx->r4 = MEM_W(ctx->r5, 0X4);
            goto L_8041B1F8;
    }
    // 0x8041B1E0: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B1E4: b           L_8041AE10
    // 0x8041B1E8: sltiu       $a1, $v1, 0x23
    ctx->r5 = ctx->r3 < 0X23 ? 1 : 0;
        goto L_8041AE10;
    // 0x8041B1E8: sltiu       $a1, $v1, 0x23
    ctx->r5 = ctx->r3 < 0X23 ? 1 : 0;
L_8041B1EC:
    // 0x8041B1EC: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B1F0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
L_8041B1F4:
    // 0x8041B1F4: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
L_8041B1F8:
    // 0x8041B1F8: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B1FC: beq         $a2, $t0, L_8041B270
    if (ctx->r6 == ctx->r8) {
        // 0x8041B200: sb          $zero, 0x1CE($a0)
        MEM_B(0X1CE, ctx->r4) = 0;
            goto L_8041B270;
    }
    // 0x8041B200: sb          $zero, 0x1CE($a0)
    MEM_B(0X1CE, ctx->r4) = 0;
    // 0x8041B204: sltiu       $t0, $a2, 0x14
    ctx->r8 = ctx->r6 < 0X14 ? 1 : 0;
    // 0x8041B208: bne         $t0, $zero, L_8041B0D8
    if (ctx->r8 != 0) {
        // 0x8041B20C: lw          $a0, 0x4($a1)
        ctx->r4 = MEM_W(ctx->r5, 0X4);
            goto L_8041B0D8;
    }
    // 0x8041B20C: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B210: b           L_8041AE60
    // 0x8041B214: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
        goto L_8041AE60;
    // 0x8041B214: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_8041B218:
    // 0x8041B218: addiu       $a1, $zero, 0x5A
    ctx->r5 = ADD32(0, 0X5A);
    // 0x8041B21C: sh          $a1, 0x272($a0)
    MEM_H(0X272, ctx->r4) = ctx->r5;
    // 0x8041B220: b           L_8041AEB0
    // 0x8041B224: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
        goto L_8041AEB0;
    // 0x8041B224: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
L_8041B228:
    // 0x8041B228: addiu       $a1, $zero, 0x5A
    ctx->r5 = ADD32(0, 0X5A);
    // 0x8041B22C: sh          $a1, 0x96($a0)
    MEM_H(0X96, ctx->r4) = ctx->r5;
    // 0x8041B230: addiu       $a1, $zero, 0x4
    ctx->r5 = ADD32(0, 0X4);
    // 0x8041B234: sb          $a1, 0xC4($a0)
    MEM_B(0XC4, ctx->r4) = ctx->r5;
    // 0x8041B238: sltiu       $a0, $v0, 0x1D
    ctx->r4 = ctx->r2 < 0X1D ? 1 : 0;
    // 0x8041B23C: beq         $a0, $zero, L_8041AD50
    if (ctx->r4 == 0) {
        // 0x8041B240: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041AD50;
    }
    // 0x8041B240: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B244: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
L_8041B248:
    // 0x8041B248: beq         $v1, $a0, L_8041B33C
    if (ctx->r3 == ctx->r4) {
        // 0x8041B24C: lbu         $a2, 0x27($a3)
        ctx->r6 = MEM_BU(ctx->r7, 0X27);
            goto L_8041B33C;
    }
    // 0x8041B24C: lbu         $a2, 0x27($a3)
    ctx->r6 = MEM_BU(ctx->r7, 0X27);
    // 0x8041B250: sltiu       $a0, $v0, 0x15
    ctx->r4 = ctx->r2 < 0X15 ? 1 : 0;
    // 0x8041B254: beq         $a0, $zero, L_8041B398
    if (ctx->r4 == 0) {
        // 0x8041B258: sltiu       $a0, $v1, 0xF
        ctx->r4 = ctx->r3 < 0XF ? 1 : 0;
            goto L_8041B398;
    }
    // 0x8041B258: sltiu       $a0, $v1, 0xF
    ctx->r4 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041B25C: bne         $a0, $zero, L_8041AE1C
    if (ctx->r4 != 0) {
        // 0x8041B260: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041AE1C;
    }
    // 0x8041B260: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B264: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B268: b           L_8041AE0C
    // 0x8041B26C: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
        goto L_8041AE0C;
    // 0x8041B26C: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
L_8041B270:
    // 0x8041B270: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041B274: sh          $zero, 0x1AE($a0)
    MEM_H(0X1AE, ctx->r4) = 0;
    // 0x8041B278: sh          $a1, 0x19A($a0)
    MEM_H(0X19A, ctx->r4) = ctx->r5;
    // 0x8041B27C: sh          $zero, 0x1B0($a0)
    MEM_H(0X1B0, ctx->r4) = 0;
    // 0x8041B280: sh          $a2, 0x1A2($a0)
    MEM_H(0X1A2, ctx->r4) = ctx->r6;
L_8041B284:
    // 0x8041B284: b           L_8041AE78
    // 0x8041B288: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
        goto L_8041AE78;
    // 0x8041B288: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041B28C:
    // 0x8041B28C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041B290: beq         $v1, $a0, L_8041B2FC
    if (ctx->r3 == ctx->r4) {
        // 0x8041B294: sltiu       $a0, $v0, 0x15
        ctx->r4 = ctx->r2 < 0X15 ? 1 : 0;
            goto L_8041B2FC;
    }
    // 0x8041B294: sltiu       $a0, $v0, 0x15
    ctx->r4 = ctx->r2 < 0X15 ? 1 : 0;
    // 0x8041B298: bne         $a0, $zero, L_8041B0B8
    if (ctx->r4 != 0) {
        // 0x8041B29C: sltiu       $t0, $v1, 0xF
        ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
            goto L_8041B0B8;
    }
    // 0x8041B29C: sltiu       $t0, $v1, 0xF
    ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041B2A0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B2A4: lw          $t1, 0x166C($a1)
    ctx->r9 = MEM_W(ctx->r5, 0X166C);
    // 0x8041B2A8: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041B2AC: beq         $t0, $zero, L_8041AD24
    if (ctx->r8 == 0) {
        // 0x8041B2B0: sb          $a0, 0xC4($t1)
        MEM_B(0XC4, ctx->r9) = ctx->r4;
            goto L_8041AD24;
    }
    // 0x8041B2B0: sb          $a0, 0xC4($t1)
    MEM_B(0XC4, ctx->r9) = ctx->r4;
    // 0x8041B2B4: b           L_8041AD44
    // 0x8041B2B8: sltiu       $a0, $v0, 0x1D
    ctx->r4 = ctx->r2 < 0X1D ? 1 : 0;
        goto L_8041AD44;
    // 0x8041B2B8: sltiu       $a0, $v0, 0x1D
    ctx->r4 = ctx->r2 < 0X1D ? 1 : 0;
L_8041B2BC:
    // 0x8041B2BC: addiu       $t0, $zero, 0x5A
    ctx->r8 = ADD32(0, 0X5A);
    // 0x8041B2C0: b           L_8041B1F4
    // 0x8041B2C4: sh          $t0, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r8;
        goto L_8041B1F4;
    // 0x8041B2C4: sh          $t0, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r8;
L_8041B2C8:
    // 0x8041B2C8: addiu       $a1, $zero, 0x5A
    ctx->r5 = ADD32(0, 0X5A);
    // 0x8041B2CC: sh          $a1, 0x272($a0)
    MEM_H(0X272, ctx->r4) = ctx->r5;
    // 0x8041B2D0: b           L_8041AED8
    // 0x8041B2D4: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
        goto L_8041AED8;
    // 0x8041B2D4: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
L_8041B2D8:
    // 0x8041B2D8: addiu       $a2, $zero, 0x5A
    ctx->r6 = ADD32(0, 0X5A);
    // 0x8041B2DC: b           L_8041B194
    // 0x8041B2E0: sh          $a2, 0x96($a0)
    MEM_H(0X96, ctx->r4) = ctx->r6;
        goto L_8041B194;
    // 0x8041B2E0: sh          $a2, 0x96($a0)
    MEM_H(0X96, ctx->r4) = ctx->r6;
L_8041B2E4:
    // 0x8041B2E4: sltiu       $t0, $a2, 0x14
    ctx->r8 = ctx->r6 < 0X14 ? 1 : 0;
    // 0x8041B2E8: bne         $t0, $zero, L_8041AE78
    if (ctx->r8 != 0) {
        // 0x8041B2EC: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041AE78;
    }
    // 0x8041B2EC: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B2F0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B2F4: b           L_8041AE5C
    // 0x8041B2F8: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
        goto L_8041AE5C;
    // 0x8041B2F8: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
L_8041B2FC:
    // 0x8041B2FC: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B300: lw          $a1, 0x166C($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X166C);
    // 0x8041B304: addiu       $a2, $zero, 0x5A
    ctx->r6 = ADD32(0, 0X5A);
    // 0x8041B308: bne         $a0, $zero, L_8041AD40
    if (ctx->r4 != 0) {
        // 0x8041B30C: sh          $a2, 0x96($a1)
        MEM_H(0X96, ctx->r5) = ctx->r6;
            goto L_8041AD40;
    }
    // 0x8041B30C: sh          $a2, 0x96($a1)
    MEM_H(0X96, ctx->r5) = ctx->r6;
    // 0x8041B310: addiu       $a0, $zero, 0x4
    ctx->r4 = ADD32(0, 0X4);
    // 0x8041B314: b           L_8041AD40
    // 0x8041B318: sb          $a0, 0xC4($a1)
    MEM_B(0XC4, ctx->r5) = ctx->r4;
        goto L_8041AD40;
    // 0x8041B318: sb          $a0, 0xC4($a1)
    MEM_B(0XC4, ctx->r5) = ctx->r4;
L_8041B31C:
    // 0x8041B31C: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041B320: lw          $a0, 0x166C($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X166C);
    // 0x8041B324: addiu       $t1, $zero, 0x5A
    ctx->r9 = ADD32(0, 0X5A);
    // 0x8041B328: sltiu       $a1, $v0, 0x1E
    ctx->r5 = ctx->r2 < 0X1E ? 1 : 0;
    // 0x8041B32C: beq         $a1, $zero, L_8041B178
    if (ctx->r5 == 0) {
        // 0x8041B330: sh          $t1, 0x272($a0)
        MEM_H(0X272, ctx->r4) = ctx->r9;
            goto L_8041B178;
    }
    // 0x8041B330: sh          $t1, 0x272($a0)
    MEM_H(0X272, ctx->r4) = ctx->r9;
    // 0x8041B334: b           L_8041AED8
    // 0x8041B338: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
        goto L_8041AED8;
    // 0x8041B338: sltiu       $a1, $v0, 0x26
    ctx->r5 = ctx->r2 < 0X26 ? 1 : 0;
L_8041B33C:
    // 0x8041B33C: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B340: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
L_8041B344:
    // 0x8041B344: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B348: addiu       $t0, $zero, 0x5A
    ctx->r8 = ADD32(0, 0X5A);
    // 0x8041B34C: sltiu       $a1, $v0, 0x15
    ctx->r5 = ctx->r2 < 0X15 ? 1 : 0;
    // 0x8041B350: bne         $a1, $zero, L_8041AE1C
    if (ctx->r5 != 0) {
        // 0x8041B354: sh          $t0, 0x11E($a0)
        MEM_H(0X11E, ctx->r4) = ctx->r8;
            goto L_8041AE1C;
    }
    // 0x8041B354: sh          $t0, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r8;
    // 0x8041B358: b           L_8041B374
    // 0x8041B35C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
        goto L_8041B374;
    // 0x8041B35C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041B360:
    // 0x8041B360: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041B364: beq         $v1, $a0, L_8041B344
    if (ctx->r3 == ctx->r4) {
        // 0x8041B368: lbu         $a2, 0x27($a3)
        ctx->r6 = MEM_BU(ctx->r7, 0X27);
            goto L_8041B344;
    }
    // 0x8041B368: lbu         $a2, 0x27($a3)
    ctx->r6 = MEM_BU(ctx->r7, 0X27);
L_8041B36C:
    // 0x8041B36C: lw          $a0, 0x4($a1)
    ctx->r4 = MEM_W(ctx->r5, 0X4);
    // 0x8041B370: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
L_8041B374:
    // 0x8041B374: sltiu       $a1, $v0, 0x23
    ctx->r5 = ctx->r2 < 0X23 ? 1 : 0;
    // 0x8041B378: sb          $t0, 0x14C($a0)
    MEM_B(0X14C, ctx->r4) = ctx->r8;
    // 0x8041B37C: beq         $a1, $zero, L_8041ADE8
    if (ctx->r5 == 0) {
        // 0x8041B380: sltiu       $t0, $v1, 0xF
        ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
            goto L_8041ADE8;
    }
    // 0x8041B380: sltiu       $t0, $v1, 0xF
    ctx->r8 = ctx->r3 < 0XF ? 1 : 0;
    // 0x8041B384: b           L_8041AE04
    // 0x8041B388: nop

        goto L_8041AE04;
    // 0x8041B388: nop

L_8041B38C:
    // 0x8041B38C: addiu       $a0, $zero, 0x5F
    ctx->r4 = ADD32(0, 0X5F);
    // 0x8041B390: b           L_8041AED8
    // 0x8041B394: sh          $a0, 0x272($t1)
    MEM_H(0X272, ctx->r9) = ctx->r4;
        goto L_8041AED8;
    // 0x8041B394: sh          $a0, 0x272($t1)
    MEM_H(0X272, ctx->r9) = ctx->r4;
L_8041B398:
    // 0x8041B398: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B39C: b           L_8041B36C
    // 0x8041B3A0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
        goto L_8041B36C;
    // 0x8041B3A0: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
L_8041B3A4:
    // 0x8041B3A4: addiu       $a1, $zero, 0x5F
    ctx->r5 = ADD32(0, 0X5F);
    // 0x8041B3A8: b           L_8041AE1C
    // 0x8041B3AC: sh          $a1, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r5;
        goto L_8041AE1C;
    // 0x8041B3AC: sh          $a1, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r5;
L_8041B3B0:
    // 0x8041B3B0: addiu       $a0, $zero, 0x5F
    ctx->r4 = ADD32(0, 0X5F);
    // 0x8041B3B4: b           L_8041AD34
    // 0x8041B3B8: sh          $a0, 0x96($t1)
    MEM_H(0X96, ctx->r9) = ctx->r4;
        goto L_8041AD34;
    // 0x8041B3B8: sh          $a0, 0x96($t1)
    MEM_H(0X96, ctx->r9) = ctx->r4;
L_8041B3BC:
    // 0x8041B3BC: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
    // 0x8041B3C0: b           L_8041AFE8
    // 0x8041B3C4: sh          $v1, 0x33C($a2)
    MEM_H(0X33C, ctx->r6) = ctx->r3;
        goto L_8041AFE8;
    // 0x8041B3C4: sh          $v1, 0x33C($a2)
    MEM_H(0X33C, ctx->r6) = ctx->r3;
L_8041B3C8:
    // 0x8041B3C8: addiu       $v0, $zero, 0x5F
    ctx->r2 = ADD32(0, 0X5F);
    // 0x8041B3CC: b           L_8041B018
    // 0x8041B3D0: sh          $v0, 0x33E($a1)
    MEM_H(0X33E, ctx->r5) = ctx->r2;
        goto L_8041B018;
    // 0x8041B3D0: sh          $v0, 0x33E($a1)
    MEM_H(0X33E, ctx->r5) = ctx->r2;
;}
RECOMP_FUNC void hm_func_8041B3D4(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041B3D4: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041B3D8: addiu       $t1, $v0, -0x4580
    ctx->r9 = ADD32(ctx->r2, -0X4580);
    // 0x8041B3DC: lbu         $v1, 0x26($t1)
    ctx->r3 = MEM_BU(ctx->r9, 0X26);
    // 0x8041B3E0: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041B3E4: beq         $v1, $a3, L_8041B770
    if (ctx->r3 == ctx->r7) {
        // 0x8041B3E8: lbu         $v0, 0x25($t1)
        ctx->r2 = MEM_BU(ctx->r9, 0X25);
            goto L_8041B770;
    }
    // 0x8041B3E8: lbu         $v0, 0x25($t1)
    ctx->r2 = MEM_BU(ctx->r9, 0X25);
    // 0x8041B3EC: sltiu       $a1, $v1, 0x4
    ctx->r5 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x8041B3F0: bne         $a1, $zero, L_8041B7E8
    if (ctx->r5 != 0) {
        // 0x8041B3F4: lbu         $a0, 0x24($t1)
        ctx->r4 = MEM_BU(ctx->r9, 0X24);
            goto L_8041B7E8;
    }
    // 0x8041B3F4: lbu         $a0, 0x24($t1)
    ctx->r4 = MEM_BU(ctx->r9, 0X24);
    // 0x8041B3F8: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B3FC: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041B400: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B404: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
    // 0x8041B408: lwc1        $f0, -0x3F3C($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F3C);
    // 0x8041B40C: sltiu       $t0, $v1, 0xD
    ctx->r8 = ctx->r3 < 0XD ? 1 : 0;
    // 0x8041B410: addiu       $t2, $zero, 0x176
    ctx->r10 = ADD32(0, 0X176);
    // 0x8041B414: swc1        $f0, 0x1C($a2)
    MEM_W(0X1C, ctx->r6) = ctx->f0.u32l;
    // 0x8041B418: bne         $t0, $zero, L_8041B7E8
    if (ctx->r8 != 0) {
        // 0x8041B41C: sh          $t2, 0xC($a2)
        MEM_H(0XC, ctx->r6) = ctx->r10;
            goto L_8041B7E8;
    }
    // 0x8041B41C: sh          $t2, 0xC($a2)
    MEM_H(0XC, ctx->r6) = ctx->r10;
    // 0x8041B420: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041B424: lwc1        $f0, -0x3F50($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F50);
    // 0x8041B428: sltiu       $t0, $v1, 0x1F
    ctx->r8 = ctx->r3 < 0X1F ? 1 : 0;
    // 0x8041B42C: addiu       $t2, $zero, 0x17E
    ctx->r10 = ADD32(0, 0X17E);
    // 0x8041B430: swc1        $f0, 0x1C($a2)
    MEM_W(0X1C, ctx->r6) = ctx->f0.u32l;
    // 0x8041B434: bne         $t0, $zero, L_8041B90C
    if (ctx->r8 != 0) {
        // 0x8041B438: sh          $t2, 0xC($a2)
        MEM_H(0XC, ctx->r6) = ctx->r10;
            goto L_8041B90C;
    }
    // 0x8041B438: sh          $t2, 0xC($a2)
    MEM_H(0XC, ctx->r6) = ctx->r10;
    // 0x8041B43C: lui         $t2, 0x8042
    ctx->r10 = S32(0X8042 << 16);
    // 0x8041B440: lwc1        $f0, -0x3F30($t2)
    ctx->f0.u32l = MEM_W(ctx->r10, -0X3F30);
    // 0x8041B444: sltiu       $t0, $v1, 0x32
    ctx->r8 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x8041B448: addiu       $t2, $zero, 0x190
    ctx->r10 = ADD32(0, 0X190);
    // 0x8041B44C: swc1        $f0, 0x1C($a2)
    MEM_W(0X1C, ctx->r6) = ctx->f0.u32l;
    // 0x8041B450: bne         $t0, $zero, L_8041B90C
    if (ctx->r8 != 0) {
        // 0x8041B454: sh          $t2, 0xC($a2)
        MEM_H(0XC, ctx->r6) = ctx->r10;
            goto L_8041B90C;
    }
    // 0x8041B454: sh          $t2, 0xC($a2)
    MEM_H(0XC, ctx->r6) = ctx->r10;
    // 0x8041B458: lui         $t0, 0x8042
    ctx->r8 = S32(0X8042 << 16);
    // 0x8041B45C: lwc1        $f0, -0x3F2C($t0)
    ctx->f0.u32l = MEM_W(ctx->r8, -0X3F2C);
    // 0x8041B460: addiu       $t0, $zero, 0x1A9
    ctx->r8 = ADD32(0, 0X1A9);
    // 0x8041B464: swc1        $f0, 0x1C($a2)
    MEM_W(0X1C, ctx->r6) = ctx->f0.u32l;
    // 0x8041B468: beq         $v0, $a3, L_8041B7C8
    if (ctx->r2 == ctx->r7) {
        // 0x8041B46C: sh          $t0, 0xC($a2)
        MEM_H(0XC, ctx->r6) = ctx->r8;
            goto L_8041B7C8;
    }
    // 0x8041B46C: sh          $t0, 0xC($a2)
    MEM_H(0XC, ctx->r6) = ctx->r8;
    // 0x8041B470: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
L_8041B474:
    // 0x8041B474: sh          $a3, 0x94($a2)
    MEM_H(0X94, ctx->r6) = ctx->r7;
L_8041B478:
    // 0x8041B478: sltiu       $a3, $v0, 0x1F
    ctx->r7 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041B47C: bnel        $a3, $zero, L_8041B49C
    if (ctx->r7 != 0) {
        // 0x8041B480: addiu       $a2, $zero, 0x1
        ctx->r6 = ADD32(0, 0X1);
            goto L_8041B49C;
    }
    goto skip_0;
    // 0x8041B480: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    skip_0:
    // 0x8041B484: lui         $a3, 0x3
    ctx->r7 = S32(0X3 << 16);
    // 0x8041B488: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x8041B48C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B490: beq         $v1, $t0, L_8041B694
    if (ctx->r3 == ctx->r8) {
        // 0x8041B494: sw          $a3, 0xA8($a2)
        MEM_W(0XA8, ctx->r6) = ctx->r7;
            goto L_8041B694;
    }
    // 0x8041B494: sw          $a3, 0xA8($a2)
    MEM_W(0XA8, ctx->r6) = ctx->r7;
L_8041B498:
    // 0x8041B498: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
L_8041B49C:
    // 0x8041B49C: beq         $a0, $a2, L_8041B654
    if (ctx->r4 == ctx->r6) {
        // 0x8041B4A0: sltiu       $a1, $v1, 0x21
        ctx->r5 = ctx->r3 < 0X21 ? 1 : 0;
            goto L_8041B654;
    }
    // 0x8041B4A0: sltiu       $a1, $v1, 0x21
    ctx->r5 = ctx->r3 < 0X21 ? 1 : 0;
    // 0x8041B4A4: bne         $a1, $zero, L_8041B840
    if (ctx->r5 != 0) {
        // 0x8041B4A8: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B840;
    }
    // 0x8041B4A8: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B4AC: lw          $a1, 0x1670($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1670);
    // 0x8041B4B0: sltiu       $a3, $a0, 0xF
    ctx->r7 = ctx->r4 < 0XF ? 1 : 0;
    // 0x8041B4B4: sh          $a2, 0xE2($a1)
    MEM_H(0XE2, ctx->r5) = ctx->r6;
    // 0x8041B4B8: addiu       $a2, $zero, 0x5
    ctx->r6 = ADD32(0, 0X5);
    // 0x8041B4BC: sh          $a2, 0xCE($a1)
    MEM_H(0XCE, ctx->r5) = ctx->r6;
    // 0x8041B4C0: addiu       $a2, $zero, 0x8
    ctx->r6 = ADD32(0, 0X8);
    // 0x8041B4C4: sh          $zero, 0xE4($a1)
    MEM_H(0XE4, ctx->r5) = 0;
    // 0x8041B4C8: bne         $a3, $zero, L_8041B858
    if (ctx->r7 != 0) {
        // 0x8041B4CC: sh          $a2, 0xD6($a1)
        MEM_H(0XD6, ctx->r5) = ctx->r6;
            goto L_8041B858;
    }
    // 0x8041B4CC: sh          $a2, 0xD6($a1)
    MEM_H(0XD6, ctx->r5) = ctx->r6;
L_8041B4D0:
    // 0x8041B4D0: sltiu       $a2, $a0, 0x23
    ctx->r6 = ctx->r4 < 0X23 ? 1 : 0;
L_8041B4D4:
    // 0x8041B4D4: bne         $a2, $zero, L_8041B8D0
    if (ctx->r6 != 0) {
        // 0x8041B4D8: addiu       $a2, $zero, 0x3
        ctx->r6 = ADD32(0, 0X3);
            goto L_8041B8D0;
    }
    // 0x8041B4D8: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B4DC: sh          $a2, 0xEC($a1)
    MEM_H(0XEC, ctx->r5) = ctx->r6;
L_8041B4E0:
    // 0x8041B4E0: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B4E4: beq         $v1, $a1, L_8041B6C8
    if (ctx->r3 == ctx->r5) {
        // 0x8041B4E8: sltiu       $a1, $v1, 0x12
        ctx->r5 = ctx->r3 < 0X12 ? 1 : 0;
            goto L_8041B6C8;
    }
    // 0x8041B4E8: sltiu       $a1, $v1, 0x12
    ctx->r5 = ctx->r3 < 0X12 ? 1 : 0;
    // 0x8041B4EC: bne         $a1, $zero, L_8041B510
    if (ctx->r5 != 0) {
        // 0x8041B4F0: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B510;
    }
    // 0x8041B4F0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B4F4: lw          $a1, 0x1670($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1670);
    // 0x8041B4F8: addiu       $a3, $zero, 0x3
    ctx->r7 = ADD32(0, 0X3);
    // 0x8041B4FC: sltiu       $a2, $v1, 0x19
    ctx->r6 = ctx->r3 < 0X19 ? 1 : 0;
    // 0x8041B500: bne         $a2, $zero, L_8041B510
    if (ctx->r6 != 0) {
        // 0x8041B504: sb          $a3, 0x147($a1)
        MEM_B(0X147, ctx->r5) = ctx->r7;
            goto L_8041B510;
    }
    // 0x8041B504: sb          $a3, 0x147($a1)
    MEM_B(0X147, ctx->r5) = ctx->r7;
L_8041B508:
    // 0x8041B508: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041B50C: sb          $a2, 0x146($a1)
    MEM_B(0X146, ctx->r5) = ctx->r6;
L_8041B510:
    // 0x8041B510: lbu         $a1, 0x27($t1)
    ctx->r5 = MEM_BU(ctx->r9, 0X27);
    // 0x8041B514: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041B518: beq         $a1, $a2, L_8041B6EC
    if (ctx->r5 == ctx->r6) {
        // 0x8041B51C: lui         $a2, 0x8041
        ctx->r6 = S32(0X8041 << 16);
            goto L_8041B6EC;
    }
    // 0x8041B51C: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041B520: sltiu       $a2, $a1, 0x11
    ctx->r6 = ctx->r5 < 0X11 ? 1 : 0;
L_8041B524:
    // 0x8041B524: bne         $a2, $zero, L_8041B820
    if (ctx->r6 != 0) {
        // 0x8041B528: lui         $a3, 0x8041
        ctx->r7 = S32(0X8041 << 16);
            goto L_8041B820;
    }
    // 0x8041B528: lui         $a3, 0x8041
    ctx->r7 = S32(0X8041 << 16);
    // 0x8041B52C: sltiu       $a2, $a1, 0x1F
    ctx->r6 = ctx->r5 < 0X1F ? 1 : 0;
    // 0x8041B530: bne         $a2, $zero, L_8041B818
    if (ctx->r6 != 0) {
        // 0x8041B534: lw          $a3, 0x1670($a3)
        ctx->r7 = MEM_W(ctx->r7, 0X1670);
            goto L_8041B818;
    }
    // 0x8041B534: lw          $a3, 0x1670($a3)
    ctx->r7 = MEM_W(ctx->r7, 0X1670);
    // 0x8041B538: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B53C: sh          $a2, 0x1B8($a3)
    MEM_H(0X1B8, ctx->r7) = ctx->r6;
L_8041B540:
    // 0x8041B540: sltiu       $a1, $a1, 0x26
    ctx->r5 = ctx->r5 < 0X26 ? 1 : 0;
    // 0x8041B544: bne         $a1, $zero, L_8041B8B8
    if (ctx->r5 != 0) {
        // 0x8041B548: addiu       $a1, $zero, 0x5
        ctx->r5 = ADD32(0, 0X5);
            goto L_8041B8B8;
    }
    // 0x8041B548: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041B54C: sh          $a1, 0x1FC($a3)
    MEM_H(0X1FC, ctx->r7) = ctx->r5;
L_8041B550:
    // 0x8041B550: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_8041B554:
    // 0x8041B554: beq         $v0, $a1, L_8041B70C
    if (ctx->r2 == ctx->r5) {
        // 0x8041B558: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B70C;
    }
    // 0x8041B558: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B55C: sltiu       $a1, $v0, 0xB
    ctx->r5 = ctx->r2 < 0XB ? 1 : 0;
L_8041B560:
    // 0x8041B560: bne         $a1, $zero, L_8041B594
    if (ctx->r5 != 0) {
        // 0x8041B564: lui         $a2, 0x8041
        ctx->r6 = S32(0X8041 << 16);
            goto L_8041B594;
    }
    // 0x8041B564: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041B568: sltiu       $a1, $v0, 0x15
    ctx->r5 = ctx->r2 < 0X15 ? 1 : 0;
    // 0x8041B56C: bne         $a1, $zero, L_8041B8C4
    if (ctx->r5 != 0) {
        // 0x8041B570: lw          $a2, 0x1670($a2)
        ctx->r6 = MEM_W(ctx->r6, 0X1670);
            goto L_8041B8C4;
    }
    // 0x8041B570: lw          $a2, 0x1670($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X1670);
    // 0x8041B574: sltiu       $a1, $v0, 0x1F
    ctx->r5 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041B578: bne         $a1, $zero, L_8041B888
    if (ctx->r5 != 0) {
        // 0x8041B57C: sltiu       $a1, $v0, 0x29
        ctx->r5 = ctx->r2 < 0X29 ? 1 : 0;
            goto L_8041B888;
    }
    // 0x8041B57C: sltiu       $a1, $v0, 0x29
    ctx->r5 = ctx->r2 < 0X29 ? 1 : 0;
    // 0x8041B580: bne         $a1, $zero, L_8041B894
    if (ctx->r5 != 0) {
        // 0x8041B584: sltiu       $a1, $v0, 0x32
        ctx->r5 = ctx->r2 < 0X32 ? 1 : 0;
            goto L_8041B894;
    }
    // 0x8041B584: sltiu       $a1, $v0, 0x32
    ctx->r5 = ctx->r2 < 0X32 ? 1 : 0;
    // 0x8041B588: bne         $a1, $zero, L_8041B8A0
    if (ctx->r5 != 0) {
        // 0x8041B58C: addiu       $a1, $zero, 0x64
        ctx->r5 = ADD32(0, 0X64);
            goto L_8041B8A0;
    }
    // 0x8041B58C: addiu       $a1, $zero, 0x64
    ctx->r5 = ADD32(0, 0X64);
    // 0x8041B590: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
L_8041B594:
    // 0x8041B594: sltiu       $a1, $v0, 0x33
    ctx->r5 = ctx->r2 < 0X33 ? 1 : 0;
    // 0x8041B598: bne         $a1, $zero, L_8041B5AC
    if (ctx->r5 != 0) {
        // 0x8041B59C: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B5AC;
    }
    // 0x8041B59C: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B5A0: lw          $a1, 0x1684($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1684);
    // 0x8041B5A4: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B5A8: sh          $a2, 0x2AA($a1)
    MEM_H(0X2AA, ctx->r5) = ctx->r6;
L_8041B5AC:
    // 0x8041B5AC: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B5B0: beql        $v1, $a1, L_8041B738
    if (ctx->r3 == ctx->r5) {
        // 0x8041B5B4: lui         $v1, 0x8041
        ctx->r3 = S32(0X8041 << 16);
            goto L_8041B738;
    }
    goto skip_1;
    // 0x8041B5B4: lui         $v1, 0x8041
    ctx->r3 = S32(0X8041 << 16);
    skip_1:
    // 0x8041B5B8: sltiu       $v1, $v1, 0x56
    ctx->r3 = ctx->r3 < 0X56 ? 1 : 0;
L_8041B5BC:
    // 0x8041B5BC: bne         $v1, $zero, L_8041B5E0
    if (ctx->r3 != 0) {
        // 0x8041B5C0: lui         $v1, 0x8041
        ctx->r3 = S32(0X8041 << 16);
            goto L_8041B5E0;
    }
    // 0x8041B5C0: lui         $v1, 0x8041
    ctx->r3 = S32(0X8041 << 16);
    // 0x8041B5C4: lw          $v1, 0x1670($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X1670);
    // 0x8041B5C8: sh          $a1, 0x302($v1)
    MEM_H(0X302, ctx->r3) = ctx->r5;
L_8041B5CC:
    // 0x8041B5CC: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041B5D0: sh          $a1, 0x2EE($v1)
    MEM_H(0X2EE, ctx->r3) = ctx->r5;
    // 0x8041B5D4: addiu       $a1, $zero, 0x8
    ctx->r5 = ADD32(0, 0X8);
    // 0x8041B5D8: sh          $zero, 0x304($v1)
    MEM_H(0X304, ctx->r3) = 0;
    // 0x8041B5DC: sh          $a1, 0x2F6($v1)
    MEM_H(0X2F6, ctx->r3) = ctx->r5;
L_8041B5E0:
    // 0x8041B5E0: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x8041B5E4: beq         $a0, $v1, L_8041B760
    if (ctx->r4 == ctx->r3) {
        // 0x8041B5E8: sltiu       $v1, $a0, 0x21
        ctx->r3 = ctx->r4 < 0X21 ? 1 : 0;
            goto L_8041B760;
    }
    // 0x8041B5E8: sltiu       $v1, $a0, 0x21
    ctx->r3 = ctx->r4 < 0X21 ? 1 : 0;
    // 0x8041B5EC: bne         $v1, $zero, L_8041B618
    if (ctx->r3 != 0) {
        // 0x8041B5F0: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B618;
    }
    // 0x8041B5F0: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B5F4: sltiu       $v1, $a0, 0x2C
    ctx->r3 = ctx->r4 < 0X2C ? 1 : 0;
    // 0x8041B5F8: bne         $v1, $zero, L_8041B8AC
    if (ctx->r3 != 0) {
        // 0x8041B5FC: lw          $a1, 0x1670($a1)
        ctx->r5 = MEM_W(ctx->r5, 0X1670);
            goto L_8041B8AC;
    }
    // 0x8041B5FC: lw          $a1, 0x1670($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1670);
    // 0x8041B600: sltiu       $v1, $a0, 0x37
    ctx->r3 = ctx->r4 < 0X37 ? 1 : 0;
    // 0x8041B604: bne         $v1, $zero, L_8041B8DC
    if (ctx->r3 != 0) {
        // 0x8041B608: sltiu       $a0, $a0, 0x42
        ctx->r4 = ctx->r4 < 0X42 ? 1 : 0;
            goto L_8041B8DC;
    }
    // 0x8041B608: sltiu       $a0, $a0, 0x42
    ctx->r4 = ctx->r4 < 0X42 ? 1 : 0;
    // 0x8041B60C: bne         $a0, $zero, L_8041B8E8
    if (ctx->r4 != 0) {
        // 0x8041B610: addiu       $v1, $zero, 0x5
        ctx->r3 = ADD32(0, 0X5);
            goto L_8041B8E8;
    }
    // 0x8041B610: addiu       $v1, $zero, 0x5
    ctx->r3 = ADD32(0, 0X5);
    // 0x8041B614: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
L_8041B618:
    // 0x8041B618: addiu       $v1, $zero, 0x1
    ctx->r3 = ADD32(0, 0X1);
    // 0x8041B61C: beq         $v0, $v1, L_8041B74C
    if (ctx->r2 == ctx->r3) {
        // 0x8041B620: sltiu       $v1, $v0, 0x1E
        ctx->r3 = ctx->r2 < 0X1E ? 1 : 0;
            goto L_8041B74C;
    }
    // 0x8041B620: sltiu       $v1, $v0, 0x1E
    ctx->r3 = ctx->r2 < 0X1E ? 1 : 0;
    // 0x8041B624: bne         $v1, $zero, L_8041B744
    if (ctx->r3 != 0) {
        // 0x8041B628: lui         $a0, 0x8041
        ctx->r4 = S32(0X8041 << 16);
            goto L_8041B744;
    }
    // 0x8041B628: lui         $a0, 0x8041
    ctx->r4 = S32(0X8041 << 16);
    // 0x8041B62C: sltiu       $v1, $v0, 0x28
    ctx->r3 = ctx->r2 < 0X28 ? 1 : 0;
    // 0x8041B630: bne         $v1, $zero, L_8041B8F4
    if (ctx->r3 != 0) {
        // 0x8041B634: lw          $a0, 0x1670($a0)
        ctx->r4 = MEM_W(ctx->r4, 0X1670);
            goto L_8041B8F4;
    }
    // 0x8041B634: lw          $a0, 0x1670($a0)
    ctx->r4 = MEM_W(ctx->r4, 0X1670);
    // 0x8041B638: sltiu       $v1, $v0, 0x32
    ctx->r3 = ctx->r2 < 0X32 ? 1 : 0;
    // 0x8041B63C: bne         $v1, $zero, L_8041B900
    if (ctx->r3 != 0) {
        // 0x8041B640: sltiu       $v0, $v0, 0x3C
        ctx->r2 = ctx->r2 < 0X3C ? 1 : 0;
            goto L_8041B900;
    }
    // 0x8041B640: sltiu       $v0, $v0, 0x3C
    ctx->r2 = ctx->r2 < 0X3C ? 1 : 0;
    // 0x8041B644: bne         $v0, $zero, L_8041B920
    if (ctx->r2 != 0) {
        // 0x8041B648: addiu       $v0, $zero, 0x64
        ctx->r2 = ADD32(0, 0X64);
            goto L_8041B920;
    }
    // 0x8041B648: addiu       $v0, $zero, 0x64
    ctx->r2 = ADD32(0, 0X64);
    // 0x8041B64C: jr          $ra
    // 0x8041B650: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
    return;
    // 0x8041B650: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
L_8041B654:
    // 0x8041B654: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B658: lw          $a1, 0x1670($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1670);
    // 0x8041B65C: sltiu       $a2, $v1, 0x21
    ctx->r6 = ctx->r3 < 0X21 ? 1 : 0;
    // 0x8041B660: bne         $a2, $zero, L_8041B4E0
    if (ctx->r6 != 0) {
        // 0x8041B664: sh          $a0, 0xEC($a1)
        MEM_H(0XEC, ctx->r5) = ctx->r4;
            goto L_8041B4E0;
    }
    // 0x8041B664: sh          $a0, 0xEC($a1)
    MEM_H(0XEC, ctx->r5) = ctx->r4;
    // 0x8041B668: addiu       $a2, $zero, 0x5
    ctx->r6 = ADD32(0, 0X5);
    // 0x8041B66C: sh          $a2, 0xCE($a1)
    MEM_H(0XCE, ctx->r5) = ctx->r6;
    // 0x8041B670: addiu       $a2, $zero, 0x8
    ctx->r6 = ADD32(0, 0X8);
    // 0x8041B674: sh          $a2, 0xD6($a1)
    MEM_H(0XD6, ctx->r5) = ctx->r6;
    // 0x8041B678: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B67C: sh          $a0, 0xE2($a1)
    MEM_H(0XE2, ctx->r5) = ctx->r4;
    // 0x8041B680: sh          $zero, 0xE4($a1)
    MEM_H(0XE4, ctx->r5) = 0;
    // 0x8041B684: b           L_8041B508
    // 0x8041B688: sb          $a2, 0x147($a1)
    MEM_B(0X147, ctx->r5) = ctx->r6;
        goto L_8041B508;
    // 0x8041B688: sb          $a2, 0x147($a1)
    MEM_B(0X147, ctx->r5) = ctx->r6;
L_8041B68C:
    // 0x8041B68C: lui         $a3, 0x1
    ctx->r7 = S32(0X1 << 16);
    // 0x8041B690: sw          $a3, 0xA8($a2)
    MEM_W(0XA8, ctx->r6) = ctx->r7;
L_8041B694:
    // 0x8041B694: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
L_8041B698:
    // 0x8041B698: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041B69C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B6A0: sh          $a3, 0xE2($a2)
    MEM_H(0XE2, ctx->r6) = ctx->r7;
    // 0x8041B6A4: sh          $a3, 0xCE($a2)
    MEM_H(0XCE, ctx->r6) = ctx->r7;
    // 0x8041B6A8: sh          $a3, 0xE4($a2)
    MEM_H(0XE4, ctx->r6) = ctx->r7;
    // 0x8041B6AC: beq         $a0, $t0, L_8041B838
    if (ctx->r4 == ctx->r8) {
        // 0x8041B6B0: sh          $a3, 0xD6($a2)
        MEM_H(0XD6, ctx->r6) = ctx->r7;
            goto L_8041B838;
    }
    // 0x8041B6B0: sh          $a3, 0xD6($a2)
    MEM_H(0XD6, ctx->r6) = ctx->r7;
    // 0x8041B6B4: sltiu       $a2, $a0, 0xF
    ctx->r6 = ctx->r4 < 0XF ? 1 : 0;
    // 0x8041B6B8: bne         $a2, $zero, L_8041B6D4
    if (ctx->r6 != 0) {
        // 0x8041B6BC: lw          $a1, 0x8($a1)
        ctx->r5 = MEM_W(ctx->r5, 0X8);
            goto L_8041B6D4;
    }
    // 0x8041B6BC: lw          $a1, 0x8($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X8);
    // 0x8041B6C0: b           L_8041B4D4
    // 0x8041B6C4: sltiu       $a2, $a0, 0x23
    ctx->r6 = ctx->r4 < 0X23 ? 1 : 0;
        goto L_8041B4D4;
    // 0x8041B6C4: sltiu       $a2, $a0, 0x23
    ctx->r6 = ctx->r4 < 0X23 ? 1 : 0;
L_8041B6C8:
    // 0x8041B6C8: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B6CC: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B6D0: lw          $a1, 0x8($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X8);
L_8041B6D4:
    // 0x8041B6D4: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041B6D8: sh          $zero, 0x146($a1)
    MEM_H(0X146, ctx->r5) = 0;
    // 0x8041B6DC: lbu         $a1, 0x27($t1)
    ctx->r5 = MEM_BU(ctx->r9, 0X27);
    // 0x8041B6E0: bne         $a1, $a2, L_8041B524
    if (ctx->r5 != ctx->r6) {
        // 0x8041B6E4: sltiu       $a2, $a1, 0x11
        ctx->r6 = ctx->r5 < 0X11 ? 1 : 0;
            goto L_8041B524;
    }
    // 0x8041B6E4: sltiu       $a2, $a1, 0x11
    ctx->r6 = ctx->r5 < 0X11 ? 1 : 0;
    // 0x8041B6E8: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
L_8041B6EC:
    // 0x8041B6EC: lw          $a2, 0x1670($a2)
    ctx->r6 = MEM_W(ctx->r6, 0X1670);
    // 0x8041B6F0: sh          $a1, 0x1B8($a2)
    MEM_H(0X1B8, ctx->r6) = ctx->r5;
    // 0x8041B6F4: addiu       $a1, $zero, 0x3
    ctx->r5 = ADD32(0, 0X3);
    // 0x8041B6F8: sh          $a1, 0x1FC($a2)
    MEM_H(0X1FC, ctx->r6) = ctx->r5;
    // 0x8041B6FC: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B700: bne         $v0, $a1, L_8041B560
    if (ctx->r2 != ctx->r5) {
        // 0x8041B704: sltiu       $a1, $v0, 0xB
        ctx->r5 = ctx->r2 < 0XB ? 1 : 0;
            goto L_8041B560;
    }
    // 0x8041B704: sltiu       $a1, $v0, 0xB
    ctx->r5 = ctx->r2 < 0XB ? 1 : 0;
    // 0x8041B708: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
L_8041B70C:
    // 0x8041B70C: lw          $a2, 0x1670($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X1670);
    // 0x8041B710: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B714: lw          $a1, 0x1684($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1684);
    // 0x8041B718: addiu       $a3, $zero, 0x37
    ctx->r7 = ADD32(0, 0X37);
    // 0x8041B71C: sh          $a3, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r7;
    // 0x8041B720: addiu       $a2, $zero, 0x12
    ctx->r6 = ADD32(0, 0X12);
    // 0x8041B724: sh          $a2, 0x2AA($a1)
    MEM_H(0X2AA, ctx->r5) = ctx->r6;
    // 0x8041B728: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B72C: bnel        $v1, $a1, L_8041B5BC
    if (ctx->r3 != ctx->r5) {
        // 0x8041B730: sltiu       $v1, $v1, 0x56
        ctx->r3 = ctx->r3 < 0X56 ? 1 : 0;
            goto L_8041B5BC;
    }
    goto skip_2;
    // 0x8041B730: sltiu       $v1, $v1, 0x56
    ctx->r3 = ctx->r3 < 0X56 ? 1 : 0;
    skip_2:
    // 0x8041B734: lui         $v1, 0x8041
    ctx->r3 = S32(0X8041 << 16);
L_8041B738:
    // 0x8041B738: lw          $v1, 0x1670($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X1670);
    // 0x8041B73C: b           L_8041B5CC
    // 0x8041B740: sh          $zero, 0x302($v1)
    MEM_H(0X302, ctx->r3) = 0;
        goto L_8041B5CC;
    // 0x8041B740: sh          $zero, 0x302($v1)
    MEM_H(0X302, ctx->r3) = 0;
L_8041B744:
    // 0x8041B744: jr          $ra
    // 0x8041B748: nop

    return;
    // 0x8041B748: nop

L_8041B74C:
    // 0x8041B74C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041B750: lw          $v0, 0x1670($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1670);
    // 0x8041B754: addiu       $v1, $zero, 0x46
    ctx->r3 = ADD32(0, 0X46);
    // 0x8041B758: jr          $ra
    // 0x8041B75C: sh          $v1, 0x382($v0)
    MEM_H(0X382, ctx->r2) = ctx->r3;
    return;
    // 0x8041B75C: sh          $v1, 0x382($v0)
    MEM_H(0X382, ctx->r2) = ctx->r3;
L_8041B760:
    // 0x8041B760: lui         $v1, 0x8041
    ctx->r3 = S32(0X8041 << 16);
    // 0x8041B764: lw          $v1, 0x1670($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X1670);
    // 0x8041B768: b           L_8041B618
    // 0x8041B76C: sh          $a0, 0x350($v1)
    MEM_H(0X350, ctx->r3) = ctx->r4;
        goto L_8041B618;
    // 0x8041B76C: sh          $a0, 0x350($v1)
    MEM_H(0X350, ctx->r3) = ctx->r4;
L_8041B770:
    // 0x8041B770: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B774: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x8041B778: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B77C: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
    // 0x8041B780: lwc1        $f0, -0x3F40($a0)
    ctx->f0.u32l = MEM_W(ctx->r4, -0X3F40);
    // 0x8041B784: addiu       $a0, $zero, 0x16D
    ctx->r4 = ADD32(0, 0X16D);
    // 0x8041B788: swc1        $f0, 0x1C($a2)
    MEM_W(0X1C, ctx->r6) = ctx->f0.u32l;
    // 0x8041B78C: sh          $a0, 0xC($a2)
    MEM_H(0XC, ctx->r6) = ctx->r4;
    // 0x8041B790: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x8041B794: lbu         $a0, 0x24($t1)
    ctx->r4 = MEM_BU(ctx->r9, 0X24);
    // 0x8041B798: beq         $v0, $v1, L_8041B68C
    if (ctx->r2 == ctx->r3) {
        // 0x8041B79C: sh          $a3, 0x94($a2)
        MEM_H(0X94, ctx->r6) = ctx->r7;
            goto L_8041B68C;
    }
    // 0x8041B79C: sh          $a3, 0x94($a2)
    MEM_H(0X94, ctx->r6) = ctx->r7;
    // 0x8041B7A0: sltiu       $a2, $v0, 0x1F
    ctx->r6 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041B7A4: bne         $a2, $zero, L_8041B698
    if (ctx->r6 != 0) {
        // 0x8041B7A8: lw          $a2, 0x8($a1)
        ctx->r6 = MEM_W(ctx->r5, 0X8);
            goto L_8041B698;
    }
    // 0x8041B7A8: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
L_8041B7AC:
    // 0x8041B7AC: lui         $a3, 0x3
    ctx->r7 = S32(0X3 << 16);
    // 0x8041B7B0: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x8041B7B4: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041B7B8: bne         $v1, $t0, L_8041B498
    if (ctx->r3 != ctx->r8) {
        // 0x8041B7BC: sw          $a3, 0xA8($a2)
        MEM_W(0XA8, ctx->r6) = ctx->r7;
            goto L_8041B498;
    }
    // 0x8041B7BC: sw          $a3, 0xA8($a2)
    MEM_W(0XA8, ctx->r6) = ctx->r7;
    // 0x8041B7C0: b           L_8041B698
    // 0x8041B7C4: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
        goto L_8041B698;
    // 0x8041B7C4: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
L_8041B7C8:
    // 0x8041B7C8: lui         $a3, 0x1
    ctx->r7 = S32(0X1 << 16);
    // 0x8041B7CC: sw          $a3, 0xA8($a2)
    MEM_W(0XA8, ctx->r6) = ctx->r7;
L_8041B7D0:
    // 0x8041B7D0: sltiu       $a3, $v1, 0x19
    ctx->r7 = ctx->r3 < 0X19 ? 1 : 0;
L_8041B7D4:
    // 0x8041B7D4: beq         $a3, $zero, L_8041B474
    if (ctx->r7 == 0) {
        // 0x8041B7D8: addiu       $a3, $zero, 0x7
        ctx->r7 = ADD32(0, 0X7);
            goto L_8041B474;
    }
    // 0x8041B7D8: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
    // 0x8041B7DC: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
    // 0x8041B7E0: b           L_8041B478
    // 0x8041B7E4: sh          $a3, 0x94($a2)
    MEM_H(0X94, ctx->r6) = ctx->r7;
        goto L_8041B478;
    // 0x8041B7E4: sh          $a3, 0x94($a2)
    MEM_H(0X94, ctx->r6) = ctx->r7;
L_8041B7E8:
    // 0x8041B7E8: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041B7EC: beq         $v0, $a1, L_8041B864
    if (ctx->r2 == ctx->r5) {
        // 0x8041B7F0: sltiu       $a1, $v1, 0x7
        ctx->r5 = ctx->r3 < 0X7 ? 1 : 0;
            goto L_8041B864;
    }
    // 0x8041B7F0: sltiu       $a1, $v1, 0x7
    ctx->r5 = ctx->r3 < 0X7 ? 1 : 0;
    // 0x8041B7F4: beq         $a1, $zero, L_8041B92C
    if (ctx->r5 == 0) {
        // 0x8041B7F8: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B92C;
    }
    // 0x8041B7F8: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B7FC: sltiu       $a1, $v0, 0x1F
    ctx->r5 = ctx->r2 < 0X1F ? 1 : 0;
    // 0x8041B800: bnel        $a1, $zero, L_8041B49C
    if (ctx->r5 != 0) {
        // 0x8041B804: addiu       $a2, $zero, 0x1
        ctx->r6 = ADD32(0, 0X1);
            goto L_8041B49C;
    }
    goto skip_3;
    // 0x8041B804: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    skip_3:
    // 0x8041B808: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B80C: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B810: b           L_8041B7AC
    // 0x8041B814: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
        goto L_8041B7AC;
    // 0x8041B814: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
L_8041B818:
    // 0x8041B818: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041B81C: sh          $a2, 0x1B8($a3)
    MEM_H(0X1B8, ctx->r7) = ctx->r6;
L_8041B820:
    // 0x8041B820: sltiu       $a2, $a1, 0x13
    ctx->r6 = ctx->r5 < 0X13 ? 1 : 0;
    // 0x8041B824: bnel        $a2, $zero, L_8041B554
    if (ctx->r6 != 0) {
        // 0x8041B828: addiu       $a1, $zero, 0x1
        ctx->r5 = ADD32(0, 0X1);
            goto L_8041B554;
    }
    goto skip_4;
    // 0x8041B828: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    skip_4:
    // 0x8041B82C: lui         $a2, 0x8041
    ctx->r6 = S32(0X8041 << 16);
    // 0x8041B830: b           L_8041B540
    // 0x8041B834: lw          $a3, 0x1670($a2)
    ctx->r7 = MEM_W(ctx->r6, 0X1670);
        goto L_8041B540;
    // 0x8041B834: lw          $a3, 0x1670($a2)
    ctx->r7 = MEM_W(ctx->r6, 0X1670);
L_8041B838:
    // 0x8041B838: b           L_8041B4E0
    // 0x8041B83C: sh          $a0, 0xEC($a2)
    MEM_H(0XEC, ctx->r6) = ctx->r4;
        goto L_8041B4E0;
    // 0x8041B83C: sh          $a0, 0xEC($a2)
    MEM_H(0XEC, ctx->r6) = ctx->r4;
L_8041B840:
    // 0x8041B840: sltiu       $a1, $a0, 0xF
    ctx->r5 = ctx->r4 < 0XF ? 1 : 0;
    // 0x8041B844: bne         $a1, $zero, L_8041B4E0
    if (ctx->r5 != 0) {
        // 0x8041B848: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B4E0;
    }
    // 0x8041B848: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B84C: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B850: b           L_8041B4D0
    // 0x8041B854: lw          $a1, 0x8($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X8);
        goto L_8041B4D0;
    // 0x8041B854: lw          $a1, 0x8($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X8);
L_8041B858:
    // 0x8041B858: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B85C: b           L_8041B508
    // 0x8041B860: sb          $a2, 0x147($a1)
    MEM_B(0X147, ctx->r5) = ctx->r6;
        goto L_8041B508;
    // 0x8041B860: sb          $a2, 0x147($a1)
    MEM_B(0X147, ctx->r5) = ctx->r6;
L_8041B864:
    // 0x8041B864: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B868: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B86C: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
L_8041B870:
    // 0x8041B870: lui         $t0, 0x1
    ctx->r8 = S32(0X1 << 16);
    // 0x8041B874: sltiu       $a3, $v1, 0x7
    ctx->r7 = ctx->r3 < 0X7 ? 1 : 0;
    // 0x8041B878: bne         $a3, $zero, L_8041B498
    if (ctx->r7 != 0) {
        // 0x8041B87C: sw          $t0, 0xA8($a2)
        MEM_W(0XA8, ctx->r6) = ctx->r8;
            goto L_8041B498;
    }
    // 0x8041B87C: sw          $t0, 0xA8($a2)
    MEM_W(0XA8, ctx->r6) = ctx->r8;
    // 0x8041B880: b           L_8041B7D4
    // 0x8041B884: sltiu       $a3, $v1, 0x19
    ctx->r7 = ctx->r3 < 0X19 ? 1 : 0;
        goto L_8041B7D4;
    // 0x8041B884: sltiu       $a3, $v1, 0x19
    ctx->r7 = ctx->r3 < 0X19 ? 1 : 0;
L_8041B888:
    // 0x8041B888: addiu       $a1, $zero, 0x46
    ctx->r5 = ADD32(0, 0X46);
    // 0x8041B88C: b           L_8041B5AC
    // 0x8041B890: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
        goto L_8041B5AC;
    // 0x8041B890: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
L_8041B894:
    // 0x8041B894: addiu       $a1, $zero, 0x50
    ctx->r5 = ADD32(0, 0X50);
    // 0x8041B898: b           L_8041B5AC
    // 0x8041B89C: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
        goto L_8041B5AC;
    // 0x8041B89C: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
L_8041B8A0:
    // 0x8041B8A0: addiu       $a1, $zero, 0x5A
    ctx->r5 = ADD32(0, 0X5A);
    // 0x8041B8A4: b           L_8041B5AC
    // 0x8041B8A8: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
        goto L_8041B5AC;
    // 0x8041B8A8: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
L_8041B8AC:
    // 0x8041B8AC: addiu       $v1, $zero, 0x2
    ctx->r3 = ADD32(0, 0X2);
    // 0x8041B8B0: b           L_8041B618
    // 0x8041B8B4: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
        goto L_8041B618;
    // 0x8041B8B4: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
L_8041B8B8:
    // 0x8041B8B8: addiu       $a1, $zero, 0x4
    ctx->r5 = ADD32(0, 0X4);
    // 0x8041B8BC: b           L_8041B550
    // 0x8041B8C0: sh          $a1, 0x1FC($a3)
    MEM_H(0X1FC, ctx->r7) = ctx->r5;
        goto L_8041B550;
    // 0x8041B8C0: sh          $a1, 0x1FC($a3)
    MEM_H(0X1FC, ctx->r7) = ctx->r5;
L_8041B8C4:
    // 0x8041B8C4: addiu       $a1, $zero, 0x3C
    ctx->r5 = ADD32(0, 0X3C);
    // 0x8041B8C8: b           L_8041B5AC
    // 0x8041B8CC: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
        goto L_8041B5AC;
    // 0x8041B8CC: sh          $a1, 0x22E($a2)
    MEM_H(0X22E, ctx->r6) = ctx->r5;
L_8041B8D0:
    // 0x8041B8D0: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041B8D4: b           L_8041B4E0
    // 0x8041B8D8: sh          $a2, 0xEC($a1)
    MEM_H(0XEC, ctx->r5) = ctx->r6;
        goto L_8041B4E0;
    // 0x8041B8D8: sh          $a2, 0xEC($a1)
    MEM_H(0XEC, ctx->r5) = ctx->r6;
L_8041B8DC:
    // 0x8041B8DC: addiu       $v1, $zero, 0x3
    ctx->r3 = ADD32(0, 0X3);
    // 0x8041B8E0: b           L_8041B618
    // 0x8041B8E4: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
        goto L_8041B618;
    // 0x8041B8E4: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
L_8041B8E8:
    // 0x8041B8E8: addiu       $v1, $zero, 0x4
    ctx->r3 = ADD32(0, 0X4);
    // 0x8041B8EC: b           L_8041B618
    // 0x8041B8F0: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
        goto L_8041B618;
    // 0x8041B8F0: sh          $v1, 0x350($a1)
    MEM_H(0X350, ctx->r5) = ctx->r3;
L_8041B8F4:
    // 0x8041B8F4: addiu       $v0, $zero, 0x4B
    ctx->r2 = ADD32(0, 0X4B);
    // 0x8041B8F8: jr          $ra
    // 0x8041B8FC: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
    return;
    // 0x8041B8FC: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
L_8041B900:
    // 0x8041B900: addiu       $v0, $zero, 0x50
    ctx->r2 = ADD32(0, 0X50);
    // 0x8041B904: jr          $ra
    // 0x8041B908: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
    return;
    // 0x8041B908: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
L_8041B90C:
    // 0x8041B90C: addiu       $a2, $zero, 0x1
    ctx->r6 = ADD32(0, 0X1);
    // 0x8041B910: beq         $v0, $a2, L_8041B870
    if (ctx->r2 == ctx->r6) {
        // 0x8041B914: lw          $a2, 0x8($a1)
        ctx->r6 = MEM_W(ctx->r5, 0X8);
            goto L_8041B870;
    }
    // 0x8041B914: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
    // 0x8041B918: b           L_8041B7D4
    // 0x8041B91C: sltiu       $a3, $v1, 0x19
    ctx->r7 = ctx->r3 < 0X19 ? 1 : 0;
        goto L_8041B7D4;
    // 0x8041B91C: sltiu       $a3, $v1, 0x19
    ctx->r7 = ctx->r3 < 0X19 ? 1 : 0;
L_8041B920:
    // 0x8041B920: addiu       $v0, $zero, 0x5A
    ctx->r2 = ADD32(0, 0X5A);
    // 0x8041B924: jr          $ra
    // 0x8041B928: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
    return;
    // 0x8041B928: sh          $v0, 0x382($a0)
    MEM_H(0X382, ctx->r4) = ctx->r2;
L_8041B92C:
    // 0x8041B92C: addiu       $a1, $a1, 0x1668
    ctx->r5 = ADD32(ctx->r5, 0X1668);
    // 0x8041B930: b           L_8041B7D0
    // 0x8041B934: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
        goto L_8041B7D0;
    // 0x8041B934: lw          $a2, 0x8($a1)
    ctx->r6 = MEM_W(ctx->r5, 0X8);
;}
RECOMP_FUNC void hm_func_8041B938(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041B938: lui         $a0, 0x8008
    ctx->r4 = S32(0X8008 << 16);
    // 0x8041B93C: addiu       $a0, $a0, -0x4580
    ctx->r4 = ADD32(ctx->r4, -0X4580);
    // 0x8041B940: lbu         $v1, 0x27($a0)
    ctx->r3 = MEM_BU(ctx->r4, 0X27);
    // 0x8041B944: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041B948: beq         $v1, $v0, L_8041BDAC
    if (ctx->r3 == ctx->r2) {
        // 0x8041B94C: sltiu       $v0, $v1, 0x4
        ctx->r2 = ctx->r3 < 0X4 ? 1 : 0;
            goto L_8041BDAC;
    }
    // 0x8041B94C: sltiu       $v0, $v1, 0x4
    ctx->r2 = ctx->r3 < 0X4 ? 1 : 0;
    // 0x8041B950: bne         $v0, $zero, L_8041B99C
    if (ctx->r2 != 0) {
        // 0x8041B954: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041B99C;
    }
    // 0x8041B954: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041B958: sltiu       $a1, $v1, 0xC
    ctx->r5 = ctx->r3 < 0XC ? 1 : 0;
    // 0x8041B95C: bne         $a1, $zero, L_8041BFF8
    if (ctx->r5 != 0) {
        // 0x8041B960: lw          $v0, 0x1674($v0)
        ctx->r2 = MEM_W(ctx->r2, 0X1674);
            goto L_8041BFF8;
    }
    // 0x8041B960: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041B964: addiu       $a2, $zero, 0x6
    ctx->r6 = ADD32(0, 0X6);
    // 0x8041B968: sh          $a2, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r6;
    // 0x8041B96C: sltiu       $a1, $v1, 0x12
    ctx->r5 = ctx->r3 < 0X12 ? 1 : 0;
    // 0x8041B970: addiu       $a2, $zero, 0x64
    ctx->r6 = ADD32(0, 0X64);
    // 0x8041B974: bne         $a1, $zero, L_8041B99C
    if (ctx->r5 != 0) {
        // 0x8041B978: sh          $a2, 0xC($v0)
        MEM_H(0XC, ctx->r2) = ctx->r6;
            goto L_8041B99C;
    }
    // 0x8041B978: sh          $a2, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r6;
    // 0x8041B97C: addiu       $a2, $zero, 0x5
    ctx->r6 = ADD32(0, 0X5);
    // 0x8041B980: sh          $a2, 0x2($v0)
    MEM_H(0X2, ctx->r2) = ctx->r6;
    // 0x8041B984: sltiu       $a1, $v1, 0x32
    ctx->r5 = ctx->r3 < 0X32 ? 1 : 0;
    // 0x8041B988: addiu       $a2, $zero, 0x50
    ctx->r6 = ADD32(0, 0X50);
    // 0x8041B98C: bne         $a1, $zero, L_8041B99C
    if (ctx->r5 != 0) {
        // 0x8041B990: sh          $a2, 0xC($v0)
        MEM_H(0XC, ctx->r2) = ctx->r6;
            goto L_8041B99C;
    }
    // 0x8041B990: sh          $a2, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r6;
    // 0x8041B994: addiu       $a1, $zero, 0x9
    ctx->r5 = ADD32(0, 0X9);
    // 0x8041B998: sh          $a1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r5;
L_8041B99C:
    // 0x8041B99C: lui         $v0, 0x8008
    ctx->r2 = S32(0X8008 << 16);
    // 0x8041B9A0: lw          $v0, 0x4EE4($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X4EE4);
    // 0x8041B9A4: addiu       $a1, $zero, 0x2
    ctx->r5 = ADD32(0, 0X2);
    // 0x8041B9A8: beq         $v0, $a1, L_8041BC74
    if (ctx->r2 == ctx->r5) {
        // 0x8041B9AC: addiu       $a1, $zero, 0x1F
        ctx->r5 = ADD32(0, 0X1F);
            goto L_8041BC74;
    }
    // 0x8041B9AC: addiu       $a1, $zero, 0x1F
    ctx->r5 = ADD32(0, 0X1F);
    // 0x8041B9B0: beq         $v0, $a1, L_8041BC78
    if (ctx->r2 == ctx->r5) {
        // 0x8041B9B4: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041BC78;
    }
    // 0x8041B9B4: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
L_8041B9B8:
    // 0x8041B9B8: addiu       $a2, $v0, -0x3
    ctx->r6 = ADD32(ctx->r2, -0X3);
    // 0x8041B9BC: sltiu       $a1, $a2, 0x1E
    ctx->r5 = ctx->r6 < 0X1E ? 1 : 0;
    // 0x8041B9C0: beq         $a1, $zero, L_8041B9F0
    if (ctx->r5 == 0) {
        // 0x8041B9C4: addiu       $a1, $v0, -0x7
        ctx->r5 = ADD32(ctx->r2, -0X7);
            goto L_8041B9F0;
    }
    // 0x8041B9C4: addiu       $a1, $v0, -0x7
    ctx->r5 = ADD32(ctx->r2, -0X7);
    // 0x8041B9C8: lui         $a1, 0x2100
    ctx->r5 = S32(0X2100 << 16);
    // 0x8041B9CC: addiu       $a1, $a1, 0x5
    ctx->r5 = ADD32(ctx->r5, 0X5);
    // 0x8041B9D0: srlv        $a1, $a1, $a2
    ctx->r5 = S32(U32(ctx->r5) >> (ctx->r6 & 31));
    // 0x8041B9D4: andi        $a1, $a1, 0x1
    ctx->r5 = ctx->r5 & 0X1;
    // 0x8041B9D8: beq         $a1, $zero, L_8041B9EC
    if (ctx->r5 == 0) {
        // 0x8041B9DC: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041B9EC;
    }
    // 0x8041B9DC: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
    // 0x8041B9E0: lw          $a1, 0x1688($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1688);
    // 0x8041B9E4: addiu       $a2, $zero, 0x3
    ctx->r6 = ADD32(0, 0X3);
    // 0x8041B9E8: sh          $a2, 0x20($a1)
    MEM_H(0X20, ctx->r5) = ctx->r6;
L_8041B9EC:
    // 0x8041B9EC: addiu       $a1, $v0, -0x7
    ctx->r5 = ADD32(ctx->r2, -0X7);
L_8041B9F0:
    // 0x8041B9F0: sltiu       $a2, $a1, 0x1B
    ctx->r6 = ctx->r5 < 0X1B ? 1 : 0;
    // 0x8041B9F4: beql        $a2, $zero, L_8041BA18
    if (ctx->r6 == 0) {
        // 0x8041B9F8: addiu       $v0, $v0, -0x9
        ctx->r2 = ADD32(ctx->r2, -0X9);
            goto L_8041BA18;
    }
    goto skip_0;
    // 0x8041B9F8: addiu       $v0, $v0, -0x9
    ctx->r2 = ADD32(ctx->r2, -0X9);
    skip_0:
    // 0x8041B9FC: lui         $a2, 0x428
    ctx->r6 = S32(0X428 << 16);
    // 0x8041BA00: addiu       $a2, $a2, 0x21
    ctx->r6 = ADD32(ctx->r6, 0X21);
    // 0x8041BA04: srlv        $a1, $a2, $a1
    ctx->r5 = S32(U32(ctx->r6) >> (ctx->r5 & 31));
    // 0x8041BA08: andi        $a1, $a1, 0x1
    ctx->r5 = ctx->r5 & 0X1;
    // 0x8041BA0C: bne         $a1, $zero, L_8041BC88
    if (ctx->r5 != 0) {
        // 0x8041BA10: lui         $a1, 0x8041
        ctx->r5 = S32(0X8041 << 16);
            goto L_8041BC88;
    }
    // 0x8041BA10: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
L_8041BA14:
    // 0x8041BA14: addiu       $v0, $v0, -0x9
    ctx->r2 = ADD32(ctx->r2, -0X9);
L_8041BA18:
    // 0x8041BA18: sltiu       $a1, $v0, 0x1A
    ctx->r5 = ctx->r2 < 0X1A ? 1 : 0;
    // 0x8041BA1C: beq         $a1, $zero, L_8041BA4C
    if (ctx->r5 == 0) {
        // 0x8041BA20: addiu       $a1, $zero, 0x1
        ctx->r5 = ADD32(0, 0X1);
            goto L_8041BA4C;
    }
    // 0x8041BA20: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041BA24: lui         $a1, 0x230
    ctx->r5 = S32(0X230 << 16);
    // 0x8041BA28: addiu       $a1, $a1, 0x5
    ctx->r5 = ADD32(ctx->r5, 0X5);
    // 0x8041BA2C: srlv        $v0, $a1, $v0
    ctx->r2 = S32(U32(ctx->r5) >> (ctx->r2 & 31));
    // 0x8041BA30: andi        $v0, $v0, 0x1
    ctx->r2 = ctx->r2 & 0X1;
    // 0x8041BA34: beq         $v0, $zero, L_8041BA48
    if (ctx->r2 == 0) {
        // 0x8041BA38: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BA48;
    }
    // 0x8041BA38: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BA3C: lw          $v0, 0x1688($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1688);
    // 0x8041BA40: addiu       $a1, $zero, 0x6
    ctx->r5 = ADD32(0, 0X6);
    // 0x8041BA44: sh          $a1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r5;
L_8041BA48:
    // 0x8041BA48: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
L_8041BA4C:
    // 0x8041BA4C: beq         $v1, $a1, L_8041BDC8
    if (ctx->r3 == ctx->r5) {
        // 0x8041BA50: lbu         $a2, 0x24($a0)
        ctx->r6 = MEM_BU(ctx->r4, 0X24);
            goto L_8041BDC8;
    }
    // 0x8041BA50: lbu         $a2, 0x24($a0)
    ctx->r6 = MEM_BU(ctx->r4, 0X24);
    // 0x8041BA54: beq         $a2, $a1, L_8041BC98
    if (ctx->r6 == ctx->r5) {
        // 0x8041BA58: sltiu       $v0, $v1, 0x10
        ctx->r2 = ctx->r3 < 0X10 ? 1 : 0;
            goto L_8041BC98;
    }
    // 0x8041BA58: sltiu       $v0, $v1, 0x10
    ctx->r2 = ctx->r3 < 0X10 ? 1 : 0;
    // 0x8041BA5C: bne         $v0, $zero, L_8041BDEC
    if (ctx->r2 != 0) {
        // 0x8041BA60: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BDEC;
    }
    // 0x8041BA60: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BA64: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BA68: sltiu       $a3, $a2, 0x26
    ctx->r7 = ctx->r6 < 0X26 ? 1 : 0;
    // 0x8041BA6C: sh          $a1, 0x9E($v0)
    MEM_H(0X9E, ctx->r2) = ctx->r5;
    // 0x8041BA70: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041BA74: sh          $a1, 0x8A($v0)
    MEM_H(0X8A, ctx->r2) = ctx->r5;
    // 0x8041BA78: addiu       $a1, $zero, 0x8
    ctx->r5 = ADD32(0, 0X8);
    // 0x8041BA7C: sh          $zero, 0xA0($v0)
    MEM_H(0XA0, ctx->r2) = 0;
    // 0x8041BA80: bne         $a3, $zero, L_8041BCC8
    if (ctx->r7 != 0) {
        // 0x8041BA84: sh          $a1, 0x92($v0)
        MEM_H(0X92, ctx->r2) = ctx->r5;
            goto L_8041BCC8;
    }
    // 0x8041BA84: sh          $a1, 0x92($v0)
    MEM_H(0X92, ctx->r2) = ctx->r5;
L_8041BA88:
    // 0x8041BA88: sltiu       $a1, $a2, 0x2D
    ctx->r5 = ctx->r6 < 0X2D ? 1 : 0;
    // 0x8041BA8C: bne         $a1, $zero, L_8041C004
    if (ctx->r5 != 0) {
        // 0x8041BA90: sltiu       $a1, $a2, 0x33
        ctx->r5 = ctx->r6 < 0X33 ? 1 : 0;
            goto L_8041C004;
    }
    // 0x8041BA90: sltiu       $a1, $a2, 0x33
    ctx->r5 = ctx->r6 < 0X33 ? 1 : 0;
    // 0x8041BA94: bne         $a1, $zero, L_8041C080
    if (ctx->r5 != 0) {
        // 0x8041BA98: addiu       $a1, $zero, 0x64
        ctx->r5 = ADD32(0, 0X64);
            goto L_8041C080;
    }
    // 0x8041BA98: addiu       $a1, $zero, 0x64
    ctx->r5 = ADD32(0, 0X64);
    // 0x8041BA9C: sh          $a1, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r5;
L_8041BAA0:
    // 0x8041BAA0: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BAA4: beq         $v1, $v0, L_8041BE54
    if (ctx->r3 == ctx->r2) {
        // 0x8041BAA8: lbu         $a3, 0x26($a0)
        ctx->r7 = MEM_BU(ctx->r4, 0X26);
            goto L_8041BE54;
    }
    // 0x8041BAA8: lbu         $a3, 0x26($a0)
    ctx->r7 = MEM_BU(ctx->r4, 0X26);
    // 0x8041BAAC: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BAB0: beq         $a3, $v0, L_8041BCD8
    if (ctx->r7 == ctx->r2) {
        // 0x8041BAB4: lbu         $a1, 0x25($a0)
        ctx->r5 = MEM_BU(ctx->r4, 0X25);
            goto L_8041BCD8;
    }
    // 0x8041BAB4: lbu         $a1, 0x25($a0)
    ctx->r5 = MEM_BU(ctx->r4, 0X25);
L_8041BAB8:
    // 0x8041BAB8: sltiu       $v0, $v1, 0x25
    ctx->r2 = ctx->r3 < 0X25 ? 1 : 0;
    // 0x8041BABC: bne         $v0, $zero, L_8041BF90
    if (ctx->r2 != 0) {
        // 0x8041BAC0: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BF90;
    }
    // 0x8041BAC0: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BAC4: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BAC8: addiu       $t0, $zero, 0x2
    ctx->r8 = ADD32(0, 0X2);
    // 0x8041BACC: sltiu       $a0, $a3, 0x28
    ctx->r4 = ctx->r7 < 0X28 ? 1 : 0;
    // 0x8041BAD0: bne         $a0, $zero, L_8041BAEC
    if (ctx->r4 != 0) {
        // 0x8041BAD4: sb          $t0, 0x102($v0)
        MEM_B(0X102, ctx->r2) = ctx->r8;
            goto L_8041BAEC;
    }
    // 0x8041BAD4: sb          $t0, 0x102($v0)
    MEM_B(0X102, ctx->r2) = ctx->r8;
L_8041BAD8:
    // 0x8041BAD8: addiu       $a0, $zero, 0x2
    ctx->r4 = ADD32(0, 0X2);
    // 0x8041BADC: sb          $a0, 0x103($v0)
    MEM_B(0X103, ctx->r2) = ctx->r4;
L_8041BAE0:
    // 0x8041BAE0: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BAE4: beq         $v1, $v0, L_8041BE84
    if (ctx->r3 == ctx->r2) {
        // 0x8041BAE8: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BE84;
    }
    // 0x8041BAE8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041BAEC:
    // 0x8041BAEC: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041BAF0: beq         $a1, $a0, L_8041BD04
    if (ctx->r5 == ctx->r4) {
        // 0x8041BAF4: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BD04;
    }
    // 0x8041BAF4: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BAF8: sltiu       $v0, $v1, 0x2A
    ctx->r2 = ctx->r3 < 0X2A ? 1 : 0;
L_8041BAFC:
    // 0x8041BAFC: bne         $v0, $zero, L_8041BF78
    if (ctx->r2 != 0) {
        // 0x8041BB00: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BF78;
    }
    // 0x8041BB00: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BB04: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BB08: sltiu       $t0, $a1, 0x26
    ctx->r8 = ctx->r5 < 0X26 ? 1 : 0;
    // 0x8041BB0C: sh          $a0, 0x126($v0)
    MEM_H(0X126, ctx->r2) = ctx->r4;
    // 0x8041BB10: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x8041BB14: sh          $a0, 0x112($v0)
    MEM_H(0X112, ctx->r2) = ctx->r4;
    // 0x8041BB18: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    // 0x8041BB1C: sh          $zero, 0x128($v0)
    MEM_H(0X128, ctx->r2) = 0;
    // 0x8041BB20: bne         $t0, $zero, L_8041BB4C
    if (ctx->r8 != 0) {
        // 0x8041BB24: sh          $a0, 0x11A($v0)
        MEM_H(0X11A, ctx->r2) = ctx->r4;
            goto L_8041BB4C;
    }
    // 0x8041BB24: sh          $a0, 0x11A($v0)
    MEM_H(0X11A, ctx->r2) = ctx->r4;
L_8041BB28:
    // 0x8041BB28: sltiu       $a0, $a1, 0x2D
    ctx->r4 = ctx->r5 < 0X2D ? 1 : 0;
L_8041BB2C:
    // 0x8041BB2C: bne         $a0, $zero, L_8041C010
    if (ctx->r4 != 0) {
        // 0x8041BB30: sltiu       $a1, $a1, 0x33
        ctx->r5 = ctx->r5 < 0X33 ? 1 : 0;
            goto L_8041C010;
    }
    // 0x8041BB30: sltiu       $a1, $a1, 0x33
    ctx->r5 = ctx->r5 < 0X33 ? 1 : 0;
    // 0x8041BB34: bne         $a1, $zero, L_8041C01C
    if (ctx->r5 != 0) {
        // 0x8041BB38: addiu       $a0, $zero, 0x64
        ctx->r4 = ADD32(0, 0X64);
            goto L_8041C01C;
    }
    // 0x8041BB38: addiu       $a0, $zero, 0x64
    ctx->r4 = ADD32(0, 0X64);
    // 0x8041BB3C: sh          $a0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r4;
L_8041BB40:
    // 0x8041BB40: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BB44: beq         $v1, $v0, L_8041BE34
    if (ctx->r3 == ctx->r2) {
        // 0x8041BB48: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BE34;
    }
    // 0x8041BB48: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041BB4C:
    // 0x8041BB4C: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BB50: beq         $a2, $v0, L_8041BD40
    if (ctx->r6 == ctx->r2) {
        // 0x8041BB54: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BD40;
    }
    // 0x8041BB54: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BB58: sltiu       $v0, $v1, 0x64
    ctx->r2 = ctx->r3 < 0X64 ? 1 : 0;
L_8041BB5C:
    // 0x8041BB5C: bne         $v0, $zero, L_8041BB78
    if (ctx->r2 != 0) {
        // 0x8041BB60: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BB78;
    }
    // 0x8041BB60: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BB64: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
L_8041BB68:
    // 0x8041BB68: sltiu       $a0, $v1, 0x96
    ctx->r4 = ctx->r3 < 0X96 ? 1 : 0;
    // 0x8041BB6C: bne         $a0, $zero, L_8041C028
    if (ctx->r4 != 0) {
        // 0x8041BB70: addiu       $a0, $zero, 0x5DC
        ctx->r4 = ADD32(0, 0X5DC);
            goto L_8041C028;
    }
    // 0x8041BB70: addiu       $a0, $zero, 0x5DC
    ctx->r4 = ADD32(0, 0X5DC);
    // 0x8041BB74: sh          $a0, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r4;
L_8041BB78:
    // 0x8041BB78: sltiu       $v0, $a2, 0x64
    ctx->r2 = ctx->r6 < 0X64 ? 1 : 0;
    // 0x8041BB7C: bne         $v0, $zero, L_8041BBB0
    if (ctx->r2 != 0) {
        // 0x8041BB80: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BBB0;
    }
    // 0x8041BB80: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BB84: sltiu       $a0, $a2, 0x69
    ctx->r4 = ctx->r6 < 0X69 ? 1 : 0;
    // 0x8041BB88: bne         $a0, $zero, L_8041C034
    if (ctx->r4 != 0) {
        // 0x8041BB8C: lw          $a1, 0x1674($v0)
        ctx->r5 = MEM_W(ctx->r2, 0X1674);
            goto L_8041C034;
    }
    // 0x8041BB8C: lw          $a1, 0x1674($v0)
    ctx->r5 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BB90: sltiu       $v0, $a2, 0x6E
    ctx->r2 = ctx->r6 < 0X6E ? 1 : 0;
    // 0x8041BB94: bne         $v0, $zero, L_8041C040
    if (ctx->r2 != 0) {
        // 0x8041BB98: sltiu       $v0, $a2, 0x73
        ctx->r2 = ctx->r6 < 0X73 ? 1 : 0;
            goto L_8041C040;
    }
    // 0x8041BB98: sltiu       $v0, $a2, 0x73
    ctx->r2 = ctx->r6 < 0X73 ? 1 : 0;
    // 0x8041BB9C: bne         $v0, $zero, L_8041C04C
    if (ctx->r2 != 0) {
        // 0x8041BBA0: sltiu       $v0, $a2, 0x78
        ctx->r2 = ctx->r6 < 0X78 ? 1 : 0;
            goto L_8041C04C;
    }
    // 0x8041BBA0: sltiu       $v0, $a2, 0x78
    ctx->r2 = ctx->r6 < 0X78 ? 1 : 0;
    // 0x8041BBA4: bne         $v0, $zero, L_8041C058
    if (ctx->r2 != 0) {
        // 0x8041BBA8: addiu       $v0, $zero, 0x6
        ctx->r2 = ADD32(0, 0X6);
            goto L_8041C058;
    }
    // 0x8041BBA8: addiu       $v0, $zero, 0x6
    ctx->r2 = ADD32(0, 0X6);
    // 0x8041BBAC: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
L_8041BBB0:
    // 0x8041BBB0: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
L_8041BBB4:
    // 0x8041BBB4: beql        $a3, $v0, L_8041BD58
    if (ctx->r7 == ctx->r2) {
        // 0x8041BBB8: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BD58;
    }
    goto skip_1;
    // 0x8041BBB8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    skip_1:
    // 0x8041BBBC: beq         $v1, $v0, L_8041BEC0
    if (ctx->r3 == ctx->r2) {
        // 0x8041BBC0: sltiu       $v0, $a3, 0x19
        ctx->r2 = ctx->r7 < 0X19 ? 1 : 0;
            goto L_8041BEC0;
    }
    // 0x8041BBC0: sltiu       $v0, $a3, 0x19
    ctx->r2 = ctx->r7 < 0X19 ? 1 : 0;
    // 0x8041BBC4: bne         $v0, $zero, L_8041BBE8
    if (ctx->r2 != 0) {
        // 0x8041BBC8: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BBE8;
    }
    // 0x8041BBC8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BBCC: lw          $a0, 0x1674($v0)
    ctx->r4 = MEM_W(ctx->r2, 0X1674);
L_8041BBD0:
    // 0x8041BBD0: addiu       $v0, $zero, 0x3
    ctx->r2 = ADD32(0, 0X3);
    // 0x8041BBD4: sltiu       $a3, $a3, 0x1F
    ctx->r7 = ctx->r7 < 0X1F ? 1 : 0;
    // 0x8041BBD8: bne         $a3, $zero, L_8041BBE8
    if (ctx->r7 != 0) {
        // 0x8041BBDC: sb          $v0, 0x257($a0)
        MEM_B(0X257, ctx->r4) = ctx->r2;
            goto L_8041BBE8;
    }
    // 0x8041BBDC: sb          $v0, 0x257($a0)
    MEM_B(0X257, ctx->r4) = ctx->r2;
    // 0x8041BBE0: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BBE4: sb          $v0, 0x256($a0)
    MEM_B(0X256, ctx->r4) = ctx->r2;
L_8041BBE8:
    // 0x8041BBE8: sltiu       $v0, $v1, 0x1F
    ctx->r2 = ctx->r3 < 0X1F ? 1 : 0;
    // 0x8041BBEC: bne         $v0, $zero, L_8041BE00
    if (ctx->r2 != 0) {
        // 0x8041BBF0: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BE00;
    }
    // 0x8041BBF0: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BBF4: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BBF8: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x8041BBFC: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
    // 0x8041BC00: lwc1        $f0, -0x3F4C($a3)
    ctx->f0.u32l = MEM_W(ctx->r7, -0X3F4C);
    // 0x8041BC04: addiu       $t0, $zero, 0x113
    ctx->r8 = ADD32(0, 0X113);
    // 0x8041BC08: sltiu       $a1, $v1, 0x24
    ctx->r5 = ctx->r3 < 0X24 ? 1 : 0;
    // 0x8041BC0C: swc1        $f0, 0x23C($a0)
    MEM_W(0X23C, ctx->r4) = ctx->f0.u32l;
    // 0x8041BC10: bne         $a1, $zero, L_8041C06C
    if (ctx->r5 != 0) {
        // 0x8041BC14: sh          $t0, 0x22C($a0)
        MEM_H(0X22C, ctx->r4) = ctx->r8;
            goto L_8041C06C;
    }
    // 0x8041BC14: sh          $t0, 0x22C($a0)
    MEM_H(0X22C, ctx->r4) = ctx->r8;
    // 0x8041BC18: sltiu       $a1, $v1, 0x30
    ctx->r5 = ctx->r3 < 0X30 ? 1 : 0;
    // 0x8041BC1C: bne         $a1, $zero, L_8041C064
    if (ctx->r5 != 0) {
        // 0x8041BC20: addiu       $a1, $zero, 0x6
        ctx->r5 = ADD32(0, 0X6);
            goto L_8041C064;
    }
    // 0x8041BC20: addiu       $a1, $zero, 0x6
    ctx->r5 = ADD32(0, 0X6);
    // 0x8041BC24: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BC28: beq         $a2, $v0, L_8041BEEC
    if (ctx->r6 == ctx->r2) {
        // 0x8041BC2C: sh          $a1, 0x2C8($a0)
        MEM_H(0X2C8, ctx->r4) = ctx->r5;
            goto L_8041BEEC;
    }
    // 0x8041BC2C: sh          $a1, 0x2C8($a0)
    MEM_H(0X2C8, ctx->r4) = ctx->r5;
    // 0x8041BC30: lwc1        $f0, -0x3F4C($a3)
    ctx->f0.u32l = MEM_W(ctx->r7, -0X3F4C);
L_8041BC34:
    // 0x8041BC34: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
L_8041BC38:
    // 0x8041BC38: sltiu       $v0, $a2, 0x14
    ctx->r2 = ctx->r6 < 0X14 ? 1 : 0;
L_8041BC3C:
    // 0x8041BC3C: bne         $v0, $zero, L_8041BDA4
    if (ctx->r2 != 0) {
        // 0x8041BC40: lui         $v1, 0x8041
        ctx->r3 = S32(0X8041 << 16);
            goto L_8041BDA4;
    }
    // 0x8041BC40: lui         $v1, 0x8041
    ctx->r3 = S32(0X8041 << 16);
    // 0x8041BC44: sltiu       $v0, $a2, 0x18
    ctx->r2 = ctx->r6 < 0X18 ? 1 : 0;
    // 0x8041BC48: bne         $v0, $zero, L_8041BFC8
    if (ctx->r2 != 0) {
        // 0x8041BC4C: lw          $v1, 0x1674($v1)
        ctx->r3 = MEM_W(ctx->r3, 0X1674);
            goto L_8041BFC8;
    }
    // 0x8041BC4C: lw          $v1, 0x1674($v1)
    ctx->r3 = MEM_W(ctx->r3, 0X1674);
    // 0x8041BC50: sltiu       $v0, $a2, 0x1D
    ctx->r2 = ctx->r6 < 0X1D ? 1 : 0;
    // 0x8041BC54: bne         $v0, $zero, L_8041BFD4
    if (ctx->r2 != 0) {
        // 0x8041BC58: sltiu       $v0, $a2, 0x24
        ctx->r2 = ctx->r6 < 0X24 ? 1 : 0;
            goto L_8041BFD4;
    }
    // 0x8041BC58: sltiu       $v0, $a2, 0x24
    ctx->r2 = ctx->r6 < 0X24 ? 1 : 0;
    // 0x8041BC5C: bne         $v0, $zero, L_8041BFE0
    if (ctx->r2 != 0) {
        // 0x8041BC60: sltiu       $a2, $a2, 0x2A
        ctx->r6 = ctx->r6 < 0X2A ? 1 : 0;
            goto L_8041BFE0;
    }
    // 0x8041BC60: sltiu       $a2, $a2, 0x2A
    ctx->r6 = ctx->r6 < 0X2A ? 1 : 0;
    // 0x8041BC64: bne         $a2, $zero, L_8041BFEC
    if (ctx->r6 != 0) {
        // 0x8041BC68: addiu       $v0, $zero, 0x271
        ctx->r2 = ADD32(0, 0X271);
            goto L_8041BFEC;
    }
    // 0x8041BC68: addiu       $v0, $zero, 0x271
    ctx->r2 = ADD32(0, 0X271);
    // 0x8041BC6C: jr          $ra
    // 0x8041BC70: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
    return;
    // 0x8041BC70: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
L_8041BC74:
    // 0x8041BC74: lui         $a1, 0x8041
    ctx->r5 = S32(0X8041 << 16);
L_8041BC78:
    // 0x8041BC78: lw          $a1, 0x1688($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1688);
    // 0x8041BC7C: addiu       $a2, $zero, 0x2
    ctx->r6 = ADD32(0, 0X2);
    // 0x8041BC80: b           L_8041B9B8
    // 0x8041BC84: sh          $a2, 0x20($a1)
    MEM_H(0X20, ctx->r5) = ctx->r6;
        goto L_8041B9B8;
    // 0x8041BC84: sh          $a2, 0x20($a1)
    MEM_H(0X20, ctx->r5) = ctx->r6;
L_8041BC88:
    // 0x8041BC88: lw          $a1, 0x1688($a1)
    ctx->r5 = MEM_W(ctx->r5, 0X1688);
    // 0x8041BC8C: addiu       $a2, $zero, 0x4
    ctx->r6 = ADD32(0, 0X4);
    // 0x8041BC90: b           L_8041BA14
    // 0x8041BC94: sh          $a2, 0x20($a1)
    MEM_H(0X20, ctx->r5) = ctx->r6;
        goto L_8041BA14;
    // 0x8041BC94: sh          $a2, 0x20($a1)
    MEM_H(0X20, ctx->r5) = ctx->r6;
L_8041BC98:
    // 0x8041BC98: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BC9C: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BCA0: addiu       $a3, $zero, 0x50
    ctx->r7 = ADD32(0, 0X50);
    // 0x8041BCA4: sltiu       $a1, $v1, 0x10
    ctx->r5 = ctx->r3 < 0X10 ? 1 : 0;
    // 0x8041BCA8: bne         $a1, $zero, L_8041BCC8
    if (ctx->r5 != 0) {
        // 0x8041BCAC: sh          $a3, 0x96($v0)
        MEM_H(0X96, ctx->r2) = ctx->r7;
            goto L_8041BCC8;
    }
    // 0x8041BCAC: sh          $a3, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r7;
    // 0x8041BCB0: addiu       $a1, $zero, 0x5
    ctx->r5 = ADD32(0, 0X5);
    // 0x8041BCB4: sh          $a1, 0x8A($v0)
    MEM_H(0X8A, ctx->r2) = ctx->r5;
    // 0x8041BCB8: addiu       $a1, $zero, 0x8
    ctx->r5 = ADD32(0, 0X8);
    // 0x8041BCBC: sh          $a2, 0x9E($v0)
    MEM_H(0X9E, ctx->r2) = ctx->r6;
    // 0x8041BCC0: sh          $zero, 0xA0($v0)
    MEM_H(0XA0, ctx->r2) = 0;
    // 0x8041BCC4: sh          $a1, 0x92($v0)
    MEM_H(0X92, ctx->r2) = ctx->r5;
L_8041BCC8:
    // 0x8041BCC8: lbu         $a3, 0x26($a0)
    ctx->r7 = MEM_BU(ctx->r4, 0X26);
    // 0x8041BCCC: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BCD0: bne         $a3, $v0, L_8041BAB8
    if (ctx->r7 != ctx->r2) {
        // 0x8041BCD4: lbu         $a1, 0x25($a0)
        ctx->r5 = MEM_BU(ctx->r4, 0X25);
            goto L_8041BAB8;
    }
    // 0x8041BCD4: lbu         $a1, 0x25($a0)
    ctx->r5 = MEM_BU(ctx->r4, 0X25);
L_8041BCD8:
    // 0x8041BCD8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BCDC: lw          $a0, 0x1674($v0)
    ctx->r4 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BCE0: sltiu       $v0, $v1, 0x25
    ctx->r2 = ctx->r3 < 0X25 ? 1 : 0;
    // 0x8041BCE4: bne         $v0, $zero, L_8041BAE0
    if (ctx->r2 != 0) {
        // 0x8041BCE8: sb          $a3, 0x103($a0)
        MEM_B(0X103, ctx->r4) = ctx->r7;
            goto L_8041BAE0;
    }
    // 0x8041BCE8: sb          $a3, 0x103($a0)
    MEM_B(0X103, ctx->r4) = ctx->r7;
    // 0x8041BCEC: addiu       $v0, $zero, 0x2
    ctx->r2 = ADD32(0, 0X2);
    // 0x8041BCF0: sb          $v0, 0x102($a0)
    MEM_B(0X102, ctx->r4) = ctx->r2;
    // 0x8041BCF4: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041BCF8: bne         $a1, $a0, L_8041BAFC
    if (ctx->r5 != ctx->r4) {
        // 0x8041BCFC: sltiu       $v0, $v1, 0x2A
        ctx->r2 = ctx->r3 < 0X2A ? 1 : 0;
            goto L_8041BAFC;
    }
    // 0x8041BCFC: sltiu       $v0, $v1, 0x2A
    ctx->r2 = ctx->r3 < 0X2A ? 1 : 0;
    // 0x8041BD00: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041BD04:
    // 0x8041BD04: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BD08: addiu       $t0, $zero, 0x50
    ctx->r8 = ADD32(0, 0X50);
    // 0x8041BD0C: sltiu       $a0, $v1, 0x2A
    ctx->r4 = ctx->r3 < 0X2A ? 1 : 0;
    // 0x8041BD10: bne         $a0, $zero, L_8041BB40
    if (ctx->r4 != 0) {
        // 0x8041BD14: sh          $t0, 0x11E($v0)
        MEM_H(0X11E, ctx->r2) = ctx->r8;
            goto L_8041BB40;
    }
    // 0x8041BD14: sh          $t0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r8;
    // 0x8041BD18: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x8041BD1C: sh          $a0, 0x112($v0)
    MEM_H(0X112, ctx->r2) = ctx->r4;
    // 0x8041BD20: addiu       $a0, $zero, 0x8
    ctx->r4 = ADD32(0, 0X8);
    // 0x8041BD24: sh          $a1, 0x126($v0)
    MEM_H(0X126, ctx->r2) = ctx->r5;
    // 0x8041BD28: sh          $zero, 0x128($v0)
    MEM_H(0X128, ctx->r2) = 0;
    // 0x8041BD2C: sh          $a0, 0x11A($v0)
    MEM_H(0X11A, ctx->r2) = ctx->r4;
    // 0x8041BD30: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BD34: bne         $a2, $v0, L_8041BB5C
    if (ctx->r6 != ctx->r2) {
        // 0x8041BD38: sltiu       $v0, $v1, 0x64
        ctx->r2 = ctx->r3 < 0X64 ? 1 : 0;
            goto L_8041BB5C;
    }
    // 0x8041BD38: sltiu       $v0, $v1, 0x64
    ctx->r2 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041BD3C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
L_8041BD40:
    // 0x8041BD40: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BD44: sltiu       $a0, $v1, 0x64
    ctx->r4 = ctx->r3 < 0X64 ? 1 : 0;
    // 0x8041BD48: beq         $a0, $zero, L_8041BB68
    if (ctx->r4 == 0) {
        // 0x8041BD4C: sh          $a2, 0x1FC($v0)
        MEM_H(0X1FC, ctx->r2) = ctx->r6;
            goto L_8041BB68;
    }
    // 0x8041BD4C: sh          $a2, 0x1FC($v0)
    MEM_H(0X1FC, ctx->r2) = ctx->r6;
    // 0x8041BD50: b           L_8041BBB4
    // 0x8041BD54: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
        goto L_8041BBB4;
    // 0x8041BD54: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
L_8041BD58:
    // 0x8041BD58: lw          $a0, 0x1674($v0)
    ctx->r4 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BD5C: bne         $v1, $a3, L_8041BBE8
    if (ctx->r3 != ctx->r7) {
        // 0x8041BD60: sh          $zero, 0x256($a0)
        MEM_H(0X256, ctx->r4) = 0;
            goto L_8041BBE8;
    }
    // 0x8041BD60: sh          $zero, 0x256($a0)
    MEM_H(0X256, ctx->r4) = 0;
    // 0x8041BD64: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041BD68: lwc1        $f0, -0x3F50($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F50);
    // 0x8041BD6C: addiu       $v0, $zero, 0xB4
    ctx->r2 = ADD32(0, 0XB4);
    // 0x8041BD70: swc1        $f0, 0x23C($a0)
    MEM_W(0X23C, ctx->r4) = ctx->f0.u32l;
    // 0x8041BD74: sh          $v0, 0x22C($a0)
    MEM_H(0X22C, ctx->r4) = ctx->r2;
    // 0x8041BD78: lui         $a1, 0x8042
    ctx->r5 = S32(0X8042 << 16);
L_8041BD7C:
    // 0x8041BD7C: lwc1        $f0, -0x3F2C($a1)
    ctx->f0.u32l = MEM_W(ctx->r5, -0X3F2C);
    // 0x8041BD80: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x8041BD84: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BD88: sh          $a3, 0x2C8($a0)
    MEM_H(0X2C8, ctx->r4) = ctx->r7;
    // 0x8041BD8C: bne         $a2, $v0, L_8041BC38
    if (ctx->r6 != ctx->r2) {
        // 0x8041BD90: swc1        $f0, 0x308($a0)
        MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
            goto L_8041BC38;
    }
    // 0x8041BD90: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
L_8041BD94:
    // 0x8041BD94: addiu       $a1, $zero, 0xFA
    ctx->r5 = ADD32(0, 0XFA);
L_8041BD98:
    // 0x8041BD98: sltiu       $v0, $v1, 0x15
    ctx->r2 = ctx->r3 < 0X15 ? 1 : 0;
    // 0x8041BD9C: beq         $v0, $zero, L_8041BF38
    if (ctx->r2 == 0) {
        // 0x8041BDA0: sh          $a1, 0x2F8($a0)
        MEM_H(0X2F8, ctx->r4) = ctx->r5;
            goto L_8041BF38;
    }
    // 0x8041BDA0: sh          $a1, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r5;
L_8041BDA4:
    // 0x8041BDA4: jr          $ra
    // 0x8041BDA8: nop

    return;
    // 0x8041BDA8: nop

L_8041BDAC:
    // 0x8041BDAC: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BDB0: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BDB4: addiu       $a1, $zero, 0x78
    ctx->r5 = ADD32(0, 0X78);
    // 0x8041BDB8: sh          $v1, 0x2($v0)
    MEM_H(0X2, ctx->r2) = ctx->r3;
    // 0x8041BDBC: sh          $a1, 0xC($v0)
    MEM_H(0XC, ctx->r2) = ctx->r5;
    // 0x8041BDC0: b           L_8041B99C
    // 0x8041BDC4: sh          $v1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r3;
        goto L_8041B99C;
    // 0x8041BDC4: sh          $v1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r3;
L_8041BDC8:
    // 0x8041BDC8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BDCC: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BDD0: lw          $a1, 0xC($v0)
    ctx->r5 = MEM_W(ctx->r2, 0XC);
    // 0x8041BDD4: addiu       $a3, $zero, 0x5
    ctx->r7 = ADD32(0, 0X5);
    // 0x8041BDD8: sh          $v1, 0x9E($a1)
    MEM_H(0X9E, ctx->r5) = ctx->r3;
    // 0x8041BDDC: sh          $a3, 0x8A($a1)
    MEM_H(0X8A, ctx->r5) = ctx->r7;
    // 0x8041BDE0: sh          $zero, 0xA0($a1)
    MEM_H(0XA0, ctx->r5) = 0;
    // 0x8041BDE4: beq         $a2, $v1, L_8041BF68
    if (ctx->r6 == ctx->r3) {
        // 0x8041BDE8: sh          $v1, 0x92($a1)
        MEM_H(0X92, ctx->r5) = ctx->r3;
            goto L_8041BF68;
    }
    // 0x8041BDE8: sh          $v1, 0x92($a1)
    MEM_H(0X92, ctx->r5) = ctx->r3;
L_8041BDEC:
    // 0x8041BDEC: sltiu       $v0, $a2, 0x26
    ctx->r2 = ctx->r6 < 0X26 ? 1 : 0;
    // 0x8041BDF0: bne         $v0, $zero, L_8041BAA0
    if (ctx->r2 != 0) {
        // 0x8041BDF4: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BAA0;
    }
    // 0x8041BDF4: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BDF8: b           L_8041BA88
    // 0x8041BDFC: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
        goto L_8041BA88;
    // 0x8041BDFC: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
L_8041BE00:
    // 0x8041BE00: addiu       $v0, $zero, 0x1
    ctx->r2 = ADD32(0, 0X1);
    // 0x8041BE04: bne         $v1, $v0, L_8041BF1C
    if (ctx->r3 != ctx->r2) {
        // 0x8041BE08: lui         $a0, 0x8042
        ctx->r4 = S32(0X8042 << 16);
            goto L_8041BF1C;
    }
    // 0x8041BE08: lui         $a0, 0x8042
    ctx->r4 = S32(0X8042 << 16);
    // 0x8041BE0C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BE10: lw          $v0, 0x1674($v0)
    ctx->r2 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BE14: lwc1        $f0, -0x3F2C($a0)
    ctx->f0.u32l = MEM_W(ctx->r4, -0X3F2C);
    // 0x8041BE18: addiu       $a1, $zero, 0x2
    ctx->r5 = ADD32(0, 0X2);
    // 0x8041BE1C: sh          $a1, 0x2C8($v0)
    MEM_H(0X2C8, ctx->r2) = ctx->r5;
    // 0x8041BE20: bne         $a2, $v1, L_8041BC38
    if (ctx->r6 != ctx->r3) {
        // 0x8041BE24: swc1        $f0, 0x308($v0)
        MEM_W(0X308, ctx->r2) = ctx->f0.u32l;
            goto L_8041BC38;
    }
    // 0x8041BE24: swc1        $f0, 0x308($v0)
    MEM_W(0X308, ctx->r2) = ctx->f0.u32l;
    // 0x8041BE28: addiu       $v1, $zero, 0xFA
    ctx->r3 = ADD32(0, 0XFA);
    // 0x8041BE2C: jr          $ra
    // 0x8041BE30: sh          $v1, 0x2F8($v0)
    MEM_H(0X2F8, ctx->r2) = ctx->r3;
    return;
    // 0x8041BE30: sh          $v1, 0x2F8($v0)
    MEM_H(0X2F8, ctx->r2) = ctx->r3;
L_8041BE34:
    // 0x8041BE34: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BE38: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
L_8041BE3C:
    // 0x8041BE3C: addiu       $a1, $zero, 0x44C
    ctx->r5 = ADD32(0, 0X44C);
    // 0x8041BE40: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041BE44: bne         $a2, $a0, L_8041BB78
    if (ctx->r6 != ctx->r4) {
        // 0x8041BE48: sh          $a1, 0x1E8($v0)
        MEM_H(0X1E8, ctx->r2) = ctx->r5;
            goto L_8041BB78;
    }
    // 0x8041BE48: sh          $a1, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r5;
    // 0x8041BE4C: b           L_8041BBB0
    // 0x8041BE50: sh          $a2, 0x1FC($v0)
    MEM_H(0X1FC, ctx->r2) = ctx->r6;
        goto L_8041BBB0;
    // 0x8041BE50: sh          $a2, 0x1FC($v0)
    MEM_H(0X1FC, ctx->r2) = ctx->r6;
L_8041BE54:
    // 0x8041BE54: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BE58: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
L_8041BE5C:
    // 0x8041BE5C: lw          $t0, 0xC($v0)
    ctx->r8 = MEM_W(ctx->r2, 0XC);
    // 0x8041BE60: addiu       $t1, $zero, 0x1
    ctx->r9 = ADD32(0, 0X1);
    // 0x8041BE64: lbu         $a1, 0x25($a0)
    ctx->r5 = MEM_BU(ctx->r4, 0X25);
    // 0x8041BE68: beq         $a3, $t1, L_8041BF60
    if (ctx->r7 == ctx->r9) {
        // 0x8041BE6C: sb          $zero, 0x102($t0)
        MEM_B(0X102, ctx->r8) = 0;
            goto L_8041BF60;
    }
    // 0x8041BE6C: sb          $zero, 0x102($t0)
    MEM_B(0X102, ctx->r8) = 0;
    // 0x8041BE70: sltiu       $a0, $a3, 0x28
    ctx->r4 = ctx->r7 < 0X28 ? 1 : 0;
    // 0x8041BE74: bnel        $a0, $zero, L_8041BE8C
    if (ctx->r4 != 0) {
        // 0x8041BE78: lw          $a0, 0xC($v0)
        ctx->r4 = MEM_W(ctx->r2, 0XC);
            goto L_8041BE8C;
    }
    goto skip_2;
    // 0x8041BE78: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
    skip_2:
    // 0x8041BE7C: b           L_8041BAD8
    // 0x8041BE80: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
        goto L_8041BAD8;
    // 0x8041BE80: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
L_8041BE84:
    // 0x8041BE84: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
L_8041BE88:
    // 0x8041BE88: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
L_8041BE8C:
    // 0x8041BE8C: addiu       $t0, $zero, 0x1
    ctx->r8 = ADD32(0, 0X1);
    // 0x8041BE90: addiu       $t2, $zero, 0x11
    ctx->r10 = ADD32(0, 0X11);
    // 0x8041BE94: addiu       $t1, $zero, 0x1
    ctx->r9 = ADD32(0, 0X1);
    // 0x8041BE98: sh          $t0, 0x126($a0)
    MEM_H(0X126, ctx->r4) = ctx->r8;
    // 0x8041BE9C: sh          $t2, 0x112($a0)
    MEM_H(0X112, ctx->r4) = ctx->r10;
    // 0x8041BEA0: sh          $zero, 0x128($a0)
    MEM_H(0X128, ctx->r4) = 0;
    // 0x8041BEA4: beq         $a1, $t1, L_8041BF54
    if (ctx->r5 == ctx->r9) {
        // 0x8041BEA8: sh          $t0, 0x11A($a0)
        MEM_H(0X11A, ctx->r4) = ctx->r8;
            goto L_8041BF54;
    }
    // 0x8041BEA8: sh          $t0, 0x11A($a0)
    MEM_H(0X11A, ctx->r4) = ctx->r8;
    // 0x8041BEAC: sltiu       $a0, $a1, 0x26
    ctx->r4 = ctx->r5 < 0X26 ? 1 : 0;
    // 0x8041BEB0: bne         $a0, $zero, L_8041BE3C
    if (ctx->r4 != 0) {
        // 0x8041BEB4: lw          $v0, 0xC($v0)
        ctx->r2 = MEM_W(ctx->r2, 0XC);
            goto L_8041BE3C;
    }
    // 0x8041BEB4: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
    // 0x8041BEB8: b           L_8041BB2C
    // 0x8041BEBC: sltiu       $a0, $a1, 0x2D
    ctx->r4 = ctx->r5 < 0X2D ? 1 : 0;
        goto L_8041BB2C;
    // 0x8041BEBC: sltiu       $a0, $a1, 0x2D
    ctx->r4 = ctx->r5 < 0X2D ? 1 : 0;
L_8041BEC0:
    // 0x8041BEC0: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BEC4: lui         $a1, 0x8042
    ctx->r5 = S32(0X8042 << 16);
    // 0x8041BEC8: lw          $a0, 0x1674($v0)
    ctx->r4 = MEM_W(ctx->r2, 0X1674);
    // 0x8041BECC: lwc1        $f0, -0x3F50($a1)
    ctx->f0.u32l = MEM_W(ctx->r5, -0X3F50);
    // 0x8041BED0: sltiu       $v0, $a3, 0x19
    ctx->r2 = ctx->r7 < 0X19 ? 1 : 0;
    // 0x8041BED4: addiu       $a1, $zero, 0xB4
    ctx->r5 = ADD32(0, 0XB4);
    // 0x8041BED8: swc1        $f0, 0x23C($a0)
    MEM_W(0X23C, ctx->r4) = ctx->f0.u32l;
    // 0x8041BEDC: beq         $v0, $zero, L_8041BBD0
    if (ctx->r2 == 0) {
        // 0x8041BEE0: sh          $a1, 0x22C($a0)
        MEM_H(0X22C, ctx->r4) = ctx->r5;
            goto L_8041BBD0;
    }
    // 0x8041BEE0: sh          $a1, 0x22C($a0)
    MEM_H(0X22C, ctx->r4) = ctx->r5;
    // 0x8041BEE4: b           L_8041BD7C
    // 0x8041BEE8: lui         $a1, 0x8042
    ctx->r5 = S32(0X8042 << 16);
        goto L_8041BD7C;
    // 0x8041BEE8: lui         $a1, 0x8042
    ctx->r5 = S32(0X8042 << 16);
L_8041BEEC:
    // 0x8041BEEC: addiu       $v0, $zero, 0xFA
    ctx->r2 = ADD32(0, 0XFA);
    // 0x8041BEF0: sh          $v0, 0x2F8($a0)
    MEM_H(0X2F8, ctx->r4) = ctx->r2;
    // 0x8041BEF4: sltiu       $v0, $v1, 0x22
    ctx->r2 = ctx->r3 < 0X22 ? 1 : 0;
L_8041BEF8:
    // 0x8041BEF8: bne         $v0, $zero, L_8041BFB8
    if (ctx->r2 != 0) {
        // 0x8041BEFC: sltiu       $v0, $v1, 0x27
        ctx->r2 = ctx->r3 < 0X27 ? 1 : 0;
            goto L_8041BFB8;
    }
    // 0x8041BEFC: sltiu       $v0, $v1, 0x27
    ctx->r2 = ctx->r3 < 0X27 ? 1 : 0;
    // 0x8041BF00: bne         $v0, $zero, L_8041C09C
    if (ctx->r2 != 0) {
        // 0x8041BF04: lui         $v0, 0x8042
        ctx->r2 = S32(0X8042 << 16);
            goto L_8041C09C;
    }
    // 0x8041BF04: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041BF08: sltiu       $v1, $v1, 0x2E
    ctx->r3 = ctx->r3 < 0X2E ? 1 : 0;
    // 0x8041BF0C: bne         $v1, $zero, L_8041C08C
    if (ctx->r3 != 0) {
        // 0x8041BF10: lui         $a3, 0x8042
        ctx->r7 = S32(0X8042 << 16);
            goto L_8041C08C;
    }
    // 0x8041BF10: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x8041BF14: b           L_8041BC34
    // 0x8041BF18: lwc1        $f0, -0x3F4C($a3)
    ctx->f0.u32l = MEM_W(ctx->r7, -0X3F4C);
        goto L_8041BC34;
    // 0x8041BF18: lwc1        $f0, -0x3F4C($a3)
    ctx->f0.u32l = MEM_W(ctx->r7, -0X3F4C);
L_8041BF1C:
    // 0x8041BF1C: beq         $a2, $v0, L_8041BFA8
    if (ctx->r6 == ctx->r2) {
        // 0x8041BF20: sltiu       $v0, $v1, 0x15
        ctx->r2 = ctx->r3 < 0X15 ? 1 : 0;
            goto L_8041BFA8;
    }
    // 0x8041BF20: sltiu       $v0, $v1, 0x15
    ctx->r2 = ctx->r3 < 0X15 ? 1 : 0;
    // 0x8041BF24: bne         $v0, $zero, L_8041BC3C
    if (ctx->r2 != 0) {
        // 0x8041BF28: sltiu       $v0, $a2, 0x14
        ctx->r2 = ctx->r6 < 0X14 ? 1 : 0;
            goto L_8041BC3C;
    }
    // 0x8041BF28: sltiu       $v0, $a2, 0x14
    ctx->r2 = ctx->r6 < 0X14 ? 1 : 0;
    // 0x8041BF2C: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BF30: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BF34: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
L_8041BF38:
    // 0x8041BF38: sltiu       $v0, $v1, 0x1B
    ctx->r2 = ctx->r3 < 0X1B ? 1 : 0;
    // 0x8041BF3C: beq         $v0, $zero, L_8041BEF8
    if (ctx->r2 == 0) {
        // 0x8041BF40: sltiu       $v0, $v1, 0x22
        ctx->r2 = ctx->r3 < 0X22 ? 1 : 0;
            goto L_8041BEF8;
    }
    // 0x8041BF40: sltiu       $v0, $v1, 0x22
    ctx->r2 = ctx->r3 < 0X22 ? 1 : 0;
    // 0x8041BF44: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041BF48: lwc1        $f0, -0x3F28($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F28);
    // 0x8041BF4C: b           L_8041BC38
    // 0x8041BF50: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
        goto L_8041BC38;
    // 0x8041BF50: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
L_8041BF54:
    // 0x8041BF54: addiu       $v0, $zero, 0x50
    ctx->r2 = ADD32(0, 0X50);
    // 0x8041BF58: b           L_8041BB40
    // 0x8041BF5C: sh          $v0, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r2;
        goto L_8041BB40;
    // 0x8041BF5C: sh          $v0, 0x11E($a0)
    MEM_H(0X11E, ctx->r4) = ctx->r2;
L_8041BF60:
    // 0x8041BF60: b           L_8041BE88
    // 0x8041BF64: sb          $a3, 0x103($t0)
    MEM_B(0X103, ctx->r8) = ctx->r7;
        goto L_8041BE88;
    // 0x8041BF64: sb          $a3, 0x103($t0)
    MEM_B(0X103, ctx->r8) = ctx->r7;
L_8041BF68:
    // 0x8041BF68: addiu       $t0, $zero, 0x50
    ctx->r8 = ADD32(0, 0X50);
    // 0x8041BF6C: lbu         $a3, 0x26($a0)
    ctx->r7 = MEM_BU(ctx->r4, 0X26);
    // 0x8041BF70: b           L_8041BE5C
    // 0x8041BF74: sh          $t0, 0x96($a1)
    MEM_H(0X96, ctx->r5) = ctx->r8;
        goto L_8041BE5C;
    // 0x8041BF74: sh          $t0, 0x96($a1)
    MEM_H(0X96, ctx->r5) = ctx->r8;
L_8041BF78:
    // 0x8041BF78: sltiu       $v0, $a1, 0x26
    ctx->r2 = ctx->r5 < 0X26 ? 1 : 0;
    // 0x8041BF7C: bne         $v0, $zero, L_8041BB40
    if (ctx->r2 != 0) {
        // 0x8041BF80: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BB40;
    }
    // 0x8041BF80: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BF84: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BF88: b           L_8041BB28
    // 0x8041BF8C: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
        goto L_8041BB28;
    // 0x8041BF8C: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
L_8041BF90:
    // 0x8041BF90: sltiu       $v0, $a3, 0x28
    ctx->r2 = ctx->r7 < 0X28 ? 1 : 0;
    // 0x8041BF94: bne         $v0, $zero, L_8041BAE0
    if (ctx->r2 != 0) {
        // 0x8041BF98: lui         $v0, 0x8041
        ctx->r2 = S32(0X8041 << 16);
            goto L_8041BAE0;
    }
    // 0x8041BF98: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BF9C: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BFA0: b           L_8041BAD8
    // 0x8041BFA4: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
        goto L_8041BAD8;
    // 0x8041BFA4: lw          $v0, 0xC($v0)
    ctx->r2 = MEM_W(ctx->r2, 0XC);
L_8041BFA8:
    // 0x8041BFA8: lui         $v0, 0x8041
    ctx->r2 = S32(0X8041 << 16);
    // 0x8041BFAC: addiu       $v0, $v0, 0x1668
    ctx->r2 = ADD32(ctx->r2, 0X1668);
    // 0x8041BFB0: b           L_8041BD94
    // 0x8041BFB4: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
        goto L_8041BD94;
    // 0x8041BFB4: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
L_8041BFB8:
    // 0x8041BFB8: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041BFBC: lwc1        $f0, -0x3F24($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F24);
    // 0x8041BFC0: b           L_8041BC38
    // 0x8041BFC4: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
        goto L_8041BC38;
    // 0x8041BFC4: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
L_8041BFC8:
    // 0x8041BFC8: addiu       $v0, $zero, 0x145
    ctx->r2 = ADD32(0, 0X145);
    // 0x8041BFCC: jr          $ra
    // 0x8041BFD0: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
    return;
    // 0x8041BFD0: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
L_8041BFD4:
    // 0x8041BFD4: addiu       $v0, $zero, 0x190
    ctx->r2 = ADD32(0, 0X190);
    // 0x8041BFD8: jr          $ra
    // 0x8041BFDC: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
    return;
    // 0x8041BFDC: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
L_8041BFE0:
    // 0x8041BFE0: addiu       $v0, $zero, 0x1DB
    ctx->r2 = ADD32(0, 0X1DB);
    // 0x8041BFE4: jr          $ra
    // 0x8041BFE8: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
    return;
    // 0x8041BFE8: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
L_8041BFEC:
    // 0x8041BFEC: addiu       $v0, $zero, 0x226
    ctx->r2 = ADD32(0, 0X226);
    // 0x8041BFF0: jr          $ra
    // 0x8041BFF4: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
    return;
    // 0x8041BFF4: sh          $v0, 0x2F8($v1)
    MEM_H(0X2F8, ctx->r3) = ctx->r2;
L_8041BFF8:
    // 0x8041BFF8: addiu       $a1, $zero, 0x3
    ctx->r5 = ADD32(0, 0X3);
    // 0x8041BFFC: b           L_8041B99C
    // 0x8041C000: sh          $a1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r5;
        goto L_8041B99C;
    // 0x8041C000: sh          $a1, 0x20($v0)
    MEM_H(0X20, ctx->r2) = ctx->r5;
L_8041C004:
    // 0x8041C004: addiu       $a1, $zero, 0x55
    ctx->r5 = ADD32(0, 0X55);
    // 0x8041C008: b           L_8041BAA0
    // 0x8041C00C: sh          $a1, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r5;
        goto L_8041BAA0;
    // 0x8041C00C: sh          $a1, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r5;
L_8041C010:
    // 0x8041C010: addiu       $a0, $zero, 0x55
    ctx->r4 = ADD32(0, 0X55);
    // 0x8041C014: b           L_8041BB40
    // 0x8041C018: sh          $a0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r4;
        goto L_8041BB40;
    // 0x8041C018: sh          $a0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r4;
L_8041C01C:
    // 0x8041C01C: addiu       $a0, $zero, 0x5F
    ctx->r4 = ADD32(0, 0X5F);
    // 0x8041C020: b           L_8041BB40
    // 0x8041C024: sh          $a0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r4;
        goto L_8041BB40;
    // 0x8041C024: sh          $a0, 0x11E($v0)
    MEM_H(0X11E, ctx->r2) = ctx->r4;
L_8041C028:
    // 0x8041C028: addiu       $a0, $zero, 0x4B0
    ctx->r4 = ADD32(0, 0X4B0);
    // 0x8041C02C: b           L_8041BB78
    // 0x8041C030: sh          $a0, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r4;
        goto L_8041BB78;
    // 0x8041C030: sh          $a0, 0x1E8($v0)
    MEM_H(0X1E8, ctx->r2) = ctx->r4;
L_8041C034:
    // 0x8041C034: addiu       $v0, $zero, 0x2
    ctx->r2 = ADD32(0, 0X2);
    // 0x8041C038: b           L_8041BBB0
    // 0x8041C03C: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
        goto L_8041BBB0;
    // 0x8041C03C: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
L_8041C040:
    // 0x8041C040: addiu       $v0, $zero, 0x3
    ctx->r2 = ADD32(0, 0X3);
    // 0x8041C044: b           L_8041BBB0
    // 0x8041C048: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
        goto L_8041BBB0;
    // 0x8041C048: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
L_8041C04C:
    // 0x8041C04C: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
    // 0x8041C050: b           L_8041BBB0
    // 0x8041C054: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
        goto L_8041BBB0;
    // 0x8041C054: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
L_8041C058:
    // 0x8041C058: addiu       $v0, $zero, 0x5
    ctx->r2 = ADD32(0, 0X5);
    // 0x8041C05C: b           L_8041BBB0
    // 0x8041C060: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
        goto L_8041BBB0;
    // 0x8041C060: sh          $v0, 0x1FC($a1)
    MEM_H(0X1FC, ctx->r5) = ctx->r2;
L_8041C064:
    // 0x8041C064: addiu       $a1, $zero, 0x4
    ctx->r5 = ADD32(0, 0X4);
    // 0x8041C068: sh          $a1, 0x2C8($a0)
    MEM_H(0X2C8, ctx->r4) = ctx->r5;
L_8041C06C:
    // 0x8041C06C: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    // 0x8041C070: bne         $a2, $a0, L_8041BF38
    if (ctx->r6 != ctx->r4) {
        // 0x8041C074: lw          $a0, 0xC($v0)
        ctx->r4 = MEM_W(ctx->r2, 0XC);
            goto L_8041BF38;
    }
    // 0x8041C074: lw          $a0, 0xC($v0)
    ctx->r4 = MEM_W(ctx->r2, 0XC);
    // 0x8041C078: b           L_8041BD98
    // 0x8041C07C: addiu       $a1, $zero, 0xFA
    ctx->r5 = ADD32(0, 0XFA);
        goto L_8041BD98;
    // 0x8041C07C: addiu       $a1, $zero, 0xFA
    ctx->r5 = ADD32(0, 0XFA);
L_8041C080:
    // 0x8041C080: addiu       $a1, $zero, 0x5F
    ctx->r5 = ADD32(0, 0X5F);
    // 0x8041C084: b           L_8041BAA0
    // 0x8041C088: sh          $a1, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r5;
        goto L_8041BAA0;
    // 0x8041C088: sh          $a1, 0x96($v0)
    MEM_H(0X96, ctx->r2) = ctx->r5;
L_8041C08C:
    // 0x8041C08C: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041C090: lwc1        $f0, -0x3F1C($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F1C);
    // 0x8041C094: b           L_8041BC38
    // 0x8041C098: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
        goto L_8041BC38;
    // 0x8041C098: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
L_8041C09C:
    // 0x8041C09C: lwc1        $f0, -0x3F20($v0)
    ctx->f0.u32l = MEM_W(ctx->r2, -0X3F20);
    // 0x8041C0A0: b           L_8041BC38
    // 0x8041C0A4: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
        goto L_8041BC38;
    // 0x8041C0A4: swc1        $f0, 0x308($a0)
    MEM_W(0X308, ctx->r4) = ctx->f0.u32l;
;}
RECOMP_FUNC void hm_func_8041C0A8(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041C0A8: jr          $ra
    // 0x8041C0AC: nop

    return;
    // 0x8041C0AC: nop

;}
RECOMP_FUNC void hm_func_8041C0F8(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041C0F8: lui         $v0, 0x8042
    ctx->r2 = S32(0X8042 << 16);
    // 0x8041C0FC: lui         $a3, 0x8042
    ctx->r7 = S32(0X8042 << 16);
    // 0x8041C100: addiu       $v0, $v0, -0x3E90
    ctx->r2 = ADD32(ctx->r2, -0X3E90);
    // 0x8041C104: addiu       $t0, $zero, -0x1
    ctx->r8 = ADD32(0, -0X1);
    // 0x8041C108: b           L_8041C118
    // 0x8041C10C: addiu       $a3, $a3, -0x3CC2
    ctx->r7 = ADD32(ctx->r7, -0X3CC2);
        goto L_8041C118;
    // 0x8041C10C: addiu       $a3, $a3, -0x3CC2
    ctx->r7 = ADD32(ctx->r7, -0X3CC2);
L_8041C110:
    // 0x8041C110: beq         $v0, $a3, L_8041C168
    if (ctx->r2 == ctx->r7) {
        // 0x8041C114: nop
    
            goto L_8041C168;
    }
    // 0x8041C114: nop

L_8041C118:
    // 0x8041C118: lh          $v1, 0x0($v0)
    ctx->r3 = MEM_H(ctx->r2, 0X0);
    // 0x8041C11C: bnel        $v1, $a0, L_8041C110
    if (ctx->r3 != ctx->r4) {
        // 0x8041C120: addiu       $v0, $v0, 0x6
        ctx->r2 = ADD32(ctx->r2, 0X6);
            goto L_8041C110;
    }
    goto skip_0;
    // 0x8041C120: addiu       $v0, $v0, 0x6
    ctx->r2 = ADD32(ctx->r2, 0X6);
    skip_0:
    // 0x8041C124: lh          $v1, 0x2($v0)
    ctx->r3 = MEM_H(ctx->r2, 0X2);
    // 0x8041C128: beql        $v1, $a1, L_8041C13C
    if (ctx->r3 == ctx->r5) {
        // 0x8041C12C: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041C13C;
    }
    goto skip_1;
    // 0x8041C12C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_1:
    // 0x8041C130: bnel        $v1, $t0, L_8041C110
    if (ctx->r3 != ctx->r8) {
        // 0x8041C134: addiu       $v0, $v0, 0x6
        ctx->r2 = ADD32(ctx->r2, 0X6);
            goto L_8041C110;
    }
    goto skip_2;
    // 0x8041C134: addiu       $v0, $v0, 0x6
    ctx->r2 = ADD32(ctx->r2, 0X6);
    skip_2:
    // 0x8041C138: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_8041C13C:
    // 0x8041C13C: lb          $v0, 0x4($v0)
    ctx->r2 = MEM_B(ctx->r2, 0X4);
    // 0x8041C140: lb          $a0, -0x33F($v1)
    ctx->r4 = MEM_B(ctx->r3, -0X33F);
    // 0x8041C144: beq         $a0, $v0, L_8041C168
    if (ctx->r4 == ctx->r2) {
        // 0x8041C148: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041C168;
    }
    // 0x8041C148: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x8041C14C: lhu         $a0, -0x33E($a1)
    ctx->r4 = MEM_HU(ctx->r5, -0X33E);
    // 0x8041C150: sb          $v0, -0x33F($v1)
    MEM_B(-0X33F, ctx->r3) = ctx->r2;
    // 0x8041C154: ori         $a0, $a0, 0x1
    ctx->r4 = ctx->r4 | 0X1;
    // 0x8041C158: lui         $v0, 0x8009
    ctx->r2 = S32(0X8009 << 16);
    // 0x8041C15C: sh          $a0, -0x33E($a1)
    MEM_H(-0X33E, ctx->r5) = ctx->r4;
    // 0x8041C160: jr          $ra
    // 0x8041C164: sh          $a2, -0x33C($v0)
    MEM_H(-0X33C, ctx->r2) = ctx->r6;
    return;
    // 0x8041C164: sh          $a2, -0x33C($v0)
    MEM_H(-0X33C, ctx->r2) = ctx->r6;
L_8041C168:
    // 0x8041C168: jr          $ra
    // 0x8041C16C: nop

    return;
    // 0x8041C16C: nop

;}
RECOMP_FUNC void hm_func_8041C340(uint8_t* rdram, recomp_context* ctx) {
    uint64_t hi = 0, lo = 0, result = 0;
    int c1cs = 0;
    // 0x8041C340: addiu       $sp, $sp, -0x50
    ctx->r29 = ADD32(ctx->r29, -0X50);
    // 0x8041C344: lui         $a2, 0x8006
    ctx->r6 = S32(0X8006 << 16);
    // 0x8041C348: sw          $s2, 0x30($sp)
    MEM_W(0X30, ctx->r29) = ctx->r18;
    // 0x8041C34C: lui         $s2, 0x8006
    ctx->r18 = S32(0X8006 << 16);
    // 0x8041C350: lbu         $a3, -0x600($s2)
    ctx->r7 = MEM_BU(ctx->r18, -0X600);
    // 0x8041C354: addiu       $v1, $a2, -0x710
    ctx->r3 = ADD32(ctx->r6, -0X710);
    // 0x8041C358: lbu         $t0, 0x1($v1)
    ctx->r8 = MEM_BU(ctx->r3, 0X1);
    // 0x8041C35C: lbu         $t1, -0x710($a2)
    ctx->r9 = MEM_BU(ctx->r6, -0X710);
    // 0x8041C360: lbu         $v1, 0x2($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X2);
    // 0x8041C364: sw          $s5, 0x3C($sp)
    MEM_W(0X3C, ctx->r29) = ctx->r21;
    // 0x8041C368: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
    // 0x8041C36C: addiu       $s5, $zero, 0x6C
    ctx->r21 = ADD32(0, 0X6C);
    // 0x8041C370: subu        $v0, $v0, $a3
    ctx->r2 = SUB32(ctx->r2, ctx->r7);
    // 0x8041C374: addiu       $a1, $a0, 0x28
    ctx->r5 = ADD32(ctx->r4, 0X28);
    // 0x8041C378: sw          $s7, 0x44($sp)
    MEM_W(0X44, ctx->r29) = ctx->r23;
    // 0x8041C37C: sw          $s0, 0x28($sp)
    MEM_W(0X28, ctx->r29) = ctx->r16;
    // 0x8041C380: addiu       $s7, $zero, 0x3
    ctx->r23 = ADD32(0, 0X3);
    // 0x8041C384: or          $s0, $a0, $zero
    ctx->r16 = ctx->r4 | 0;
    // 0x8041C388: sw          $s5, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r21;
    // 0x8041C38C: addiu       $a3, $zero, 0xF0
    ctx->r7 = ADD32(0, 0XF0);
    // 0x8041C390: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x8041C394: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041C398: sw          $ra, 0x4C($sp)
    MEM_W(0X4C, ctx->r29) = ctx->r31;
    // 0x8041C39C: sb          $t1, 0x18($sp)
    MEM_B(0X18, ctx->r29) = ctx->r9;
    // 0x8041C3A0: sb          $t0, 0x19($sp)
    MEM_B(0X19, ctx->r29) = ctx->r8;
    // 0x8041C3A4: sb          $v1, 0x1A($sp)
    MEM_B(0X1A, ctx->r29) = ctx->r3;
    // 0x8041C3A8: sb          $v0, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = ctx->r2;
    // 0x8041C3AC: sw          $fp, 0x48($sp)
    MEM_W(0X48, ctx->r29) = ctx->r30;
    // 0x8041C3B0: sw          $s6, 0x40($sp)
    MEM_W(0X40, ctx->r29) = ctx->r22;
    // 0x8041C3B4: sw          $s4, 0x38($sp)
    MEM_W(0X38, ctx->r29) = ctx->r20;
    // 0x8041C3B8: addiu       $s6, $zero, 0x68
    ctx->r22 = ADD32(0, 0X68);
    // 0x8041C3BC: sw          $s3, 0x34($sp)
    MEM_W(0X34, ctx->r29) = ctx->r19;
    // 0x8041C3C0: jal         0x80029B58
    // 0x8041C3C4: sw          $s1, 0x2C($sp)
    MEM_W(0X2C, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_0;
    // 0x8041C3C4: sw          $s1, 0x2C($sp)
    MEM_W(0X2C, ctx->r29) = ctx->r17;
    after_0:
    // 0x8041C3C8: sw          $s7, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r23;
    // 0x8041C3CC: addiu       $a3, $zero, 0xF0
    ctx->r7 = ADD32(0, 0XF0);
    // 0x8041C3D0: addiu       $a2, $zero, 0x8B
    ctx->r6 = ADD32(0, 0X8B);
    // 0x8041C3D4: addiu       $a1, $s0, 0x2B
    ctx->r5 = ADD32(ctx->r16, 0X2B);
    // 0x8041C3D8: jal         0x80029B58
    // 0x8041C3DC: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_1;
    // 0x8041C3DC: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_1:
    // 0x8041C3E0: addiu       $s1, $zero, 0x1
    ctx->r17 = ADD32(0, 0X1);
    // 0x8041C3E4: sw          $s6, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r22;
    // 0x8041C3E8: addiu       $a3, $zero, 0x2
    ctx->r7 = ADD32(0, 0X2);
    // 0x8041C3EC: addiu       $a2, $zero, 0x24
    ctx->r6 = ADD32(0, 0X24);
    // 0x8041C3F0: addiu       $a1, $s0, 0x119
    ctx->r5 = ADD32(ctx->r16, 0X119);
    // 0x8041C3F4: jal         0x80029B58
    // 0x8041C3F8: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_2;
    // 0x8041C3F8: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_2:
    // 0x8041C3FC: addiu       $a1, $s0, 0x27
    ctx->r5 = ADD32(ctx->r16, 0X27);
    // 0x8041C400: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C404: addiu       $a3, $zero, 0xF2
    ctx->r7 = ADD32(0, 0XF2);
    // 0x8041C408: addiu       $a2, $zero, 0x1D
    ctx->r6 = ADD32(0, 0X1D);
    // 0x8041C40C: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    // 0x8041C410: jal         0x80029B58
    // 0x8041C414: sw          $a1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r5;
    func_80029B58(rdram, ctx);
        goto after_3;
    // 0x8041C414: sw          $a1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r5;
    after_3:
    // 0x8041C418: lw          $a1, 0x20($sp)
    ctx->r5 = MEM_W(ctx->r29, 0X20);
    // 0x8041C41C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C420: addiu       $a3, $zero, 0xF2
    ctx->r7 = ADD32(0, 0XF2);
    // 0x8041C424: addiu       $a2, $zero, 0x8A
    ctx->r6 = ADD32(0, 0X8A);
    // 0x8041C428: jal         0x80029B58
    // 0x8041C42C: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_4;
    // 0x8041C42C: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_4:
    // 0x8041C430: addiu       $fp, $zero, 0x6D
    ctx->r30 = ADD32(0, 0X6D);
    // 0x8041C434: lw          $a1, 0x20($sp)
    ctx->r5 = MEM_W(ctx->r29, 0X20);
    // 0x8041C438: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    // 0x8041C43C: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041C440: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x8041C444: jal         0x80029B58
    // 0x8041C448: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_5;
    // 0x8041C448: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_5:
    // 0x8041C44C: addiu       $s3, $s0, 0x29
    ctx->r19 = ADD32(ctx->r16, 0X29);
    // 0x8041C450: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    // 0x8041C454: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041C458: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x8041C45C: addiu       $a1, $s0, 0x118
    ctx->r5 = ADD32(ctx->r16, 0X118);
    // 0x8041C460: jal         0x80029B58
    // 0x8041C464: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    func_80029B58(rdram, ctx);
        goto after_6;
    // 0x8041C464: addiu       $a0, $zero, 0x37
    ctx->r4 = ADD32(0, 0X37);
    after_6:
    // 0x8041C468: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x8041C46C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C470: addiu       $a3, $zero, 0xED
    ctx->r7 = ADD32(0, 0XED);
    // 0x8041C474: addiu       $a2, $zero, 0x1F
    ctx->r6 = ADD32(0, 0X1F);
    // 0x8041C478: jal         0x80029B58
    // 0x8041C47C: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    func_80029B58(rdram, ctx);
        goto after_7;
    // 0x8041C47C: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    after_7:
    // 0x8041C480: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x8041C484: sw          $s6, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r22;
    // 0x8041C488: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041C48C: addiu       $a2, $zero, 0x20
    ctx->r6 = ADD32(0, 0X20);
    // 0x8041C490: jal         0x80029B58
    // 0x8041C494: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    func_80029B58(rdram, ctx);
        goto after_8;
    // 0x8041C494: addiu       $a0, $zero, 0x38
    ctx->r4 = ADD32(0, 0X38);
    after_8:
    // 0x8041C498: or          $a1, $s3, $zero
    ctx->r5 = ctx->r19 | 0;
    // 0x8041C49C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C4A0: addiu       $a3, $zero, 0xEE
    ctx->r7 = ADD32(0, 0XEE);
    // 0x8041C4A4: addiu       $a2, $zero, 0x88
    ctx->r6 = ADD32(0, 0X88);
    // 0x8041C4A8: jal         0x80029B58
    // 0x8041C4AC: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_9;
    // 0x8041C4AC: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    after_9:
    // 0x8041C4B0: addiu       $v0, $zero, 0x69
    ctx->r2 = ADD32(0, 0X69);
    // 0x8041C4B4: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041C4B8: addiu       $a3, $zero, 0x1
    ctx->r7 = ADD32(0, 0X1);
    // 0x8041C4BC: addiu       $a2, $zero, 0x1F
    ctx->r6 = ADD32(0, 0X1F);
    // 0x8041C4C0: addiu       $a1, $s0, 0x116
    ctx->r5 = ADD32(ctx->r16, 0X116);
    // 0x8041C4C4: jal         0x80029B58
    // 0x8041C4C8: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_10;
    // 0x8041C4C8: addiu       $a0, $zero, 0x39
    ctx->r4 = ADD32(0, 0X39);
    after_10:
    // 0x8041C4CC: sw          $s5, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r21;
    // 0x8041C4D0: addiu       $a3, $zero, 0x15
    ctx->r7 = ADD32(0, 0X15);
    // 0x8041C4D4: addiu       $a2, $zero, 0x1E
    ctx->r6 = ADD32(0, 0X1E);
    // 0x8041C4D8: addiu       $a1, $s0, 0x3A
    ctx->r5 = ADD32(ctx->r16, 0X3A);
    // 0x8041C4DC: jal         0x80029B58
    // 0x8041C4E0: addiu       $a0, $zero, 0x3C
    ctx->r4 = ADD32(0, 0X3C);
    func_80029B58(rdram, ctx);
        goto after_11;
    // 0x8041C4E0: addiu       $a0, $zero, 0x3C
    ctx->r4 = ADD32(0, 0X3C);
    after_11:
    // 0x8041C4E4: addiu       $v0, $zero, 0x2
    ctx->r2 = ADD32(0, 0X2);
    // 0x8041C4E8: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041C4EC: addiu       $s3, $zero, 0x7
    ctx->r19 = ADD32(0, 0X7);
    // 0x8041C4F0: addiu       $a3, $zero, 0xA7
    ctx->r7 = ADD32(0, 0XA7);
    // 0x8041C4F4: addiu       $a2, $zero, 0x56
    ctx->r6 = ADD32(0, 0X56);
    // 0x8041C4F8: addiu       $a1, $s0, 0x5D
    ctx->r5 = ADD32(ctx->r16, 0X5D);
    // 0x8041C4FC: jal         0x80029B58
    // 0x8041C500: addiu       $a0, $zero, 0x3D
    ctx->r4 = ADD32(0, 0X3D);
    func_80029B58(rdram, ctx);
        goto after_12;
    // 0x8041C500: addiu       $a0, $zero, 0x3D
    ctx->r4 = ADD32(0, 0X3D);
    after_12:
    // 0x8041C504: addiu       $a1, $s0, 0x8D
    ctx->r5 = ADD32(ctx->r16, 0X8D);
    // 0x8041C508: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x8041C50C: addiu       $a3, $zero, 0x54
    ctx->r7 = ADD32(0, 0X54);
    // 0x8041C510: addiu       $a2, $zero, 0x3E
    ctx->r6 = ADD32(0, 0X3E);
    // 0x8041C514: addiu       $a0, $zero, 0x24
    ctx->r4 = ADD32(0, 0X24);
    // 0x8041C518: jal         0x80029B58
    // 0x8041C51C: sw          $a1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r5;
    func_80029B58(rdram, ctx);
        goto after_13;
    // 0x8041C51C: sw          $a1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r5;
    after_13:
    // 0x8041C520: lw          $a1, 0x20($sp)
    ctx->r5 = MEM_W(ctx->r29, 0X20);
    // 0x8041C524: addiu       $fp, $s0, 0xEA
    ctx->r30 = ADD32(ctx->r16, 0XEA);
    // 0x8041C528: addiu       $s6, $zero, 0x8
    ctx->r22 = ADD32(0, 0X8);
    // 0x8041C52C: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x8041C530: addiu       $a3, $zero, 0x54
    ctx->r7 = ADD32(0, 0X54);
    // 0x8041C534: addiu       $a2, $zero, 0x6C
    ctx->r6 = ADD32(0, 0X6C);
    // 0x8041C538: jal         0x80029B58
    // 0x8041C53C: addiu       $a0, $zero, 0x24
    ctx->r4 = ADD32(0, 0X24);
    func_80029B58(rdram, ctx);
        goto after_14;
    // 0x8041C53C: addiu       $a0, $zero, 0x24
    ctx->r4 = ADD32(0, 0X24);
    after_14:
    // 0x8041C540: addiu       $s1, $zero, 0xB
    ctx->r17 = ADD32(0, 0XB);
    // 0x8041C544: or          $a1, $fp, $zero
    ctx->r5 = ctx->r30 | 0;
    // 0x8041C548: addiu       $s5, $s0, 0x6C
    ctx->r21 = ADD32(ctx->r16, 0X6C);
    // 0x8041C54C: sw          $s6, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r22;
    // 0x8041C550: addiu       $a3, $zero, 0x18
    ctx->r7 = ADD32(0, 0X18);
    // 0x8041C554: addiu       $a2, $zero, 0x3A
    ctx->r6 = ADD32(0, 0X3A);
    // 0x8041C558: jal         0x80029B58
    // 0x8041C55C: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    func_80029B58(rdram, ctx);
        goto after_15;
    // 0x8041C55C: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    after_15:
    // 0x8041C560: or          $a1, $s5, $zero
    ctx->r5 = ctx->r21 | 0;
    // 0x8041C564: addiu       $a3, $zero, 0x14
    ctx->r7 = ADD32(0, 0X14);
    // 0x8041C568: addiu       $a2, $zero, 0x3D
    ctx->r6 = ADD32(0, 0X3D);
    // 0x8041C56C: addiu       $a0, $zero, 0x2A
    ctx->r4 = ADD32(0, 0X2A);
    // 0x8041C570: jal         0x80029B58
    // 0x8041C574: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_16;
    // 0x8041C574: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    after_16:
    // 0x8041C578: or          $a1, $fp, $zero
    ctx->r5 = ctx->r30 | 0;
    // 0x8041C57C: sw          $s6, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r22;
    // 0x8041C580: addiu       $a3, $zero, 0x18
    ctx->r7 = ADD32(0, 0X18);
    // 0x8041C584: addiu       $a2, $zero, 0x6B
    ctx->r6 = ADD32(0, 0X6B);
    // 0x8041C588: jal         0x80029B58
    // 0x8041C58C: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    func_80029B58(rdram, ctx);
        goto after_17;
    // 0x8041C58C: addiu       $a0, $zero, 0x29
    ctx->r4 = ADD32(0, 0X29);
    after_17:
    // 0x8041C590: or          $a1, $s5, $zero
    ctx->r5 = ctx->r21 | 0;
    // 0x8041C594: addiu       $a3, $zero, 0x14
    ctx->r7 = ADD32(0, 0X14);
    // 0x8041C598: addiu       $a2, $zero, 0x68
    ctx->r6 = ADD32(0, 0X68);
    // 0x8041C59C: addiu       $a0, $zero, 0x2A
    ctx->r4 = ADD32(0, 0X2A);
    // 0x8041C5A0: jal         0x80029B58
    // 0x8041C5A4: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_18;
    // 0x8041C5A4: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    after_18:
    // 0x8041C5A8: addiu       $v0, $zero, 0xC
    ctx->r2 = ADD32(0, 0XC);
    // 0x8041C5AC: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041C5B0: addiu       $a3, $zero, 0x34
    ctx->r7 = ADD32(0, 0X34);
    // 0x8041C5B4: addiu       $a2, $zero, 0x3B
    ctx->r6 = ADD32(0, 0X3B);
    // 0x8041C5B8: addiu       $a1, $s0, 0x31
    ctx->r5 = ADD32(ctx->r16, 0X31);
    // 0x8041C5BC: addiu       $a0, $zero, 0x22
    ctx->r4 = ADD32(0, 0X22);
    // 0x8041C5C0: jal         0x80029B58
    // 0x8041C5C4: addiu       $fp, $zero, 0x10
    ctx->r30 = ADD32(0, 0X10);
    func_80029B58(rdram, ctx);
        goto after_19;
    // 0x8041C5C4: addiu       $fp, $zero, 0x10
    ctx->r30 = ADD32(0, 0X10);
    after_19:
    // 0x8041C5C8: addiu       $a3, $zero, 0x2C
    ctx->r7 = ADD32(0, 0X2C);
    // 0x8041C5CC: addiu       $a2, $zero, 0x65
    ctx->r6 = ADD32(0, 0X65);
    // 0x8041C5D0: addiu       $a1, $s0, 0x35
    ctx->r5 = ADD32(ctx->r16, 0X35);
    // 0x8041C5D4: addiu       $a0, $zero, 0x23
    ctx->r4 = ADD32(0, 0X23);
    // 0x8041C5D8: jal         0x80029B58
    // 0x8041C5DC: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    func_80029B58(rdram, ctx);
        goto after_20;
    // 0x8041C5DC: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    after_20:
    // 0x8041C5E0: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x8041C5E4: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
    // 0x8041C5E8: addiu       $a2, $zero, 0x1C
    ctx->r6 = ADD32(0, 0X1C);
    // 0x8041C5EC: addiu       $a1, $s0, 0x45
    ctx->r5 = ADD32(ctx->r16, 0X45);
    // 0x8041C5F0: jal         0x80029B58
    // 0x8041C5F4: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    func_80029B58(rdram, ctx);
        goto after_21;
    // 0x8041C5F4: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    after_21:
    // 0x8041C5F8: sw          $s3, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r19;
    // 0x8041C5FC: addiu       $a3, $zero, 0x7
    ctx->r7 = ADD32(0, 0X7);
    // 0x8041C600: addiu       $a2, $zero, 0x1C
    ctx->r6 = ADD32(0, 0X1C);
    // 0x8041C604: addiu       $a1, $s0, 0xF3
    ctx->r5 = ADD32(ctx->r16, 0XF3);
    // 0x8041C608: addiu       $a0, $zero, 0x5
    ctx->r4 = ADD32(0, 0X5);
    // 0x8041C60C: jal         0x80029B58
    // 0x8041C610: lui         $s3, 0x8009
    ctx->r19 = S32(0X8009 << 16);
    func_80029B58(rdram, ctx);
        goto after_22;
    // 0x8041C610: lui         $s3, 0x8009
    ctx->r19 = S32(0X8009 << 16);
    after_22:
    // 0x8041C614: lw          $v1, -0x2F0($s3)
    ctx->r3 = MEM_W(ctx->r19, -0X2F0);
    // 0x8041C618: lui         $s5, 0x8006
    ctx->r21 = S32(0X8006 << 16);
    // 0x8041C61C: sra         $v0, $v1, 31
    ctx->r2 = S32(SIGNED(ctx->r3) >> 31);
    // 0x8041C620: andi        $v0, $v0, 0x3
    ctx->r2 = ctx->r2 & 0X3;
    // 0x8041C624: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x8041C628: sra         $v1, $v0, 31
    ctx->r3 = S32(SIGNED(ctx->r2) >> 31);
    // 0x8041C62C: srl         $v1, $v1, 28
    ctx->r3 = S32(U32(ctx->r3) >> 28);
    // 0x8041C630: sra         $v0, $v0, 2
    ctx->r2 = S32(SIGNED(ctx->r2) >> 2);
    // 0x8041C634: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x8041C638: andi        $v0, $v0, 0xF
    ctx->r2 = ctx->r2 & 0XF;
    // 0x8041C63C: subu        $v0, $v0, $v1
    ctx->r2 = SUB32(ctx->r2, ctx->r3);
    // 0x8041C640: addiu       $s5, $s5, -0xF40
    ctx->r21 = ADD32(ctx->r21, -0XF40);
    // 0x8041C644: addu        $v0, $s5, $v0
    ctx->r2 = ADD32(ctx->r21, ctx->r2);
    // 0x8041C648: lb          $s6, 0x0($v0)
    ctx->r22 = MEM_B(ctx->r2, 0X0);
    // 0x8041C64C: addiu       $a3, $zero, 0xE
    ctx->r7 = ADD32(0, 0XE);
    // 0x8041C650: addu        $a1, $s0, $s6
    ctx->r5 = ADD32(ctx->r16, ctx->r22);
    // 0x8041C654: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x8041C658: addiu       $a1, $a1, 0x36
    ctx->r5 = ADD32(ctx->r5, 0X36);
    // 0x8041C65C: addiu       $a0, $zero, 0x6
    ctx->r4 = ADD32(0, 0X6);
    // 0x8041C660: jal         0x80029B58
    // 0x8041C664: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_23;
    // 0x8041C664: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    after_23:
    // 0x8041C668: subu        $a1, $s0, $s6
    ctx->r5 = SUB32(ctx->r16, ctx->r22);
    // 0x8041C66C: addiu       $a3, $zero, 0xE
    ctx->r7 = ADD32(0, 0XE);
    // 0x8041C670: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x8041C674: addiu       $a1, $a1, 0xFB
    ctx->r5 = ADD32(ctx->r5, 0XFB);
    // 0x8041C678: addiu       $a0, $zero, 0x7
    ctx->r4 = ADD32(0, 0X7);
    // 0x8041C67C: jal         0x80029B58
    // 0x8041C680: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_24;
    // 0x8041C680: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    after_24:
    // 0x8041C684: addiu       $s4, $s0, 0x8E
    ctx->r20 = ADD32(ctx->r16, 0X8E);
    // 0x8041C688: addiu       $a3, $zero, 0x50
    ctx->r7 = ADD32(0, 0X50);
    // 0x8041C68C: addiu       $a2, $zero, 0x1A
    ctx->r6 = ADD32(0, 0X1A);
    // 0x8041C690: addiu       $a1, $s0, 0x77
    ctx->r5 = ADD32(ctx->r16, 0X77);
    // 0x8041C694: addiu       $a0, $zero, 0x21
    ctx->r4 = ADD32(0, 0X21);
    // 0x8041C698: jal         0x80029B58
    // 0x8041C69C: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    func_80029B58(rdram, ctx);
        goto after_25;
    // 0x8041C69C: sw          $fp, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r30;
    after_25:
    // 0x8041C6A0: or          $a1, $s4, $zero
    ctx->r5 = ctx->r20 | 0;
    // 0x8041C6A4: sw          $s7, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r23;
    // 0x8041C6A8: addiu       $a3, $zero, 0x50
    ctx->r7 = ADD32(0, 0X50);
    // 0x8041C6AC: addiu       $a2, $zero, 0x3F
    ctx->r6 = ADD32(0, 0X3F);
    // 0x8041C6B0: jal         0x80029B58
    // 0x8041C6B4: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    func_80029B58(rdram, ctx);
        goto after_26;
    // 0x8041C6B4: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    after_26:
    // 0x8041C6B8: addiu       $s6, $s0, 0xB3
    ctx->r22 = ADD32(ctx->r16, 0XB3);
    // 0x8041C6BC: or          $a1, $s4, $zero
    ctx->r5 = ctx->r20 | 0;
    // 0x8041C6C0: sw          $s7, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r23;
    // 0x8041C6C4: addiu       $a3, $zero, 0x50
    ctx->r7 = ADD32(0, 0X50);
    // 0x8041C6C8: addiu       $a2, $zero, 0x6D
    ctx->r6 = ADD32(0, 0X6D);
    // 0x8041C6CC: jal         0x80029B58
    // 0x8041C6D0: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    func_80029B58(rdram, ctx);
        goto after_27;
    // 0x8041C6D0: addiu       $a0, $zero, 0x25
    ctx->r4 = ADD32(0, 0X25);
    after_27:
    // 0x8041C6D4: or          $a1, $s6, $zero
    ctx->r5 = ctx->r22 | 0;
    // 0x8041C6D8: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041C6DC: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    // 0x8041C6E0: addiu       $a0, $zero, 0x27
    ctx->r4 = ADD32(0, 0X27);
    // 0x8041C6E4: jal         0x80029B58
    // 0x8041C6E8: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    func_80029B58(rdram, ctx);
        goto after_28;
    // 0x8041C6E8: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    after_28:
    // 0x8041C6EC: lui         $s7, 0x8009
    ctx->r23 = S32(0X8009 << 16);
    // 0x8041C6F0: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C6F4: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041C6F8: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    // 0x8041C6FC: or          $a1, $s6, $zero
    ctx->r5 = ctx->r22 | 0;
    // 0x8041C700: jal         0x80029B58
    // 0x8041C704: addiu       $a0, $zero, 0x27
    ctx->r4 = ADD32(0, 0X27);
    func_80029B58(rdram, ctx);
        goto after_29;
    // 0x8041C704: addiu       $a0, $zero, 0x27
    ctx->r4 = ADD32(0, 0X27);
    after_29:
    // 0x8041C708: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041C70C: andi        $v1, $v0, 0x200
    ctx->r3 = ctx->r2 & 0X200;
    // 0x8041C710: beq         $v1, $zero, L_8041C88C
    if (ctx->r3 == 0) {
        // 0x8041C714: lui         $v1, 0x8006
        ctx->r3 = S32(0X8006 << 16);
            goto L_8041C88C;
    }
    // 0x8041C714: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x8041C718: andi        $v0, $v0, 0x100
    ctx->r2 = ctx->r2 & 0X100;
    // 0x8041C71C: beq         $v0, $zero, L_8041C800
    if (ctx->r2 == 0) {
        // 0x8041C720: lb          $v0, -0x600($s2)
        ctx->r2 = MEM_B(ctx->r18, -0X600);
            goto L_8041C800;
    }
    // 0x8041C720: lb          $v0, -0x600($s2)
    ctx->r2 = MEM_B(ctx->r18, -0X600);
    // 0x8041C724: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x8041C728: addiu       $v1, $v1, -0x70C
    ctx->r3 = ADD32(ctx->r3, -0X70C);
    // 0x8041C72C: sll         $v0, $v0, 2
    ctx->r2 = S32(ctx->r2 << 2);
    // 0x8041C730: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x8041C734: lh          $v1, 0x2($v0)
    ctx->r3 = MEM_H(ctx->r2, 0X2);
    // 0x8041C738: addiu       $s6, $sp, 0x18
    ctx->r22 = ADD32(ctx->r29, 0X18);
L_8041C73C:
    // 0x8041C73C: lh          $a1, 0x0($v0)
    ctx->r5 = MEM_H(ctx->r2, 0X0);
    // 0x8041C740: addu        $v0, $s6, $v1
    ctx->r2 = ADD32(ctx->r22, ctx->r3);
    // 0x8041C744: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x8041C748: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041C74C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C750: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    // 0x8041C754: jal         0x80029B58
    // 0x8041C758: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    func_80029B58(rdram, ctx);
        goto after_30;
    // 0x8041C758: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    after_30:
    // 0x8041C75C: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041C760: andi        $v1, $v0, 0x2000
    ctx->r3 = ctx->r2 & 0X2000;
    // 0x8041C764: bne         $v1, $zero, L_8041C850
    if (ctx->r3 != 0) {
        // 0x8041C768: lbu         $v1, -0x600($s2)
        ctx->r3 = MEM_BU(ctx->r18, -0X600);
            goto L_8041C850;
    }
    // 0x8041C768: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
    // 0x8041C76C: andi        $v1, $v0, 0x800
    ctx->r3 = ctx->r2 & 0X800;
L_8041C770:
    // 0x8041C770: beq         $v1, $zero, L_8041C8E4
    if (ctx->r3 == 0) {
        // 0x8041C774: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041C8E4;
    }
    // 0x8041C774: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041C778: lh          $v1, 0x2876($v1)
    ctx->r3 = MEM_H(ctx->r3, 0X2876);
    // 0x8041C77C: bltz        $v1, L_8041C9B4
    if (SIGNED(ctx->r3) < 0) {
        // 0x8041C780: andi        $a2, $v0, 0x200
        ctx->r6 = ctx->r2 & 0X200;
            goto L_8041C9B4;
    }
    // 0x8041C780: andi        $a2, $v0, 0x200
    ctx->r6 = ctx->r2 & 0X200;
    // 0x8041C784: lw          $a0, -0x2F0($s3)
    ctx->r4 = MEM_W(ctx->r19, -0X2F0);
    // 0x8041C788: lui         $s1, 0x8009
    ctx->r17 = S32(0X8009 << 16);
    // 0x8041C78C: srl         $v1, $a0, 31
    ctx->r3 = S32(U32(ctx->r4) >> 31);
    // 0x8041C790: addu        $v1, $v1, $a0
    ctx->r3 = ADD32(ctx->r3, ctx->r4);
    // 0x8041C794: sra         $a0, $v1, 31
    ctx->r4 = S32(SIGNED(ctx->r3) >> 31);
    // 0x8041C798: srl         $a0, $a0, 28
    ctx->r4 = S32(U32(ctx->r4) >> 28);
    // 0x8041C79C: sra         $v1, $v1, 1
    ctx->r3 = S32(SIGNED(ctx->r3) >> 1);
    // 0x8041C7A0: addu        $v1, $v1, $a0
    ctx->r3 = ADD32(ctx->r3, ctx->r4);
    // 0x8041C7A4: andi        $v1, $v1, 0xF
    ctx->r3 = ctx->r3 & 0XF;
    // 0x8041C7A8: subu        $v1, $v1, $a0
    ctx->r3 = SUB32(ctx->r3, ctx->r4);
    // 0x8041C7AC: addu        $s5, $s5, $v1
    ctx->r21 = ADD32(ctx->r21, ctx->r3);
    // 0x8041C7B0: lb          $s3, 0x0($s5)
    ctx->r19 = MEM_B(ctx->r21, 0X0);
    // 0x8041C7B4: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041C7B8: bne         $a2, $zero, L_8041C9F0
    if (ctx->r6 != 0) {
        // 0x8041C7BC: andi        $v1, $v0, 0x100
        ctx->r3 = ctx->r2 & 0X100;
            goto L_8041C9F0;
    }
    // 0x8041C7BC: andi        $v1, $v0, 0x100
    ctx->r3 = ctx->r2 & 0X100;
L_8041C7C0:
    // 0x8041C7C0: beq         $v1, $zero, L_8041CB40
    if (ctx->r3 == 0) {
        // 0x8041C7C4: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041CB40;
    }
    // 0x8041C7C4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_8041C7C8:
    // 0x8041C7C8: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_8041C7CC:
    // 0x8041C7CC: lbu         $v1, 0x2872($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X2872);
    // 0x8041C7D0: addiu       $v1, $v1, 0x1E
    ctx->r3 = ADD32(ctx->r3, 0X1E);
    // 0x8041C7D4: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041C7D8: sltiu       $v1, $v1, 0x3D
    ctx->r3 = ctx->r3 < 0X3D ? 1 : 0;
    // 0x8041C7DC: bne         $v1, $zero, L_8041CB8C
    if (ctx->r3 != 0) {
        // 0x8041C7E0: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041CB8C;
    }
    // 0x8041C7E0: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x8041C7E4: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041C7E8: beq         $v1, $zero, L_8041CC98
    if (ctx->r3 == 0) {
        // 0x8041C7EC: andi        $v0, $v0, 0xFEFF
        ctx->r2 = ctx->r2 & 0XFEFF;
            goto L_8041CC98;
    }
    // 0x8041C7EC: andi        $v0, $v0, 0xFEFF
    ctx->r2 = ctx->r2 & 0XFEFF;
    // 0x8041C7F0: addiu       $v0, $zero, 0x65
    ctx->r2 = ADD32(0, 0X65);
L_8041C7F4:
    // 0x8041C7F4: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
    // 0x8041C7F8: b           L_8041CB14
    // 0x8041C7FC: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
        goto L_8041CB14;
    // 0x8041C7FC: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
L_8041C800:
    // 0x8041C800: lb          $v0, -0xFF0($v1)
    ctx->r2 = MEM_B(ctx->r3, -0XFF0);
    // 0x8041C804: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041C808: sll         $v0, $v0, 2
    ctx->r2 = S32(ctx->r2 << 2);
    // 0x8041C80C: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
    // 0x8041C810: addu        $fp, $fp, $v0
    ctx->r30 = ADD32(ctx->r30, ctx->r2);
    // 0x8041C814: lh          $v0, 0x2($fp)
    ctx->r2 = MEM_H(ctx->r30, 0X2);
    // 0x8041C818: addiu       $s6, $sp, 0x18
    ctx->r22 = ADD32(ctx->r29, 0X18);
    // 0x8041C81C: lh          $a1, 0x0($fp)
    ctx->r5 = MEM_H(ctx->r30, 0X0);
    // 0x8041C820: addu        $v0, $s6, $v0
    ctx->r2 = ADD32(ctx->r22, ctx->r2);
    // 0x8041C824: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x8041C828: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041C82C: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C830: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    // 0x8041C834: jal         0x80029B58
    // 0x8041C838: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    func_80029B58(rdram, ctx);
        goto after_31;
    // 0x8041C838: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    after_31:
    // 0x8041C83C: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041C840: andi        $v1, $v0, 0x2000
    ctx->r3 = ctx->r2 & 0X2000;
    // 0x8041C844: beq         $v1, $zero, L_8041C770
    if (ctx->r3 == 0) {
        // 0x8041C848: andi        $v1, $v0, 0x800
        ctx->r3 = ctx->r2 & 0X800;
            goto L_8041C770;
    }
    // 0x8041C848: andi        $v1, $v0, 0x800
    ctx->r3 = ctx->r2 & 0X800;
    // 0x8041C84C: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
L_8041C850:
    // 0x8041C850: lw          $ra, 0x4C($sp)
    ctx->r31 = MEM_W(ctx->r29, 0X4C);
    // 0x8041C854: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
    // 0x8041C858: subu        $v0, $v0, $v1
    ctx->r2 = SUB32(ctx->r2, ctx->r3);
    // 0x8041C85C: lw          $fp, 0x48($sp)
    ctx->r30 = MEM_W(ctx->r29, 0X48);
    // 0x8041C860: lw          $s7, 0x44($sp)
    ctx->r23 = MEM_W(ctx->r29, 0X44);
    // 0x8041C864: lw          $s6, 0x40($sp)
    ctx->r22 = MEM_W(ctx->r29, 0X40);
    // 0x8041C868: lw          $s5, 0x3C($sp)
    ctx->r21 = MEM_W(ctx->r29, 0X3C);
    // 0x8041C86C: lw          $s4, 0x38($sp)
    ctx->r20 = MEM_W(ctx->r29, 0X38);
    // 0x8041C870: lw          $s3, 0x34($sp)
    ctx->r19 = MEM_W(ctx->r29, 0X34);
    // 0x8041C874: lw          $s1, 0x2C($sp)
    ctx->r17 = MEM_W(ctx->r29, 0X2C);
    // 0x8041C878: lw          $s0, 0x28($sp)
    ctx->r16 = MEM_W(ctx->r29, 0X28);
    // 0x8041C87C: sb          $v0, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = ctx->r2;
    // 0x8041C880: lw          $s2, 0x30($sp)
    ctx->r18 = MEM_W(ctx->r29, 0X30);
    // 0x8041C884: jr          $ra
    // 0x8041C888: addiu       $sp, $sp, 0x50
    ctx->r29 = ADD32(ctx->r29, 0X50);
    return;
    // 0x8041C888: addiu       $sp, $sp, 0x50
    ctx->r29 = ADD32(ctx->r29, 0X50);
L_8041C88C:
    // 0x8041C88C: lb          $v0, -0xFF0($v1)
    ctx->r2 = MEM_B(ctx->r3, -0XFF0);
    // 0x8041C890: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041C894: sll         $v0, $v0, 2
    ctx->r2 = S32(ctx->r2 << 2);
    // 0x8041C898: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
    // 0x8041C89C: addu        $fp, $fp, $v0
    ctx->r30 = ADD32(ctx->r30, ctx->r2);
    // 0x8041C8A0: lh          $v0, 0x2($fp)
    ctx->r2 = MEM_H(ctx->r30, 0X2);
    // 0x8041C8A4: addiu       $s6, $sp, 0x18
    ctx->r22 = ADD32(ctx->r29, 0X18);
    // 0x8041C8A8: lh          $a1, 0x0($fp)
    ctx->r5 = MEM_H(ctx->r30, 0X0);
    // 0x8041C8AC: addu        $v0, $s6, $v0
    ctx->r2 = ADD32(ctx->r22, ctx->r2);
    // 0x8041C8B0: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x8041C8B4: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041C8B8: sw          $s1, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r17;
    // 0x8041C8BC: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    // 0x8041C8C0: jal         0x80029B58
    // 0x8041C8C4: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    func_80029B58(rdram, ctx);
        goto after_32;
    // 0x8041C8C4: addu        $a1, $a1, $s0
    ctx->r5 = ADD32(ctx->r5, ctx->r16);
    after_32:
    // 0x8041C8C8: lb          $v0, -0x600($s2)
    ctx->r2 = MEM_B(ctx->r18, -0X600);
    // 0x8041C8CC: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x8041C8D0: addiu       $v1, $v1, -0x70C
    ctx->r3 = ADD32(ctx->r3, -0X70C);
    // 0x8041C8D4: sll         $v0, $v0, 2
    ctx->r2 = S32(ctx->r2 << 2);
    // 0x8041C8D8: addu        $v0, $v0, $v1
    ctx->r2 = ADD32(ctx->r2, ctx->r3);
    // 0x8041C8DC: b           L_8041C73C
    // 0x8041C8E0: lh          $v1, 0x2($v0)
    ctx->r3 = MEM_H(ctx->r2, 0X2);
        goto L_8041C73C;
    // 0x8041C8E0: lh          $v1, 0x2($v0)
    ctx->r3 = MEM_H(ctx->r2, 0X2);
L_8041C8E4:
    // 0x8041C8E4: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041C8E8: lb          $a0, 0x2871($v1)
    ctx->r4 = MEM_B(ctx->r3, 0X2871);
    // 0x8041C8EC: slti        $v1, $a0, 0x15
    ctx->r3 = SIGNED(ctx->r4) < 0X15 ? 1 : 0;
    // 0x8041C8F0: beq         $v1, $zero, L_8041C950
    if (ctx->r3 == 0) {
        // 0x8041C8F4: lui         $s0, 0x8009
        ctx->r16 = S32(0X8009 << 16);
            goto L_8041C950;
    }
    // 0x8041C8F4: lui         $s0, 0x8009
    ctx->r16 = S32(0X8009 << 16);
    // 0x8041C8F8: lhu         $v1, 0x2876($s0)
    ctx->r3 = MEM_HU(ctx->r16, 0X2876);
    // 0x8041C8FC: andi        $a1, $v1, 0x10
    ctx->r5 = ctx->r3 & 0X10;
    // 0x8041C900: bnel        $a1, $zero, L_8041C954
    if (ctx->r5 != 0) {
        // 0x8041C904: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041C954;
    }
    goto skip_0;
    // 0x8041C904: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_0:
    // 0x8041C908: slti        $a0, $a0, -0x14
    ctx->r4 = SIGNED(ctx->r4) < -0X14 ? 1 : 0;
    // 0x8041C90C: bne         $a0, $zero, L_8041C91C
    if (ctx->r4 != 0) {
        // 0x8041C910: andi        $a0, $v1, 0x2000
        ctx->r4 = ctx->r3 & 0X2000;
            goto L_8041C91C;
    }
    // 0x8041C910: andi        $a0, $v1, 0x2000
    ctx->r4 = ctx->r3 & 0X2000;
    // 0x8041C914: beql        $a0, $zero, L_8041C984
    if (ctx->r4 == 0) {
        // 0x8041C918: sll         $v1, $v1, 16
        ctx->r3 = S32(ctx->r3 << 16);
            goto L_8041C984;
    }
    goto skip_1;
    // 0x8041C918: sll         $v1, $v1, 16
    ctx->r3 = S32(ctx->r3 << 16);
    skip_1:
L_8041C91C:
    // 0x8041C91C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041C920: ori         $v0, $v0, 0x3000
    ctx->r2 = ctx->r2 | 0X3000;
    // 0x8041C924: sw          $zero, -0x2FC($v1)
    MEM_W(-0X2FC, ctx->r3) = 0;
    // 0x8041C928: addiu       $a3, $zero, -0x140
    ctx->r7 = ADD32(0, -0X140);
    // 0x8041C92C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041C930: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041C934: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x8041C938: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041C93C: sw          $a3, -0x2F8($v1)
    MEM_W(-0X2F8, ctx->r3) = ctx->r7;
    // 0x8041C940: jal         0x800268D4
    // 0x8041C944: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_33;
    // 0x8041C944: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_33:
    // 0x8041C948: b           L_8041C980
    // 0x8041C94C: lhu         $v1, 0x2876($s0)
    ctx->r3 = MEM_HU(ctx->r16, 0X2876);
        goto L_8041C980;
    // 0x8041C94C: lhu         $v1, 0x2876($s0)
    ctx->r3 = MEM_HU(ctx->r16, 0X2876);
L_8041C950:
    // 0x8041C950: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_8041C954:
    // 0x8041C954: addiu       $a3, $zero, 0x140
    ctx->r7 = ADD32(0, 0X140);
    // 0x8041C958: ori         $v0, $v0, 0x2000
    ctx->r2 = ctx->r2 | 0X2000;
    // 0x8041C95C: sw          $a3, -0x2FC($v1)
    MEM_W(-0X2FC, ctx->r3) = ctx->r7;
    // 0x8041C960: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041C964: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041C968: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x8041C96C: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041C970: sw          $zero, -0x2F8($v1)
    MEM_W(-0X2F8, ctx->r3) = 0;
    // 0x8041C974: jal         0x800268D4
    // 0x8041C978: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_34;
    // 0x8041C978: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_34:
    // 0x8041C97C: lhu         $v1, 0x2876($s0)
    ctx->r3 = MEM_HU(ctx->r16, 0X2876);
L_8041C980:
    // 0x8041C980: sll         $v1, $v1, 16
    ctx->r3 = S32(ctx->r3 << 16);
L_8041C984:
    // 0x8041C984: sra         $v1, $v1, 16
    ctx->r3 = S32(SIGNED(ctx->r3) >> 16);
    // 0x8041C988: bgezl       $v1, L_8041C850
    if (SIGNED(ctx->r3) >= 0) {
        // 0x8041C98C: lbu         $v1, -0x600($s2)
        ctx->r3 = MEM_BU(ctx->r18, -0X600);
            goto L_8041C850;
    }
    goto skip_2;
    // 0x8041C98C: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
    skip_2:
    // 0x8041C990: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041C994: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041C998: ori         $v0, $v0, 0x800
    ctx->r2 = ctx->r2 | 0X800;
    // 0x8041C99C: addiu       $a1, $zero, 0x1
    ctx->r5 = ADD32(0, 0X1);
    // 0x8041C9A0: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041C9A4: jal         0x800268D4
    // 0x8041C9A8: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_35;
    // 0x8041C9A8: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_35:
    // 0x8041C9AC: b           L_8041C850
    // 0x8041C9B0: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
        goto L_8041C850;
    // 0x8041C9B0: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
L_8041C9B4:
    // 0x8041C9B4: beq         $a2, $zero, L_8041CD70
    if (ctx->r6 == 0) {
        // 0x8041C9B8: lw          $a0, -0x2F0($s3)
        ctx->r4 = MEM_W(ctx->r19, -0X2F0);
            goto L_8041CD70;
    }
    // 0x8041C9B8: lw          $a0, -0x2F0($s3)
    ctx->r4 = MEM_W(ctx->r19, -0X2F0);
    // 0x8041C9BC: lui         $s1, 0x8009
    ctx->r17 = S32(0X8009 << 16);
    // 0x8041C9C0: srl         $v1, $a0, 31
    ctx->r3 = S32(U32(ctx->r4) >> 31);
    // 0x8041C9C4: addu        $v1, $v1, $a0
    ctx->r3 = ADD32(ctx->r3, ctx->r4);
    // 0x8041C9C8: sra         $a0, $v1, 31
    ctx->r4 = S32(SIGNED(ctx->r3) >> 31);
    // 0x8041C9CC: srl         $a0, $a0, 28
    ctx->r4 = S32(U32(ctx->r4) >> 28);
    // 0x8041C9D0: sra         $v1, $v1, 1
    ctx->r3 = S32(SIGNED(ctx->r3) >> 1);
    // 0x8041C9D4: addu        $v1, $v1, $a0
    ctx->r3 = ADD32(ctx->r3, ctx->r4);
    // 0x8041C9D8: andi        $v1, $v1, 0xF
    ctx->r3 = ctx->r3 & 0XF;
    // 0x8041C9DC: subu        $v1, $v1, $a0
    ctx->r3 = SUB32(ctx->r3, ctx->r4);
    // 0x8041C9E0: addu        $s5, $s5, $v1
    ctx->r21 = ADD32(ctx->r21, ctx->r3);
    // 0x8041C9E4: lb          $s3, 0x0($s5)
    ctx->r19 = MEM_B(ctx->r21, 0X0);
    // 0x8041C9E8: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041C9EC: andi        $v1, $v0, 0x100
    ctx->r3 = ctx->r2 & 0X100;
L_8041C9F0:
    // 0x8041C9F0: beq         $v1, $zero, L_8041CAA8
    if (ctx->r3 == 0) {
        // 0x8041C9F4: addiu       $a0, $zero, 0x100
        ctx->r4 = ADD32(0, 0X100);
            goto L_8041CAA8;
    }
    // 0x8041C9F4: addiu       $a0, $zero, 0x100
    ctx->r4 = ADD32(0, 0X100);
    // 0x8041C9F8: bne         $v1, $a0, L_8041CA7C
    if (ctx->r3 != ctx->r4) {
        // 0x8041C9FC: lui         $fp, 0x8006
        ctx->r30 = S32(0X8006 << 16);
            goto L_8041CA7C;
    }
    // 0x8041C9FC: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041CA00: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x8041CA04: lb          $a0, -0xFF0($v1)
    ctx->r4 = MEM_B(ctx->r3, -0XFF0);
    // 0x8041CA08: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x8041CA0C: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
    // 0x8041CA10: addu        $a0, $fp, $a0
    ctx->r4 = ADD32(ctx->r30, ctx->r4);
    // 0x8041CA14: lh          $a1, 0x0($a0)
    ctx->r5 = MEM_H(ctx->r4, 0X0);
    // 0x8041CA18: beql        $a1, $a3, L_8041CC5C
    if (ctx->r5 == ctx->r7) {
        // 0x8041CA1C: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041CC5C;
    }
    goto skip_3;
    // 0x8041CA1C: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    skip_3:
    // 0x8041CA20: beql        $a2, $zero, L_8041C7CC
    if (ctx->r6 == 0) {
        // 0x8041CA24: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041C7CC;
    }
    goto skip_4;
    // 0x8041CA24: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_4:
L_8041CA28:
    // 0x8041CA28: lb          $v1, -0xFF0($v1)
    ctx->r3 = MEM_B(ctx->r3, -0XFF0);
L_8041CA2C:
    // 0x8041CA2C: sll         $v0, $v1, 2
    ctx->r2 = S32(ctx->r3 << 2);
    // 0x8041CA30: addu        $fp, $fp, $v0
    ctx->r30 = ADD32(ctx->r30, ctx->r2);
    // 0x8041CA34: lh          $v1, 0x0($fp)
    ctx->r3 = MEM_H(ctx->r30, 0X0);
    // 0x8041CA38: lh          $v0, 0x2($fp)
    ctx->r2 = MEM_H(ctx->r30, 0X2);
    // 0x8041CA3C: slt         $a2, $a3, $v1
    ctx->r6 = SIGNED(ctx->r7) < SIGNED(ctx->r3) ? 1 : 0;
    // 0x8041CA40: addu        $s6, $s6, $v0
    ctx->r22 = ADD32(ctx->r22, ctx->r2);
    // 0x8041CA44: lbu         $a0, 0x0($s6)
    ctx->r4 = MEM_BU(ctx->r22, 0X0);
    // 0x8041CA48: beq         $a2, $zero, L_8041CDA0
    if (ctx->r6 == 0) {
        // 0x8041CA4C: addu        $a1, $s0, $a3
        ctx->r5 = ADD32(ctx->r16, ctx->r7);
            goto L_8041CDA0;
    }
    // 0x8041CA4C: addu        $a1, $s0, $a3
    ctx->r5 = ADD32(ctx->r16, ctx->r7);
    // 0x8041CA50: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x8041CA54: addiu       $a1, $a1, 0x1
    ctx->r5 = ADD32(ctx->r5, 0X1);
    // 0x8041CA58: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CA5C:
    // 0x8041CA5C: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x8041CA60: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041CA64: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041CA68: jal         0x80029B58
    // 0x8041CA6C: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    func_80029B58(rdram, ctx);
        goto after_36;
    // 0x8041CA6C: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    after_36:
    // 0x8041CA70: addiu       $v0, $zero, 0x65
    ctx->r2 = ADD32(0, 0X65);
    // 0x8041CA74: b           L_8041CB14
    // 0x8041CA78: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
        goto L_8041CB14;
    // 0x8041CA78: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
L_8041CA7C:
    // 0x8041CA7C: beq         $a2, $zero, L_8041C7C0
    if (ctx->r6 == 0) {
        // 0x8041CA80: nop
    
            goto L_8041C7C0;
    }
    // 0x8041CA80: nop

    // 0x8041CA84: bne         $v1, $zero, L_8041CB74
    if (ctx->r3 != 0) {
        // 0x8041CA88: nop
    
            goto L_8041CB74;
    }
    // 0x8041CA88: nop

    // 0x8041CA8C: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
L_8041CA90:
    // 0x8041CA90: lui         $s4, 0x8006
    ctx->r20 = S32(0X8006 << 16);
    // 0x8041CA94: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x8041CA98: addiu       $s4, $s4, -0x70C
    ctx->r20 = ADD32(ctx->r20, -0X70C);
    // 0x8041CA9C: addu        $v0, $s4, $v1
    ctx->r2 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CAA0: b           L_8041CAD0
    // 0x8041CAA4: lh          $t0, 0x0($v0)
    ctx->r8 = MEM_H(ctx->r2, 0X0);
        goto L_8041CAD0;
    // 0x8041CAA4: lh          $t0, 0x0($v0)
    ctx->r8 = MEM_H(ctx->r2, 0X0);
L_8041CAA8:
    // 0x8041CAA8: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
    // 0x8041CAAC: lui         $s4, 0x8006
    ctx->r20 = S32(0X8006 << 16);
    // 0x8041CAB0: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x8041CAB4: addiu       $s4, $s4, -0x70C
    ctx->r20 = ADD32(ctx->r20, -0X70C);
    // 0x8041CAB8: addu        $a0, $s4, $v1
    ctx->r4 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CABC: lh          $t0, 0x0($a0)
    ctx->r8 = MEM_H(ctx->r4, 0X0);
    // 0x8041CAC0: beq         $t0, $a3, L_8041CCE0
    if (ctx->r8 == ctx->r7) {
        // 0x8041CAC4: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041CCE0;
    }
    // 0x8041CAC4: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x8041CAC8: beql        $a2, $zero, L_8041CB40
    if (ctx->r6 == 0) {
        // 0x8041CACC: lui         $v1, 0x8009
        ctx->r3 = S32(0X8009 << 16);
            goto L_8041CB40;
    }
    goto skip_5;
    // 0x8041CACC: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    skip_5:
L_8041CAD0:
    // 0x8041CAD0: addu        $v1, $s4, $v1
    ctx->r3 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CAD4: lh          $v1, 0x2($v1)
    ctx->r3 = MEM_H(ctx->r3, 0X2);
    // 0x8041CAD8: slt         $v0, $a3, $t0
    ctx->r2 = SIGNED(ctx->r7) < SIGNED(ctx->r8) ? 1 : 0;
    // 0x8041CADC: addu        $s6, $s6, $v1
    ctx->r22 = ADD32(ctx->r22, ctx->r3);
    // 0x8041CAE0: lbu         $a0, 0x0($s6)
    ctx->r4 = MEM_BU(ctx->r22, 0X0);
    // 0x8041CAE4: beq         $v0, $zero, L_8041CD58
    if (ctx->r2 == 0) {
        // 0x8041CAE8: addu        $a1, $s0, $a3
        ctx->r5 = ADD32(ctx->r16, ctx->r7);
            goto L_8041CD58;
    }
    // 0x8041CAE8: addu        $a1, $s0, $a3
    ctx->r5 = ADD32(ctx->r16, ctx->r7);
    // 0x8041CAEC: addiu       $a3, $a3, 0x1
    ctx->r7 = ADD32(ctx->r7, 0X1);
    // 0x8041CAF0: addiu       $a1, $a1, 0x1
    ctx->r5 = ADD32(ctx->r5, 0X1);
    // 0x8041CAF4: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CAF8:
    // 0x8041CAF8: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x8041CAFC: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041CB00: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041CB04: jal         0x80029B58
    // 0x8041CB08: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_37;
    // 0x8041CB08: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    after_37:
    // 0x8041CB0C: addiu       $v0, $zero, 0x3A
    ctx->r2 = ADD32(0, 0X3A);
    // 0x8041CB10: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
L_8041CB14:
    // 0x8041CB14: subu        $a1, $s0, $s3
    ctx->r5 = SUB32(ctx->r16, ctx->r19);
L_8041CB18:
    // 0x8041CB18: addiu       $v0, $zero, 0xD
    ctx->r2 = ADD32(0, 0XD);
    // 0x8041CB1C: lw          $a2, 0x24($sp)
    ctx->r6 = MEM_W(ctx->r29, 0X24);
    // 0x8041CB20: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041CB24: addiu       $a3, $zero, 0x10
    ctx->r7 = ADD32(0, 0X10);
    // 0x8041CB28: addiu       $a1, $a1, 0x1E
    ctx->r5 = ADD32(ctx->r5, 0X1E);
    // 0x8041CB2C: jal         0x80029B58
    // 0x8041CB30: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    func_80029B58(rdram, ctx);
        goto after_38;
    // 0x8041CB30: addiu       $a0, $zero, 0x1
    ctx->r4 = ADD32(0, 0X1);
    after_38:
    // 0x8041CB34: b           L_8041C850
    // 0x8041CB38: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
        goto L_8041C850;
    // 0x8041CB38: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
L_8041CB3C:
    // 0x8041CB3C: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
L_8041CB40:
    // 0x8041CB40: lbu         $v1, 0x2872($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X2872);
    // 0x8041CB44: addiu       $v1, $v1, 0x1E
    ctx->r3 = ADD32(ctx->r3, 0X1E);
    // 0x8041CB48: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041CB4C: sltiu       $v1, $v1, 0x3D
    ctx->r3 = ctx->r3 < 0X3D ? 1 : 0;
    // 0x8041CB50: bne         $v1, $zero, L_8041CBF0
    if (ctx->r3 != 0) {
        // 0x8041CB54: lui         $a1, 0x8009
        ctx->r5 = S32(0X8009 << 16);
            goto L_8041CBF0;
    }
    // 0x8041CB54: lui         $a1, 0x8009
    ctx->r5 = S32(0X8009 << 16);
    // 0x8041CB58: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041CB5C: beq         $v1, $zero, L_8041CD1C
    if (ctx->r3 == 0) {
        // 0x8041CB60: andi        $v0, $v0, 0xFEFF
        ctx->r2 = ctx->r2 & 0XFEFF;
            goto L_8041CD1C;
    }
    // 0x8041CB60: andi        $v0, $v0, 0xFEFF
    ctx->r2 = ctx->r2 & 0XFEFF;
    // 0x8041CB64: addiu       $v0, $zero, 0x3A
    ctx->r2 = ADD32(0, 0X3A);
L_8041CB68:
    // 0x8041CB68: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
    // 0x8041CB6C: b           L_8041CB14
    // 0x8041CB70: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
        goto L_8041CB14;
    // 0x8041CB70: sw          $v0, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r2;
L_8041CB74:
    // 0x8041CB74: bnel        $v1, $a0, L_8041CB18
    if (ctx->r3 != ctx->r4) {
        // 0x8041CB78: subu        $a1, $s0, $s3
        ctx->r5 = SUB32(ctx->r16, ctx->r19);
            goto L_8041CB18;
    }
    goto skip_6;
    // 0x8041CB78: subu        $a1, $s0, $s3
    ctx->r5 = SUB32(ctx->r16, ctx->r19);
    skip_6:
    // 0x8041CB7C: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041CB80: lui         $v1, 0x8006
    ctx->r3 = S32(0X8006 << 16);
    // 0x8041CB84: b           L_8041CA28
    // 0x8041CB88: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
        goto L_8041CA28;
    // 0x8041CB88: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
L_8041CB8C:
    // 0x8041CB8C: lb          $v1, 0x2871($a1)
    ctx->r3 = MEM_B(ctx->r5, 0X2871);
L_8041CB90:
    // 0x8041CB90: slti        $a0, $v1, -0x14
    ctx->r4 = SIGNED(ctx->r3) < -0X14 ? 1 : 0;
    // 0x8041CB94: beq         $a0, $zero, L_8041CE20
    if (ctx->r4 == 0) {
        // 0x8041CB98: slti        $v1, $v1, 0x15
        ctx->r3 = SIGNED(ctx->r3) < 0X15 ? 1 : 0;
            goto L_8041CE20;
    }
    // 0x8041CB98: slti        $v1, $v1, 0x15
    ctx->r3 = SIGNED(ctx->r3) < 0X15 ? 1 : 0;
    // 0x8041CB9C: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041CBA0: bnel        $v1, $zero, L_8041C7F4
    if (ctx->r3 != 0) {
        // 0x8041CBA4: addiu       $v0, $zero, 0x65
        ctx->r2 = ADD32(0, 0X65);
            goto L_8041C7F4;
    }
    goto skip_7;
    // 0x8041CBA4: addiu       $v0, $zero, 0x65
    ctx->r2 = ADD32(0, 0X65);
    skip_7:
    // 0x8041CBA8: lui         $a1, 0x8006
    ctx->r5 = S32(0X8006 << 16);
    // 0x8041CBAC: lb          $a0, -0xFF0($a1)
    ctx->r4 = MEM_B(ctx->r5, -0XFF0);
    // 0x8041CBB0: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041CBB4: addiu       $v1, $a0, -0x1
    ctx->r3 = ADD32(ctx->r4, -0X1);
    // 0x8041CBB8: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
    // 0x8041CBBC: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x8041CBC0: sll         $v1, $v1, 24
    ctx->r3 = S32(ctx->r3 << 24);
    // 0x8041CBC4: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x8041CBC8: addu        $a0, $fp, $a0
    ctx->r4 = ADD32(ctx->r30, ctx->r4);
    // 0x8041CBCC: lh          $a3, 0x0($a0)
    ctx->r7 = MEM_H(ctx->r4, 0X0);
    // 0x8041CBD0: bgez        $v1, L_8041CBE0
    if (SIGNED(ctx->r3) >= 0) {
        // 0x8041CBD4: sb          $v1, -0xFF0($a1)
        MEM_B(-0XFF0, ctx->r5) = ctx->r3;
            goto L_8041CBE0;
    }
    // 0x8041CBD4: sb          $v1, -0xFF0($a1)
    MEM_B(-0XFF0, ctx->r5) = ctx->r3;
    // 0x8041CBD8: sb          $zero, -0xFF0($a1)
    MEM_B(-0XFF0, ctx->r5) = 0;
    // 0x8041CBDC: or          $v1, $zero, $zero
    ctx->r3 = 0 | 0;
L_8041CBE0:
    // 0x8041CBE0: ori         $v0, $v0, 0x600
    ctx->r2 = ctx->r2 | 0X600;
    // 0x8041CBE4: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CBE8: b           L_8041CA2C
    // 0x8041CBEC: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CA2C;
    // 0x8041CBEC: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CBF0:
    // 0x8041CBF0: lb          $v1, 0x2871($a1)
    ctx->r3 = MEM_B(ctx->r5, 0X2871);
L_8041CBF4:
    // 0x8041CBF4: slti        $a0, $v1, -0x14
    ctx->r4 = SIGNED(ctx->r3) < -0X14 ? 1 : 0;
    // 0x8041CBF8: beq         $a0, $zero, L_8041CDBC
    if (ctx->r4 == 0) {
        // 0x8041CBFC: slti        $v1, $v1, 0x15
        ctx->r3 = SIGNED(ctx->r3) < 0X15 ? 1 : 0;
            goto L_8041CDBC;
    }
    // 0x8041CBFC: slti        $v1, $v1, 0x15
    ctx->r3 = SIGNED(ctx->r3) < 0X15 ? 1 : 0;
    // 0x8041CC00: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041CC04: bnel        $v1, $zero, L_8041CB68
    if (ctx->r3 != 0) {
        // 0x8041CC08: addiu       $v0, $zero, 0x3A
        ctx->r2 = ADD32(0, 0X3A);
            goto L_8041CB68;
    }
    goto skip_8;
    // 0x8041CC08: addiu       $v0, $zero, 0x3A
    ctx->r2 = ADD32(0, 0X3A);
    skip_8:
    // 0x8041CC0C: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
    // 0x8041CC10: lui         $s4, 0x8006
    ctx->r20 = S32(0X8006 << 16);
    // 0x8041CC14: addiu       $a0, $v1, -0x1
    ctx->r4 = ADD32(ctx->r3, -0X1);
    // 0x8041CC18: sll         $a1, $v1, 2
    ctx->r5 = S32(ctx->r3 << 2);
    // 0x8041CC1C: addiu       $s4, $s4, -0x70C
    ctx->r20 = ADD32(ctx->r20, -0X70C);
    // 0x8041CC20: sll         $v1, $a0, 24
    ctx->r3 = S32(ctx->r4 << 24);
    // 0x8041CC24: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x8041CC28: addu        $a0, $s4, $a1
    ctx->r4 = ADD32(ctx->r20, ctx->r5);
    // 0x8041CC2C: lh          $a3, 0x0($a0)
    ctx->r7 = MEM_H(ctx->r4, 0X0);
    // 0x8041CC30: bgez        $v1, L_8041CC40
    if (SIGNED(ctx->r3) >= 0) {
        // 0x8041CC34: sb          $v1, -0x600($s2)
        MEM_B(-0X600, ctx->r18) = ctx->r3;
            goto L_8041CC40;
    }
    // 0x8041CC34: sb          $v1, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = ctx->r3;
    // 0x8041CC38: sb          $zero, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = 0;
    // 0x8041CC3C: or          $v1, $zero, $zero
    ctx->r3 = 0 | 0;
L_8041CC40:
    // 0x8041CC40: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x8041CC44: ori         $v0, $v0, 0x600
    ctx->r2 = ctx->r2 | 0X600;
    // 0x8041CC48: addu        $a0, $s4, $v1
    ctx->r4 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CC4C: lh          $t0, 0x0($a0)
    ctx->r8 = MEM_H(ctx->r4, 0X0);
    // 0x8041CC50: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CC54: b           L_8041CAD0
    // 0x8041CC58: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CAD0;
    // 0x8041CC58: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CC5C:
    // 0x8041CC5C: lbu         $a2, 0x2871($a1)
    ctx->r6 = MEM_BU(ctx->r5, 0X2871);
    // 0x8041CC60: andi        $v0, $v0, 0xF9FF
    ctx->r2 = ctx->r2 & 0XF9FF;
    // 0x8041CC64: addiu       $a2, $a2, 0x13
    ctx->r6 = ADD32(ctx->r6, 0X13);
    // 0x8041CC68: andi        $a2, $a2, 0xFF
    ctx->r6 = ctx->r6 & 0XFF;
    // 0x8041CC6C: sltiu       $a2, $a2, 0x27
    ctx->r6 = ctx->r6 < 0X27 ? 1 : 0;
    // 0x8041CC70: bne         $a2, $zero, L_8041CEE4
    if (ctx->r6 != 0) {
        // 0x8041CC74: sh          $v0, -0x2F4($s7)
        MEM_H(-0X2F4, ctx->r23) = ctx->r2;
            goto L_8041CEE4;
    }
    // 0x8041CC74: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CC78: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041CC7C: lbu         $v1, 0x2872($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X2872);
    // 0x8041CC80: addiu       $v1, $v1, 0x1E
    ctx->r3 = ADD32(ctx->r3, 0X1E);
    // 0x8041CC84: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041CC88: sltiu       $v1, $v1, 0x3D
    ctx->r3 = ctx->r3 < 0X3D ? 1 : 0;
    // 0x8041CC8C: bne         $v1, $zero, L_8041CB90
    if (ctx->r3 != 0) {
        // 0x8041CC90: lb          $v1, 0x2871($a1)
        ctx->r3 = MEM_B(ctx->r5, 0X2871);
            goto L_8041CB90;
    }
    // 0x8041CC90: lb          $v1, 0x2871($a1)
    ctx->r3 = MEM_B(ctx->r5, 0X2871);
    // 0x8041CC94: andi        $v0, $v0, 0xFEFF
    ctx->r2 = ctx->r2 & 0XFEFF;
L_8041CC98:
    // 0x8041CC98: ori         $v0, $v0, 0x400
    ctx->r2 = ctx->r2 | 0X400;
    // 0x8041CC9C: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041CCA0: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x8041CCA4: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041CCA8: jal         0x800268D4
    // 0x8041CCAC: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_39;
    // 0x8041CCAC: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_39:
    // 0x8041CCB0: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CCB4: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041CCB8: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
L_8041CCBC:
    // 0x8041CCBC: addiu       $v1, $zero, 0x65
    ctx->r3 = ADD32(0, 0X65);
    // 0x8041CCC0: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
    // 0x8041CCC4: beq         $v0, $zero, L_8041CB14
    if (ctx->r2 == 0) {
        // 0x8041CCC8: sw          $v1, 0x24($sp)
        MEM_W(0X24, ctx->r29) = ctx->r3;
            goto L_8041CB14;
    }
    // 0x8041CCC8: sw          $v1, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r3;
    // 0x8041CCCC: lui         $v0, 0x8006
    ctx->r2 = S32(0X8006 << 16);
    // 0x8041CCD0: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041CCD4: lb          $v1, -0xFF0($v0)
    ctx->r3 = MEM_B(ctx->r2, -0XFF0);
    // 0x8041CCD8: b           L_8041CA2C
    // 0x8041CCDC: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
        goto L_8041CA2C;
    // 0x8041CCDC: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
L_8041CCE0:
    // 0x8041CCE0: lbu         $v1, 0x2871($a1)
    ctx->r3 = MEM_BU(ctx->r5, 0X2871);
    // 0x8041CCE4: andi        $v0, $v0, 0xF9FF
    ctx->r2 = ctx->r2 & 0XF9FF;
    // 0x8041CCE8: addiu       $v1, $v1, 0x13
    ctx->r3 = ADD32(ctx->r3, 0X13);
    // 0x8041CCEC: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041CCF0: sltiu       $v1, $v1, 0x27
    ctx->r3 = ctx->r3 < 0X27 ? 1 : 0;
    // 0x8041CCF4: bne         $v1, $zero, L_8041CE9C
    if (ctx->r3 != 0) {
        // 0x8041CCF8: sh          $v0, -0x2F4($s7)
        MEM_H(-0X2F4, ctx->r23) = ctx->r2;
            goto L_8041CE9C;
    }
    // 0x8041CCF8: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CCFC: lui         $v1, 0x8009
    ctx->r3 = S32(0X8009 << 16);
    // 0x8041CD00: lbu         $v1, 0x2872($v1)
    ctx->r3 = MEM_BU(ctx->r3, 0X2872);
    // 0x8041CD04: addiu       $v1, $v1, 0x1E
    ctx->r3 = ADD32(ctx->r3, 0X1E);
    // 0x8041CD08: andi        $v1, $v1, 0xFF
    ctx->r3 = ctx->r3 & 0XFF;
    // 0x8041CD0C: sltiu       $v1, $v1, 0x3D
    ctx->r3 = ctx->r3 < 0X3D ? 1 : 0;
    // 0x8041CD10: bne         $v1, $zero, L_8041CBF4
    if (ctx->r3 != 0) {
        // 0x8041CD14: lb          $v1, 0x2871($a1)
        ctx->r3 = MEM_B(ctx->r5, 0X2871);
            goto L_8041CBF4;
    }
    // 0x8041CD14: lb          $v1, 0x2871($a1)
    ctx->r3 = MEM_B(ctx->r5, 0X2871);
    // 0x8041CD18: andi        $v0, $v0, 0xFEFF
    ctx->r2 = ctx->r2 & 0XFEFF;
L_8041CD1C:
    // 0x8041CD1C: ori         $v0, $v0, 0x500
    ctx->r2 = ctx->r2 | 0X500;
    // 0x8041CD20: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041CD24: or          $a1, $zero, $zero
    ctx->r5 = 0 | 0;
    // 0x8041CD28: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041CD2C: jal         0x800268D4
    // 0x8041CD30: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_40;
    // 0x8041CD30: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_40:
    // 0x8041CD34: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CD38: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041CD3C: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
L_8041CD40:
    // 0x8041CD40: addiu       $v1, $zero, 0x3A
    ctx->r3 = ADD32(0, 0X3A);
    // 0x8041CD44: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
    // 0x8041CD48: beq         $v0, $zero, L_8041CB14
    if (ctx->r2 == 0) {
        // 0x8041CD4C: sw          $v1, 0x24($sp)
        MEM_W(0X24, ctx->r29) = ctx->r3;
            goto L_8041CB14;
    }
    // 0x8041CD4C: sw          $v1, 0x24($sp)
    MEM_W(0X24, ctx->r29) = ctx->r3;
    // 0x8041CD50: b           L_8041CA90
    // 0x8041CD54: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
        goto L_8041CA90;
    // 0x8041CD54: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
L_8041CD58:
    // 0x8041CD58: slt         $t0, $t0, $a3
    ctx->r8 = SIGNED(ctx->r8) < SIGNED(ctx->r7) ? 1 : 0;
    // 0x8041CD5C: beq         $t0, $zero, L_8041CD90
    if (ctx->r8 == 0) {
        // 0x8041CD60: addiu       $a3, $a3, -0x1
        ctx->r7 = ADD32(ctx->r7, -0X1);
            goto L_8041CD90;
    }
    // 0x8041CD60: addiu       $a3, $a3, -0x1
    ctx->r7 = ADD32(ctx->r7, -0X1);
    // 0x8041CD64: addiu       $a1, $a1, -0x1
    ctx->r5 = ADD32(ctx->r5, -0X1);
    // 0x8041CD68: b           L_8041CAF8
    // 0x8041CD6C: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CAF8;
    // 0x8041CD6C: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CD70:
    // 0x8041CD70: andi        $v0, $v0, 0xF6FF
    ctx->r2 = ctx->r2 & 0XF6FF;
    // 0x8041CD74: addiu       $a2, $zero, 0xFF
    ctx->r6 = ADD32(0, 0XFF);
    // 0x8041CD78: addiu       $a1, $zero, 0x2
    ctx->r5 = ADD32(0, 0X2);
    // 0x8041CD7C: or          $a0, $zero, $zero
    ctx->r4 = 0 | 0;
    // 0x8041CD80: jal         0x800268D4
    // 0x8041CD84: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    func_800268D4(rdram, ctx);
        goto after_41;
    // 0x8041CD84: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    after_41:
    // 0x8041CD88: b           L_8041C850
    // 0x8041CD8C: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
        goto L_8041C850;
    // 0x8041CD8C: lbu         $v1, -0x600($s2)
    ctx->r3 = MEM_BU(ctx->r18, -0X600);
L_8041CD90:
    // 0x8041CD90: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CD94: andi        $v0, $v0, 0xFBFF
    ctx->r2 = ctx->r2 & 0XFBFF;
    // 0x8041CD98: b           L_8041CAF8
    // 0x8041CD9C: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
        goto L_8041CAF8;
    // 0x8041CD9C: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
L_8041CDA0:
    // 0x8041CDA0: slt         $v1, $v1, $a3
    ctx->r3 = SIGNED(ctx->r3) < SIGNED(ctx->r7) ? 1 : 0;
    // 0x8041CDA4: beq         $v1, $zero, L_8041CE78
    if (ctx->r3 == 0) {
        // 0x8041CDA8: lhu         $v0, -0x2F4($s7)
        ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
            goto L_8041CE78;
    }
    // 0x8041CDA8: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CDAC: addiu       $a3, $a3, -0x1
    ctx->r7 = ADD32(ctx->r7, -0X1);
    // 0x8041CDB0: addiu       $a1, $a1, -0x1
    ctx->r5 = ADD32(ctx->r5, -0X1);
    // 0x8041CDB4: b           L_8041CA5C
    // 0x8041CDB8: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CA5C;
    // 0x8041CDB8: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CDBC:
    // 0x8041CDBC: bne         $v1, $zero, L_8041CE90
    if (ctx->r3 != 0) {
        // 0x8041CDC0: andi        $v1, $v0, 0xFBFF
        ctx->r3 = ctx->r2 & 0XFBFF;
            goto L_8041CE90;
    }
    // 0x8041CDC0: andi        $v1, $v0, 0xFBFF
    ctx->r3 = ctx->r2 & 0XFBFF;
    // 0x8041CDC4: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041CDC8: bnel        $v1, $zero, L_8041CB68
    if (ctx->r3 != 0) {
        // 0x8041CDCC: addiu       $v0, $zero, 0x3A
        ctx->r2 = ADD32(0, 0X3A);
            goto L_8041CB68;
    }
    goto skip_9;
    // 0x8041CDCC: addiu       $v0, $zero, 0x3A
    ctx->r2 = ADD32(0, 0X3A);
    skip_9:
    // 0x8041CDD0: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
    // 0x8041CDD4: lui         $s4, 0x8006
    ctx->r20 = S32(0X8006 << 16);
    // 0x8041CDD8: addiu       $a0, $v1, 0x1
    ctx->r4 = ADD32(ctx->r3, 0X1);
    // 0x8041CDDC: sll         $a0, $a0, 24
    ctx->r4 = S32(ctx->r4 << 24);
    // 0x8041CDE0: sra         $a0, $a0, 24
    ctx->r4 = S32(SIGNED(ctx->r4) >> 24);
    // 0x8041CDE4: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x8041CDE8: addiu       $s4, $s4, -0x70C
    ctx->r20 = ADD32(ctx->r20, -0X70C);
    // 0x8041CDEC: ori         $v0, $v0, 0x600
    ctx->r2 = ctx->r2 | 0X600;
    // 0x8041CDF0: addu        $v1, $s4, $v1
    ctx->r3 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CDF4: slti        $a1, $a0, 0x5
    ctx->r5 = SIGNED(ctx->r4) < 0X5 ? 1 : 0;
    // 0x8041CDF8: lh          $a3, 0x0($v1)
    ctx->r7 = MEM_H(ctx->r3, 0X0);
    // 0x8041CDFC: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CE00: beq         $a1, $zero, L_8041CF3C
    if (ctx->r5 == 0) {
        // 0x8041CE04: sb          $a0, -0x600($s2)
        MEM_B(-0X600, ctx->r18) = ctx->r4;
            goto L_8041CF3C;
    }
    // 0x8041CE04: sb          $a0, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = ctx->r4;
    // 0x8041CE08: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x8041CE0C: addu        $v0, $s4, $a0
    ctx->r2 = ADD32(ctx->r20, ctx->r4);
    // 0x8041CE10: lh          $t0, 0x0($v0)
    ctx->r8 = MEM_H(ctx->r2, 0X0);
    // 0x8041CE14: or          $v1, $a0, $zero
    ctx->r3 = ctx->r4 | 0;
    // 0x8041CE18: b           L_8041CAD0
    // 0x8041CE1C: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CAD0;
    // 0x8041CE1C: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CE20:
    // 0x8041CE20: bne         $v1, $zero, L_8041CE84
    if (ctx->r3 != 0) {
        // 0x8041CE24: andi        $v1, $v0, 0xFBFF
        ctx->r3 = ctx->r2 & 0XFBFF;
            goto L_8041CE84;
    }
    // 0x8041CE24: andi        $v1, $v0, 0xFBFF
    ctx->r3 = ctx->r2 & 0XFBFF;
    // 0x8041CE28: andi        $v1, $v0, 0x400
    ctx->r3 = ctx->r2 & 0X400;
    // 0x8041CE2C: bnel        $v1, $zero, L_8041C7F4
    if (ctx->r3 != 0) {
        // 0x8041CE30: addiu       $v0, $zero, 0x65
        ctx->r2 = ADD32(0, 0X65);
            goto L_8041C7F4;
    }
    goto skip_10;
    // 0x8041CE30: addiu       $v0, $zero, 0x65
    ctx->r2 = ADD32(0, 0X65);
    skip_10:
    // 0x8041CE34: lui         $a1, 0x8006
    ctx->r5 = S32(0X8006 << 16);
    // 0x8041CE38: lb          $a0, -0xFF0($a1)
    ctx->r4 = MEM_B(ctx->r5, -0XFF0);
    // 0x8041CE3C: lui         $fp, 0x8006
    ctx->r30 = S32(0X8006 << 16);
    // 0x8041CE40: addiu       $v1, $a0, 0x1
    ctx->r3 = ADD32(ctx->r4, 0X1);
    // 0x8041CE44: sll         $v1, $v1, 24
    ctx->r3 = S32(ctx->r3 << 24);
    // 0x8041CE48: sra         $v1, $v1, 24
    ctx->r3 = S32(SIGNED(ctx->r3) >> 24);
    // 0x8041CE4C: sll         $a0, $a0, 2
    ctx->r4 = S32(ctx->r4 << 2);
    // 0x8041CE50: addiu       $fp, $fp, -0x6EC
    ctx->r30 = ADD32(ctx->r30, -0X6EC);
    // 0x8041CE54: addu        $a0, $fp, $a0
    ctx->r4 = ADD32(ctx->r30, ctx->r4);
    // 0x8041CE58: ori         $v0, $v0, 0x600
    ctx->r2 = ctx->r2 | 0X600;
    // 0x8041CE5C: slti        $a2, $v1, 0x5
    ctx->r6 = SIGNED(ctx->r3) < 0X5 ? 1 : 0;
    // 0x8041CE60: lh          $a3, 0x0($a0)
    ctx->r7 = MEM_H(ctx->r4, 0X0);
    // 0x8041CE64: sb          $v1, -0xFF0($a1)
    MEM_B(-0XFF0, ctx->r5) = ctx->r3;
    // 0x8041CE68: beq         $a2, $zero, L_8041CF28
    if (ctx->r6 == 0) {
        // 0x8041CE6C: sh          $v0, -0x2F4($s7)
        MEM_H(-0X2F4, ctx->r23) = ctx->r2;
            goto L_8041CF28;
    }
    // 0x8041CE6C: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
    // 0x8041CE70: b           L_8041CA2C
    // 0x8041CE74: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CA2C;
    // 0x8041CE74: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
L_8041CE78:
    // 0x8041CE78: andi        $v0, $v0, 0xFBFF
    ctx->r2 = ctx->r2 & 0XFBFF;
    // 0x8041CE7C: b           L_8041CA5C
    // 0x8041CE80: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
        goto L_8041CA5C;
    // 0x8041CE80: sh          $v0, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r2;
L_8041CE84:
    // 0x8041CE84: sh          $v1, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r3;
    // 0x8041CE88: b           L_8041CCBC
    // 0x8041CE8C: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
        goto L_8041CCBC;
    // 0x8041CE8C: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
L_8041CE90:
    // 0x8041CE90: sh          $v1, -0x2F4($s7)
    MEM_H(-0X2F4, ctx->r23) = ctx->r3;
    // 0x8041CE94: b           L_8041CD40
    // 0x8041CE98: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
        goto L_8041CD40;
    // 0x8041CE98: andi        $v0, $v0, 0x200
    ctx->r2 = ctx->r2 & 0X200;
L_8041CE9C:
    // 0x8041CE9C: lh          $v0, 0x2($a0)
    ctx->r2 = MEM_H(ctx->r4, 0X2);
    // 0x8041CEA0: addu        $a1, $s0, $a3
    ctx->r5 = ADD32(ctx->r16, ctx->r7);
    // 0x8041CEA4: addu        $v0, $s6, $v0
    ctx->r2 = ADD32(ctx->r22, ctx->r2);
    // 0x8041CEA8: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x8041CEAC: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x8041CEB0: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041CEB4: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041CEB8: jal         0x80029B58
    // 0x8041CEBC: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    func_80029B58(rdram, ctx);
        goto after_42;
    // 0x8041CEBC: addiu       $a2, $zero, 0x39
    ctx->r6 = ADD32(0, 0X39);
    after_42:
    // 0x8041CEC0: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CEC4: andi        $v1, $v0, 0x200
    ctx->r3 = ctx->r2 & 0X200;
    // 0x8041CEC8: beq         $v1, $zero, L_8041CB3C
    if (ctx->r3 == 0) {
        // 0x8041CECC: lw          $a3, -0x2EC($s1)
        ctx->r7 = MEM_W(ctx->r17, -0X2EC);
            goto L_8041CB3C;
    }
    // 0x8041CECC: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041CED0: lb          $v1, -0x600($s2)
    ctx->r3 = MEM_B(ctx->r18, -0X600);
    // 0x8041CED4: sll         $v1, $v1, 2
    ctx->r3 = S32(ctx->r3 << 2);
    // 0x8041CED8: addu        $v0, $s4, $v1
    ctx->r2 = ADD32(ctx->r20, ctx->r3);
    // 0x8041CEDC: b           L_8041CAD0
    // 0x8041CEE0: lh          $t0, 0x0($v0)
    ctx->r8 = MEM_H(ctx->r2, 0X0);
        goto L_8041CAD0;
    // 0x8041CEE0: lh          $t0, 0x0($v0)
    ctx->r8 = MEM_H(ctx->r2, 0X0);
L_8041CEE4:
    // 0x8041CEE4: lh          $v0, 0x2($a0)
    ctx->r2 = MEM_H(ctx->r4, 0X2);
    // 0x8041CEE8: addu        $a1, $s0, $a3
    ctx->r5 = ADD32(ctx->r16, ctx->r7);
    // 0x8041CEEC: addu        $v0, $s6, $v0
    ctx->r2 = ADD32(ctx->r22, ctx->r2);
    // 0x8041CEF0: lbu         $a0, 0x0($v0)
    ctx->r4 = MEM_BU(ctx->r2, 0X0);
    // 0x8041CEF4: addiu       $v0, $zero, 0xB
    ctx->r2 = ADD32(0, 0XB);
    // 0x8041CEF8: sw          $v0, 0x10($sp)
    MEM_W(0X10, ctx->r29) = ctx->r2;
    // 0x8041CEFC: addiu       $a3, $zero, 0x8
    ctx->r7 = ADD32(0, 0X8);
    // 0x8041CF00: addiu       $a2, $zero, 0x67
    ctx->r6 = ADD32(0, 0X67);
    // 0x8041CF04: jal         0x80029B58
    // 0x8041CF08: sw          $v1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r3;
    func_80029B58(rdram, ctx);
        goto after_43;
    // 0x8041CF08: sw          $v1, 0x20($sp)
    MEM_W(0X20, ctx->r29) = ctx->r3;
    after_43:
    // 0x8041CF0C: lhu         $v0, -0x2F4($s7)
    ctx->r2 = MEM_HU(ctx->r23, -0X2F4);
    // 0x8041CF10: andi        $a0, $v0, 0x200
    ctx->r4 = ctx->r2 & 0X200;
    // 0x8041CF14: beq         $a0, $zero, L_8041C7C8
    if (ctx->r4 == 0) {
        // 0x8041CF18: lw          $a3, -0x2EC($s1)
        ctx->r7 = MEM_W(ctx->r17, -0X2EC);
            goto L_8041C7C8;
    }
    // 0x8041CF18: lw          $a3, -0x2EC($s1)
    ctx->r7 = MEM_W(ctx->r17, -0X2EC);
    // 0x8041CF1C: lw          $v1, 0x20($sp)
    ctx->r3 = MEM_W(ctx->r29, 0X20);
    // 0x8041CF20: b           L_8041CA2C
    // 0x8041CF24: lb          $v1, -0xFF0($v1)
    ctx->r3 = MEM_B(ctx->r3, -0XFF0);
        goto L_8041CA2C;
    // 0x8041CF24: lb          $v1, -0xFF0($v1)
    ctx->r3 = MEM_B(ctx->r3, -0XFF0);
L_8041CF28:
    // 0x8041CF28: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
    // 0x8041CF2C: sb          $v0, -0xFF0($a1)
    MEM_B(-0XFF0, ctx->r5) = ctx->r2;
    // 0x8041CF30: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
    // 0x8041CF34: b           L_8041CA2C
    // 0x8041CF38: addiu       $v1, $zero, 0x4
    ctx->r3 = ADD32(0, 0X4);
        goto L_8041CA2C;
    // 0x8041CF38: addiu       $v1, $zero, 0x4
    ctx->r3 = ADD32(0, 0X4);
L_8041CF3C:
    // 0x8041CF3C: addiu       $v0, $zero, 0x4
    ctx->r2 = ADD32(0, 0X4);
    // 0x8041CF40: lh          $t0, 0x10($s4)
    ctx->r8 = MEM_H(ctx->r20, 0X10);
    // 0x8041CF44: addiu       $v1, $zero, 0x10
    ctx->r3 = ADD32(0, 0X10);
    // 0x8041CF48: sb          $v0, -0x600($s2)
    MEM_B(-0X600, ctx->r18) = ctx->r2;
    // 0x8041CF4C: b           L_8041CAD0
    // 0x8041CF50: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
        goto L_8041CAD0;
    // 0x8041CF50: sw          $a3, -0x2EC($s1)
    MEM_W(-0X2EC, ctx->r17) = ctx->r7;
;}
