cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES 
  arch
  lib
  dram
  memory
  network
)

foreach(subdir ${SUBDIRECTORIES} )
    add_subdirectory(${subdir})
endforeach(subdir)

add_executable(m2s m2s.cc)
target_link_libraries(m2s 
  cpp
  elf
  esim
  mhandle
  util
  
  dram
  memory
  network

  archcommon

  armdisasm
  armemu

  hsadisasm
  hsadriver
  hsaemu

  keplerdisasm
  keplerdriver
  kepleremu

  sidisasm
  sidriver
  siemu
  sitiming

  vidisasm

  x86disasm
  x86emu
  x86timing

  x86_64disasm

  pthread
  z
  m
)
