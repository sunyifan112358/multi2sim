cmake_minimum_required(VERSION 3.2)

add_library(hsadisasm
  AsmService.cc
  BrigCodeEntryDump.cc
  BrigCodeEntryGetter.cc
  BrigDataEntry.cc
  BrigEntry.cc
  BrigFile.cc
  BrigImmed.cc
  BrigOperandEntry.cc
  BrigSection.cc
  Disassembler.cc
)  
