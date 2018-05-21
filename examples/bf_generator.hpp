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
    bool bang_is_breakpoint { false };
};

class BrainfuckGenerator
{
public:
    BrainfuckGenerator(BuildFlags = {});

    void prologue();

    void move_forwards(size_t off = 1);
    void move_backwards(size_t off = 1);
    void increment(size_t off = 1);
    void decrement(size_t off = 1);
    void set(uint8_t val);
    void print();
    void get_input();
    void loop_begin();
    void loop_end();

    void breakpoint();

    void epilogue();

    GeneratedCode retrieve_data() &&;

private:
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
    const BuildFlags m_flags;

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
