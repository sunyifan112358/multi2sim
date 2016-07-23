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

#ifndef ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_INSTRUCTION_H
#define ARCH_VOLCANIC_ISLANDS_DISASSEMBLER_INSTRUCTION_H

#include <lib/cpp/String.h>

namespace VI
{

class Disassembler;


class Instruction
{
public:
	/// Microcode Formats		
	enum Format
	{
		
		FormatInvalid = 0,
		
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
		FormatVOP_SDWA,
		FormatVOP_DPP,

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

		CategoryInvalid = 0,
				
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
		CategoryVOP_SDWA,
		CategoryVOP_DPP,

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
	

	enum SpecialReg
	{
		SpecialRegInvalid = 0,
		SpecialRegVcc,
		SpecialRegM0,
		SpecialRegExec,
		SpecialRegScc,
		SpecialRegTma
	};


	static const misc::StringMap special_reg_map;

	enum BufDataFormat
	{
		BufDataFormatInvalid = 0,
		BufDataFormat8 = 1,
		BufDataFormat16 = 2,
		BufDataFormat8_8 = 3,
		BufDataFormat32 = 4,
		BufDataFormat16_16 = 5,
		BufDataFormat10_11_11 = 6,
		BufDataFormat11_11_10 = 7,
		BufDataFormat10_10_10_12 = 8,
		BufDataFormat2_10_10_10 = 9,
		BufDataFormat8_8_8_8 = 10,
		BufDataFormat32_32 = 11,
		BufDataFormat16_16_16_16 = 12,
		BufDataFormat32_32_32 = 13,
		BufDataFormat32_32_32_32 = 14,
		BufDataFormatReserved = 15
	};

	static const misc::StringMap buf_data_format_map;	

	enum BufNumFormat
	{
		BufNumFormatUnorm = 0,
		BufNumFormatSnorm = 1,
		BufNumFormatUscaled = 2,
		BufNumFormatSscaled = 3,
		BufNumFormatUint = 4,
		BufNumFormatSint = 5,
		BufNumFormatFloat = 7,
		BufNumFormatSrgb = 9
	};
	
	static const misc::StringMap buf_num_format_map;

	enum ExpTgt
	{
		ExpTgtExp_Mrt = 0,
		ExpTgtExp_Mrtz = 8,
		ExpTgtExp_Null = 9,
		ExpTgtExp_Pos = 12,
		ExpTgtExp_Param = 32

	};

	static const misc::StringMap exp_tgt_map;

	enum Flag
	{
		FlagNone = 0x0000,
		FlagOp8 = 0x0001,
		FlagOp16 = 0x0002
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
	
	struct BytesSOP2
	{
		unsigned int ssrc0	: 8; // [7:0]
		unsigned int ssrc1	: 8; // [15:8]
		unsigned int sdst	: 7; // [22:16]
		unsigned int op		: 7; // [29:23]
		unsigned int enc	: 2; // [31:30]
		unsigned int lit_cnst	: 32;// [63:32]
	};

	struct BytesSOPK
	{
		unsigned int simm16	: 16; // [15:0]
		unsigned int sdst	: 7;  // [22:16]
		unsigned int op		: 5;  // [27:23]
		unsigned int enc	: 4;  // [31:28]

	};

	struct BytesSOP1
	{
		unsigned int ssrc0	: 8; // [7:0]
		unsigned int op		: 8; // [15:8]
		unsigned int sdst	: 7; // [22:16]
		unsigned int enc	: 9; // [31:23]
		unsigned int lit_cnst	: 32; // [63:32]
	};

	struct BytesSOPP
	{
		unsigned int simm16	: 16; // [15:0]
		unsigned int op		: 7; // [22:16]
		unsigned int enc	: 9; // [31:23]
	};

	struct BytesSOPC
	{
		unsigned int ssrc0	: 8; // [7:0]
		unsigned int ssrc1	: 8; // [15:8]
		unsigned int op		: 8; // [22:16]
		unsigned int enc	: 9; // [31:23]
		unsigned int lit_cnst	: 32; // [63:32]
	};

