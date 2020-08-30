# (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(killer KillerBlock.cpp KillerBlock.h KillerSums.h)