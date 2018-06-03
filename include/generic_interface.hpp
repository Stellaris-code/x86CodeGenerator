#include <vector>
#include <array>
#include <functional>
#include <utility>
#include "codegenerator.hpp"

#pragma once

namespace x86gen {

enum operand_type_ID {
       Imm8_ = 0,
       Imm16_,   // 1
       Imm32_,   // 2
       Rel8_,    // 3
       Rel16_,   // 4
       Rel32_,   // 5
       GPR8_,    // 6
       GPR16_,   // 7
       GPR32_,   // 8
       ModRM_,   // 9
       JumpCond_ // A
};

// Add as needed following the pattern

constexpr Opcode std         = {0, &CodeGenerator::general_std};
constexpr Opcode cld         = {0, &CodeGenerator::general_cld};
constexpr Opcode repe_scasb  = {0, &CodeGenerator::general_repe_scasb};
constexpr Opcode repe_scasw  = {0, &CodeGenerator::general_repe_scasw};
constexpr Opcode repe_scasd  = {0, &CodeGenerator::general_repe_scasd};
constexpr Opcode repne_scasb = {0, &CodeGenerator::general_repne_scasb};
constexpr Opcode repne_scasw = {0, &CodeGenerator::general_repne_scasw};
constexpr Opcode repne_scasd = {0, &CodeGenerator::general_repne_scasd};
constexpr Opcode call        = {1, &CodeGenerator::general_call};
constexpr Opcode ret         = {0, &CodeGenerator::general_ret};
constexpr Opcode far_ret     = {0, &CodeGenerator::general_far_ret};
constexpr Opcode cmp         = {2, &CodeGenerator::general_cmp};
constexpr Opcode sub         = {2, &CodeGenerator::general_sub};
constexpr Opcode add         = {2, &CodeGenerator::general_add};
constexpr Opcode xor_        = {2, &CodeGenerator::general_xor};
constexpr Opcode imul2       = {2, &CodeGenerator::general_imul2};
constexpr Opcode imul3       = {3, &CodeGenerator::general_imul3};
constexpr Opcode jmp         = {1, &CodeGenerator::general_jmp};
constexpr Opcode jcc         = {2, &CodeGenerator::general_jcc};
constexpr Opcode mov         = {2, &CodeGenerator::general_mov};
constexpr Opcode lea         = {2, &CodeGenerator::general_lea};
constexpr Opcode push        = {1, &CodeGenerator::general_push};
constexpr Opcode pop         = {1, &CodeGenerator::general_pop};
constexpr Opcode inc         = {1, &CodeGenerator::general_inc};
constexpr Opcode dec         = {1, &CodeGenerator::general_dec};
constexpr Opcode int3        = {3, &CodeGenerator::general_int3};
constexpr Opcode int_        = {1, &CodeGenerator::general_int};
constexpr Opcode ud2         = {0, &CodeGenerator::general_ud2};


constexpr static std::array<std::array<int, 0>, 1> valid_args_std         = {{ {{} }}};
constexpr static std::array<std::array<int, 0>, 1> valid_args_cld         = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repe_scasb  = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repe_scasw  = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repe_scasd  = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repne_scasb = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repne_scasw = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_repne_scasd = {{ {{}} }};

constexpr static std::array<std::array<int, 1>, 3> valid_args_call = {{
       {{operand_type_ID::Rel16_}},
       {{operand_type_ID::Rel32_}},
       {{operand_type_ID::ModRM_}} }};

constexpr static std::array<std::array<int, 0>, 1> valid_args_ret     = {{ {{}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_far_ret = {{ {{}} }};

constexpr static std::array<std::array<int, 2>, 9> valid_args_cmp = {{
       {{operand_type_ID::ModRM_, operand_type_ID::Imm8_ }},
       {{operand_type_ID::ModRM_, operand_type_ID::Imm16_}},
       {{operand_type_ID::ModRM_, operand_type_ID::Imm32_}},
       {{operand_type_ID::ModRM_, operand_type_ID::GPR8_ }},
       {{operand_type_ID::ModRM_, operand_type_ID::GPR16_}},
       {{operand_type_ID::ModRM_, operand_type_ID::GPR32_}},
       {{operand_type_ID::GPR8_ , operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR32_, operand_type_ID::ModRM_}} }};

// Some opcodes share their allowed arguments.
constexpr static std::array<std::array<int, 2>, 9> valid_args_sub  = valid_args_cmp;
constexpr static std::array<std::array<int, 2>, 9> valid_args_add  = valid_args_sub;
constexpr static std::array<std::array<int, 2>, 9> valid_args_xor_ = valid_args_add;

constexpr static std::array<std::array<int, 2>, 4> valid_args_imul2 = {{
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR32_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_, operand_type_ID::Imm8_ }},
       {{operand_type_ID::GPR32_, operand_type_ID::Imm8_ }} }};

constexpr static std::array<std::array<int, 3>, 2> valid_args_imul3 = {{
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_, operand_type_ID::Imm8_}},
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_, operand_type_ID::Imm8_}} }};

