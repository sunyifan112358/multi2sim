/*
 *  Multi2Sim
 *  Copyright (C) 2016 yifan
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

#include <cstring>
#include <algorithm>
#include <fstream>

#include <lib/cpp/String.h>
#include <lib/cpp/Error.h>

#include "Error.h"
#include "File32.h"
#include "Section32.h"
#include "ProgramHeader32.h"
#include "Symbol32.h"

namespace elf
{

void File32::ReadHeader()
{
	// Read ELF header
	info = (Elf32_Ehdr *) buffer.get();
	if (size < sizeof(Elf32_Ehdr))
		throw Error(path, "Invalid ELF file");

	// Check that file is a valid ELF file
	if (strncmp((char *) info->e_ident, ELFMAG, 4))
		throw Error(path, "Invalid ELF file");

	// Check that ELF file is a 32-bit object
	if (info->e_ident[EI_CLASS] == ELFCLASS64)
		throw Error(path, misc::fmt(
				"Cannot load an ELF-64 file with a ELF-32 "
				"loader."));
}


void File32::ReadSections()
{
	// Check section size and number
	if (!info->e_shnum || info->e_shentsize != sizeof(Elf32_Shdr))
		throw Error(path, misc::fmt("Number of sections is 0 or "
				"section header size is not %d",
				(int) sizeof(Elf32_Shdr)));

	// Read section headers
	for (int i = 0; i < info->e_shnum; i++)
		sections.emplace_back(misc::new_unique<Section32>(
				this,
				i,
				info->e_shoff + i * info->e_shentsize));

	// Check string table index
	if (info->e_shstrndx >= info->e_shnum)
		throw Error(path, "Invalid string table index");

	// Read string table
	string_table = sections[info->e_shstrndx].get();
	if (string_table->getType() != 3)
		throw Error(path, "Invalid string table type");

	// Read section names
	for (auto &section : sections)
		section->setName(string_table->getBuffer() +
				section->getNameOffset());
}


void File32::ReadProgramHeaders()
{
	// Nothing if there are no program headers. Don't even check if the
	// program header size is the right one, it could be 0 in this case.
	if (!info->e_phnum)
		return;

	// Check program header size
	if (info->e_phentsize != sizeof(Elf32_Phdr))
		throw Error(path, misc::fmt(
				"Program header size %d (should be %d)",
				info->e_phentsize, (int) sizeof(Elf32_Phdr)));

	// Read program headers
	for (int i = 0; i < info->e_phnum; i++)
		program_headers.emplace_back(misc::new_unique<ProgramHeader32>(
				this,
				i,
				info->e_phoff + i * info->e_phentsize));
}


void File32::ReadSymbols()
{
	// Load symbols from sections
	for (auto &section : sections)
	{
		// Ignore section that don't represent symbol tables
		if (section->getType() != 2 && section->getType() != 11)
			continue;

		// Read symbol table
		int num_symbols = section->getSize() / sizeof(Elf32_Sym);
		for (int i = 0; i < num_symbols; i++)
		{
			// Create symbol in symbol list
			symbols.emplace_back(misc::new_unique<Symbol32>(
					this,
					section.get(),
					i * sizeof(Elf32_Sym)));

			// Discard empty symbol
			Symbol32 *symbol = symbols.back().get();
			if (symbol->getName().empty())
				symbols.pop_back();
		}
	}

	// Sort
	sort(symbols.begin(), symbols.end(), Symbol32::Compare);
}


File32::File32(const std::string &path, bool read_content) :
		File(path)
{
	// Open file
	std::ifstream f(path);
	if (!f)
		throw Error(path, "Cannot open file");

	// Get file size
	f.seekg(0, std::ios_base::end);
	size = f.tellg();
	f.seekg(0, std::ios_base::beg);

	// Check that size is at least equal to header size
	if (size < sizeof(Elf32_Ehdr))
		throw Error(path, "Invalid ELF file");

	// Load entire file into buffer and close
	buffer = misc::new_unique_array<char>(size);
	f.read(buffer.get(), size);
	f.close();

	// Make string stream point to buffer
	std::stringbuf *buf = stream.rdbuf();
	buf->pubsetbuf(buffer.get(), size);

	// Read ELF header
	ReadHeader();

	// Read content
	if (read_content)
	{
		ReadSections();
		ReadProgramHeaders();
		ReadSymbols();
	}
}


File32::File32(const char *buffer, unsigned size, bool read_content)
	: File("<anonymous>")
{

	// Check that size is at least equal to header size
	if (size < sizeof(Elf32_Ehdr))
		throw Error(path, "Invalid ELF file");

	// Copy buffer
	this->size = size;
	this->buffer = misc::new_unique_array<char>(size);
	memcpy(this->buffer.get(), buffer, size);

	// Make string stream point to buffer
	std::stringbuf *buf = stream.rdbuf();
	buf->pubsetbuf(this->buffer.get(), size);

	// Read ELF header
	ReadHeader();

	// Read content
	if (read_content)
	{
		ReadSections();
		ReadProgramHeaders();
		ReadSymbols();
	}
}


Section32 *File32::getSection(const std::string &name) const
{
	// Search
	for (auto &section : sections)
		if (section->getName() == name)
			return section.get();

	// Not found
	return nullptr;
}


Symbol32 *File32::getSymbol(const std::string &name) const
{
	// Search
	for (auto &symbol : symbols)
		if (symbol->getName() == name)
			return symbol.get();

	// Not found
	return nullptr;
}


Symbol32 *File32::getSymbolByAddress(unsigned int address) const
{
	unsigned int offset;
	return getSymbolByAddress(address, offset);
}


Symbol32 *File32::getSymbolByAddress(unsigned int address,
		unsigned int &offset) const
{
	// Empty symbol table
	if (!symbols.size())
		return nullptr;

	// All symbols in the table have a higher address
	if (address < symbols[0]->getValue())
		return nullptr;

	// Binary search
	int min = 0;
	int max = symbols.size();
	while (min + 1 < max)
	{
		int mid = (max + min) / 2;
		Symbol32 *symbol = symbols[mid].get();
		if (symbol->getValue() > address)
		{
			max = mid;
		}
		else if (symbol->getValue() < address)
		{
			min = mid;
		}
		else
		{
			min = mid;
			break;
		}
	}

	// Invalid symbol
	Symbol32 *symbol = symbols[min].get();
	if (!symbol->getValue())
		return nullptr;

	// Go backwards to find first symbol with that address
	for (;;)
	{
		// One symbol before
		min--;
		if (min < 0)
			break;

		// If address is lower, stop
		Symbol32 *prev_symbol = symbols[min].get();
		if (prev_symbol->getValue() != symbol->getValue())
			break;

		// Take symbol if it has global/local/weak binding
		if (ELF32_ST_BIND(prev_symbol->getInfo()) < 3)
			symbol = prev_symbol;
	}

	// Return the symbol and its address
	offset = address - symbol->getValue();
	return symbol;
}


}  // namespace elf
