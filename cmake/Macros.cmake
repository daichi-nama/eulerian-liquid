function(create_header project_name)
    set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")
    set(BINARY_DIR "${CMAKE_BINARY_DIR}")

    configure_file(
        "${CMAKE_SOURCE_DIR}/include/${project_name}/path.h.in" "${CMAKE_SOURCE_DIR}/include/${project_name}/path.h"
        @ONLY
    )
    configure_file("${CMAKE_SOURCE_DIR}/script/paths.py.in" "${CMAKE_SOURCE_DIR}/script/paths.py" @ONLY)
endfunction(create_header)
