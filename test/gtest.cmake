set( target gtest )
set( target_main gtest_main )

set( gtest_dir /usr/src/gtest )

file( GLOB_RECURSE headers gtest_include_dirs )

set( source ${gtest_dir}/src/gtest-all.cc )
set( source_main ${gtest_dir}/src/gtest_main.cc )

include_directories(
    ${gtest_include_dirs}
    ${gtest_dir}
)

add_library(
    ${target}
    STATIC
    ${source}
    ${headers}
)

add_library(
    ${target_main}
    STATIC
    ${source_main}
    ${headers}
)
