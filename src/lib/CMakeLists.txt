cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES 
  cpp
  elf
  esim
  mhandle
  util
)

foreach(subdir ${SUBDIRECTORIES})
    add_subdirectory(${subdir})
endforeach(subdir) 
