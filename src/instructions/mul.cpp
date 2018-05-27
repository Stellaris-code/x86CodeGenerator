/*
mul.cpp

Copyright (c) 24 Yann BOUCHER (yann)

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

void CodeGenerator::imul(GPR16 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x0F);
    emit<uint8_t>(0xAF);
    emit_modrm(rm);
}

void CodeGenerator::imul(GPR32 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x0F);
    emit<uint8_t>(0xAF);
    emit_modrm(rm);
}

void CodeGenerator::imul(GPR16 reg, ModRM rm, Imm8 imm)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x6B);
    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::imul(GPR32 reg, ModRM rm, Imm8 imm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x6B);
    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::imul(GPR16 reg, Imm8 imm)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x6B);
    emit_modrm(reg);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::imul(GPR32 reg, Imm8 imm)
{
    emit<uint8_t>(0x6B);
    emit_modrm(reg);
    emit<uint8_t>(imm.val);
}

}
