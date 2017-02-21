/*
 *  Multi2Sim
 *  Copyright (C) 2017  Yifan Sun (yifansun@coe.neu.edu)
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

#include <lib/cpp/CommandLine.h>

#include "Disassembler.h"

namespace x86_64 {

std::string Disassembler::path;
std::unique_ptr<Disassembler> Disassembler::instance;

Disassembler::Disassembler() : comm::Disassembler("x86_64") {

}

Disassembler::~Disassembler() {

}

Disassembler *Disassembler::getInstance() {
  // Instance already exists
  if (instance.get())
    return instance.get();

  // Create instance
  instance.reset(new Disassembler());
  return instance.get();
}

void Disassembler::RegisterOptions() {
  // Get command line object
  misc::CommandLine *command_line = misc::CommandLine::getInstance();

  // Category
  command_line->setCategory("x86_64");

  // Option --x86_64-disasm <file>
  command_line->RegisterString("--x86_64-disasm <file>", path,
                               "Disassemble the x86_64 ELF file provided in <arg>, "
                                   "using the internal x86_64 disassembler. This option is "
                                   "incompatible with any other option.");

  // Incompatible options
  command_line->setIncompatible("--x86_64-disasm");
}

void Disassembler::ProcessOptions() {
  // Run x86 disassembler
  if (!path.empty())
  {
    Disassembler *disassembler = Disassembler::getInstance();
    disassembler->DisassembleBinary(path);
    exit(0);
  }
}

void Disassembler::DisassembleBinary(const std::string &path, std::ostream &os) const {
  os << "Disassembling\n";
}
}
