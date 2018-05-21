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

namespace x86gen
{

void CodeGenerator::add(ModRM rm, Imm8 imm)
{
    rm.set_reg(0);

    if (rm.width == Byte) emit<uint8_t>(0x80);
    else
    {
        if (rm.width == Word) emit<uint8_t>(operand_size_override);
        if (rm.width == QWord) emit<uint8_t>(REX{1, RAX, RAX, rm.needs_rex?R8:RAX});
        emit<uint8_t>(0x83);
    }

    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::add(ModRM rm, Imm16 imm)
{
    rm.set_reg(0);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint16_t>(imm.val);
}

void CodeGenerator::add(ModRM rm, Imm32 imm)
{
    rm.set_reg(0);

    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint32_t>(imm.val);
}

void CodeGenerator::add(ModRM rm, GPR8 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(0x00);
    emit_modrm(rm);
}

void CodeGenerator::add(ModRM rm, GPR16 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x01);
    emit_modrm(rm);
}

void CodeGenerator::add(ModRM rm, GPR32 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(0x01);
    emit_modrm(rm);
}

void CodeGenerator::add(ModRM rm, GPR64 gpr)
{
    emit<uint8_t>(REX{1, gpr, RAX, rm.needs_rex?R8:RAX});

    rm.set_reg(gpr);

    emit<uint8_t>(0x01);
    emit_modrm(rm);
}

void CodeGenerator::add(GPR8 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x02);
    emit_modrm(rm);
}

void CodeGenerator::add(GPR16 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x03);
    emit_modrm(rm);
}

void CodeGenerator::add(GPR32 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x03);
    emit_modrm(rm);
}

void CodeGenerator::add(GPR64 reg, ModRM rm)
{
    emit<uint8_t>(REX{1, reg, RAX, rm.needs_rex?R8:RAX});

    rm.set_reg(reg);

    emit<uint8_t>(0x03);
    emit_modrm(rm);
}

void CodeGenerator::sub(ModRM rm, Imm8 imm)
{
    rm.set_reg(5);

    if (rm.width == Byte) emit<uint8_t>(0x80);
    else
    {
        if (rm.width == Word) emit<uint8_t>(operand_size_override);
        if (rm.width == QWord) emit<uint8_t>(REX{1, RAX, RAX, rm.needs_rex?R8:RAX});
        emit<uint8_t>(0x83);
    }

    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::sub(ModRM rm, Imm16 imm)
{
    rm.set_reg(5);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint16_t>(imm.val);
}

void CodeGenerator::sub(ModRM rm, Imm32 imm)
{
    rm.set_reg(5);

    emit<uint8_t>(0x81);
    emit_modrm(rm);
    emit<uint32_t>(imm.val);
}

void CodeGenerator::sub(ModRM rm, GPR8 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(0x28);
    emit_modrm(rm);
}

void CodeGenerator::sub(ModRM rm, GPR16 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x29);
    emit_modrm(rm);
}

void CodeGenerator::sub(ModRM rm, GPR32 gpr)
{
    rm.set_reg(gpr);

    emit<uint8_t>(0x29);
    emit_modrm(rm);
}

void CodeGenerator::sub(ModRM rm, GPR64 gpr)
{
    emit<uint8_t>(REX{1, gpr, RAX, rm.needs_rex?R8:RAX});

    rm.set_reg(gpr);

    emit<uint8_t>(0x29);
    emit_modrm(rm);
}

void CodeGenerator::sub(GPR8 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x2A);
    emit_modrm(rm);
}

void CodeGenerator::sub(GPR16 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x2B);
    emit_modrm(rm);
}

void CodeGenerator::sub(GPR32 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x2B);
    emit_modrm(rm);
}

void CodeGenerator::sub(GPR64 reg, ModRM rm)
{
    emit<uint8_t>(REX{1, reg, RAX, rm.needs_rex?R8:RAX});

    rm.set_reg(reg);

    emit<uint8_t>(0x2B);
    emit_modrm(rm);
}

}
