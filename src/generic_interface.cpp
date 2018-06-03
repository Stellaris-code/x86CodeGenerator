#include "generic_interface.hpp"
#include <iostream>

#define GENERATE_OPCODE(NAME, OPERANDS)                                                                                              \
              do {                                                                                                                   \
                     constexpr auto lot = valid_args_##NAME;                                                                         \
                     wrong_argnum_except<lot[0].size()>(OPERANDS, #NAME);                                                            \
                     int valcomb = invalid_args_except(evaluate_valid_types(valid_args_##NAME, OPERANDS), std::string(#NAME));       \
                     auto f = [=](auto... args) {                                                                                    \
                            this->NAME(args...);                                                                                     \
                     };                                                                                                              \
                     search_and_exe_overload<lot.size(), lot[0].size(), valid_args_##NAME, decltype(f)>                              \
                     (OPERANDS, valcomb, f, std::make_index_sequence<lot.size()>());                                                 \
              } while(0)

namespace x86gen {

CodeGenerator& operator<<(CodeGenerator& apply_to, Opcode to_set) {
       apply_to.m_current_opcode = to_set;
       apply_to.m_operands_consumed.clear();

       if(apply_to.m_current_opcode.m_argument_number == 0) {
              std::invoke(apply_to.m_current_opcode.m_parsing_complete_callback, apply_to, apply_to.m_operands_consumed);
       }

       return apply_to;
}

CodeGenerator& operator<<(CodeGenerator& apply_to, Operand to_consume) {
       apply_to.m_operands_consumed.push_back(to_consume);

       if(apply_to.m_operands_consumed.size() >= apply_to.m_current_opcode.m_argument_number) {
              std::invoke(apply_to.m_current_opcode.m_parsing_complete_callback, apply_to, apply_to.m_operands_consumed);
              apply_to.m_operands_consumed.clear();
       }

       return apply_to;
}

/* ------------------ HELPER FUNCTIONS ------------ */

template<std::size_t N, std::size_t P, int currN, const std::array<std::array<int, P>, N>& lot, std::size_t... S>
constexpr auto extract_types(std::index_sequence<S...>) {
       constexpr auto sublot = lot[currN];
       return std::integer_sequence<int, sublot[S]...>();
}


template<std::size_t N, std::size_t P, const std::array<std::array<int, P>, N>& lot, typename l, std::size_t... S>
static void search_and_exe_overload(std::vector<Operand> opvec, int valid_comb, l lmbda, std::index_sequence<S...>) {
       return search_and_exe_overload<N, P, lot, l, S...>(opvec, valid_comb, lmbda);
}


template<std::size_t N, std::size_t P, const std::array<std::array<int, P>, N>& lot, typename l, std::size_t first, std::size_t... S>
static void search_and_exe_overload(std::vector<Operand> opvec, int valid_comb, l lmbda) {

       if constexpr(P == 0) {
              lmbda();
              return;
       } else {

              if(first == valid_comb) {
                     exe_overload(lmbda, opvec, extract_types<N, P, first, lot>(std::make_index_sequence<P>()),
                            std::make_index_sequence<P>());
                    }

              if constexpr(sizeof...(S) == 0) { return; }
              else { search_and_exe_overload<lot.size(), lot[0].size(), lot, decltype(lmbda), S...>(opvec, valid_comb, lmbda); }
       }
}

template<std::size_t... S, int... T, typename l>
static void exe_overload(l lmbda, const std::vector<Operand>& opvec, std::integer_sequence<int, T...>, std::index_sequence<S...>) {
       lmbda(std::get<T>(opvec[S])...);
}


template<std::size_t num_arg>
static void wrong_argnum_except(const std::vector<Operand>& operands, std::string instr) {
       if(operands.size() != num_arg) {
              throw std::runtime_error(std::string("general ") + instr + std::string(" call with a wrong number of arguments"));
       }
}

template<std::size_t N, std::size_t P>
std::optional<int> evaluate_valid_types(const std::array<std::array<int, P>, N>& valid_combinations, std::vector<Operand>& operands) {
       if constexpr (P == 0) return std::optional<int>(-1);
       std::vector<int> operand_types;
       for(auto& operand : operands) {
              operand_types.push_back(operand.index());
       }

       for(std::size_t i = 0; i < N; ++i) {
              bool flag_equal = true;
              // We compare the array and the vector in a cumbersome way indeed

              for(std::size_t j = 0; j < valid_combinations[i].size(); ++j) {
                     flag_equal &= (valid_combinations[i][j] == operand_types[j]);
              }

              if(flag_equal) return i;
       }

       return std::optional<int>();
}

int static invalid_args_except(std::optional<int> ok, std::string instr) {
       if(!ok.has_value()) throw std::runtime_error(std::string("general ") + instr + std::string(" call with invalid arguments"));
       return ok.value();
}


/* ------------------ GENERIC OVERLOADS ------------------*/
// Add as needed following the pattern

void CodeGenerator::general_std(std::vector<Operand> operands)         { GENERATE_OPCODE(std, operands);         }
void CodeGenerator::general_cld(std::vector<Operand> operands)         { GENERATE_OPCODE(cld, operands);         }
void CodeGenerator::general_repe_scasb(std::vector<Operand>  operands) { GENERATE_OPCODE(repe_scasb, operands);  }
void CodeGenerator::general_repe_scasw(std::vector<Operand>  operands) { GENERATE_OPCODE(repe_scasw, operands);  }
void CodeGenerator::general_repe_scasd(std::vector<Operand>  operands) { GENERATE_OPCODE(repe_scasd, operands);  }
void CodeGenerator::general_repne_scasb(std::vector<Operand> operands) { GENERATE_OPCODE(repne_scasb, operands); }
void CodeGenerator::general_repne_scasw(std::vector<Operand> operands) { GENERATE_OPCODE(repne_scasw, operands); }
void CodeGenerator::general_repne_scasd(std::vector<Operand> operands) { GENERATE_OPCODE(repne_scasd, operands); }
void CodeGenerator::general_call(std::vector<Operand> operands)        { GENERATE_OPCODE(call, operands);        }
void CodeGenerator::general_ret(std::vector<Operand> operands)         { GENERATE_OPCODE(ret, operands);         }
void CodeGenerator::general_far_ret(std::vector<Operand> operands)     { GENERATE_OPCODE(far_ret, operands);     }
void CodeGenerator::general_cmp(std::vector<Operand> operands)         { GENERATE_OPCODE(cmp, operands);         }
void CodeGenerator::general_sub(std::vector<Operand> operands)         { GENERATE_OPCODE(sub, operands);         }
void CodeGenerator::general_add(std::vector<Operand> operands)         { GENERATE_OPCODE(add, operands);         }
void CodeGenerator::general_xor(std::vector<Operand> operands)         { GENERATE_OPCODE(xor_, operands);        }
void CodeGenerator::general_imul2(std::vector<Operand> operands)       { GENERATE_OPCODE(imul2, operands);       }
void CodeGenerator::general_imul3(std::vector<Operand> operands)       { GENERATE_OPCODE(imul3, operands);       }
void CodeGenerator::general_jmp(std::vector<Operand> operands)         { GENERATE_OPCODE(jmp, operands);         }
void CodeGenerator::general_jcc(std::vector<Operand> operands)         { GENERATE_OPCODE(jcc, operands);         }
void CodeGenerator::general_mov(std::vector<Operand> operands)         { GENERATE_OPCODE(mov, operands);         }
void CodeGenerator::general_lea(std::vector<Operand> operands)         { GENERATE_OPCODE(lea, operands);         }
void CodeGenerator::general_push(std::vector<Operand> operands)        { GENERATE_OPCODE(push, operands);        }
void CodeGenerator::general_pop(std::vector<Operand> operands)         { GENERATE_OPCODE(pop, operands);         }
void CodeGenerator::general_inc(std::vector<Operand> operands)         { GENERATE_OPCODE(inc, operands);         }
void CodeGenerator::general_dec(std::vector<Operand> operands)         { GENERATE_OPCODE(dec, operands);         }
void CodeGenerator::general_int3(std::vector<Operand> operands)        { GENERATE_OPCODE(int3, operands);        }
void CodeGenerator::general_int(std::vector<Operand> operands)         { GENERATE_OPCODE(int_, operands);        }
void CodeGenerator::general_ud2(std::vector<Operand> operands)         { GENERATE_OPCODE(ud2, operands);         }
};
