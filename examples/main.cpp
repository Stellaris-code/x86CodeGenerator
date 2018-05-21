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

#include "elf_builder.hpp"

int main(int argc, char* argv[])
{
    std::string input_file;

    if (argc < 2) return 1;
    else
    {
        input_file = std::string(argv[1]);
    }

    std::ifstream t(input_file);
    if (!t)
    {
        return 2;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    //auto data = bfgen::compile_bf("[]");
    auto data = bfgen::compile_bf(str);
    const auto& vec = data.code;
    for (auto b : vec)
    {
        printf("%02x ", b);
    }
    printf("\n");
    std::cout << "\nOutput size : " << vec.size() << "\n";
    std::cout << "PC : " << std::hex << data.start_address << "\n";

    //((void(*)())&vec[data.start_address])();

    auto elf_vec = bfgen::build_elf_executable(vec, data.start_address);

    std::ofstream out("out.bin");

}
