install(PROGRAMS mise-dcleaner DESTINATION bin)

configure_file(mise-dcleaner "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mise-dcleaner" COPYONLY)
