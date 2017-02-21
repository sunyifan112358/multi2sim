cmake_minimum_required(VERSION 3.2)

add_library(elf
  File32.cc
  File64.cc
  File.cc
  ProgramHeader32.cc
  ProgramHeader64.cc
  ProgramHeader.cc
  Section32.cc
  Section64.cc
  Section.cc
  Symbol32.cc
  Symbol64.cc
  Symbol.cc
)
