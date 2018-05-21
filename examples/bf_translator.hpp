/*
bf_generator.hpp

Copyright (c) 18 Yann BOUCHER (yann)

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
#ifndef BF_TRANSLATOR_HPP
#define BF_TRANSLATOR_HPP

#include <cstdint>

#include <string_view>
#include <vector>
#include <stdexcept>

#include "bf_generator.hpp"

namespace bfgen
{

struct compilation_error : std::runtime_error
{
        using std::runtime_error::runtime_error;
};

[[noreturn]] inline void compilation_error_throw(const std::string& why, unsigned col)
{
    throw compilation_error("Compilation error : " + why + ", col " + std::to_string(col));
}

struct GeneratedCode;

GeneratedCode compile_bf(std::string_view input, BuildFlags flags = BuildFlags{});

}

#endif // BF_GENERATOR_HPP
