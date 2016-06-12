/*
 *  Multi2Sim
 *  Copyright (C) 2016  Harrison Barclay (barclay.h@husky.neu.edu)
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>

#include <lib/cpp/CommandLine.h>
#include <lib/cpp/Misc.h>

#include "Disassembler.h"
#include "Binary.h"
#include "Instruction.h"


namespace VI
{

std::string Disassembler::binary_file;


std::unique_ptr<Disassembler> Disassembler::instance;

Disassembler::Disassembler() : comm::Disassembler("VolcanicIslands")
{
	Instruction::Info *info;

	

#define DEFINST(_name, _fmtstr, _fmt, _op, _size, _flags) \
	info = &inst_info[Instruction::Opcode_##_name]; \
	info->opcode = Instruction::Opcode_##_name; \
	info->name = #_name; \
	info->fmt_str = _fmtstr; \
	info->fmt = Instruction:: Format##_fmt; \
	info->op = _op; \
	info->size = _size; \
	info->flags = (Instruction::Flag) _flags;

#include "Instruction.def"
#undef DEFINST

	for (int i = 1; i < Instruction::OpcodeCount; i++)
	{
		info = &inst_info[i];
		

		if (info->fmt == Instruction::FormatSOPP)
		{
			assert(misc::inRange(info->op, 0, dec_table_sopp_count - 1));
			dec_table_sopp[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatSOPC)
		{
			assert(misc::inRange(info->op, 0, dec_table_sopc_count - 1));
			dec_table_sopc[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatSOP1)
		{
			assert(misc::inRange(info->op, 0, dec_table_sop1_count - 1));
			dec_table_sop1[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatSOPK)
		{
			assert(misc::inRange(info->op, 0, dec_table_sopk_count - 1));
			dec_table_sopk[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatSOP2)
		{
			assert(misc::inRange(info->op, 0, dec_table_sop2_count - 1));
			dec_table_sop2[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatSMEM) 
		{
			assert(misc::inRange(info->op, 0, dec_table_smem_count - 1));
			dec_table_smem[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVOP3a || info->fmt == Instruction::FormatVOP3b)
		{
			int i;

			assert(misc::inRange(info->op, 0, dec_table_vop3_count - 1));
			dec_table_vop3[info->op] = info;
			if (info->flags & Instruction::FlagOp8)
			{
				for (i = 1; i < 8; i++)
				{
					dec_table_vop3[info->op + i] = 
						info;
				}
			}
			if (info->flags & Instruction::FlagOp16)
			{
				for (i = 1; i < 16; i++)
				{
					dec_table_vop3[info->op + i] = 
						info;
				}
			}
			continue;
		}
		else if (info->fmt == Instruction::FormatVOPC)
		{
			assert(misc::inRange(info->op, 0, dec_table_vopc_count - 1));
			dec_table_vopc[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVOP1)
		{
			assert(misc::inRange(info->op, 0, dec_table_vop1_count - 1));
			dec_table_vop1[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVOP2)
		{
			assert(misc::inRange(info->op, 0, dec_table_vop2_count - 1));
			dec_table_vop2[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVOP_SDWA)
		{
			assert(misc::inRange(info->op, 0, dec_table_vop_sdwa_count -1));
			dec_table_vop_sdwa[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVOP_DPP)
		{
			assert(misc::inRange(info->op, 0, dec_table_vop_dpp_count -1));
			dec_table_vop_dpp[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatVINTRP)
		{
			assert(misc::inRange(info->op, 0, dec_table_vintrp_count - 1));
			dec_table_vintrp[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatDS)
		{
			assert(misc::inRange(info->op, 0, dec_table_ds_count - 1));
			dec_table_ds[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatMTBUF)
		{
			assert(misc::inRange(info->op, 0, dec_table_mtbuf_count - 1));
			dec_table_mtbuf[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatMUBUF)
		{
			assert(misc::inRange(info->op, 0, dec_table_mubuf_count - 1));
			dec_table_mubuf[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatMIMG)
		{
			assert(misc::inRange(info->op, 0, dec_table_mimg_count - 1));
			dec_table_mimg[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatEXP)
		{
			assert(misc::inRange(info->op, 0, dec_table_exp_count - 1));
			dec_table_exp[info->op] = info;
			continue;
		}
		else if (info->fmt == Instruction::FormatFLAT)
		{
			assert(misc::inRange(info->op, 0, dec_table_flat_count - 1));
			dec_table_flat[info->op] = info;
			continue;
		}
		else 
		{
			std::cerr << "warning: '" << info->name
					<< "' not indexed\n";
		}
	}	
}

Disassembler *Disassembler::getInstance() 
{
	if (instance.get())
		return instance.get();

	instance = misc::new_unique<Disassembler>();
	return instance.get();
}

void Disassembler::DisassembleBuffer(std::ostream& os, const char *buffer, int size)
{
	std::stringstream ss;

	const char *original_buffer = buffer;

	int inst_count = 0;
	int rel_addr = 0;

	int label_addr[size/4];	// list of labels sorted by rel_addr

	int *next_label = &label_addr[0];
	int *end_label = &label_addr[0];

	Instruction::Format format;
	Instruction::Bytes *bytes;

	Instruction inst;

	while(buffer < original_buffer + size)
	{
		inst.Decode(buffer, rel_addr);
		format = inst.getFormat();
		bytes = inst.getBytes();

		// s_endpgm
		if(format == Instruction::FormatSOPP && bytes->sopp.op == 1)
			break;


		// FIXME branching
		if(format == Instruction::FormatSOPP &&
			(bytes->sopp.op >=2 && bytes->sopp.op <= 9))
		{
			short simm16 = bytes->sopp.simm16;
			int label = rel_addr + ((int)simm16 + 4) +4;
			int *label_p = &label_addr[0];

			while(label_p < end_label && *label_p < label)
				label_p++;

			if(label != *label_p || label_p == end_label)
			{
				int *label_p2 = end_label;

				while(label_p2 > label_p)
				{
					*label_p2 = *(label_p2 - 1);
					label_p2--;
				}
				end_label++;

				*label_p = label;
			}
		}
		buffer += inst.getSize();
		rel_addr += inst.getSize();
	}

	buffer = original_buffer;
	rel_addr = 0;

	while(buffer < original_buffer + size)
	{
		inst.Decode(buffer, rel_addr);
		format = inst.getFormat();
		bytes = inst.getBytes();
		inst_count++;

		if(*next_label == rel_addr && next_label != end_label)
		{
			os << misc::fmt("label_%04X:\n", rel_addr / 4);
			next_label++;
		}
		
		// dump instruction
		ss.str("");
		ss << ' ';
		inst.Dump(ss);

		// spacing
		if(ss.str().length() < 59)
			ss << std::string(59 - ss.str().length(), ' ');
		

		os << ss.str();
		os << misc::fmt(" // %08X: %08X", rel_addr, bytes->word[0]);
		if(inst.getSize() == 8)
			os << misc::fmt(" %08X", bytes->word[1]);
		os << '\n';

		if(format == Instruction::FormatSOPP && bytes->sopp.op == 1)
			break;

		buffer += inst.getSize();
		rel_addr += inst.getSize();
	}
}

void Disassembler::DisassembleBinary(const std::string &path) 
{
	std::cerr << "VI disassembler disassembling " << path << "\n";

	ELFReader::File file(path);

	for (int i = 0; i < file.getNumSymbols(); i++)
	{
		ELFReader::Symbol *symbol = file.getSymbol(i);
		std::string symbol_name = symbol->getName();

		// If symbol is '__OpenCL_X_kernel', it points
		// to an internal ELF
		if (misc::StringPrefix(symbol_name, "__OpenCL_") &&
			misc::StringSuffix(symbol_name, "_kernel"))
		{
			if (!symbol->getBuffer())
				throw Error(misc::fmt(
					"%s: symbol '%s' invalid content",
					path.c_str(), symbol_name.c_str()));

			std::string kernel_name = symbol_name.substr(9, 
					symbol_name.length() -16);
			std::cout << "**\n** Disassembly for '__kernel " <<
					kernel_name << "'\n**\n\n";

			// Get the text section where symbol is currently 
			// pointing
			std::istringstream symbol_stream;
			symbol->getStream(symbol_stream);

			auto buffer = misc::new_unique_array<char>(
					symbol->getSize());
			symbol_stream.read(buffer.get(),
					(unsigned) symbol->getSize());
			
			Binary binary(buffer.get(), symbol->getSize(), kernel_name);
			
			BinaryDictEntry *vi_dict_entry = binary.GetVIDictEntry();
			ELFReader::Section *section = vi_dict_entry->text_section;

			DisassembleBuffer(std::cout, section->getBuffer(), section->getSize());

			std::cout << "\n\n\n";
		}
	}
}


void Disassembler::RegisterOptions() 
{
	misc::CommandLine *command_line = misc::CommandLine::getInstance();
	command_line->setCategory("Volcanic Islands");
	command_line->RegisterString("--vi-disasm <file>", binary_file, 
			"Dump Volcanic Islands ISA file for the provided binary"
			" file.");
}


void Disassembler::ProcessOptions() 
{
	if (!binary_file.empty()) 
	{
		Disassembler *disassembler = Disassembler::getInstance();
		disassembler->DisassembleBinary(binary_file);
		exit(0);
	}
}

}
