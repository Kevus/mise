add_library(MISE
  Mutator.cpp
)

llvm_config(MISE "${USE_LLVM_SHARED}" support)
target_include_directories(MISE PRIVATE ${KLEE_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_compile_options(MISE PRIVATE ${KLEE_COMPONENT_CXX_FLAGS})
target_compile_definitions(MISE PRIVATE ${KLEE_COMPONENT_CXX_DEFINES})