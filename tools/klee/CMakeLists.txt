#===------------------------------------------------------------------------===#
#
#                     The KLEE Symbolic Virtual Machine
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#
add_executable(mise
  main.cpp
)

set(KLEE_LIBS
  kleeCore
)

target_link_libraries(mise ${KLEE_LIBS})
target_include_directories(mise PRIVATE ${KLEE_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_compile_options(mise PRIVATE ${KLEE_COMPONENT_CXX_FLAGS})
target_compile_definitions(mise PRIVATE ${KLEE_COMPONENT_CXX_DEFINES})


install(TARGETS mise RUNTIME DESTINATION bin)

# The KLEE binary depends on the runtimes
add_dependencies(mise BuildKLEERuntimes)
