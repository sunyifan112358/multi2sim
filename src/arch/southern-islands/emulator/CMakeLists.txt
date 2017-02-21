cmake_minimum_required(VERSION 3.2)

add_library(siemu
  Emulator.cc
  NDRange.cc
  Wavefront.cc
  WorkGroup.cc
  WorkItem.cc
  WorkItemIsa.cc
)
