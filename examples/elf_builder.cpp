/*
elf_builder.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include "elf_builder.hpp"

#include <cstring>

#include <elf.h>

namespace bfgen
{

std::vector<uint8_t> build_elf_executable(const std::vector<uint8_t> &code, uint32_t entry_point)
{
    const size_t total_size = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + code.size();

    uint32_t org = 0x08048000;

    Elf32_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    Elf32_Phdr phdr;
    memset(&phdr, 0, sizeof(phdr));

    memcpy(ehdr.e_ident, ELFMAG, SELFMAG);
    ehdr.e_ident[EI_CLASS] = ELFCLASS32;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_ident[EI_ABIVERSION] = 0;
    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_386;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_entry = sizeof(ehdr) + sizeof(phdr) + entry_point + org;
    ehdr.e_phoff = sizeof(ehdr);
    ehdr.e_shoff = 0; // no section header table
    ehdr.e_flags = 0;
    ehdr.e_ehsize = sizeof(ehdr);
    ehdr.e_phentsize = sizeof(phdr);
    ehdr.e_phnum = 1;
    ehdr.e_shentsize = 0;
    ehdr.e_shnum = 0;
    ehdr.e_shstrndx = SHN_UNDEF;

    phdr.p_type = PT_LOAD;
    phdr.p_offset = 0;
    phdr.p_vaddr = org;
    phdr.p_paddr = 0;
    phdr.p_filesz = total_size;
    phdr.p_memsz = total_size;
    phdr.p_flags = PF_R|PF_X;
    phdr.p_align = 0x1000;

    std::vector<uint8_t> file_data(total_size);
    memcpy(file_data.data(), &ehdr, sizeof(ehdr));
    memcpy(file_data.data() + sizeof(ehdr), &phdr, sizeof(phdr));

    memcpy(file_data.data() + sizeof(ehdr) + sizeof(phdr), (void*)code.data(), code.size());

    return file_data;
}

}
