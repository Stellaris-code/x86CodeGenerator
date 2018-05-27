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

#include "bf_translator.hpp"

#include "bf_peephole.hpp"

#include <iostream>

namespace bfgen
{

void check_matching_brackets(std::string_view input)
{
    size_t last_bracket_pos { 0 };

    int bracket_counter { 0 };
    for (size_t i { 0 }; i < input.size(); ++i)
    {
        const char c = input[i];

        if (c == '[')
        {
            ++bracket_counter;
            last_bracket_pos = i;
        }
        else if (c == ']')
        {
            --bracket_counter;
            if (bracket_counter < 0)
            {
                compilation_error_throw("Unmatched ']' bracket", i+1);
            }
        }
    }

    if (bracket_counter != 0)
    {
        compilation_error_throw("Unmatched '[' bracket", last_bracket_pos+1);
    }
}

GeneratedCode translate_bf(std::string_view input, BuildFlags flags)
{
    BrainfuckGenerator gen(flags);

    gen.prologue();

    for (size_t i { 0 }; i < input.size();)
    {
        auto do_lookahead = [flags, &gen, &input, &i](auto fun, char c)
        {
            if (flags.opt_level >= BuildFlags::RunLength)
            {
                size_t counter { 0 };
                do
                {
                    ++counter;
                } while (input[++i] == c);
                (gen.*fun)(counter);
            }
            else
            {
                (gen.*fun)(1);
                ++i;
            }
        };


        if (flags.opt_level == BuildFlags::All)
        {
            std::string_view atom = input.substr(i, input.find_first_of("[]", i+1)-i+1);
            int offset = apply_peephole_optimization(gen, atom);
            if (offset != -1) // could optimize, skip to next instruction
            {
                i += offset;
                continue;
            }
        }

        switch (input[i])
        {
            case '+':
                do_lookahead(&BrainfuckGenerator::increment, '+');
                continue;
            case '-':
                do_lookahead(&BrainfuckGenerator::decrement, '-');
                continue;
            case '>':
                do_lookahead(&BrainfuckGenerator::move_forwards, '>');
                continue;
            case '<':
                do_lookahead(&BrainfuckGenerator::move_backwards, '<');
                continue;
            case ',':
                gen.get_input(); break;
            case '.':
                gen.print(); break;
            case '[':
                gen.loop_begin(); break;
            case ']':
                gen.loop_end(); break;
            case '#':
                if (flags.hash_is_breakpoint)
                    gen.breakpoint();
                break;
        }

        ++i;
    }

    gen.epilogue();

    return std::move(gen).retrieve_data();
}

GeneratedCode compile_bf(std::string_view input, BuildFlags flags)
{
    check_matching_brackets(input);

    return translate_bf(input, flags);
}

}
