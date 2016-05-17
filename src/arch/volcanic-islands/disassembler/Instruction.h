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

#ifndef ARCH_SOUTHERN_ISLANDS_DISASSEMBLER_INSTRUCTION_H
#define ARCH_SOUTHERN_ISLANDS_DISASSEMBLER_INSTRUCTION_H

namespace VI
{
//Forward declarations
class Disassembler;


class Instruction
{
public:
	/// Microcode Formats		
	enum Format
	{
		//Scalar ALU Formats
		FormatSOP2,
		FormatSOPK,
		FormatSOP1,
		FormatSOPC,
		FormatSOPP,

		//Scalar Memory Format
		FormatSMEM,

		//Vector ALU Formats
		FormatVOP2,
		FormatVOP1,
		FormatVOPC,
		FormatVOP3a,
		FormatVOP3b,

		// Vector Parameter Interpolation Format
		FormatVINTRP,

		// LDS/GDS Format
		FormatDS,

		// Vector Memory Buffer Formats
		FormatMUBUF,
		FormatMTBUF,
		
		// Vector Memory Image Format
		FormatMIMG,

		// Export Formats
		FormatEXP,
		
		// FLAT Format
		FormatFLAT,

		// Max
		FormatCount
	};
	
	/// String map to convert values of type Format	
	static const misc::StringMap format_map;	

	/// Instruction category
	enum Category
	{
		
		// Scalar ALU and control Intructions
		CategorySOP2,
		CategorySOPK,
		CategorySOP1,
		CategorySOPP,
		CategorySOPC,
		
		// Scalar Memory Instruction
		CategorySMEM,
		
		// Vector ALU
		CategoryVOP2,
		CategoryVOP1,
		CategoryVOPC,
		CategoryVOP3a,
		CategoryVOP3b,

		// Vector Parameter Interpolation
		CategoryVINTRP,

		// LDS/GDS
		CategoryDS,

		// Vector Memory Buffer
		CategoryMUBUF,
		CategoryMTBUF,

		// Vector Memory Image
		CategoryMIMG,

		// EXPORT
		CategoryEXP,

		// FLAT memory instructions
		CategoryFLAT,

		CategoryCount
	};
	

	
	enum Opcode
	{
		OpcodeInvalid = 0,

#define DEFINST(_name, _fmt_str, _fmt, _opcode, _size, _flags) \
	Opcode_##_name,

#include "Instruction.def"
#undef DEFINST

		OpcodeCount
	};


	struct Info
	{
		Opcode opcode;

		Category category;

		const char *name;
		
		const char *fmt_str;

		Format fmt;

		/// Opcode bits
		int op;

		/// Bitmap of flags
		Flag flags;

		/// Size of the micro-code format in bytes, not counting a
		/// possible additional literal added by a particular instance.
		int size;
	};


private:

	Disassembler *disassembler;




};

} //namespace VI



#endif 
