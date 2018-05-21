/*  inputreader_tests.cpp %{Cpp:License:ClassName} - Yann BOUCHER (yann) 20
**
**
**            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
**                    Version 2, December 2004
**
** Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
**
** Everyone is permitted to copy and distribute verbatim or modified
** copies of this license document, and changing it is allowed as long
** as the name is changed.
**
**            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
**   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
**
**  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "gtest/gtest.h"

#include "addressing.hpp"

// TODO : test more the REX and all

namespace
{
using namespace x86gen;

TEST(SIB, Normal)
{
    EXPECT_EQ((SIB{1, EAX, EAX}), 0x0);
    EXPECT_EQ((SIB{EAX*1+EAX}), 0x0);
    EXPECT_EQ((SIB{1*EAX+EAX}), 0x0);
    EXPECT_EQ((SIB{EAX+EAX}), 0x0);
    EXPECT_EQ((SIB{1, EAX, ECX}), 0x01);
    EXPECT_EQ((SIB{1, EAX, ESP}), 0x04);
    EXPECT_EQ((SIB{ESP+EAX*1}), 0x04);

    EXPECT_EQ((SIB{1, EDX, EAX}), 0x10);
    EXPECT_EQ((SIB{1, EDX, ECX}), 0x11);
    EXPECT_EQ((SIB{1, EDX, ESP}), 0x14);

    EXPECT_EQ((SIB{2, EAX, EAX}), 0x40);
    EXPECT_EQ((SIB{2, EAX, ECX}), 0x41);
    EXPECT_EQ((SIB{2, EAX, ESP}), 0x44);

    EXPECT_EQ((SIB{2, EDX, EAX}), 0x50);
    EXPECT_EQ((SIB{2, EDX, ECX}), 0x51);
    EXPECT_EQ((SIB{2, EDX, ESP}), 0x54);

    EXPECT_EQ((SIB{4, EAX, EAX}), 0x80);
    EXPECT_EQ((SIB{4, EAX, ECX}), 0x81);
    EXPECT_EQ((SIB{4, EAX, ESP}), 0x84);

    EXPECT_EQ((SIB{4, EDX, EAX}), 0x90);
    EXPECT_EQ((SIB{4, EDX, ECX}), 0x91);
    EXPECT_EQ((SIB{4, EDX, ESP}), 0x94);
}

TEST(SIB, Disp)
{
    EXPECT_EQ((SIB{2, EAX, 32}), 0x45);
    EXPECT_EQ((SIB{2, EAX, -32}), 0x45);

    EXPECT_EQ((SIB{4, EDX, 64}), 0x95);
    EXPECT_FALSE((SIB{4, EDX, -64}.rel_to_ebp));
    EXPECT_EQ((SIB{4, EDX, -64}.disp), -64);
    EXPECT_EQ((SIB{4, EDX, -64, SIB_EBP{}}.disp), -64);
    EXPECT_TRUE((SIB{4, EDX, -64, SIB_EBP{}}.rel_to_ebp));
    EXPECT_EQ((SIB{4, EDX, -640, SIB_EBP{}}.disp), -640);
    EXPECT_TRUE((SIB{4, EDX, -640, SIB_EBP{}}.rel_to_ebp));
}

TEST(ModRM, Mod00)
{
    EXPECT_EQ((ModRM{IndirectReg{}, EAX, DWord}).modrm, 0x00);
    EXPECT_EQ((ModRM{IndirectReg{}, EBX, DWord}).modrm, 0x03);
    EXPECT_DEATH((ModRM{IndirectReg{}, ESP, DWord}), "Assertion");
    EXPECT_EQ((ModRM{-32}).modrm, 0x05);
    EXPECT_EQ((ModRM{-32}).displacement, -32);
    EXPECT_EQ((ModRM{SIB{4, EDX, 64}}).modrm, 0x04);
    EXPECT_EQ((ModRM{SIB{4, EDX, 64}}).sib, 0x95);
    EXPECT_EQ((ModRM{SIB{4, EDX, 64}}).displacement, 64);

    (ModRM{EAX*2+32});
    (ModRM{EAX*2-32});
    (ModRM{EAX*2-16-16});
    EXPECT_DEATH((ModRM{EAX*3-16-16}), "Assertion");
}

TEST(ModRM, Reg)
{
    EXPECT_EQ((ModRM{EAX}).modrm, 0xC0);
    EXPECT_EQ((ModRM{AX}).modrm, 0xC0);
    EXPECT_EQ((ModRM{AL}).modrm, 0xC0);

    EXPECT_EQ((ModRM{EBX}).modrm, 0xC3);
    EXPECT_EQ((ModRM{BX}).modrm, 0xC3);
    EXPECT_EQ((ModRM{BL}).modrm, 0xC3);

    EXPECT_EQ((ModRM{ESP}).modrm, 0xC4);
    EXPECT_EQ((ModRM{SP}).modrm, 0xC4);
    EXPECT_EQ((ModRM{AH}).modrm, 0xC4);
}
}
