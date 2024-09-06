# Specify the source files for the library
set(SOURCES
  Event.C
  Gen.C
  Particle.C
)

set(HDRFILES
  Event.h
  Gen.h
  Particle.h
)

# Create the shared library
add_library(DataFormats SHARED ${SOURCES})

# Specify output directory for the shared library
set_target_properties(DataFormats PROPERTIES
  PUBLIC_HEADER "${HDRFILES}"
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

# Link the library with ROOT
target_link_libraries(DataFormats PUBLIC ${ROOT_LIBRARIES})

install(TARGETS DataFormats
    EXPORT PDStudy-targets
    PUBLIC_HEADER DESTINATION include/DataFormats COMPONENT Development)
