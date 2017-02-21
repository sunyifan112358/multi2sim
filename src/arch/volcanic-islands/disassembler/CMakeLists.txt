cmake_minimum_required(VERSION 3.2)

add_library(vidisasm
  Disassembler.cc
  Instruction.cc
  Binary.cc
)  
target_link_libraries(vidisasm elf)

