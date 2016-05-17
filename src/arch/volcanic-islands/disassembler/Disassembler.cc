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

#include <lib/cpp/CommandLine.h>
#include <lib/cpp/Misc.h>

#include "Disassembler.h"
#include "Binary.h"
#include "Instruction.h"


namespace VI
{

std::string Disassembler::binary_file;


std::unique_ptr<Disassembler> Disassembler::instance;


Disassembler *Disassembler::getInstance() 
{
	if (instance.get())
		return instance.get();

	instance = misc::new_unique<Disassembler>();
	return instance.get();
}


void Disassembler::DisassembleBinary(const std::string &path) 
{
	std::cerr << "VI disassembler disassembling " << path << "\n";

	ELFReader::File file(path);

	// Decode ELF
	for (int i = 0; i < file.getNumSymbols(); i++)
	{
		// Symbol
		ELFReader::Symbol *symbol = file.getSymbol(i);
		std::string symbol_name = symbol->getName();

		// If symbol is '__OpenCL_X_kernel', it points
		// to an internal ELF
		if (misc::StringPrefix(symbol_name, "__OpenCL_") &&
			misc::StringSuffix(symbol_name, "_kernel"))
		{
			// Content at *symbol must be valid
			if (!symbol->getBuffer())
				throw Error(misc::fmt(
					"%s: symbol '%s' invalid content",
					path.c_str(), symbol_name.c_str()));

			// Get kernel name
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
			
			// Create internal ELF
			Binary binary(buffer.get(), symbol->getSize(), kernel_name);
			
			// Get section with VOlcanic Islands ISA
			BinaryDictEntry *vi_dict_entry = binary.GetSIDictEntry();
			ElfReader::Section *section = vi_dict_entry->text_section;

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
