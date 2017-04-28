/*
*  Multi2Sim
*  Copyright (C) 2017  Yifan Sun (yifansun@coe.neu.edu)
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <cassert>
#include <cstring>
#include <elf.h>

#include <lib/cpp/CommandLine.h>
#include <lib/cpp/String.h>
#include <lib/elf/File32.h>
#include <lib/elf/Section32.h>
#include <lib/elf/Symbol32.h>

#include "Disassembler.h"
#include "Instruction.h"


namespace x86_64
{

std::string Disassembler::path;

void Disassembler::RegisterOptions()
{
    // Get command line object
    misc::CommandLine *command_line = misc::CommandLine::getInstance();

    // Category
    command_line->setCategory("x86_64");

    // Option --x86-disasm <file>
    command_line->RegisterString("--x86_64-disasm <file>", path,
                               "Disassemble the x86_64 ELF file provided in <arg>, "
                                       "using the internal x86_64 disassembler. This option is "
                                       "incompatible with any other option.");

    // Incompatible options
    command_line->setIncompatible("--x86_64-disasm");
}


void Disassembler::ProcessOptions()
{
    // Run x86 disassembler
    if (!path.empty())
    {
        Disassembler *disassembler = Disassembler::getInstance();
        disassembler->DisassembleBinary(path);
        exit(0);
    }
}

// List of possible prefixes
static const unsigned char asm_prefixes[] =
{
    0xf0,  // lock
    0xf2,  // repne/repnz
    0xf3,  // rep repe/repz
    0x66,  // opsize override
    0x67,  // addrsize override
    0x2e,  // use cs
    0x36,  // use ss
    0x3e,  // use ds
    0x26,  // use es
    0x64,  // use fs
    0x65,   // use gs

};

std::unique_ptr<Disassembler> Disassembler::instance;

void Disassembler::InsertInstInfo(Instruction::DecodeInfo **table,
                                  Instruction::DecodeInfo *elem,
                                  int at)
{
    // First entry
    if (!table[at])
    {
        table[at] = elem;
        return;
    }

    // Go to the end of the list
    Instruction::DecodeInfo *prev = table[at];
    while (prev->next)
        prev = prev->next;
    prev->next = elem;
}


void Disassembler::InsertInstInfo(Instruction::Info *info)
{
    // Obtain the table where to insert, the initial index, and
    // the number of times we must insert the instruction.
    int index;
    int count;
    Instruction::DecodeInfo **table;
    if ((info->prefix0f & 0xff) == 0x0f)
    {
        table = dec_table_0f;
        index = info->popcode & 0xff;
        count = 1;//info->sopcode & INDEX ? 8 : 1;
    }
    else
    {
        table = dec_table;
        index = info->popcode & 0xff;
        count = 1;//info->popcode & INDEX ? 8 : 1;
    }

    // Insert
    for (int i = 0; i < count; i++)
    {
        Instruction::DecodeInfo *elem = new Instruction::DecodeInfo();
        elem->info = info;
        InsertInstInfo(table, elem, index + i);
    }
}


void Disassembler::FreeInstDecodeInfo(Instruction::DecodeInfo *elem)
{
    while (elem)
    {
        Instruction::DecodeInfo *next = elem->next;
        delete elem;
        elem = next;
    }
}


Disassembler::Disassembler() : comm::Disassembler("x86_64")
{
    // Initialize instruction information list
    memset(inst_info, 0, sizeof inst_info);
    inst_info[Instruction::OpcodeInvalid].fmt = "";
    struct Instruction::Info *info;
#define DEFINST(__name, __prefix, __prefix0f, __opcode_ext, __po, __so, __op1, __op2, __op3, __op4) \
    info = &inst_info[Instruction::Opcode_##__name]; \
    info->opcode = Instruction::Opcode_##__name; \
    info->opcode_ext = __opcode_ext; \
    info->popcode = __po; \
    info->sopcode = __so; \
    info->op1 = __op1; \
    info->op2 = __op2; \
    info->op3 = __op3; \
    info->op4 = __op4; \
    info->prefix = __prefix; \
    info->prefix0f = __prefix0f; \
    info->fmt = #__name;
#include "Instruction.def"
#undef DEFINST

    // Initialize table of prefixes
    memset(is_prefix, 0, sizeof is_prefix);
    for (unsigned i = 0; i < sizeof asm_prefixes; i++)
    is_prefix[asm_prefixes[i]] = true;

    // Initialize decoding table. This table contains lists of information
    // structures for x86_64 instructions. To find an instruction in the table,
    // the table can be indexed by the first byte of its opcode.
    memset(dec_table, 0, sizeof dec_table);
    memset(dec_table_0f, 0, sizeof dec_table_0f);
    for (int op = 1; op < Instruction::OpcodeCount; op++)
    {
        // Insert into table
        info = &inst_info[op];
        InsertInstInfo(info);

        // Compute 'match_mask' and 'mach_result' fields. Start with
        // the 'modrm' field in the instruction format definition.
        // A 3-bit opcode extension might be present, such as for sub_rm_imm8
        //  in which case the opx is 5: 101b, present in modrm byte

        // If instruction doesn't SKIP opx
        if (!(info->opcode_ext & SKIP))
        {
            // The instruction will have a modrm byte
            info->modrm_size = 1;

            // If part of the offset is in the 'reg' field of the ModR/M byte,
            // it must be matched.
            info->match_mask = 0x38;
            info->match_result = (info->opcode_ext & 0x7) << 3;

            // If instruction expects a memory operand, the 'mod' field of
            // the ModR/M byte cannot be 11.
            // TODO: Uncomment when you figure out why this is here
//            if (info->opcode_ext & MEM)
//            {
//                info->nomatch_mask = 0xc0;
//                info->nomatch_result = 0xc0;
//            }
        }

        // Second opcode
        if (!(info->sopcode & SKIP))
        {
            info->opcode_size++;
            info->match_mask <<= 8;
            info->match_result <<= 8;
            info->nomatch_mask <<= 8;
            info->nomatch_result <<= 8;
            info->match_mask |= 0xff;
            info->match_result |= info->sopcode & 0xff;

        }

        // First opcode byte (always there)
        info->opcode_size++;
        info->match_mask <<= 8;
        info->match_result <<= 8;
        info->nomatch_mask <<= 8;
        info->nomatch_result <<= 8;
        info->match_mask |= 0xff;
        info->match_result |= info->popcode & 0xff;
        if (info->popcode & INDEX)
        {
            info->match_mask &= 0xfffffff8;
            info->opindex_shift = 0;
        }

        // Prefix 0f
        if (!(info->prefix0f & SKIP)) {
            info->match_mask <<= 8;
            info->match_result <<= 8;
            info->match_mask |= 0xFF;
            info->match_result |= (info->prefix0f & 0xFF);

            info->modrm_size = 1; // I guess this should be here?
        }

        // Decode immediate fields
        if (!(info->op1 & SKIP)) {
            if (info->op1 & IMM) {
                // last 4 bits indicate number of imm bytes
                info->imm_size += (info->op1 & 0x0F);
            }
            else if (info->op1 & REG) {
                // If there is a size
                if (info->op1 & 0xFF) {
                    info->imm_size += (info->op1 & 0xFF);
                }
                else {
                    std::cout << "I don't know, the default I guess" << std::endl;
                    info->imm_size += 0; // ? placeholder

                    // depends on modrm :(
                }
            }
        }

        if (info->op2 & IMM) {
            // last 4 bits indicate number of imm bytes
            info->imm_size += (info->op2 & 0x0F);
        }

        if (info->op1 & REL) {
            info->imm_size += 4; // TODO: figure out how to express more neatly
        }

    } // for op in instruction::opcount
}


Disassembler::~Disassembler()
{
    // Free instruction info tables
    for (int i = 0; i < 0x100; i++)
    {
        FreeInstDecodeInfo(dec_table[i]);
        FreeInstDecodeInfo(dec_table_0f[i]);
    }
}


Disassembler *Disassembler::getInstance()
{
    // Instance already exists
    if (instance.get())
        return instance.get();

    // Create instance
    instance.reset(new Disassembler());
    return instance.get();
}


void Disassembler::DisassembleBinary(const std::string &path, std::ostream &os) const
{
    // Traverse sections of ELF file
    elf::File64 file(path);
    for (int idx = 0; idx < file.getNumSections(); idx++)
    {
        // Get section and skip if it does not contain code
        elf::Section64 *section = file.getSection(idx);
        if ((section->getFlags() & SHF_EXECINSTR) == 0)
            continue;

        // Title
        os << "Disassembly of section " << section->getName()
           << ":\n";

        // Keep track of current symbol
        int current_symbol = 0;
        elf::Symbol64 *symbol = file.getSymbol(current_symbol);

        // Disassemble
        Instruction inst;
        unsigned offset = 0;
        while (offset < section->getSize())
        {
            // Get position in section
            unsigned eip = section->getAddr() + offset;
            const char *buffer = section->getBuffer() + offset;

            // Decode instruction
            inst.Decode(buffer, eip);
            if (inst.getOpcode())
            {
                os << "Opcode: " << inst.getOpcode() << std::endl;
                assert(inst.getSize());
                offset += inst.getSize();
            }
            else
                offset++;

            // Symbol
            while (symbol && symbol->getValue() < eip)
            {
                current_symbol++;
                symbol = file.getSymbol(current_symbol);
            }

            if (symbol && symbol->getValue() == eip)
                os << misc::fmt("\n%08x <%s>:\n", eip, symbol->getName().c_str());

            // Address
            os << misc::fmt("%8x:\t", eip);

            // Hex dump of bytes 0..6
            for (int i = 0; i < 7; i++)
            {
                if (i < inst.getSize())
                    os << misc::fmt("%02x ", (unsigned char) buffer[i]);
                else
                    os << "   ";
            }

            // Instruction
            os << '\t';
            if (inst.getOpcode())
                os << inst;
            else
                os << "???";
            os << '\n';

            // Hex dump of bytes 7..13
            if (inst.getSize() > 7)
            {
                os << misc::fmt("%8x:\t", eip + 7);
                for (int i = 7; i < 14 && i < inst.getSize(); i++)
                    os << misc::fmt("%02x ", (unsigned char) buffer[i]);
                os << '\n';
            }
        } // while

        // Pad
        os << '\n';
    }
} // Dissassemble binary

} // namespace x86_64
