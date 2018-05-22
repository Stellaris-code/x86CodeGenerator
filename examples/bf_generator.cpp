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

#define DECLARE_ARITH_OP(single, multi, single_addressing, multi_addressing) \
    if (off == 1) \
{ \
    gen.single(single_addressing); \
    } \
    else \
{ \
    if (off <= (uint8_t)-1) \
    gen.multi(multi_addressing, Imm8{(uint8_t)off}); \
    else if (off <= (uint16_t)-1) \
    gen.multi(multi_addressing, Imm16{(uint16_t)off}); \
    else if (off <= (uint32_t)-1) \
    gen.multi(multi_addressing, Imm32{(uint32_t)off}); \
    else \
    assert(!"Increment offset is too large"); \
    }

namespace bfgen
{

using namespace x86gen;

constexpr auto pointer_reg = ECX;

BrainfuckGenerator::BrainfuckGenerator(BuildFlags flags)
    : m_flags(flags)
{
    define_putchar();
    define_getchar();
}

void BrainfuckGenerator::prologue()
{
    create_cell_array();
}

void BrainfuckGenerator::move_forwards(size_t off)
{
    DECLARE_ARITH_OP(inc, add, ModRM{pointer_reg}, pointer_reg);

    needs_cmp = true;
}

void BrainfuckGenerator::move_backwards(size_t off)
{
    DECLARE_ARITH_OP(dec, sub, ModRM{pointer_reg}, pointer_reg);

    needs_cmp = true;
}

void BrainfuckGenerator::increment(size_t off)
{
    DECLARE_ARITH_OP(inc, add, indirect(pointer_reg, Byte), indirect(pointer_reg, Byte));

    // We don't need to do a cmp after an increment of decrement op, it changes the ZF accordingly to *ptr
    needs_cmp = false;
}

void BrainfuckGenerator::decrement(size_t off)
{
    DECLARE_ARITH_OP(dec, sub, indirect(pointer_reg, Byte), indirect(pointer_reg, Byte));

    // We don't need to do a cmp after an increment of decrement op, it changes the ZF accordingly to *ptr
    needs_cmp = false;
}

void BrainfuckGenerator::set(uint8_t val)
{
    gen.mov(indirect(pointer_reg, Byte), Imm8{val});
}

void BrainfuckGenerator::print()
{
    call_function(putchar_addr);

    needs_cmp = true;
}

void BrainfuckGenerator::get_input()
{
    call_function(getchar_addr);

    needs_cmp = true;
}

void BrainfuckGenerator::call_function(uint32_t addr)
{
    int32_t offset = addr - (gen.current_index() + 5); // max size of the call instruction

    gen.call(Rel32{offset});
}

void BrainfuckGenerator::loop_begin()
{
    if (needs_cmp) gen.cmp(indirect(pointer_reg, Byte), Imm8{0});

    gen.jcc(JZ, Rel32{0});

    bracket_stack.emplace(gen.current_index());

    needs_cmp = true;
}

void BrainfuckGenerator::loop_end()
{
    assert(!bracket_stack.empty()); // unmatched bracket

    if (needs_cmp) gen.cmp(indirect(pointer_reg, Byte), Imm8{0});

    size_t opening_bracket_pc = bracket_stack.top();

    gen.jcc(JNZ, Rel32{0});

    link_jump(gen.current_index(), opening_bracket_pc);
    link_jump(opening_bracket_pc, gen.current_index());

    bracket_stack.pop();

    needs_cmp = true;
}

void BrainfuckGenerator::breakpoint()
{
    gen.int3();
}

void BrainfuckGenerator::epilogue()
{
#if 1
    gen.mov(EAX, 0x1); // SYS_EXIt
    gen.mov(EBX, 0); // exit(0)
    gen.int_(0x80);
#else
    gen.ret();
#endif
}

GeneratedCode BrainfuckGenerator::retrieve_data() &&
{
    return {std::move(gen).data(), start_addr};
}

void BrainfuckGenerator::define_putchar()
{
    putchar_addr = gen.current_index();

    // pointer_reg is ECX

    gen.mov(EAX, 0x4);
    gen.mov(EBX, STDOUT_FILENO);
    gen.mov(EDX, 1);

    gen.int_(0x80);

    gen.ret();
}

void BrainfuckGenerator::define_getchar()
{
    getchar_addr = gen.current_index();

    // pointer_reg is ECX

    gen.mov(EAX, 0x3);
    gen.mov(EBX, STDIN_FILENO);
    gen.mov(EDX, 1);

    gen.int_(0x80);

    gen.ret();
}

void BrainfuckGenerator::create_cell_array()
{
    //    void* cell_array_ptr = mmap(nullptr, 0x10000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_32BIT, 0, 0);

    //    if (cell_array_ptr == (void*)-1)
    //    {
    //        perror("error on cell array allocation");
    //        abort();
    //    }

    start_addr = gen.current_index();

    gen.mov(EAX, 0x2d); // get brk()
    gen.mov(EBX, 0);

    gen.int_(0x80);

    gen.mov(pointer_reg, EAX); // set address

    gen.mov(EBX, EAX);
    gen.add(EBX, Imm32{0x10000});
    gen.mov(EAX, 0x2d); // set brk

    gen.int_(0x80);
}

void BrainfuckGenerator::link_jump(size_t ins_pc, size_t target_pc)
{
    int32_t offset = target_pc - ins_pc;

    int32_t* rel_jump_ptr = reinterpret_cast<int32_t*>(&gen.data()[ins_pc-4]);

    // change the placeholder relative offset to the computed offset
    *rel_jump_ptr = offset;
}

}
