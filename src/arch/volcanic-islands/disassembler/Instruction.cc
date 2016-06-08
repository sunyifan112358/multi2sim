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


#include <iomanip>
#include <sstream>
#include <cassert>

#include <arch/common/Disassembler.h>
#include <lib/cpp/Misc.h>
#include "Disassembler.h"
#include "Instruction.h"

namespace VI
{

const misc:StringMap Instruction::format_map =
{
	{"<invalid>", FormatInvalid},
	{"sop2", FormatSOP2},
	{"sopk", FormatSOPK},
	{"sop1", FormatSOP1},
	{"sopc", FormatSOPC},
	{"sopp", FormatSOPP},
	{"smem", FormatSMEM},
	{"vop2", FormatVOP2},
	{"vop1", FormatVOP1},
	{"vopc", FormatVOPC},
	{"vop3a", FormatVOP3a},
	{"vop3b", FormatVOP3b},
	{"vop_sdwa", FormatVOP_SDWA},
	{"vop_dpp", FormatVOP_DPP},
	{"vintrp", FormatVINTRP},
	{"ds", FormatDS},
	{"mubuf", FormatMUBUF},
	{"mtbuf", FormatMTBUF},
	{"mimg", FormatMIMG},
	{"exp", FormatEX},
	{"flat", FormatFLAT}
}

const misc::StringMap Instruction::sdst_map =
{
	{"flat_scr_lo", 0},
	{"flat_scr_hi", 1},
	{"xnack_mask_lo", 2},
	{"xnack_mask_hi", 3},
	{"vcc_lo", 4},
	{"vcc_hi", 5},
	{"tba_lo", 6},
	{"tba_hi", 7},
	{"tma_lo", 8},
	{"tma_hi", 9},
	{"ttmp0", 10},
	{"ttmp1", 11},
	{"ttmp2", 12},
	{"ttmp3", 13},
	{"ttmp4", 14},
	{"ttmp5", 15},
	{"ttmp6", 16},
	{"ttmp7", 17},
	{"ttmp8", 18},
	{"ttmp9", 19},
	{"ttmp10", 20},
	{"ttmp11", 21},
	{"m0", 22},
	{"reserved", 23},
	{"exec_hi", 24},
	{"exec_lo", 25}
};

const misc::StringMap Instruction::ssrc_map =
{
	{"0.5", 0},
	{"-0.5", 1},
	{"1.0", 2},
	{"-1.0", 3},
	{"2.0", 4},
	{"-2.0, 5},
	{"4.0", 6},
	{"-4.0", 7},
	{"1/(2*PI)", 8},
	{"reserved", 9},
	{"reserved", 10},
	{"vccz", 11},
	{"execz", 12},
	{"scc", 13},
	{"reserved", 14},
	{"literal constant", 15}
};

const misc::StringMap Instruction::buf_data_format_map =
{
	{"invalid", BufDataFormatInvalid},
	{"BUF_DATA_FORMAT_8", BufDataFormat8},
	{"BUF_DATA_FORMAT_16", BufDataFormat16},
	{"BUF_DATA_FORMAT_8_8", BufDataFormat8_8},
	{"BUF_DATA_FORMAT_32", BufDataFormat32},
	{"BUF_DATA_FORMAT_16_16", BufDataFormat16_16},
	{"BUF_DATA_FORMAT_10_11_11", BufDataFormat10_11_11},
	{"BUF_DATA_FORMAT_11_11_10", BufDataFormat11_11_10},
	{"BUF_DATA_FORMAT_10_10_10_12", BufDataFormat10_10_10_12},
	{"BUF_DATA_FORMAT_2_10_10_10", BufDataFormat2_10_10_10},
	{"BUF_DATA_FORMAT_8_8_8_8", BufDataFormat8_8_8_8},
	{"BUF_DATA_FORMAT_32_32", BufDataFormat32_32},
	{"BUF_DATA_FORMAT_16_16_16_16", BufDataFormat16_16_16_16},
	{"BUF_DATA_FORMAT_32_32_32", BufDataFormat32_32_32},
	{"BUF_DATA_FORMAT_32_32_32_32", BufDataFormat32_32_32_32},
	{"reserved", BufDataFormatReserved}
};

const misc::StringMap Instruction::buf_num_format_map =
{
	{"BUF_NUM_FORMAT_UNORM", BufNumFormatUnorm},
	{"BUF_NUM_FORMAT_SNORM", BufNumFormatSnorm},
	{"BUF_NUM_FORMAT_USCALED", BufNumFormatUscaled},
	{"BUF_NHUM_FORMAT_SSCALED", BufNumFormatSscaled},
	{"BUF_NUM_FORMAT_UINT", BufNumFormatUint},
	{"BUF_NUM_FORMAT_SINT", BufNumFormatSint},
	{"BUF_NUM_FORMAT_FLOAT", BufNumFormatFloat},
	{"BUF_NUM_FORMAT_SRGB", BufNumFormatSrgb},
	{"reserved", 6},
	{"reserved", 8},
	{"reserved", 10},
	{"reserved", 11},
	{"reserved", 12},
	{"reserved", 13},
	{"reserved", 14},
	{"reserved", 15}
};

const misc::StringMap Instruction::op8_map =
{
	{"f", 0},
	{"lt", 1},
	{"eq", 2},
	{"le", 3},
	{"gt", 4},
	{"lg", 5},
	{"ge", 6},
	{"tru", 7}
};

const misc::StringMap Instruction::op16_map =
{
	{"f", 0},
	{"lt", 1},
	{"eq", 2},
	{"le", 3},
	{"gt", 4},
	{"lg", 5},
	{"ge", 6},
	{"o", 7},
	{"u", 8},
	{"nge", 9},
	{"nlg", 10},
	{"ngt", 11},
	{"nle", 12},
	{"neq", 13},
	{"nlt", 14},
	{"tru", 15}
};

const misc::StringMap Instruction::special_reg_map =
{
	{"<invalid>", SpecialRegInvalid},
	{"vcc", SpecialRegVcc},
	{"m0", SpecialRegM0},
	{"exec", SpecialRegExec},
	{"scc", SpecialRegScc},
	{"tma", SpecialRegTma}
};


Instruction::Instruction()
{
	this->disassembler = Disassembler::getInstance();
	Clear();
}


void Instruction::DumpOperand(std::ostream& os, int operand)
{
	assert(operand >= 0 && operand <= 511);
	if(operand <= 101)
	{
		// SGPR 0-101
		os << "s" << operand;
	}
	else if(operand <= 127)
	{
		os << sdst_map.MapValue(operand - 102);
	}
	else if(operand <= 192)
	{
		os << operand - 128;
	}
	else if(operand <= 208)
	{
		os << "-" << operand - 192;
	}
	else if(operand <= 239)
	{
		throw Disassembler::Error(misc::fmt("Unused operand code (%d)", operand));
	}
	else if(operand <= 255)
	{
		os << ssrc_map.MapValue(operand - 240);
	}
	else if(operand <=511
	{
		// VGPR 0-255	
		os << "v" << operand - 256;
	}
}

void Instruction::DumpOperandSeries(std::ostream& os, int start, int end)
{
	
}

void Instruction::Dump(std::ostream &os) const
{
	// TODO

}


void Instruction::DumpAddress(std::ostream &os) const
{
	
	if(os.tellp() < 59)
		os << std::string(59 - os.tellp(), ' ');

	os << misc::fmt("// %08X: %08x", address, bytes.word[0]);
	
	if(size == 8)
		os << misc::fmt(" %08X", bytes.word[1]);

	os << '\n';
}

void Instruction::Clear()
{
	bytes.dword = 0;
	size = 0;
	address = 0;
	info = NULL;
}

Instruction::Decode()
{
	// TODO	
}

} // namespace VI