	struct BytesSMEM
	{
		unsigned int sbase	: 6; // [5:0]
		unsigned int sdata	: 7; // [12:6]
		unsigned int reserved0	: 3; // [15:13]
		unsigned int glc	: 1; // 16
		unsigned int imm	: 1; // 17
		unsigned int op		: 8; // [25:18]
		unsigned int enc	: 6; // [31:26]
		unsigned int offset	: 20; // [51:32]
		unsigned int reserved1	: 12; // [63:52]
	};

	struct BytesVOP2
	{
		unsigned int src0	: 9; //[8:0]
		unsigned int vsrc1	: 8; // [16:9]
		unsigned int vdst	: 8; // [24:17]
		unsigned int op		: 6; // [30:25]
		unsigned int enc	: 1; // 31
		unsigned int lit_cnst	: 32; // [63:32]
	};
	
	struct BytesVOP1
	{
		unsigned int src0	: 9; // [8:0]
		unsigned int op		: 8; // [16:9]
		unsigned int vdst	: 8; // [24:17]
		unsigned int enc	: 7; // [31:25]
		unsigned int lit_cnst	: 32; // [63:32]
	};

	struct BytesVOPC
	{
		unsigned int src0	: 9; // [8:0]
		unsigned int vsrc1	: 8; // [16:9]
		unsigned int op		: 8; // [24:17]
		unsigned int enc	: 7; // [31:25]
		unsigned int lit_cnst	: 32; // [63:32]
	};

	struct BytesVOP3a
	{
		unsigned int vdst	: 8; // [7:0]
		unsigned int abs	: 3; // [10:8]
		unsigned int reserved	: 4; // [14:11]
		unsigned int clamp	: 1; // 15
		unsigned int op		: 10; // [25:16]
		unsigned int enc	: 6; // [31:26]
		unsigned int src0	: 9; // [40:32]
		unsigned int src1	: 9; // [49:41]
		unsigned int src2	: 9; // [58:50]
		unsigned int omod	: 2; // [60:59]
		unsigned int neg	: 3; // [63:61]
	};

	struct BytesVOP3b
	{
		unsigned int vdst	: 8; // [7:0]
		unsigned int sdst	: 7; // [14:8]
		unsigned int clamp	: 1; // 15
		unsigned int op		: 10; // [25:16]
		unsigned int enc	: 6; // [31:26]
		unsigned int src0	: 9; // [40:32]
		unsigned int src1	: 9; // [49:41]
		unsigned int src2	: 9; // [58:50]
		unsigned int omod	: 2; // [60:59]
		unsigned int neg	: 3; // [63:61]
	};

	struct BytesVOP_SDWA
	{
		unsigned int src0	: 8; // [7:0]
		unsigned int dst_sel	: 3; // [10:8]
		unsigned int dst_unused	: 2; // [12:11]
		unsigned int clamp	: 1; // 13
		unsigned int src0_sel	: 3; // [18:16]
		unsigned int src0_sext	: 1; // 19
		unsigned int src0_neg	: 1; // 20
		unsigned int src0_abs	: 1; // 21
		unsigned int reserved0	: 2; // [23:22]
		unsigned int src1_sel	: 3; // [26:24]
		unsigned int src1_sext	: 1; // 27
		unsigned int src1_neg	: 1; // 28
		unsigned int src1_abs	: 1; // 29
		unsigned int reserved1	: 2; // [31:30]
	};

	struct BytesVOP_DPP
	{
		unsigned int src0	: 8; // [7:0]
		unsigned int dpp_ctrl	: 9; // [16:8]
		unsigned int reserved	: 2; // [18:17]
		unsigned int bound_ctrl	: 1; // 19
		unsigned int src0_neg	: 1; // 20
		unsigned int src0_abs	: 1; // 21
		unsigned int src1_neg	: 1; // 22
		unsigned int src1_abs	: 1; // 23
		unsigned int bank_mask	: 4; // [27:24]
		unsigned int row_mask	: 4; // [31:28]
	};

