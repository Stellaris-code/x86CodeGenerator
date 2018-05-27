/*
sub.cpp

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

#include "codegenerator.hpp"
// TODO : DEFINE_ALU_OP
namespace x86gen
{

void CodeGenerator::xor_(ModRM rm, Imm8 imm)
{
    rm.set_reg(6);

    if (rm.width == Byte) emit<uint8_t>(0x80);
    else
    {
        if (rm.width == Word) emit<uint8_t>(operand_size_override);
        emit<uint8_t>(0x83);
    }

    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::xor_(ModRM rm, Imm16 imm)
{
    assert(rm.width == Word);

    rm.set_reg(6);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint16_t>(imm.val);
}

void CodeGenerator::xor_(ModRM rm, Imm32 imm)
{
    assert(rm.width == DWord);

    rm.set_reg(6);

    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint32_t>(imm.val);
}

void CodeGenerator::xor_(ModRM rm, GPR8 gpr)
{
    assert(rm.width == Byte);

    rm.set_reg(gpr);

    emit<uint8_t>(0x30);
    emit_modrm(rm);
}

void CodeGenerator::xor_(ModRM rm, GPR16 gpr)
{
    assert(rm.width == Word);

    rm.set_reg(gpr);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x31);
    emit_modrm(rm);
}

void CodeGenerator::xor_(ModRM rm, GPR32 gpr)
{
    assert(rm.width == DWord);

    rm.set_reg(gpr);

    emit<uint8_t>(0x31);
    emit_modrm(rm);
}

void CodeGenerator::xor_(GPR8 reg, ModRM rm)
{
    assert(rm.width == Byte);

    rm.set_reg(reg);

    emit<uint8_t>(0x32);
    emit_modrm(rm);
}

void CodeGenerator::xor_(GPR16 reg, ModRM rm)
{
    assert(rm.width == Word);

    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x33);
    emit_modrm(rm);
}

void CodeGenerator::xor_(GPR32 reg, ModRM rm)
{
    assert(rm.width == DWord);

    rm.set_reg(reg);

    emit<uint8_t>(0x33);
    emit_modrm(rm);
}

}
