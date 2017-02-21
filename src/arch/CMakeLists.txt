cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES
  common
  arm
  mips
  hsa
  x86
  southern-islands
  volcanic-islands
  kepler
)

foreach(subdir ${SUBDIRECTORIES})
  add_subdirectory(${subdir})
endforeach(subdir)
