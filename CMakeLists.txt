cmake_minimum_required(VERSION 3.0.0)
project(TestPoco VERSION 0.1.0)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

message("Generator is set to ${CMAKE_GENERATOR}")

add_subdirectory (contrib EXCLUDE_FROM_ALL)

add_subdirectory(tls_attach)
