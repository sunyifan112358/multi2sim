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

#ifndef SRC_LIB_ELF_SECTION32_H
#define SRC_LIB_ELF_SECTION32_H

#include "Section.h"
#include "Section32.h"

namespace elf
{
class File32;

/// A elf::Section32 is a 32-bit section.
class Section32: public Section
{
	// File that it belongs to
	File32 *file;

	// Raw section header
	Elf32_Shdr *info;

public:
	/// Constructor. An ELF section is only created internally by the ELF
	/// reader, so this function should not be invoked.
	///
	/// \param file
	///	ELF file that the section belongs to.
	///
	/// \param index
	///	Section index within the ELF file
	///
	/// \param info_offset
	///	Offset within the ELF file where the section header can be
	///	found.
	Section32(File32 *file, int index, unsigned info_offset);

	/// Return a pointer to the Elf32_Shdr structure representing the
	/// section header. Each field of this structure can be queried with
	/// dedicated getters instead.
	Elf32_Shdr *getRawInfo() const { return info; }

	/// Return the section name
	const std::string &getName() const { return name; }

	/// Return the value of field \a sh_name of the section header
	Elf32_Word getNameOffset() const { return info->sh_name; }

	/// Return the value of field \a sh_type of the section header
	Elf32_Word getType() const { return info->sh_type; }

	/// Return the value of field \a sh_flags of the section header
	Elf32_Word getFlags() const { return info->sh_flags; }

	/// Return the value of field \a sh_addr of the section header
	Elf32_Addr getAddr() const { return info->sh_addr; }

	/// Return the value of field \a sh_offset of the section header
	Elf32_Off getOffset() const { return info->sh_offset; }

	/// Return the value of field \a sh_size of the section header
	Elf32_Word getSize() const { return info->sh_size; }

	/// Return the value of field \a sh_link of the section header
	Elf32_Word getLink() const { return info->sh_link; }

	/// Return the value of field \a sh_info of the section header
	Elf32_Word getInfo() const { return info->sh_info; }

	/// Return the value of field \a sh_addralign of the section
	/// header.
	Elf32_Word getAddrAlign() const { return info->sh_addralign; }

	/// Return the value of field \a sh_entsize of the section header
	Elf32_Word getEntSize() const { return info->sh_entsize; }
};

}  // namespace elf

#endif  // SRC_LIB_ELF_SECTION32_H
