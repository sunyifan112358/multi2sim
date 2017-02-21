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
#include <lib/elf/File64.h>
#include <lib/elf/Section32.h>

#include "Disassembler.h"
#include "Instruction.h"

namespace x86_64 {

std::string Disassembler::path;
std::unique_ptr<Disassembler> Disassembler::instance;

Disassembler::Disassembler() : comm::Disassembler("x86_64") {}

Disassembler::~Disassembler() {}

Disassembler *Disassembler::getInstance() {
  if (instance.get()) return instance.get();

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
                               "Disassemble the x86_64 ELF file provided in "
                               "<arg>, using the internal x86_64 disassembler. "
                               "This option is incompatible with any other "
                               "option.");

  // Incompatible options
  command_line->setIncompatible("--x86_64-disasm");
}

void Disassembler::ProcessOptions() {
  // Run x86 disassembler
  if (!path.empty()) {
    Disassembler *disassembler = Disassembler::getInstance();
    disassembler->DisassembleBinary(path);
    exit(0);
  }
}

void Disassembler::DisassembleBinary(const std::string &path,
                                     std::ostream &os) const {
  elf::File64 file(path);

  for (int idx = 0; idx < file.getNumSections(); idx++) {
    elf::Section64 *section = file.getSection(idx);
    if ((section->getFlags() & SHF_EXECINSTR) == 0) {
      continue;
    }

    DisassembleSection(section, os);

  }
}

void Disassembler::DisassembleSection(elf::Section64 *section,
                                      std::ostream &os) const {
  os << "Disassembly of section " << section->getName() << ":\n";

  Instruction inst;
  uint64_t offset = 0;
  while(offset < section->getSize()) {
    uint64_t eip = section->getAddr() + offset;
    const char *buffer = section->getBuffer() + offset;

    inst.Decode(buffer, eip);
    if (inst.getOpcode()) {
      assert(inst.getSize());
      offset += inst.getSize();
    } else {
      offset++;
    }
  }
}
}
