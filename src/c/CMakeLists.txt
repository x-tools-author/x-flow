cmake_minimum_required(VERSION 3.0)
project(
  c_plugin
  VERSION 1.0
  LANGUAGES C)

add_library(c_plugin SHARED c_plugin-crc-calculator.c)
