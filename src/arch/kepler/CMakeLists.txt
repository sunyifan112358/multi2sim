cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES
  disassembler
  driver
  emulator
)

foreach(subdir ${SUBDIRECTORIES})
  add_subdirectory(${subdir})
endforeach(subdir)
