#include "gtest/gtest.h"

#include "codegenerator.hpp"
#include "generic_interface.hpp"

namespace {
using namespace x86gen;

TEST(GenericInterface, ZeroArgInstr) {
       bool flag = true;
       CodeGenerator gen;

       try {
              gen << ud2;
       } catch(std::exception &e) {
              flag = false;
       }

       EXPECT_EQ(flag, true);

       CodeGenerator genref;
       genref.ud2();

       EXPECT_EQ(genref.data(), gen.data());
}

TEST(GenericInterface, OneArgInstr) {
       bool flag = true;
       CodeGenerator gen;

       try {
              gen << jmp << ModRM(EAX);
       } catch(std::exception &e) {
              flag = false;
       }

       EXPECT_EQ(flag, true);

       CodeGenerator genref;
       genref.jmp(ModRM(EAX));

       EXPECT_EQ(genref.data(), gen.data());
}

TEST(GenericInterface, TwoArgInstr) {
       bool flag = true;
       CodeGenerator gen;

       try {
              gen << add << EAX << ModRM(EAX);
       } catch(std::exception &e) {
              flag = false;
       }

       EXPECT_EQ(flag, true);

       CodeGenerator genref;
       genref.add(EAX, ModRM(EAX));

       EXPECT_EQ(genref.data(), gen.data());
}

TEST(GenericInterface, ThreeArgInstr) {
       bool flag = true;
       CodeGenerator gen;

       try {
              gen << imul3 << AX << ModRM(EAX) << Imm8{1};
       } catch (std::exception &e) {
              flag = false;
       }

       EXPECT_EQ(flag, true);

       CodeGenerator genref;
       genref.imul(AX, ModRM(EAX), 1);

       EXPECT_EQ(genref.data(), gen.data());
}

TEST(GenericInterface, InvalidArgs) {
       bool flag = false;
       CodeGenerator gen;

       try {
              gen << jmp << EAX;
       } catch(std::runtime_error &e) {
              flag = true;
       }

       EXPECT_EQ(flag, true);
}

}
