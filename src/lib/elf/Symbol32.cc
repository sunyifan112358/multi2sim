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

#include "Error.h"
#include "File32.h"
#include "Section32.h"
#include "Symbol32.h"

#include <lib/cpp/String.h>


namespace elf
{

Symbol32::Symbol32(File32 *file, Section32 *section, unsigned int pos)
{
	// Initialize
	this->file = file;

	// Read symbol
	info = (Elf32_Sym *) (section->getBuffer() + pos);
	if (pos + sizeof(Elf32_Sym) > section->getSize())
		throw Error(file->getPath(), "Invalid position for symbol");

	// Get section with symbol name
	unsigned name_section_index = section->getLink();
	Section32 *name_section = file->getSection(name_section_index);
	if (!name_section)
		throw Error(file->getPath(), "Invalid index for symbol name "
				"section");

	// Get symbol name
	if (info->st_name >= name_section->getSize())
		throw Error(file->getPath(), "Invalid symbol name offset");
	name = name_section->getBuffer() + info->st_name;

	// Get section in 'st_shndx'
	this->section = file->getSection(info->st_shndx);

	// If symbol points to a valid region of the section, set
	// variable buffer.
	buffer = nullptr;
	if (this->section && info->st_value + info->st_size <=
			this->section->getSize())
		buffer = this->section->getBuffer() + info->st_value;
}


bool Symbol32::Compare(const std::unique_ptr<Symbol32>& a,
		const std::unique_ptr<Symbol32>& b)
{
	int bind_a;
	int bind_b;

	// Sort by value
	if (a->info->st_value < b->info->st_value)
		return true;
	else if (a->info->st_value > b->info->st_value)
		return false;

	// Sort symbol with the same address as per their
	// ST_BIND field in st_info (bits 4 to 8)
	bind_a = ELF32_ST_BIND(a->info->st_info);
	bind_b = ELF32_ST_BIND(b->info->st_info);
	if (bind_a < bind_b)
		return true;
	else if (bind_a > bind_b)
		return false;

	// Sort alphabetically
	return a->name < b->name;
}


void Symbol32::getStream(std::istringstream &stream, unsigned int offset,
		unsigned int size) const
{
	// Symbol without content
	if (!buffer)
		throw Error(file->getPath(),
				misc::fmt("symbol '%s' does not have any valid content",
				name.c_str()));

	// Check valid offset/size
	if (size > info->st_size)
		throw Error(file->getPath(),
				misc::fmt("symbol '%s': invalid size",
				name.c_str()));

	// Set substream
	std::stringbuf *buf = stream.rdbuf();
	buf->pubsetbuf(const_cast<char *>(buffer) + offset, size);
}

}  // namespace elf
