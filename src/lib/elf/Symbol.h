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

#ifndef SRC_LIB_ELF_SYMBOL_H
#define SRC_LIB_ELF_SYMBOL_H

#include <memory>
#include <string>
#include <elf.h>

namespace elf
{

class File;
class Section;

/// A elf::Symbol represents a symbol in an ELF file.
class Symbol
{
protected:

	// Symbol name
	std::string name;

	// Content pointed to by symbol in the section, or null if it points
	// to an invalid region of the section, or doesn't point to any section.
	const char *buffer;

public:

	/// Return the symbol name
	const std::string &getName() const { return name; }

	/// Return a pointer to the section content pointed to by the
	/// symbol, if any.
	const char *getBuffer() const { return buffer; }
};

}  // namespace elf

#endif  // SRC_LIB_ELF_SYMBOL_H