constexpr static std::array<std::array<int, 1>, 4> valid_args_jmp = {{
       {{operand_type_ID::Rel8_ }},
       {{operand_type_ID::Rel16_}},
       {{operand_type_ID::Rel32_}},
       {{operand_type_ID::ModRM_}} }};

constexpr static std::array<std::array<int, 2>, 3> valid_args_jcc = {{
       {{operand_type_ID::JumpCond_, operand_type_ID::Rel8_ }},
       {{operand_type_ID::JumpCond_, operand_type_ID::Rel16_}},
       {{operand_type_ID::JumpCond_, operand_type_ID::Rel32_}} }};

constexpr static std::array<std::array<int, 2>, 15> valid_args_mov = {{
       {{operand_type_ID::ModRM_, operand_type_ID::GPR8_ }},
       {{operand_type_ID::ModRM_, operand_type_ID::GPR16_}},
       {{operand_type_ID::ModRM_, operand_type_ID::GPR32_}},
       {{operand_type_ID::ModRM_, operand_type_ID::Imm8_ }},
       {{operand_type_ID::ModRM_, operand_type_ID::Imm16_}},
       {{operand_type_ID::ModRM_, operand_type_ID::Imm32_}},
       {{operand_type_ID::GPR8_ , operand_type_ID::GPR8_ }},
       {{operand_type_ID::GPR16_, operand_type_ID::GPR16_}},
       {{operand_type_ID::GPR32_, operand_type_ID::GPR32_}},
       {{operand_type_ID::GPR8_ , operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR32_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR8_ , operand_type_ID::Imm8_ }},
       {{operand_type_ID::GPR16_, operand_type_ID::Imm16_}},
       {{operand_type_ID::GPR32_, operand_type_ID::Imm32_}} }};

constexpr static std::array<std::array<int, 2>, 2> valid_args_lea = {{
       {{operand_type_ID::GPR16_, operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR32_, operand_type_ID::ModRM_}} }};

constexpr static std::array<std::array<int, 1>, 6> valid_args_push = {{
       {{operand_type_ID::Imm8_ }},
       {{operand_type_ID::Imm16_}},
       {{operand_type_ID::Imm32_}},
       {{operand_type_ID::GPR16_}},
       {{operand_type_ID::GPR32_}} }};

constexpr static std::array<std::array<int, 1>, 3> valid_args_pop = {{
       {{operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_}},
       {{operand_type_ID::GPR8_ }} }};

constexpr static std::array<std::array<int, 1>, 3> valid_args_inc = {{
       {{operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_}},
       {{operand_type_ID::GPR32_}} }};

constexpr static std::array<std::array<int, 1>, 3> valid_args_dec = {{
       {{operand_type_ID::ModRM_}},
       {{operand_type_ID::GPR16_}},
       {{operand_type_ID::GPR32_}} }};

constexpr static std::array<std::array<int, 0>, 1> valid_args_int3 = {{ {{}} }};
constexpr static std::array<std::array<int, 1>, 1> valid_args_int_ = {{ {{operand_type_ID::Imm8_}} }};
constexpr static std::array<std::array<int, 0>, 1> valid_args_ud2  = {{ {{}} }};
};
