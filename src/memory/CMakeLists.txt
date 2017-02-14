cmake_minimum_required(VERSION 3.2)

add_library(memory
  Cache.cc
  Directory.cc
  Frame.cc
  Manager.cc
  Memory.cc
  Mmu.cc
  Module.cc
  SpecMem.cc
  System.cc
  SystemConfig.cc
  SystemEvents.cc
)
target_link_libraries(memory cpp esim util)
