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

TEST(Mov, MovRM8R8)
{
    {
        CodeGenerator gen;
        gen.mov(AL, CL);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x88, 0xC8}));
    }
    {
        CodeGenerator gen;
        gen.mov(AH, CH);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x88, 0xEC}));
    }
    {
        CodeGenerator gen;
        gen.mov(BL, DH);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x88, 0xF3}));
    }
}

TEST(Mov, MovRM16R16)
{
    {
        CodeGenerator gen;
        gen.mov(AX, BX);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0x89, 0xD8}));
    }
}

TEST(Mov, MovRM32R32)
{
    {
        CodeGenerator gen;
        gen.mov(EAX, EBX);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x89, 0xD8}));
    }
}

TEST(Mov, MovR8RM8)
{
    {
        CodeGenerator gen;
        gen.mov(AL, ModRM{CL});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x8A, 0xC1}));
    }
    {
        CodeGenerator gen;
        gen.mov(AH, ModRM{CH});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x8A, 0xE5}));
    }
    {
        CodeGenerator gen;
        gen.mov(BL, ModRM{DH});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x8A, 0xDE}));
    }
}

TEST(Mov, MovR16RM16)
{
    {
        CodeGenerator gen;
        gen.mov(AX, ModRM{BX});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0x8B, 0xC3}));
    }
}

TEST(Mov, MovR32RM32)
{
    {
        CodeGenerator gen;
        gen.mov(EAX, EBX);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x89, 0xD8}));
    }
    {
        CodeGenerator gen;
        gen.mov(EAX, ModRM{0x5});
        EXPECT_EQ(ModRM{0x5}.displacement, 0x5);
        EXPECT_EQ(ModRM{0x5}.rm(), 0b101);
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x8B, 0x05, 0x05, 0x00, 0x00, 0x00}));
    }
}

TEST(Mov, MovR16Imm16)
{
    {
        CodeGenerator gen;
        gen.mov(AX, Imm16{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0xB8, 0x05, 0x00}));
    }
}

TEST(Mov, MovRM16Imm16)
{
    {
        CodeGenerator gen;
        gen.mov(indirect(EAX), Imm16{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{0x66, 0xC7, 0x00, 0x05, 0x00}));
    }
}

TEST(Mov, MovRM32Imm32)
{
    {
        CodeGenerator gen;
        gen.mov(SIB{EAX*1}, Imm32{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xC7, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00 } ));
    }
    {
        CodeGenerator gen;
        gen.mov(EAX*1, Imm32{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xC7, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00 } ));
    }
    {
        CodeGenerator gen;
        gen.mov(SIB{EAX*2+ECX}, Imm32{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xC7, 0x04, 0x41, 0x05, 0x00, 0x00, 0x00 }  ));
    }
    {
        CodeGenerator gen;
        gen.mov(ModRM{EAX*2+ECX-5-5}, Imm32{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xC7, 0x44, 0x41, 0xF6, 0x05, 0x00, 0x00, 0x00 }));
    }
    {
        CodeGenerator gen;
        gen.mov(EAX*2+ECX-5-5, Imm32{5});
        EXPECT_EQ(gen.data(), (std::vector<uint8_t>{ 0xC7, 0x44, 0x41, 0xF6, 0x05, 0x00, 0x00, 0x00 }));
    }
}
}
