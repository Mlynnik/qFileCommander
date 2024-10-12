# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\qFileCommander_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\qFileCommander_autogen.dir\\ParseCache.txt"
  "qFileCommander_autogen"
  )
endif()
