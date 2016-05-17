/*
 *  Multi2Sim
 *  Copyright (C) 2016  Harrison Barclay (barclay.h@husky.neu.edu)
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

#ifndef ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_BINARY_H
#define ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_BINARY_H

#include <lib/cpp/ELFReader.h>
#include <lib/cpp/String.h>


namespace VI
{



struct BinaryDictEntry
{

};

class Binary : ELFReader::File
{

	std::vector<BinaryDictEntry*> dict;

	// Member of vector dict - encoding entry containing
	// the Volcanic Islands kernel
	BinaryDictEntry *vi_dict_entry;

	// File name
	std::string name;
	
	void ReadNote(BinaryDictEntry *dict_entry, std::istringstream& ss,
			const char *buffer);
	void ReadNotes(BinaryDictEntry *dict_entry);
	void ReadDictionary();
	void ReadSegments();
	void ReadSections();

public:
	
	Binary(const char *buffer, unsigned int size, std::string name);
	~Binary();

	BinaryDictEntry *GetVIDictEntry() { return vi_dict_entry; }
};


} // namespace VI

#endif
