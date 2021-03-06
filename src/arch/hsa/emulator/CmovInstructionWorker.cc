/*
 *  Multi2Sim
 *  Copyright (C) 2014  Yifan Sun (yifansun@coe.neu.edu)
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

#include <lib/cpp/String.h>
#include <lib/cpp/Error.h>
#include <arch/hsa/disassembler/AsmService.h>
#include <arch/hsa/disassembler/Brig.h>
#include <arch/hsa/disassembler/BrigCodeEntry.h>

#include "CmovInstructionWorker.h"
#include "WorkItem.h"

namespace HSA {

CmovInstructionWorker::CmovInstructionWorker (WorkItem *work_item,
		StackFrame *stack_frame) :
		HsaInstructionWorker(work_item, stack_frame)
{
}


CmovInstructionWorker::~CmovInstructionWorker ()
{
}


template<typename T>
void CmovInstructionWorker::Inst_CMOV_Aux(BrigCodeEntry *instruction)
{
	// Retrieve src0 value
	unsigned char src0;
	operand_value_retriever->Retrieve(instruction, 1, &src0);

	// Retrieve src1, src2
	T src1, src2;
	operand_value_retriever->Retrieve(instruction, 2, &src1);
	operand_value_retriever->Retrieve(instruction, 3, &src2);

	// Move to dst value
	T dst;
	if (src0)
		dst = src1;
	else
		dst = src2;
	operand_value_writer->Write(instruction, 0, &dst);

	// Move PC forward
	work_item->MovePcForwardByOne();
}


void CmovInstructionWorker::Execute(BrigCodeEntry *instruction)
{
	// Call auxiliary function on different type
	switch (instruction->getType()){
	case BRIG_TYPE_B1:

		Inst_CMOV_Aux<unsigned char>(instruction);
		break;

	case BRIG_TYPE_B32:
	case BRIG_TYPE_U32:

		Inst_CMOV_Aux<unsigned int>(instruction);
		break;

	case BRIG_TYPE_B64:
	case BRIG_TYPE_U64:

		Inst_CMOV_Aux<unsigned long long>(instruction);
		break;

	case BRIG_TYPE_S32:

		Inst_CMOV_Aux<int>(instruction);
		break;

	case BRIG_TYPE_S64:

		Inst_CMOV_Aux<long long>(instruction);
		break;

	case BRIG_TYPE_F32:

		Inst_CMOV_Aux<float>(instruction);
		break;

	case BRIG_TYPE_F64:

		Inst_CMOV_Aux<double>(instruction);
		break;

	default:

		throw misc::Panic("Unsupported type for opcode CMOV.");
		break;

	}
}

}