	struct BytesVINTRP
	{
		unsigned int vsrc	: 8; // [7:0]
		unsigned int attrchan	: 2; // [9:8]
		unsigned int attr	: 6; // [15:10]
		unsigned int op		: 2; // [17:16]
		unsigned int vdst	: 8; // [25:18]
		unsigned int enc	: 6; // [31:26]
	};

	struct BytesDS
	{
		unsigned int offset0	: 8; // [7:0]
		unsigned int offset1	: 8; // [15:8]
		unsigned int gds	: 1; // 16
		unsigned int op		: 8; // [24:17]
		unsigned int reserved	: 1; // 25
		unsigned int enc	: 6; // [31:26]
		unsigned int addr	: 8; // [39:32]
		unsigned int data0	: 8; // [47:40]
		unsigned int data1	: 8; // [55:48]
		unsigned int vdst	: 8; // [63:56]
	};

	struct BytesMUBUF
	{
		unsigned int offset	: 12; // [11:0]
		unsigned int offen	: 1; // 12
		unsigned int idxen	: 1; // 13
		unsigned int glc 	: 1; // 14
		unsigned int reserved0	: 1; // 15
		unsigned int lds	: 1; // 16
		unsigned int slc 	: 1; // 17
		unsigned int op		: 8; // [24:18]
		unsigned int reserved1	: 1; // 25
		unsigned int enc	: 6; // [31:26]
		unsigned int vaddr	: 8; // [39:32]
		unsigned int vdata	: 8; // [47:40]
		unsigned int srsrc	: 5; // [52:48]
		unsigned int reserved2	: 2; // [54:53]
		unsigned int tfe	: 1; // 55
		unsigned int soffset	: 6; // [63:56]

	};

	struct BytesMTBUF
	{
		unsigned int offset	: 12; // [11:0]
		unsigned int offen	: 1; // 12
		unsigned int idxen	: 1; // 13
		unsigned int glc	: 1; // 14
		unsigned int op		: 4; // [18:15]
		unsigned int demt	: 4; // [22:19]
		unsigned int nemt	: 3; // [25:23]
		unsigned int enc	: 7; // [31:26]
		unsigned int vaddr	: 8; // [39:32]
		unsigned int vdata	: 8; // [47:40]
		unsigned int srsrc	: 5; // [52:48]
		unsigned int reserved	: 1; // 53
		unsigned int slc	: 1; // 54
		unsigned int tfe	: 1; // 55
		unsigned int soffset	: 6; // [63:56]
	};

	struct BytesMIMG
	{
		unsigned int reserved0	: 8; // [7:0]
		unsigned int dmask	: 4; // [11:8]
		unsigned int unorm	: 1; // 12
		unsigned int glc	: 1; // 13
		unsigned int da		: 1; // 14
		unsigned int r128	: 1; // 15
		unsigned int tfe	: 1; // 16
		unsigned int lwe	: 1; // 17
		unsigned int op		: 8; // [24:18]
		unsigned int slc	: 1; // 25
		unsigned int enc	: 7; // [31:26]
		unsigned int vaddr	: 8; // [39:32]
		unsigned int vdata	: 8; // [47:40]
		unsigned int srsrc	: 5; // [52:48]
		unsigned int ssamp	: 5; // [57:53]
		unsigned int reserved1	: 5; // [62:58]
		unsigned int d16	: 1; // 63
	};

	struct BytesEXP
	{
		unsigned int en		: 4; // [3:0]
		unsigned int tgt	: 6; // [9:4]
		unsigned int compr	: 1; // 10
		unsigned int done	: 1; // 11
		unsigned int vm		: 1; // 12
		unsigned int reserved 	: 13; // [25:13]
		unsigned int enc	: 7; // [31:32]
		unsigned int vsrc0	: 8; // [39:32]
		unsigned int vsrc1	: 8; // [47:40]
		unsigned int vsrc2	: 8; // [55:48]
		unsigned int vsrc3	: 8; // [63:56]
	};

