cmake_minimum_required(VERSION 3.2)

add_library(armdisasm
  Disassembler.cc
  Instruction.cc
)  
target_link_libraries(armdisasm archcommon)

