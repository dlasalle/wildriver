include_directories(.)
file(GLOB sources *.cpp)

if (_WIN32)
  # export dll
  message("Exporting all symbols in DLL.")
  cmake_minimum_required(VERSION 3.4)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# libraries
add_library(wildriver ${WILDRIVER_LIBRARY_TYPE} 
  ${sources}
) 

install(TARGETS wildriver
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (DEFINED TESTS AND NOT TESTS EQUAL 0)
  add_subdirectory("test")
endif()
