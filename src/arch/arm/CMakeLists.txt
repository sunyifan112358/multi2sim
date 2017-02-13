cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES
  disassembler
  emulator
)

foreach(subdir ${SUBDIRECTORIES})
  add_subdirectory(${subdir})
endforeach(subdir)
