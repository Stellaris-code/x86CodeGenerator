/*
bf_generator.hpp

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
#ifndef BF_GENERATOR_HPP
#define BF_GENERATOR_HPP

#include "codegenerator.hpp"

#include <array>
#include <stack>

namespace bfgen
{

struct GeneratedCode
{
    GeneratedCode(const GeneratedCode&) = delete;
    GeneratedCode & operator=(const GeneratedCode&) = delete;

    GeneratedCode(GeneratedCode&&) = default;
    GeneratedCode & operator=(GeneratedCode&&) = default;

    std::vector<uint8_t> code;
    uint32_t start_address;
};

struct BuildFlags
{
    bool hash_is_breakpoint { true };
    enum OptLevel
    {
        None,
        RunLength,
        All
    } opt_level = All;
    x86gen::PtrWidth cell_width { x86gen::Byte };
    size_t cell_amount { 0x10000 };
};

class BrainfuckGenerator
{
public:
    BrainfuckGenerator(BuildFlags i_flags = {});

    void prologue();

    // elementary ops

    void move_forwards(size_t off = 1);
    void move_backwards(size_t off = 1);
    void increment(size_t off = 1);
    void decrement(size_t off = 1);
    void print();
    void get_input();
    void loop_begin();
    void loop_end();

    // optimized ops

    void scan(int8_t granularity = 1);
    void set(int8_t offset, uint32_t val);
    void add(int8_t offset, int32_t val);
    void add_copy(int8_t offset, int8_t factor);
    void sub_copy(int8_t offset, int8_t factor);

    void breakpoint();

    void epilogue();

    GeneratedCode retrieve_data() &&;

    int min_offset() const
    { return INT8_MIN; }
    int max_offset() const
    { return INT8_MAX; }
    unsigned long max_value() const
    { return (0x1ul << (flags.cell_width*8)) - 1; }
    long min_value() const
    { return -max_value()-1; }

public:
    const BuildFlags flags;

private:
    void test_current_cell();

    void define_putchar();
    void define_getchar();

    void create_cell_array();

    void call_function(uint32_t addr);

    enum LabelType
    {
        Start,
        End
    };

    void link_jump(size_t ins_pc, size_t target_pc);

private:
    struct LinkEntry
    {
        size_t ins_idx;
        size_t label;
        LabelType type;

        LinkEntry(size_t in_idx, size_t in_lbl, LabelType in_type) : ins_idx(in_idx), label(in_lbl), type(in_type) {}
    };

    uint32_t putchar_addr;
    uint32_t getchar_addr;

    uint32_t start_addr;

    std::stack<size_t> bracket_stack;

    bool needs_cmp { true };
    x86gen::CodeGenerator gen;

public:
};

}

#endif // BF_GENERATOR_HPP
