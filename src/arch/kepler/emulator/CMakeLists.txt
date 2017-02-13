cmake_minimum_required(VERSION 3.2)

add_library(kepleremu
  Emulator.cc
  Grid.cc
  ReturnAddressStack.cc
  SyncStack.cc
  ThreadBlock.cc
  Thread.cc
  ThreadIsa.cc
  Warp.cc
)
