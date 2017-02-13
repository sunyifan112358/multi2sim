cmake_minimum_required(VERSION 3.2)

add_library(siemu
  BranchUnit.cc
  ComputeUnit.cc
  ExecutionUnit.cc
  FetchBuffer.cc
  Gpu.cc
  LdsUnit.cc
  ScalarUnit.cc
  SimdUnit.cc
  Timing.cc
  Uop.cc
  VectorMemoryUnit.cc
  WavefrontPool.cc
)
