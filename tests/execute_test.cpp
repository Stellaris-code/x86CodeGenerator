/*
execute_test.cpp

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

/*
combined_gen_test.cpp

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
#include "gtest/gtest.h"

#include <sys/mman.h>

#include "codegenerator.hpp"

namespace
{
using namespace x86gen;

TEST(CombinedGen, ValueReturn)
{
    CodeGenerator gen;
    gen.mov(EAX, 6);
    gen.ret();

    EXPECT_EQ(((int(*)())gen.data().data())(), 6);
}

TEST(CombinedGen, ArgPassing)
{
    CodeGenerator gen;
    gen.mov(ECX, EDI); // 64-bit
    gen.mov(EAX, ECX);
    gen.ret();

    EXPECT_EQ(((int(*)(int))gen.data().data())(3), 3);
}

int test_fun()
{
    return 3;
}

TEST(CombinedGen, CFunCall)
{
    CodeGenerator gen;
    gen.mov(RAX, (uintptr_t)test_fun);
    gen.call(EAX);
    gen.ret();

    EXPECT_EQ(((int(*)(int))gen.data().data())(3), 3);
}

TEST(CombinedGen, CFunCallVoid)
{
    CodeGenerator gen;
    gen.ret();

    EXPECT_NO_FATAL_FAILURE(((void(*)())gen.data().data())());
}

}
