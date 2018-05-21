/*
move_tests.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "gtest/gtest.h"

#include "codegenerator.hpp"

namespace
{
using namespace x86gen;

TEST(Lea, R16)
{
    {
        CodeGenerator gen;
        gen.lea(AX, SIB{2, EAX});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0x8D, 0x04, 0x45, 0x00, 0x00, 0x00, 0x00}));
    }
    {
        CodeGenerator gen;
        gen.lea(CX, SIB{4, EBP, 0x6});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0x8D, 0x0C, 0xAD, 0x06, 0x00, 0x00, 0x00}));
    }
    {
        CodeGenerator gen;
        gen.lea(EBP, {SIB{4, EBP, EAX}, 6});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x8D, 0xAC, 0xA8, 0x06, 0x00, 0x00, 0x00}));
    }
}
}
