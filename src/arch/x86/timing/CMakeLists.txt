cmake_minimum_required(VERSION 3.2)

add_library(x86emu
  Alu.cc
  BranchPredictor.cc
  Core.cc
  Cpu.cc
  CpuScheduler.cc
  FunctionalUnit.cc
  RegisterFile.cc
  Thread.cc
  ThreadCommit.cc
  ThreadDecode.cc
  ThreadDispatch.cc
  ThreadFetch.cc
  ThreadIssue.cc
  ThreadRecover.cc
  ThreadScheduler.cc
  Timing.cc
  TraceCache.cc
  Uop.cc
)
