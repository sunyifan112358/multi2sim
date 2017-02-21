/*
 *  Multi2Sim
 *  Copyright (C) 2017 Yifan Sun (yifansun@coe.neu.edu)
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

#ifndef ARCH_X86_64_DISASSEMBLER_DISASSEMBLER_H
#define ARCH_X86_64_DISASSEMBLER_DISASSEMBLER_H

#include <arch/common/Disassembler.h>
#include <lib/cpp/Error.h>
#include <memory>

namespace x86_64 {

class Disassembler : public comm::Disassembler {

  // Disassemble a file
  static std::string path;

  // Private constructor
  Disassembler();

  // Unique instance of x86_64 disassembler
  static std::unique_ptr<Disassembler> instance;

public:

  /// Exception for the x86 disassembler
  class Error : public misc::Error {
  public:
    Error(const std::string &message) : misc::Error(message) {
      AppendPrefix("x86_64 disassembler");
    }
  };

  /// Destructor
	~Disassembler();

	/// Get instance of singleton
	static Disassembler *getInstance();

  /// Register options in the command line
  static void RegisterOptions();

  /// Process command-line options
  static void ProcessOptions();

  /// Disassemble the x86_64 ELF executable contained in file \a path, and
	/// dump its content into the output stream given in \c os (or the
	/// standard output if no output stream is specified.
	void DisassembleBinary(const std::string &path,
			std::ostream &os = std::cout) const;
};


}  // namespace x86_64

#endif // ARCH_X86_64_DISASSEMBLER_DISASSEMBLER_H
