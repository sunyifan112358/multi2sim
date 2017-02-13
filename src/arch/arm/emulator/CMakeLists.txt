cmake_minimum_required(VERSION 3.2)

add_library(armemu
  Context.cc
  ContextIsaArm32.cc
  ContextIsa.cc
  ContextIsaThumb16.cc
  ContextIsaThumb32.cc
  ContextLoader.cc
  ContextSignal.cc
  ContextSyscall.cc
  Emulator.cc
  Regs.cc
)
