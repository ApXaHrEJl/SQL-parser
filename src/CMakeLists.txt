set(target_name rdb)

add_library(
  ${target_name} STATIC
  librdb/sql/Lexer.cpp
  librdb/sql/Parser.cpp
  librdb/sql/Statements.cpp
  librdb/sql/Token.cpp
)

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/include/    
    
)
