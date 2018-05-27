/*
bf_generator.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include "bf_generator.hpp"

#include <limits>

#include <cstdio>
#include <cstring>

#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define DECLARE_ARITH_OP(single, multi, addressing, off) \
    if (off == 1) \
{ \
    gen.single(addressing); \
    } \
    else \
{ \
    if (off <= INT8_MAX) \
    gen.multi(addressing, Imm8{(uint8_t)off}); \
    else if (off <= INT32_MAX) \
    gen.multi(addressing, Imm32{(uint32_t)off}); \
    else \
    assert(false && "Increment offset is too large"); \
    }

namespace bfgen
{

using namespace x86gen;

constexpr auto pointer_reg = ECX;
constexpr auto zero_reg    = EBX;
constexpr auto putchar_addr_reg = ESI;
constexpr bool elide_cmp = true;

BrainfuckGenerator::BrainfuckGenerator(BuildFlags i_flags)
    : flags(i_flags)
{
    define_putchar();
    define_getchar();
}

void BrainfuckGenerator::prologue()
{
    create_cell_array();
    gen.mov(zero_reg, 0);
    gen.mov(putchar_addr_reg, 0x08048054 + putchar_addr);
    // 0f 18 09                prefetcht0 [ecx]
    gen.emit<uint8_t>(0x0F);
    gen.emit<uint8_t>(0x18);
    gen.emit<uint8_t>(0x09);
}

void BrainfuckGenerator::move_forwards(size_t off)
{
    assert(off);
    if (off*flags.cell_width == 2) // saves one byte
    {
        gen.inc(pointer_reg);
        gen.inc(pointer_reg);
    }
    else
    {
        DECLARE_ARITH_OP(inc, add, pointer_reg, off*flags.cell_width);
    }

    needs_cmp = true;
}

void BrainfuckGenerator::move_backwards(size_t off)
{
    assert(off);
    if (off*flags.cell_width == 2) // saves one byte
    {
        gen.dec(pointer_reg);
        gen.dec(pointer_reg);
    }
    else
    {
        DECLARE_ARITH_OP(dec, sub, pointer_reg, off*flags.cell_width);
    }

    needs_cmp = true;
}

void BrainfuckGenerator::increment(size_t off)
{
    assert(off);
    DECLARE_ARITH_OP(inc, add, indirect(pointer_reg, flags.cell_width), off);

    // We don't need to do a cmp after an increment of decrement op, it changes the ZF accordingly to *ptr
    needs_cmp = false;
}

void BrainfuckGenerator::decrement(size_t off)
{
    assert(off);
    DECLARE_ARITH_OP(dec, sub, indirect(pointer_reg, flags.cell_width), off);

    // We don't need to do a cmp after an increment of decrement op, it changes the ZF accordingly to *ptr
    needs_cmp = false;
}


void BrainfuckGenerator::print()
{
    call_function(putchar_addr);
}

void BrainfuckGenerator::get_input()
{
    call_function(getchar_addr);

    needs_cmp = true;
}

void BrainfuckGenerator::call_function(uint32_t addr)
{
    int32_t offset = addr - (gen.current_index() + 5); // max size of the call instruction

    if (addr == putchar_addr)
        gen.call(putchar_addr_reg); // putchar is at address 0x0, so call [putchar_addr_reg] is three bytes shorter
    else
        gen.call(Rel32{offset});
}

void BrainfuckGenerator::loop_begin()
{
    test_current_cell();

    gen.jcc(JZ, Rel32{0});

    bracket_stack.emplace(gen.current_index());

    needs_cmp = false;
}

void BrainfuckGenerator::loop_end()
{
    assert(!bracket_stack.empty()); // unmatched bracket

    test_current_cell();

    size_t opening_bracket_pc = bracket_stack.top();

    gen.jcc(JNZ, Rel32{0});

    link_jump(gen.current_index(), opening_bracket_pc);
    link_jump(opening_bracket_pc, gen.current_index());

    bracket_stack.pop();

    needs_cmp = false;
}

void BrainfuckGenerator::breakpoint()
{
    gen.int3();
}

void BrainfuckGenerator::epilogue()
{
    gen.mov(EAX, 0x1); // SYS_EXIT
    gen.mov(EBX, 0); // exit(0)
    gen.int_(0x80);
}

//////////////////////////////////////////////////////////////////////////
///////////////////////// Optimization routines //////////////////////////
//////////////////////////////////////////////////////////////////////////

void BrainfuckGenerator::scan(int8_t granularity)
{
    assert(granularity &&
           granularity*flags.cell_width <= INT8_MAX && granularity*flags.cell_width >= INT8_MIN);

    needs_cmp = true;

    int current_pc = gen.current_index();

    test_current_cell();
    gen.jcc(JZ, Rel8{+5}); // goto label
    gen.add(pointer_reg, Imm8{granularity*flags.cell_width});
    gen.jmp(Rel8{(int8_t)(current_pc-gen.current_index() - 2)}); // jmp rel8 is 2 bytes long

    needs_cmp = false; // cmp is already computed here
}

void BrainfuckGenerator::set(int8_t offset, uint32_t val)
{
    auto modrm = offset == 0 ? indirect(pointer_reg, flags.cell_width) : indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width);

    if (val == 0)
    {
        if (flags.cell_width == Byte)
            gen.mov(modrm, (GPR8)zero_reg);
        else if (flags.cell_width == Word)
            gen.mov(modrm, (GPR16)zero_reg);
        else
            gen.mov(modrm, zero_reg);
    }
    else gen.mov(modrm, Imm8{val}); // TODO

    if (offset == 0) needs_cmp = true;
}

void BrainfuckGenerator::add(int8_t offset, int32_t val)
{
    assert(val >= min_value() && (long)val <= (long)max_value());

    auto modrm = offset == 0 ? indirect(pointer_reg, flags.cell_width) : indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width);

    if      (val ==  1) gen.inc(modrm);
    else if (val == -1) gen.dec(modrm);
    else if (val !=  0)
    {
        // Again, shoud just be gen.add(modrm, val)
        if (flags.cell_width == Byte)
            gen.add(modrm, Imm8{val});
        else if (flags.cell_width == Word)
            gen.add(modrm, Imm16{val});
        else
            gen.add(modrm, Imm32{val});
    }


    needs_cmp = (offset != 0);
}

void BrainfuckGenerator::add_copy(int8_t offset, int8_t factor)
{
    assert(factor);

    if (factor == 1)
    {
        if (flags.cell_width == Byte) gen.mov (AL,  indirect(pointer_reg, flags.cell_width));
        if (flags.cell_width == Word) gen.mov (AX,  indirect(pointer_reg, flags.cell_width));
        if (flags.cell_width == DWord) gen.mov(EAX,  indirect(pointer_reg, flags.cell_width));
    }
    else             gen.imul(EAX, indirect(pointer_reg, flags.cell_width), Imm8{factor});

    if (flags.cell_width == Byte)
        gen.add(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), AL);
    if (flags.cell_width == Word)
        gen.add(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), AX);
    if (flags.cell_width == DWord)
        gen.add(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), EAX);

    needs_cmp = (offset != 0);
}
void BrainfuckGenerator::sub_copy(int8_t offset, int8_t factor)
{
    assert(factor);

    if (factor == 1)
    {
        if (flags.cell_width == Byte) gen.mov (AL,  indirect(pointer_reg, flags.cell_width));
        if (flags.cell_width == Word) gen.mov (AX,  indirect(pointer_reg, flags.cell_width));
        if (flags.cell_width == DWord) gen.mov(EAX,  indirect(pointer_reg, flags.cell_width));
    }
    else gen.imul(EAX, indirect(pointer_reg, flags.cell_width), Imm8{factor});

    if (flags.cell_width == Byte)
        gen.sub(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), AL);
    if (flags.cell_width == Word)
        gen.sub(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), AX);
    if (flags.cell_width == DWord)
        gen.sub(indirect_off(pointer_reg, offset*flags.cell_width, flags.cell_width), EAX);

    needs_cmp = (offset != 0);
}

GeneratedCode BrainfuckGenerator::retrieve_data() &&
{
    return {std::move(gen).data(), start_addr};
}

void BrainfuckGenerator::test_current_cell()
{
    if (!elide_cmp || needs_cmp)
    {
        // TODO : reduce when ModRM<width>
        if (flags.cell_width == Byte)
            gen.cmp(indirect(pointer_reg, flags.cell_width), (GPR8)zero_reg);
        else if (flags.cell_width == Word)
            gen.cmp(indirect(pointer_reg, flags.cell_width), (GPR16)zero_reg);
        else if (flags.cell_width == DWord)
            gen.cmp(indirect(pointer_reg, flags.cell_width), zero_reg);
    }

    needs_cmp = false;
}

void BrainfuckGenerator::define_putchar()
{
    putchar_addr = gen.current_index();

    // pointer_reg is ECX

    gen.mov(EAX, 0x4);
    gen.mov(EBX, STDOUT_FILENO);
    gen.mov(EDX, 1);

    gen.int_(0x80);

    gen.mov(zero_reg, 0);

    gen.ret();
}

void BrainfuckGenerator::define_getchar()
{
    getchar_addr = gen.current_index();

    // pointer_reg is ECX

    if (flags.cell_width != Byte) gen.mov(indirect(pointer_reg, flags.cell_width), Imm8{0});

    gen.mov(EAX, 0x3);
    gen.mov(EBX, STDIN_FILENO);
    gen.mov(EDX, 1);

    gen.int_(0x80);

    gen.mov(zero_reg, 0);

    gen.ret();
}

void BrainfuckGenerator::create_cell_array()
{
    // brk() allocated pages are automatically set to zero

    // Create a margin in order to allow add [ecx-0x28], 0 even for invalid addresses in order to
    // allow the copy loop optimization not to call loop_begin()/loop_end()
    const size_t margin = 0x100*flags.cell_width;

    start_addr = gen.current_index();

    gen.mov(AL, 0x2d); // get brk()
    gen.mov(BL, 0);

    gen.int_(0x80);

    gen.mov(pointer_reg, EAX); // set address
    gen.add(pointer_reg, Imm32{margin});

    gen.mov(EBX, EAX);
    gen.add(EBX, Imm32{flags.cell_amount*flags.cell_width + margin});
    gen.mov(EAX, 0x2d); // set brk

    gen.int_(0x80);
}

void BrainfuckGenerator::link_jump(size_t ins_pc, size_t target_pc)
{
    long int offset = target_pc - ins_pc;
    assert(offset > INT32_MIN && offset < INT32_MAX);

    int32_t* rel_jump_ptr = reinterpret_cast<int32_t*>(&gen.data()[ins_pc-4]);

    // change the placeholder relative offset to the computed offset
    *rel_jump_ptr = offset;
}

}
