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

#ifndef SRC_LIB_ELF_PROGRAMHEADER64_H
#define SRC_LIB_ELF_PROGRAMHEADER64_H

#include "ProgramHeader.h"

#include <elf.h>

namespace elf
{
class File64;

/// A elf::ProgramHeader64 is
class ProgramHeader64 : public ProgramHeader
{
	// File that it belongs to
	File64 *file;

	// Program header information
	Elf64_Phdr *info;

public:
	/// Constructor. A program header is created internally by the ELF
	/// file, so this function should not be invoked directly.
	///
	/// \param file
	///	ELF file that the program header belongs to.
	///
	/// \param index
	///	Index in the list of program headers of the ELF file.
	///
	/// \param info_offset
	///	Offset in ELF file content where the program header is found.
	ProgramHeader64(File64 *file, int index, unsigned info_offset);

	/// Destructor
	virtual ~ProgramHeader64() {}

	/// Return a pointer to the Elf32_Phdr structure representing the
	/// program header. Each field of this structure can be queried with
	/// dedicated getters instead.
	Elf64_Phdr *getRawInfo() const { return info; }

	/// Return the \a p_type field of the ELF program header
	Elf64_Word getType() const { return info->p_type; }

	/// Return the \a p_offset field
	Elf64_Off getOffset() const { return info->p_offset; }

	/// Return the \a p_vaddr field
	Elf64_Addr getVaddr() const { return info->p_vaddr; }

	/// Return the \a p_paddr field
	Elf64_Addr getPaddr() const { return info->p_paddr; }

	/// Return the \a p_filesz field
	Elf64_Word getFilesz() const { return info->p_filesz; }

	/// Return the \a p_memsz field
	Elf64_Word getMemsz() const { return info->p_memsz; }

	/// Return the \a p_flags field
	Elf64_Word getFlags() const { return info->p_flags; }

	/// Return the \a p_align field
	Elf64_Word getAlign() const { return info->p_align; }

	/// Modify an input stream to hold the content pointed to by the program
	/// header, without involving any copy operations.
	///
	/// \param stream
	///	Input stream whose internal pointers will be modified.
	///
	/// \param offset
	///	Offset in the program header content that the stream points to.
	///
	/// \param size
	///	Number of bytes to include in the stream.
	void getStream(std::istringstream &stream,
			unsigned offset,
			unsigned size)
			const;

	/// Alternative syntax for getStream() to obtain the entire content
	/// pointed to by the program header.
	void getStream(std::istringstream &stream) const
	{
		getStream(stream, 0, size);
	}
};

}  // namespace elf

#endif  // SRC_LIB_ELF_PROGRAMHEADER64_H
