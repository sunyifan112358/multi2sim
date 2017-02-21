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

#ifndef SRC_LIB_ELF_SECTION_H
#define SRC_LIB_ELF_SECTION_H

#include <string>
#include <elf.h>

namespace elf
{

class File;


/// A elf::Section is a section in an ELF file
class Section
{
protected:

	// Name of the section
	std::string name;

	// Section content
	const char *buffer = nullptr;

	// Size of the section in bytes
	unsigned size;

	// Section information
	int index;

public:

	/// Destructor
	virtual ~Section() {}

	/// Set the section name. This function is used internally by the ELF
	/// reader, and should not be called.
	void setName(const std::string &name) { this->name = name; }

	/// Return the index that the section occupies in the section
	/// list of the File object where it belongs.
	int getIndex() const { return index; }

	/// Return the section name
	const std::string &getName() const { return name; }

	/// Return a pointer to the section content. The section content
	/// is not a dedicated buffer. Instead, the returned pointer points to
	/// the internal buffer representing the entire ELF file.
	const char *getBuffer() const { return buffer; }
};

}  // namespace elf

#endif  // SRC_LIB_ELF_SECTION_H
