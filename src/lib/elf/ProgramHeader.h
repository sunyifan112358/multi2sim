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

#ifndef SRC_LIB_ELF_PROGRAMHEADER_H
#define SRC_LIB_ELF_PROGRAMHEADER_H

#include <elf.h>
#include <sstream>

namespace elf
{

class File;

/// A elf::ProgramHeader is an ELF program header.
class ProgramHeader
{
protected:

	// Index of the program header inf the ELF file
	int index;

	// Content pointed to by the program header
	const char *buffer = nullptr;

	// Size of the content pointed to by program header
	unsigned size;

public:
	/// Constructor
	ProgramHeader(int index) : index(index)
	{
	}

	/// Default destructor
	virtual ~ProgramHeader() {};

	/// Return the index of the program header in the program header
	/// list of the File object where it belongs.
	int getIndex() const { return index; }

	/// Return the size of the segment pointed to by the program
	/// header
	unsigned int getSize() const { return size; }

	/// Return a pointer to the segment content
	const char *getBuffer() const { return buffer; }
};

}  // namespace elf

#endif  // SRC_LIB_ELF_PROGRAMHEADER_H
