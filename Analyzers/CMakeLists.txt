# Specify the source files for the library
set(SOURCES
  FLUKANtuple.C
  GEANT4Ntuple.C
  AnalyzerCore.C
  PionAnalyzer.C
)

set(HDRFILES
  FLUKANtuple.h
  GEANT4Ntuple.h
  AnalyzerCore.h
  PionAnalyzer.h
)

# Create the shared library
add_library(Analyzers SHARED ${SOURCES})

# Specify output directory for the shared library
set_target_properties(Analyzers PROPERTIES
  PUBLIC_HEADER "${HDRFILES}"
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

#include_directories(${CMAKE_SOURCE_DIR}/DataFormats)
#include_directories(${CMAKE_SOURCE_DIR}/AnalyzerTools)

# Link the library with dir1_lib and dir3_lib
#target_link_libraries(Analyzers PRIVATE AnalyzerTools DataFormats ${ROOT_LIBRARIES})
target_link_libraries(Analyzers
PUBLIC
  AnalyzerTools
  DataFormats
  ${ROOT_LIBRARIES}
)

install(TARGETS Analyzers
    EXPORT PDStudy-targets
    PUBLIC_HEADER DESTINATION include/Analyzers COMPONENT Development)
