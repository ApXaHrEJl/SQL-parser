set(target_name rdb_test)

add_executable(
  ${target_name}
  librdb/sql/LexerTest.cpp
  librdb/sql/ParserTest.cpp
)

include(CompileOptions)
set_compile_options(${target_name})

target_link_libraries(
  ${target_name}
  PRIVATE
    rdb
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${target_name})
