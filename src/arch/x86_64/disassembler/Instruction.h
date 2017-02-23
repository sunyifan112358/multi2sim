/*
 *  Multi2Sim
 *  Copyright (C) 2017 Yifan Sun(yifansun@coe.neu.edu)
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

#ifndef ARCH_X86_64_DISASSEMBLER_INSTRUCTION_H
#define ARCH_X86_64_DISASSEMBLER_INSTRUCTION_H

#include <cstdint>
#include <assert.h>

namespace x86_64 {

/// X86_64 instructions
class Instruction {

  enum Opcode {
		OpcodeInvalid = 0,
#define DEFINST(name, prefix, prefix0f, opcode_ext, po, so, op1, op2, op3, op4) \
    Opcode_##name,
#include "Instruction.def"
#undef DEFINST

		OpcodeCount
	};

	bool decoded = false;

	Opcode opcode = OpcodeInvalid;
  int size = 0;
  uint64_t eip = 0;
  uint8_t rex = 0;

public:

  /// Return the instruction opcode, or \c OpcodeInvalid if the
	/// sequence of bytes failed to decode. This function must be invoked
	/// after a previous call to Decode().
	Opcode getOpcode() const
	{
		assert(decoded);
		return opcode;
	}

  /// Return the number of bytes of this instruction, or 0 if the sequence
	/// of bytes failed to decode. This function must be invoked after a
	/// previous call to Decode().
	int getSize() const
	{
		assert(decoded);
		return size;
	}

  /// Read the bytes in the beginning of \a buffer and decode the x86
	/// instruction represented by them. The value given in \a address
	/// specifies the virtual address of the instruction, stored internally
	/// and used later for branch decoding purposes.
	void Decode(const char *buffer, uint64_t eip);
};

}  // namespace x86_64

#endif  // ARCH_X86_64_DISASSEMBLER_INSTRUCTION_H
