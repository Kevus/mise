add_library(kleeMT
  Mutator.cpp
)

llvm_config(kleeMT "${USE_LLVM_SHARED}" support)
target_include_directories(kleeMT PRIVATE ${KLEE_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_compile_options(kleeMT PRIVATE ${KLEE_COMPONENT_CXX_FLAGS})
target_compile_definitions(kleeMT PRIVATE ${KLEE_COMPONENT_CXX_DEFINES})