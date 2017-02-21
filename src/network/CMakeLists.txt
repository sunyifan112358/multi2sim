cmake_minimum_required(VERSION 3.2)

add_library(network
  Buffer.cc
  Bus.cc
  Connection.cc
  EndNode.cc
  Graph.cc
  Link.cc
  Message.cc
  Network.cc
  Node.cc
  Packet.cc
  RoutingTable.cc
  Switch.cc
  System.cc
  SystemConfig.cc
  SystemEvents.cc
)
target_link_libraries(network esim util)
