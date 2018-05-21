/*
pushpop.cpp

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

#include "codegenerator.hpp"

namespace x86gen
{

void CodeGenerator::push(ModRM rm)
{
    rm.set_reg(6);

    assert(rm.width != Byte);

    if (rm.width == Word) emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xFF);

    emit_modrm(rm);
}

void CodeGenerator::push(GPR16 reg)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x50 + reg);
}
void CodeGenerator::push(GPR32 reg)
{
    emit<uint8_t>(0x50 + reg);
}
void CodeGenerator::push(GPR64 reg)
{
    if (reg >= R8)
        emit<uint8_t>(REX{0, RAX, RAX, reg});

    emit<uint8_t>(0x50 + (reg&0b111));
}

void CodeGenerator::push(Imm8 imm)
{
    emit<uint8_t>(0x6A);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::push(Imm16 imm)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x68);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::push(Imm32 imm)
{
    emit<uint8_t>(0x68);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::pop(ModRM rm)
{
    assert(rm.width != Byte);

    rm.set_reg(0);

    if (rm.width == Word) emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x8F);
    emit_modrm(rm);
}

void CodeGenerator::pop(GPR16 reg)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x58 + reg);
}

void CodeGenerator::pop(GPR32 reg)
{
    emit<uint8_t>(0x58 + reg);
}

void CodeGenerator::pop(GPR64 reg)
{
    if (reg >= R8)
        emit<uint8_t>(REX{0, RAX, RAX, reg});

    emit<uint8_t>(0x58 + (reg&0b111));
}

}
