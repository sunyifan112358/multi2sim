cmake_minimum_required(VERSION 3.2)

add_library(mipsemu
  Context.cc
  ContextIsa.cc
  ContextLoader.cc
  ContextSignal.cc
  ContextSyscall.cc
  Emulator.cc
  File.cc
  Mem.cc
  Regs.cc
  Signal.cc
)
