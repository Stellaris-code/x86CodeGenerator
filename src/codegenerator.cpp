/*
codegenerator.cpp

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

#include <sys/mman.h>
#include <unistd.h>

#include <system_error>

namespace x86gen
{

CodeGenerator::CodeGenerator()
{
    m_data.reserve(sysconf(_SC_PAGESIZE));
}

void CodeGenerator::emit_modrm(ModRM rm)
{
    emit<uint8_t>(rm.modrm);
    if (rm.mod() != 0b11)
    {
        if (rm.rm() == 0b100)
        {
            emit<uint8_t>(rm.sib);
            if (rm.sib.has_disp)
                emit<uint32_t>(rm.displacement);
        }
        if (rm.rm() == 0b101 || rm.mod() == 0b10 || (rm.mod() == 0b00 && rm.rm() == 0b101))
        {
            emit<int32_t>(rm.displacement);
        }
        else if (rm.mod() == 0b01)
        {
            emit<int8_t>(rm.displacement);
        }
    }
}

// TODO : handle that ugly af resize
void CodeGenerator::resize_buffer(size_t size)
{
    m_data.resize(size);

    static const size_t page_size = sysconf(_SC_PAGESIZE);
    const uintptr_t data_addr = reinterpret_cast<uintptr_t>(m_data.data());

    void* base = reinterpret_cast<void*>(data_addr&~(page_size-1));
    size_t page_offset = data_addr&(page_size-1);

    if (mprotect(base, m_data.size() + page_offset, PROT_READ|PROT_WRITE|PROT_EXEC) == -1)
    {
        throw std::system_error(errno, std::system_category(), "Error while calling mprotect()");
    }
}

}
