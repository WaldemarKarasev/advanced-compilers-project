set(C_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
)

set(CXX_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
)

foreach(option IN LISTS C_COMPILE_OPTIONS)
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${option}>")
endforeach()

foreach(option IN LISTS CXX_COMPILE_OPTIONS)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${option}>")
endforeach()