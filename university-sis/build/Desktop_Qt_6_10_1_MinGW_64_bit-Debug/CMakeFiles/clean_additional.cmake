# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\university-sis_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\university-sis_autogen.dir\\ParseCache.txt"
  "university-sis_autogen"
  )
endif()