	struct BytesFLAT
	{
		unsigned int reserved0	: 16; // [15:0]
		unsigned int glc	: 1; // 16
		unsigned int slc	: 1; // 17
		unsigned int op		: 7; // [24:18]
		unsigned int unused	: 1; // 25 FIXME what is this?
		unsigned int enc	: 6; // [31:26]
		unsigned int addr	: 8; // [39:32]
		unsigned int data	: 8; // [47:40]
		unsigned int reserved1	: 7; // [54:48]
		unsigned int tfe	: 1; // 55
		unsigned int vdst	: 14; // [63:56]
	};

	union Bytes
	{
		unsigned char byte[8];
		unsigned int word[2];

		unsigned long long dword;

		BytesSOP2 sop2;
		BytesSOPK sopk;
		BytesSOP1 sop1;
		BytesSOPP sopp;
		BytesSOPC sopc;
		BytesSMEM smem;
		BytesVOP2 vop2;
		BytesVOP1 vop1;
		BytesVOPC vopc;
		BytesVOP3a vop3a;
		BytesVOP3b vop3b;
		BytesVOP_SDWA vop_sdwa;
		BytesVOP_DPP vop_dpp;
		BytesVINTRP vintrp;
		BytesDS ds;
		BytesMUBUF mubuf;
		BytesMTBUF mtbuf;
		BytesMIMG mimg;
		BytesEXP exp;
		BytesFLAT flat;
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

	
	static const misc::StringMap dpp_cntl_map;

private:

	Disassembler *disassembler;

	// all information
	Info *info;

	// instruction bytes
	Bytes bytes;

	// size in bites. includes literal constant
	int size;

	int address;

	static const misc::StringMap ssrc_map;
	static const misc::StringMap sdst_map;

	static const misc::StringMap op8_map;
	static const misc::StringMap op16_map;

	// Dump Functions

	static void DumpOperand(std::ostream& os, int operand);
	static void DumpOperandSeries(std::ostream& os, int start, int end);
	static void DumpVector(std::ostream& os, int operand);
	static void DumpVectorSeries(std::ostream& os, int start, int end);
	static void DumpOperandExp(std::ostream& os, int operand);
	static void DumpScalar(std::ostream& os, int operand);
	static void DumpScalarSeries(std::ostream& os, int start, int end);
	void DumpSsrc(std::ostream& os, unsigned int ssrc) const;
	void DumpVop364Src(std::ostream& os, unsigned int src, int neg) const;
	void DumpVop3Src(std::ostream& os, unsigned int src, int neg) const; 

public:

	Instruction();

	static const unsigned RegisterFLAT_SCR = 102;
	static const unsigned RegisterVCC = 106;	
	static const unsigned RegisterM0 = 124;
	static const unsigned RegisterEXEC = 126;
	static const unsigned RegisterVCCZ = 251;
	static const unsigned RegisterEXECZ = 252;
	static const unsigned RegisterSCC = 253;
	
	
	void Decode(const char *buf, unsigned int address);

	void Dump(std::ostream &os = std::cout) const;	

	void DumpAddress(std::ostream &os = std::cout) const;


	friend std::ostream &operator<<(std::ostream& os, const Instruction &ins)
	{
		ins.Dump(os);
		return os;
	}

	// reset content
	void Clear();

	int getOp() { return info ? info->op : 0; }
	Opcode getOpcode() { return info ? info->opcode : OpcodeInvalid; }
	Format getFormat() { return info ? info->fmt : FormatInvalid; }
	const char *getName() { return info ? info->name : "<unkown>"; }
	Bytes *getBytes(){ return &bytes; }
	int getSize(){ return size; }	

};

} //namespace VI



#endif 
