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

#include "Section64.h"
#include "Error.h"

namespace elf
{

Section64::Section64(File64 *file, int index, unsigned info_offset) {
	// Read section header
	info = (Elf64_Shdr *) (file->getBuffer() + info_offset);
	if (info_offset + sizeof(Elf64_Shdr) > file->getSize())
		throw Error(file->getPath(), "Invalid position for "
				"section header");

	// Initialize
	size = info->sh_size;

	// Get section contents, if section type is not SHT_NOBITS
	// (sh_type = 8).
	if (info->sh_type != 8)
	{
		// Check valid range
		if (info->sh_offset + info->sh_size > file->getSize())
			throw Error(file->getPath(), "Section out of range");

		// Set up buffer and stream
		buffer = file->getBuffer() + info->sh_offset;
	}
}

}  // namespace elf
