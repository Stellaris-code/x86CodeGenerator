/*
calljmp.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "codegenerator.hpp"

namespace x86gen
{

void CodeGenerator::call(Rel16 rel)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xE8);
    emit<int16_t>(rel.val);
}

void CodeGenerator::call(Rel32 rel)
{
    emit<uint8_t>(0xE8);
    emit<int32_t>(rel.val);
}

void CodeGenerator::call(ModRM rm)
{
    assert(rm.width != Byte);

    rm.set_reg(2);

    if (rm.width == Word) emit<uint8_t>(operand_size_override);

    emit<uint8_t>(0xFF);

    emit_modrm(rm);
}

void CodeGenerator::jmp(Rel8 rel)
{
    emit<uint8_t>(0xEB);
    emit<int8_t>(rel.val);
}

void CodeGenerator::jmp(Rel16 rel)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xE9);
    emit<int16_t>(rel.val);
}

void CodeGenerator::jmp(Rel32 rel)
{
    emit<uint8_t>(0xE9);
    emit<int32_t>(rel.val);
}

void CodeGenerator::jmp(ModRM rm)
{
    assert(rm.width != Byte);

    rm.set_reg(4);

    if (rm.width == Word) emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xFF);

    emit_modrm(rm);
}

void CodeGenerator::jcc(JumpCond cond, Rel8 rel)
{
    emit<uint8_t>(0x70 + cond);
    emit<int8_t>(rel.val);
}

void CodeGenerator::jcc(JumpCond cond, Rel16 rel)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x0F);
    emit<uint8_t>(0x80 + cond);
    emit<int16_t>(rel.val);
}

void CodeGenerator::jcc(JumpCond cond, Rel32 rel)
{
    emit<uint8_t>(0x0F);
    emit<uint8_t>(0x80 + cond);
    emit<int32_t>(rel.val);
}

}
