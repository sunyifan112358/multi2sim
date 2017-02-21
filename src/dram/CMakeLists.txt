cmake_minimum_required(VERSION 3.2)

add_library(dram
  Action.cc
  Address.cc
  Bank.cc
  Channel.cc
  Command.cc
  Controller.cc
  Rank.cc
  Request.cc
  Scheduler.cc
  System.cc
  TimingParameters.cc
)
