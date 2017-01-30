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

#ifndef SRC_LIB_ELF_FILE64_H
#define SRC_LIB_ELF_FILE64_H

#include <elf.h>

#include "File.h"
#include "Section64.h"
#include "Symbol64.h"
#include "ProgramHeader64.h"

namespace elf
{

/// A elf::File64 is
class File64 : public File
{
	// ELF header
	Elf64_Ehdr *info;

	// Read the ELF header
	void ReadHeader();

	// Populate the section list
	void ReadSections();

	// Populate the program header list
	void ReadProgramHeaders();

	// Populate the symbol list
	void ReadSymbols();

	// String table section
	Section64 *string_table;

	// List of sections
	std::vector<std::unique_ptr<Section64>> sections;

	// List of program headers
	std::vector<std::unique_ptr<ProgramHeader64>> program_headers;

	// List of symbols
	std::vector<std::unique_ptr<Symbol64>> symbols;

public:
	/// Load an ELF file from the file system.
	///
	/// \param path
	///	Path to load the ELF file from.
	///
	/// \param read_content
	///	If true (or omitted), interpret the entire content of the ELF
	///	file. If false, read only the ELF reader. The ELF reader will
	///	return no program header, section, or symbol for the file.
	File64(const std::string &path, bool read_content = true);

	/// Load an ELF file from a buffer in memory.
	///
	/// \param buffer
	///	Buffer to read the ELF file from. The content of the buffer
	///	will be replicated internally, so any external changes in it
	///	will not affect the ELF reader.
	///
	/// \param size
	///	Size of the buffer in bytes.
	///
	/// \param read_content
	///	If true (or omitted), interpret the entire content of the ELF
	///	file. If false, read only the ELF reader. The ELF reader will
	///	return no program header, section, or symbol for the file.
	File64(const char *buffer, unsigned size, bool read_content = true);

	/// Destructor
	virtual ~File64() {}

	/// Return the number of sections
	int getNumSections() const { return sections.size(); }

	/// Return the section at position \a index, or \a null if the
	/// value given in \a index is out of range.
	Section64 *getSection(int index) const
	{
		return index >= 0 && index < (int) sections.size() ?
				sections[index].get() :
				nullptr;
	}

	/// Return a section by its name, or \a null if not found.
	Section64 *getSection(const std::string &name) const;

	/// Return a read-only reference to the vector of sections. This
	/// function is useful for convenient iteration through sections using
	/// the following code:
	///
	/// \code
	///	for (auto &section : file.getSections())
	/// \endcode
	const std::vector<std::unique_ptr<Section64>> &getSections() const
	{
		return sections;
	}

	/// Return the number of program headers
	int getNumProgramHeaders() const { return program_headers.size(); }

	/// Return the program header at position \a index, or \a null if
	/// the value given in \a index is out of range.
	ProgramHeader64 *getProgramHeader(int index) const
	{
		return index >= 0 && index < (int) program_headers.size() ?
				program_headers[index].get() :
				nullptr;
	}

	/// Return a constant reference to the list of program headers for
	/// convenient iteration.
	const std::vector<std::unique_ptr<ProgramHeader64>>
			&getProgramHeaders() const
	{
		return program_headers;
	}

	/// Return the number of symbols
	int getNumSymbols() const { return symbols.size(); }

	/// Return the symbol at position \a index, or \a null if the
	/// value given in \a index is out of range.
	Symbol64 *getSymbol(int index) const
	{
		return index >= 0 && index < (int) symbols.size() ?
				symbols[index].get() :
				nullptr;
	}

	/// Return a symbol given its \a name. Symbols are stored
	/// internally in a sorted data structure, with an access cost of
	/// log(n).
	Symbol64 *getSymbol(const std::string &name) const;

	/// Return a constant reference to the list of symbols for convenient
	/// iteration.
	const std::vector<std::unique_ptr<Symbol64>> &getSymbols() const
	{
		return symbols;
	}

	/// Return the first symbol at a given address/name, or the
	/// closest one with a higher address.
	Symbol64 *getSymbolByAddress(unsigned int address) const;

	/// Alternative syntax to getSymbolByAddress(), where the offset of the
	/// returned symbol (if any) is also returned in argument \a offset.
	Symbol64 *getSymbolByAddress(unsigned int address,
			unsigned int &offset) const;

	/// Return the section corresponding to the string table, or \a null if
	/// the ELF file doesn't contain one.
	Section64 *getStringTable() const { return string_table; }

	/// Return \a e_ident field of ELF header
	unsigned char *getIdent() const { return info->e_ident; }

	/// Return \a e_type field of ELF header
	Elf64_Half getType() const { return info->e_type; }

	/// Return \a e_machine field of ELF header
	Elf64_Half getMachine() const { return info->e_machine; }

	/// Return \a e_version field of ELF header
	Elf64_Word getVersion() const { return info->e_version; }

	/// Return \a e_entry field of ELF header
	Elf64_Addr getEntry() const { return info->e_entry; }

	/// Return \a e_phoff field of ELF header
	Elf64_Off getPhoff() const { return info->e_phoff; }

	/// Return \a e_shoff field of ELF header
	Elf64_Off getShoff() const { return info->e_shoff; }

	/// Return \a e_flags field of ELF header
	Elf64_Word getFlags() const { return info->e_flags; }

	/// Return \a e_ehsize field of ELF header
	Elf64_Half getEhsize() const { return info->e_ehsize; }

	/// Return \a e_phentsize field of ELF header
	Elf64_Half getPhentsize() const { return info->e_phentsize; }

	/// Return \a e_phnum field of ELF header
	Elf64_Half getPhnum() const { return info->e_phnum; }

	/// Return \a e_shentsize field of ELF header
	Elf64_Half getShentsize() const { return info->e_shentsize; }

	/// Return \a e_shnum field of ELF header
	Elf64_Half getShnum() const { return info->e_shnum; }

	/// Return \a e_shstrndx field of ELF header
	Elf64_Half getShstrndx() const { return info->e_shstrndx; }
};

}  // namespace elf

#endif  // SRC_LIB_ELF_FILE64_H
