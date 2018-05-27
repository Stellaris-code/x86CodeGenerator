/*
main.cpp

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

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "elf_builder.hpp"

int main(int argc, char* argv[])
{
    std::string input_file;

    if (argc < 2)
    {
        std::cerr << "Invalid argument amount\n";
        return -1;
    }
    else
    {
        input_file = std::string(argv[1]);
    }

    bfgen::BuildFlags flags;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i] == std::string("-h") || argv[i] == std::string("--help"))
        {
            std::cout << "Usage : \n";
            std::cout << "\tbfcomp <input_file> [options]\n";
            std::cout << "\t -O(0,1,2) : optimization level (default : 2)\n";
            std::cout << "\t -b : treat '#' characters as trap breakpoints (default : off)\n";
            std::cout << "\t -w <width> : set cell byte width (1, 2 or 4) (default 1) \n";
            std::cout << "\t -s <size> : set cell array size (default 30000)\n";
            return 0;
        }

        if (argv[i] == std::string("-O0"))
        {
            flags.opt_level = bfgen::BuildFlags::None;
        }
        else if (argv[i] == std::string("-O1"))
        {
            flags.opt_level = bfgen::BuildFlags::RunLength;
        }
        else if (argv[i] == std::string("-O2") || argv[i] == std::string("-O3") || argv[i] == std::string("-Os"))
        {
            flags.opt_level = bfgen::BuildFlags::All;
        }

        if (argv[i] == std::string("-b"))
        {
            flags.hash_is_breakpoint = true;
        }

        if (argv[i] == std::string("-w"))
        {
            if (i+1 >= argc)
            {
                std::cerr << "No cell width specified\n";
                return -2;
            }
            int size = std::stoi(argv[i+1]);
            if (size != 1 && size != 2 && size != 4)
            {
                std::cerr << "Invalid cell width specified\n";
                return -2;
            }

            flags.cell_width = (x86gen::PtrWidth)size;

            ++i;
        }

        if (argv[i] == std::string("-s"))
        {
            if (i+1 >= argc)
            {
                std::cerr << "No cell array size specified\n";
                return -2;
            }
            int size = std::stoi(argv[i+1]);
            if (size <= 0)
            {
                std::cerr << "Invalid cell array size specified\n";
                return -2;
            }

            flags.cell_amount = size;

            ++i;
        }
    }

    std::ifstream t(input_file);
    if (!t)
    {
        return 2;
    }
    std::string unfiltered_str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    std::string filtered_str;

    // filter illegal characters
    std::copy_if(unfiltered_str.begin(), unfiltered_str.end(), std::back_inserter(filtered_str),
                 [](char c) { return std::string(1,c).find_first_not_of("><+-[].,#") == std::string::npos; });

    auto data = bfgen::compile_bf(filtered_str, flags);
    const auto& vec = data.code;

    std::cout << "Compiled " << vec.size() << " bytes to file out.bin\n";

    auto elf_vec = bfgen::build_elf_executable(vec, data.start_address);

    std::ofstream out("out.bin");

    for (auto b : elf_vec)
    {
        out << b;
    }
}
