/*
combined_gen_test.cpp

Copyright (c) 12 Yann BOUCHER (yann)

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

TEST(CombinedGen, BfInstructions)
{
    CodeGenerator gen;

    gen.inc(EDX); // >
    gen.dec(EDX); // <

    gen.inc(indirect(EDX, Byte)); // +
    gen.dec(indirect(EDX, Byte)); // -

    // .
    gen.push(indirect(EDX));
    gen.call(Rel32{0x00}); // putchar
    gen.sub(ESP, Imm8{4});

    // ,
    gen.call(Rel32{0x00});
    gen.mov(indirect(EDX), AL);

    // [
    gen.cmp(indirect(EDX, Byte), Imm8{0});
    gen.jcc(JZ, Rel8{0x00});

    // ]
    gen.cmp(indirect(EDX, Byte), Imm8{0});
    gen.jcc(JNZ, Rel8{0x00});

    EXPECT_EQ(gen.data(), std::vector<uint8_t>({ 0x42, 0x4A, 0xFE, 0x02, 0xFE, 0x0A, 0xFF, 0x32, 0xE8,
                                                 0x00, 0x00, 0x00, 0x00, 0x83, 0xEC, 0x04, 0xE8, 0x00,
                                                 0x00, 0x00, 0x00, 0x88, 0x02, 0x80, 0x3A, 0x00, 0x74,
                                                 0x00, 0x80, 0x3A, 0x00, 0x75, 0x00 } ));
}

}
