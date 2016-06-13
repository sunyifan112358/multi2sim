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



const misc::StringMap Instruction::dpp_cntl_map =
{
	{"", 0} // TODO
};

const misc::StringMap Instruction::format_map =
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
	{"exp", FormatEXP},
	{"flat", FormatFLAT}
};

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
	{"-2.0", 5},
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

void Instruction::DumpOperandExp(std::ostream& os, int operand)
{
	assert(operand >= 0 && operand <= 63);
	if(operand <=7)
	{
		os << "exp_mrt_" << operand;
	}
	else if(operand == 8)
	{
		os << "exp_mrtz";

	}
	else if(operand == 9)
	{
		os << "exp_null";
	}
	else if(operand < 12)
	{
		throw Disassembler::Error(misc::fmt("Operand code [%d] unused.",
					operand));
	}
	else if(operand <= 15)
	{
		os<< "exp_pos_" << operand - 12;
	}
	else if(operand < 32)
	{
		throw Disassembler::Error(misc::fmt("Operand code [%d] unused.",
					operand));	
	}
	else if(operand <= 63)
	{
		os << "exp_prm_" << operand - 32;
	}
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
	else if(operand <=511)
	{
		// VGPR 0-255	
		os << "v" << operand - 256;
	}
}

void Instruction::DumpOperandSeries(std::ostream& os, int start, int end)
{
	assert(start <= end);
	if(start == end)
	{
		DumpOperand(os, start);
		return;
	}

	if(start <= 101)
	{
		os << "s[" << start << ':' << end << ']';
	}
	else if(start <= 245)
	{
		if(start >= 112 && start <= 123)
		{
			os << "ttmp[" << start - 112 << ':' << end - 112 << ']';
		}
		else
		{
			assert(end == start + 1);
			switch(start)
			{
			case 102:
				os << "flat_scr";
				break;
			case 104:
				os << "xnack_mask";
				break;
			case 106:
				os << "vcc";
				break;
			case 108:
				os << "tba";
				break;
			case 110:
				os << "tma";
				break;
			case 126:
				os << "exec";
				break;
			case 128:
				os << "0";
				break;
			case 240:
				os << "0.5";
				break;
			case 242:
				os << "1.0";
				break;
			case 243:
				os << "-1.0";
				break;
			case 244:
				os << "2.0";
				break;
			case 245:
				os << "-2.0";
				break;
			default:
				throw Disassembler::Error(misc::fmt(
							"Unimplemented series: "
							"[%d:%d]", start, end));
			}
		}
	}
	else if(start <= 255)
	{
		throw Disassembler::Error(misc::fmt("Illegal operand series: [%d:%d]",
						start, end));
	}
	else if(start <=511)
	{
		os << "v[" << start - 256 << ':' << end - 256 << ']';
	}
}

void Instruction::DumpVector(std::ostream& os, int operand)
{
	DumpOperand(os, operand + 256);
}

void Instruction::Dump(std::ostream &os) const
{
	int token_len;
	const char *fmt_str;

	fmt_str = info ? info->fmt_str : "";
	while(*fmt_str)
	{
		if(*fmt_str != '%')
		{
			os << *fmt_str;
			fmt_str++;
			continue;
		}

		fmt_str++; // '%'
		// TODO add token handlers here
		if (comm::Disassembler::isToken(fmt_str, "LABEL", token_len))
		{		
			const BytesSOPP *sopp = &bytes.sopp;
	
			short simm16 = sopp->simm16;
			int se_simm = simm16;

			os << misc::fmt("label_%04X",
					(address + (se_simm * 4) + 4) / 4);
		}
		else if(comm::Disassembler::isToken(fmt_str, "TGT", token_len))
		{
			DumpOperandExp(os, bytes.exp.tgt);
		}
		else if(comm::Disassembler::isToken(fmt_str, "EXP_VSRCs", token_len))
		{
			if(bytes.exp.compr == 0 && (bytes.exp.en && 0x0) == 0x0)
			{
				os << '[';
				DumpVector(os, bytes.exp.vsrc0);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc1);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc2);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc3);
				os << ']';
			}
			else if(bytes.exp.compr == 1 && (bytes.exp.en && 0x0) == 0x0)
			{
				os << '[';
				DumpVector(os, bytes.exp.vsrc0);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc1);
				os << ']';
			}
		}
		else
		{
			throw misc::Panic(misc::fmt("%s: token not recognized.",
						fmt_str));
		}

		fmt_str += token_len;
	}
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

