/*
addressing.hpp

Copyright (c) 09 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef ADDRESSING_HPP
#define ADDRESSING_HPP

#include <cstdint>
#include <cassert>

#include "registers.hpp"

namespace x86gen
{

enum PtrWidth
{
    Byte,
    Word,
    DWord,
    QWord
};

constexpr uint8_t make_modrm(uint8_t mod, uint8_t rm, uint8_t reg = 0)
{
    assert(mod < 0b100 && rm < 0b1000 && reg < 0b1000);

    return (mod << 6) | (reg << 3) | rm;
}

constexpr uint8_t make_sib  (uint8_t scale, GPR32 offset, GPR32 base)
{
    assert(scale == 1 || scale == 2 || scale == 4 || scale == 8);

    const uint8_t adjusted_scale = (scale == 1) ? 0b00 : (scale == 2) ? 0b01 : (scale == 4) ? 0b10 : 0b11;
    return (adjusted_scale << 6) | (offset << 3) | base;
}

struct IndirectReg{};
struct SIB_EBP{};

struct SIBOpResultPlusDisp
{
    GPR32 base;
    GPR32 offset;
    uint8_t scale;
    int32_t disp;
};

struct SIBOpResult
{
    constexpr SIBOpResult(GPR32 in_base, GPR32 in_offset, uint8_t in_scale)
        : base(in_base), offset(in_offset), scale(in_scale)
    {}

    operator SIBOpResultPlusDisp() const
    { return {EBP, offset, scale, 0}; }

    GPR32 base;
    GPR32 offset;
    uint8_t scale;
};

struct SIBBase
{
    GPR32 reg;
};
struct SIBOffset
{
    GPR32 reg;
};

struct SIBIndexPair
{
    GPR32 offset;
    uint8_t scale;

    operator SIBOpResult() const
    { return {EBP, offset, scale}; }
};

constexpr SIBIndexPair operator*(SIBOffset off, uint8_t scale)
{
    return {off.reg, scale};
}
constexpr SIBIndexPair operator*(uint8_t scale, SIBOffset off)
{
    return off*scale;
}

constexpr SIBIndexPair operator*(GPR32 reg, int scale)
{
    /*    static_assert(__builtin_choose_expr(__builtin_constant_p(scale),
                              false, true), "parameter port must be < 2");*/
    return {reg, (uint8_t)scale};
}
constexpr SIBIndexPair operator*(int scale, GPR32 reg)
{
    return reg*scale;
}

constexpr SIBOpResult operator+(const SIBIndexPair& idx, GPR32 base)
{
    return {base, idx.offset, idx.scale};
}
constexpr SIBOpResult operator+(GPR32 base, const SIBIndexPair& idx)
{
    return idx+base;
}
constexpr SIBOpResult operator+(GPR32 base, GPR32 offset)
{
    return base*1+offset;
}

constexpr SIBOpResultPlusDisp operator+(const SIBOpResult& in, int32_t disp)
{
    return {in.base, in.offset, in.scale, disp};
}
constexpr SIBOpResultPlusDisp operator+(int32_t disp, const SIBOpResult& in)
{
    return in+disp;
}

constexpr SIBOpResultPlusDisp operator-(const SIBOpResult& in, int32_t disp)
{
    return in + (-disp);
}
constexpr SIBOpResultPlusDisp operator-(int32_t disp, const SIBOpResult& in)
{
    return in-disp;
}

constexpr SIBOpResultPlusDisp operator+(const SIBOpResultPlusDisp& in, int32_t disp)
{
    return {in.base, in.offset, in.scale, in.disp+disp};
}
constexpr SIBOpResultPlusDisp operator+(int32_t disp, const SIBOpResultPlusDisp& in)
{
    return in+disp;
}

constexpr SIBOpResultPlusDisp operator-(const SIBOpResultPlusDisp& in, int32_t disp)
{
    return in + (-disp);
}
constexpr SIBOpResultPlusDisp operator-(int32_t disp, const SIBOpResultPlusDisp& in)
{
    return in-disp;
}

struct SIB
{
    SIB(const SIBOpResult& op)
        : sib{make_sib(op.scale, op.offset, op.base)}, has_disp{op.base == EBP}
    {}
    SIB(GPR32 base)
        : sib{make_sib(1, ESP, base)}
    {}
    SIB(uint8_t scale, GPR32 offset, GPR32 base)
        : sib{make_sib(scale, offset, base)}, has_disp{base == EBP}
    {
    }
    SIB(uint8_t scale, GPR32 offset, int32_t in_disp = 0)
        : sib{make_sib(scale, offset, EBP)}, disp{in_disp}, has_disp{true}
    {}
    SIB(uint8_t scale, GPR32 offset, int8_t in_disp, SIB_EBP)
        : sib{make_sib(scale, offset, EBP)}, disp{static_cast<int8_t>(in_disp)}, has_disp{true}, rel_to_ebp{true}
    {}
    SIB(uint8_t scale, GPR32 offset, int32_t in_disp, SIB_EBP)
        : sib{make_sib(scale, offset, EBP)}, disp{in_disp}, has_disp{true}, rel_to_ebp{true}
    {}

    operator uint8_t() const { return sib; }

