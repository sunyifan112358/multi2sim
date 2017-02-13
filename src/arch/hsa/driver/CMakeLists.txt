cmake_minimum_required(VERSION 3.2)

add_library(hsadrive
  DriverCall.cc
  Driver.cc
  HsaExecutable.cc
  HsaExecutableSymbol.cc
  HsaProgram.cc
  Signal.cc
  SignalDestroyHandler.cc
  SignalManager.cc
)  

