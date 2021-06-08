cmake_minimum_required(VERSION 3.13)

target_sources(tcpcli_server PRIVATE
  CliPacket
  CliServer                             
  tcpcli_server)