void Instruction::Decode(const char *buf, unsigned int address)
{
	info = NULL;
	size = 4;
	bytes.word[0] = *(unsigned int*) buf;
	bytes.word[1] = 0;
	this->address = address;

	if(bytes.sop2.enc == 0x2)
	{
		if(!disassembler->getDecTableSop2(bytes.sop2.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SOP2: %d "
					"// %08X: %08X\n", bytes.sop2.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableSop2(bytes.sop2.op);


		// only one src field may use a literal constant, indicated by 255
		assert(!(bytes.sop2.ssrc0 == 0xFF && bytes.sop2.ssrc1 == 0xFF));
		if(bytes.sop2.ssrc0 == 0xFF || bytes.sop2.ssrc1 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.sopk.enc == 0xB)
	{
		if(!disassembler->getDecTableSopk(bytes.sopk.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SOPK:%d  "
					"// %08X: %08X\n", bytes.sopk.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableSopk(bytes.sopk.op);
	}
	else if(bytes.sop1.enc == 0x17D)
	{
		if(!disassembler->getDecTableSop1(bytes.sop1.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SOP1:%d  "
					"// %08X: %08X\n", bytes.sop1.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableSop1(bytes.sop1.op);

		// literal constant
		if(bytes.sop1.ssrc0 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.sopc.enc == 0x17E)
	{
		if(!disassembler->getDecTableSopc(bytes.sopc.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SOPC:%d  "
					"// %08X: %08X\n", bytes.sopc.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableSopc(bytes.sopc.op);

		// only one src field may use a literal constant, indicated by 255
		assert(!(bytes.sopc.ssrc0 == 0xFF && bytes.sopc.ssrc1 == 0xFF));
		if(bytes.sopc.ssrc0 == 0xFF || bytes.sopc.ssrc1 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.sopp.enc == 0x17F)
	{
		if(!disassembler->getDecTableSopp(bytes.sopp.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SOPP:%d  "
					"// %08X: %08X\n", bytes.sopp.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableSopp(bytes.sopp.op);
	}
	else if(bytes.smem.enc == 0x30)
	{
		// 64-bit instruction
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if(!disassembler->getDecTableSmem(bytes.smem.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: SMEM:%d  "
					"// %08X: %08X %08X\n", bytes.smem.op, address, 
					*(unsigned int *) buf, 
					*(unsigned int *) (buf +4)));
		}

		info = disassembler->getDecTableSmem(bytes.smem.op);
	}
	else if(bytes.vop2.enc == 0x0) // FIXME dpp/sdwa
	{
		if (!disassembler->getDecTableVop2(bytes.vop2.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: VOP2:%d  "
					"// %08X: %08X\n", bytes.vop2.op, address, * (unsigned int *) buf));
		}

		info = disassembler->getDecTableVop2(bytes.vop2.op);

		if (bytes.vop2.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}

		// some opcodes define a 32- or 16-bit inline constant. opcodes 36 and 37 define a 16-bit inline constant
		// stored in the following dword
		if (bytes.vop2.op == 23 || bytes.vop2.op == 24 || bytes.vop2.op == 36 || bytes.vop2.op == 37)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.vop1.enc == 0x3F) // FIXME dpp/sdwa
	{
		if(!disassembler->getDecTableVop1(bytes.vop1.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: VOP1:%d  "
					"// %08X: %08X\n", bytes.vop1.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableVop1(bytes.vop1.op);

		if(bytes.vop1.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.vopc.enc == 0x3E) // FIXME dpp/sdwa
	{
		if(!disassembler->getDecTableVopc(bytes.vopc.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: VOPC:%d  "
					"// %08X: %08X\n", bytes.vopc.op, address, *(unsigned int *) buf));
		}

		info = disassembler->getDecTableVopc(bytes.vopc.op);

		if(bytes.vopc.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = *(unsigned long long *) buf;
		}
	}
	else if(bytes.vop3a.enc == 0x34)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableVop3(bytes.vop3a.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: VOP3:%d  "
					"// %08X: %08X %08X\n", bytes.vop3a.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableVop3(bytes.vop3a.op);
	}
	else if(bytes.vintrp.enc == 0x32)
	{
		if(!disassembler->getDecTableVintrp(bytes.vintrp.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: VINTRP:%d  "
					"// %08X: %08X\n", bytes.vintrp.op, address, *(unsigned int *) buf));
		}
		
		info = disassembler->getDecTableVintrp(bytes.vintrp.op);
	}
	else if(bytes.ds.enc == 0x36)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableDs(bytes.ds.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: DS:%d  "
					"// %08X: %08X %08X\n", bytes.ds.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableDs(bytes.ds.op);

	}
	else if(bytes.mubuf.enc == 0x38)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableMubuf(bytes.mubuf.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: MUBUF:%d  "
					"// %08X: %08X %08X\n", bytes.mubuf.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableMubuf(bytes.mubuf.op);

	}
	else if(bytes.mtbuf.enc == 0x3A)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableMtbuf(bytes.mtbuf.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: MTBUF:%d  "
					"// %08X: %08X %08X\n", bytes.mtbuf.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableMtbuf(bytes.mtbuf.op);

	}
	else if(bytes.mimg.enc == 0x3C)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableMimg(bytes.mimg.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: MIMG:%d  "
					"// %08X: %08X %08X\n", bytes.mimg.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableMimg(bytes.mimg.op);

	}
/*	else if(bytes.exp.enc == 0x31)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if (!disassembler->getDecTableExp(bytes.exp.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction: EXP:%d  "
					"// %08X: %08X %08X\n", bytes.exp.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}

		info = disassembler->getDecTableExp(bytes.exp.op);

	} */
	else if(bytes.flat.enc == 0x37)
	{
		size = 8;
		bytes.dword = *(unsigned long long *) buf;

		if(!disassembler->getDecTableFlat(bytes.flat.op))
		{
			throw misc::Panic(misc::fmt(
					"Unimplemented Instruction:: FLAT:%d  "
					"// %08X: %08X %08X\n", bytes.flat.op, address,
					*(unsigned int *) buf,
					*(unsigned int *) (buf + 4)));
		}
		
		info = disassembler->getDecTableFlat(bytes.flat.op);
	}
	else
	{
		throw misc::Panic(misc::fmt(
				"Unimplemented format. Instruction is:  "
				"// %08x: %08x\n", address, ((unsigned int*) buf)[0]));
	}
	// FIXME dpp/sdwa format?
}

} // namespace VI


