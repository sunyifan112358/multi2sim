cmake_minimum_required(VERSION 3.2)

add_library(x86emu
  Context.cc
  ContextIsa.cc
  ContextIsaCtrl.cc
  ContextIsaFp.cc
  ContextIsaMisc.cc
  ContextIsaRot.cc
  ContextIsaSse2.cc
  ContextIsaSse3.cc
  ContextIsaSse4.cc
  ContextIsaSse.cc
  ContextIsaStd.cc
  ContextIsaStr.cc
  ContextIsaXmm.cc
  ContextLoader.cc
  ContextSignal.cc
  ContextSyscall.cc
  ContextUinst.cc
  Emulator.cc
  Extended.cc
  Regs.cc
  Signal.cc
  Uinst.cc
  XmmValue.cc
)
