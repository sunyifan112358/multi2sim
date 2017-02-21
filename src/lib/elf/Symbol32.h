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

#ifndef SRC_LIB_ELF_SYMBOL32_H
#define SRC_LIB_ELF_SYMBOL32_H

#include "Symbol.h"

namespace elf
{

/// A elf::Symbol32 is
class Symbol32: public Symbol
{
	// File that it belongs to
	File32 *file;

	// Section that the symbol points to. This section is not the section
	// passed as an argument to the constructor (i.e., the symbol table
	// section). It is the section pointed to by its 'st_shndx' field, or
	// null if this fields points to an invalid section.
	Section32 *section;

	// Symbol information, pointing to an internal position of the ELF
	// file's buffer.
	Elf32_Sym *info;

public:

	/// Constructor. A symbol is only created internally by the ELF reader,
	/// so this function should not be invoked.
	///
	/// \param file
	///	ELF file that the symbol belongs to.
	///
	/// \param section
	///	Section pointed to by the symbol.
	///
	/// \param info_offset
	///	Offset in ELF file content where the symbol is found.
	Symbol32(File32 *file, Section32 *section, unsigned info_offset);

	/// Destructor
	virtual ~Symbol32() {}

	/// Comparison function between symbols, used for sorting purposes.
	/// This function is used internally by the ELF file and should not be
	/// called.
	static bool Compare(const std::unique_ptr<Symbol32> &a,
			const std::unique_ptr<Symbol32> &b);

	/// Return the section associated with the symbol
	Section32 *getSection() const { return section; }

	/// Return a pointer to the Elf32_Sym structure representing the
	/// symbol. Each field of this structure can be queried with dedicated
	/// getters instead.
	Elf32_Sym *getRawInfo() const { return info; }

	/// Return field \a st_value
	Elf32_Addr getValue() const { return info->st_value; }

	/// Return field \a st_size
	Elf32_Word getSize() const { return info->st_size; }

	/// Return field \a st_info
	unsigned char getInfo() const { return info->st_info; }

	/// Return field \a st_other
	unsigned char getOther() const { return info->st_other; }

	/// Return field \a st_shndx
	Elf32_Section getShndx() const { return info->st_shndx; }

	/// Modify an input stream to hold the content pointed to by the symbol,
	/// without involving any copy operations.
	///
	/// \param stream
	///	Input stream whose internal pointers will be modified.
	///
	/// \param offset
	///	Offset in the symbol content that the stream will point to.
	///
	/// \param size
	///	Number of bytes to include in the stream.
	void getStream(std::istringstream &stream,
			unsigned offset,
			unsigned size) const;


	/// Alternative syntax for getStream() to obtain the entire content
	/// pointed to by the symbol.
	void getStream(std::istringstream &stream) const
	{
		getStream(stream, 0, info->st_size);
	}
};

}  // namespace elf

#endif  // SRC_LIB_ELF_SYMBOL32_H
