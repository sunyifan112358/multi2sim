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

#ifndef SRC_LIB_ELF_FILE_H
#define SRC_LIB_ELF_FILE_H

#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <elf.h>

namespace elf
{

/// A elf::File is a File that follows the Linux Executable and Linkable File
/// format. This class provides some convenient interface for retrieving the
/// information from the ELF file.
class File
{
protected:

	// Path if loaded from a file
	std::string path;

	// Content of the ELF file
	std::unique_ptr<char[]> buffer;

	// Total size of the ELF file
	unsigned size;

	// Stream pointing to the file content
	std::istringstream stream;

public:
	File(const std::string &path) : path(path)
	{
	};

	/// Default destructor
	virtual ~File() {}

	/// Dump file information into output stream
	friend std::ostream &operator<<(std::ostream &os, const File &file);

	/// Return the path where the file was loaded from, in case is
	/// was loaded from the file system. If the ELF file was loaded from a
	/// buffer in memory, it returns an empty string.
	const std::string &getPath() const { return path; }

	/// Return the total size of the file
	unsigned int getSize() const { return size; }

	/// Return a buffer to the content of the file
	const char *getBuffer() const { return buffer.get(); }

	/// Return \a e_ident field of ELF header
	virtual unsigned char *getIdent() const = 0;
};

}  // namespace elf

#endif  // SRC_LIB_ELF_FILE_H
