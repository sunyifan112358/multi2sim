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

#include "ProgramHeader64.h"
#include "File64.h"
#include "Section64.h"
#include "Error.h"

namespace elf
{

ProgramHeader64::ProgramHeader64(File64 *file, int index, unsigned info_offset) :
		ProgramHeader(index),
		file(file)
{
	// Initialize
	this->file = file;
	this->index = index;

	// Read program header
	info = (Elf64_Phdr *) (file->getBuffer() + info_offset);
	if (info_offset + sizeof(Elf64_Phdr) > file->getSize())
		throw Error(file->getPath(), "Invalid position for program "
				"header");

	// File content
	size = info->p_filesz;
	buffer = file->getBuffer() + info->p_offset;
}


void ProgramHeader64::getStream(std::istringstream &stream, unsigned int offset,
		unsigned int size) const
{
	// Check valid offset/size
	if (offset + size > this->size)
		throw Error(file->getPath(), "Invalid offset/size");

	// Set substream
	std::stringbuf *buf = stream.rdbuf();
	buf->pubsetbuf(const_cast<char *>(buffer) + offset, size);
}

}  // namespace elf
