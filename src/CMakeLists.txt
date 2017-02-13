cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES 
  arch
  lib
)

foreach(subdir ${SUBDIRECTORIES} )
    add_subdirectory(${subdir})
endforeach(subdir)

add_executable(m2s m2s.cc)
target_link_libraries(m2s 
  cpp

  armdisasm
  armemu

  hsadisasm
  hsadriver
  hsaemu

  keplerdisasm
  keplerdriver
  kepleremu

  mipsdisasm
  mipsemu

  sidisasm
  sidriver
  siemu
  sitiming

  vidiasm

  x86disasm
  x86emu
  x86timing
)
