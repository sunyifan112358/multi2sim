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

#ifndef ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_DISASSEMBLER_H
#define ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_DISASSEMBLER_H

#include <memory>

#include <arch/common/Disassembler.h>

#include "Instruction.h"

namespace VI 
{

class Disassembler : public comm::Disassembler
{ 
 	// Unique instance of Disassembler 
	static std::unique_ptr<Disassembler> instance;
	
	// FIXME change all count values	
	static const int dec_table_sopp_count = 29;
	static const int dec_table_sopc_count = 21;
	static const int dec_table_sop1_count = 49;
	static const int dec_table_sopk_count = 20; // no Opcode 19
	static const int dec_table_sop2_count = 43;
	static const int dec_table_smem_count = 25;


	Instruction::Info inst_info[Instruction::OpcodeCount];

	// Pointers to elements in 'inst_info'
	// FIXME names
	Instruction::Info *dec_table_sopp[dec_table_sopp_count];
	Instruction::Info *dec_table_sopc[dec_table_sopc_count];
	Instruction::Info *dec_table_sop1[dec_table_sop1_count];
	Instruction::Info *dec_table_sopk[dec_table_sopk_count];
	Instruction::Info *dec_table_sop2[dec_table_sop2_count];
	Instruction::Info *dec_table_smem[dec_table_smrd_count];
	Instruction::Info *dec_table_vop3[dec_table_vop3_count];
	Instruction::Info *dec_table_vopc[dec_table_vopc_count];
	Instruction::Info *dec_table_vop1[dec_table_vop1_count];
	Instruction::Info *dec_table_vop2[dec_table_vop2_count];
	Instruction::Info *dec_table_vintrp[dec_table_vintrp_count];
	Instruction::Info *dec_table_ds[dec_table_ds_count];
	Instruction::Info *dec_table_mtbuf[dec_table_mtbuf_count];
	Instruction::Info *dec_table_mubuf[dec_table_mubuf_count];
	Instruction::Info *dec_table_mimg[dec_table_mimg_count];
	Instruction::Info *dec_table_exp[dec_table_exp_count];

	void DisassembleBuffer(std::ostream& os, const char *buffer, int size);	
		
	// Path to binary file
	static std::string binary_file;

public:

	Disassembler() : comm::Disassembler("vi") {}
   
	/// Disassemble binary file
	void DisassembleBinary(const std::string &path);

	/// Get the only instance of the Volcanic Islands disassembler.	
	static Disassembler *getInstance();

	/// Destroy the disassembler singleton	
	static void Destroy () 
	{
		instance = nullptr;
	}

	/// Register command-line options
	static void RegisterOptions();

	/// Process command-line options
	static void ProcessOptions();
	
	Instruction::Info *getDecTableSop2(int index)
	{
		assert(index>= 0 && index < dec_table_sop2_count);
		return dec_table_sop2[index];	
	}
	// ...
};

}

#endif

