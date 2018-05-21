/*
mov.cpp

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

void CodeGenerator::mov(ModRM rm, GPR8 reg)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x88);
    emit_modrm(rm);
}

void CodeGenerator::mov(ModRM rm, GPR16 reg)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x89);
    emit_modrm(rm);
}

void CodeGenerator::mov(ModRM rm, GPR32 reg)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x89);
    emit_modrm(rm);
}

void CodeGenerator::mov(GPR8 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x8A);
    emit_modrm(rm);
}

void CodeGenerator::mov(GPR16 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0x8B);
    emit_modrm(rm);
}

void CodeGenerator::mov(GPR32 reg, ModRM rm)
{
    rm.set_reg(reg);

    emit<uint8_t>(0x8B);
    emit_modrm(rm);
}

// wrappers
void CodeGenerator::mov(GPR8 r1, GPR8 r2)
{
    mov(ModRM{r1}, r2);
}
void CodeGenerator::mov(GPR16 r1, GPR16 r2)
{
    mov(ModRM{r1}, r2);
}
void CodeGenerator::mov(GPR32 r1, GPR32 r2)
{
    mov(ModRM{r1}, r2);
}

void CodeGenerator::mov(GPR8 reg, Imm8 imm)
{
    emit<uint8_t>(0xB0 + reg);
    emit<uint8_t>(imm.val);
}
void CodeGenerator::mov(GPR16 reg, Imm16 imm)
{
    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xB8 + reg);
    emit<uint16_t>(imm.val);
}
void CodeGenerator::mov(GPR32 reg, Imm32 imm)
{
    emit<uint8_t>(0xB8 + reg);
    emit<uint32_t>(imm.val);
}

void CodeGenerator::mov(GPR64 reg, Imm64 imm)
{
    emit<uint8_t>(REX{1, RAX, RAX, reg});
    emit<uint8_t>(0xB8 + (reg&0b111));
    emit<uint64_t>(imm.val);
}

void CodeGenerator::mov(ModRM rm, Imm8 imm)
{
    rm.set_reg(0);

    emit<uint8_t>(0xC6);
    emit_modrm(rm);
    emit<uint8_t>(imm.val);
}

void CodeGenerator::mov(ModRM rm, Imm16 imm)
{
    rm.set_reg(0);

    emit<uint8_t>(operand_size_override);
    emit<uint8_t>(0xC7);
    emit_modrm(rm);
    emit<uint16_t>(imm.val);
}

void CodeGenerator::mov(ModRM rm, Imm32 imm)
{
    rm.set_reg(0);

    emit<uint8_t>(0xC7);
    emit_modrm(rm);
    emit<uint32_t>(imm.val);
}

}
