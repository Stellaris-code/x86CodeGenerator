/*
addsub_tests.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

TEST(Add, R64)
{
    {
        CodeGenerator gen;
        gen.add(R12, ModRM{RAX});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4C, 0x03, 0xE0}));
    }
    {
        CodeGenerator gen;
        gen.add(R12, ModRM{R11});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4D, 0x03, 0xE3}));
    }

    {
        CodeGenerator gen;
        gen.add(ModRM{R12}, RAX);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x49, 0x01, 0xc4}));
    }
    {
        CodeGenerator gen;
        gen.add(ModRM{R12}, R11);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4D, 0x01, 0xDC}));
    }
    {
        CodeGenerator gen;
        gen.add(RAX, Imm8{0});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x48, 0x83, 0xC0, 0x00 } ));
    }
    {
        CodeGenerator gen;
        gen.add(R12, Imm8{0});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x49, 0x83, 0xC4, 0x00 } ));
    }
}

TEST(Inc, R64)
{
    {
        CodeGenerator gen;
        gen.inc(R12);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x49, 0xFF, 0xC4 }));
    }
    {
        CodeGenerator gen;
        gen.inc(indirect(RBX, Byte));
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xFE, 0x03 }));
    }
    // TODO
//    {
//        CodeGenerator gen;
//        gen.inc(indirect(R12, Byte));
//        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x41, 0xFE, 0x04, 0x24 } ));
//    }
    {
        CodeGenerator gen;
        gen.inc(indirect(R14, Byte));
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x41, 0xFE, 0x06 }));
    }
}

TEST(Dec, R64)
{
    {
        CodeGenerator gen;
        gen.dec(R12);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x49, 0xFF, 0xCC }));
    }
    {
        CodeGenerator gen;
        gen.dec(indirect(RBX, Byte));
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xFE, 0x0B }));
    }
}

TEST(Sub, R64)
{
    {
        CodeGenerator gen;
        gen.sub(R12, ModRM{RAX});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4C, 0x2B, 0xE0}));
    }
    {
        CodeGenerator gen;
        gen.sub(R12, ModRM{R11});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4D, 0x2B, 0xE3}));
    }

    {
        CodeGenerator gen;
        gen.sub(ModRM{R12}, RAX);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x49, 0x29, 0xc4}));
    }
    {
        CodeGenerator gen;
        gen.sub(ModRM{R12}, R11);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x4D, 0x29, 0xDC}));
    }
    {
        CodeGenerator gen;
        gen.sub(RAX, Imm8{0});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x48, 0x83, 0xE8, 0x00 } ));
    }
    {
        CodeGenerator gen;
        gen.sub(R12, Imm8{0});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0x49, 0x83, 0xEC, 0x00 } ));
    }
}
}