    const uint8_t sib;
    const int32_t disp { 0 };
    const bool has_disp { false };
    const bool rel_to_ebp { false };
};

struct ModRM
{
    ModRM(IndirectReg, GPR32 reg, PtrWidth in_width)
        : modrm{make_modrm(0, reg)}, width(in_width)
    {
        assert(reg != ESP && reg != EBP);
    }
    ModRM(IndirectReg, GPR64 reg, PtrWidth in_width)
        : modrm{make_modrm(0, reg&0b111)}, width(in_width), is_64(true), needs_rex(reg>=R8)
    {
        assert(reg != RSP && reg != RBP);
    }

    explicit ModRM(int32_t disp)
        : modrm{make_modrm(0, 0b101)}, displacement{disp}, width(DWord)
    {}
    ModRM(SIB in_sib, PtrWidth in_width = DWord)
        : modrm{make_modrm(0, 0b100)}, sib{in_sib}, displacement{in_sib.disp}, width(in_width)
    {}

    ModRM(IndirectReg, GPR32 reg, int8_t disp)
        : modrm{make_modrm(1, reg)}, displacement{disp}, width(DWord)
    {
        assert(reg != ESP);
    }
    ModRM(SIB in_sib, int8_t disp, PtrWidth in_width = DWord)
        : modrm{make_modrm(1, 0b100)}, sib{in_sib}, displacement{in_sib.disp + disp}, width(in_width)
    {}
    // TODO : take a look at this
    ModRM(const SIBOpResult& op)
        : modrm{make_modrm(0, 0b100)}, sib{op.scale, op.offset, op.base}, width(DWord)
    {}
    ModRM(const SIBIndexPair& op)
        : ModRM(SIBOpResult(op))
    {}
    ModRM(const SIBOpResultPlusDisp& op)
        : modrm{make_modrm(1, 0b100)}, sib{op.scale, op.offset, op.base}, displacement{op.disp}, width(DWord)
    {}

    ModRM(IndirectReg, GPR32 reg, int32_t disp)
        : modrm{make_modrm(0b10, reg)}, displacement{disp}, width(DWord)
    {
        assert(reg != ESP);
    }

    ModRM(SIB in_sib, int32_t disp, PtrWidth in_width = DWord)
        : modrm{make_modrm(0b10, 0b100)}, sib{in_sib}, displacement{in_sib.disp + disp}, width(in_width)
    {}

    ModRM(GPR8 reg)
        : modrm{make_modrm(0b11, reg)}, width(Byte)
    {}
    ModRM(GPR16 reg)
        : modrm{make_modrm(0b11, reg)}, width(Word)
    {}
    ModRM(GPR32 reg)
        : modrm{make_modrm(0b11, reg)}, width(DWord)
    {}
    ModRM(GPR64 reg)
        : modrm{make_modrm(0b11, reg&0b111)}, width(QWord), is_64(true), needs_rex(reg>=R8)
    {}

    uint8_t mod() const
    { return modrm >> 6; }

    uint8_t rm() const
    { return modrm & 0b111; }

    uint8_t reg() const
    { return (modrm >> 3) & 0b111; }

    void set_reg(RegIndex reg)
    {
        modrm &= 0b11000111;
        modrm |= ((reg&0b111) << 3);
    }

    uint8_t modrm { 0 };
    const SIB sib {EAX};
    const int32_t displacement { 0 };
    const PtrWidth width;
    bool is_64 { false };
    bool needs_rex { false };
};

struct REX
{
    REX(bool operand64 = 0, GPR64 reg = (GPR64)0, GPR64 index = (GPR64)0, GPR64 base = (GPR64)0)
    {
        byte = 0b01000000;
        byte |= (operand64<<3);
        byte |= ((reg>=8)<<2);
        byte |= ((index>=8)<<1);
        byte |= (base>=8);
    }

    operator uint8_t() const { return byte; }

    uint8_t byte {};
};

struct FarPtr
{
    FarPtr(uint16_t in_seg, uint32_t in_addr)
        : addr(in_addr), seg(in_seg)
    {}
    FarPtr(uint32_t in_addr)
        : addr(in_addr), seg(0)
    {}

    uint32_t addr { 0 };
    uint16_t seg  { 0 };
};

template <typename T>
struct Immediate
{
    static_assert(std::is_integral_v<T>);

    Immediate(T in_val) : val(in_val)
    {}
    T val;
};
using Imm8  = Immediate<uint8_t>;
using Imm16 = Immediate<uint16_t>;
using Imm32 = Immediate<uint32_t>;
using Imm64 = Immediate<uint64_t>;

template <typename T>
struct Relative
{
    static_assert(std::is_integral_v<T>);

    explicit Relative(T in_val) : val(in_val)
    {}
    T val;
};
using Rel8  = Relative<int8_t>;
using Rel16 = Relative<int16_t>;
using Rel32 = Relative<int32_t>;

inline ModRM indirect(GPR32 reg, PtrWidth width = DWord)
{
    return ModRM{IndirectReg{}, reg, width};
}
inline ModRM indirect(GPR64 reg, PtrWidth width = DWord)
{
    return ModRM{IndirectReg{}, reg, width};
}
}

#endif // ADDRESSING_HPP
