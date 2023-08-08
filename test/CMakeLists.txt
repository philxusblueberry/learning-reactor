file(GLOB_RECURSE reactor_test_src "${PROJECT_SOURCE_DIR}/test/*.cpp")

foreach(src ${reactor_test_src})
    get_filename_component(test_file ${src} NAME)
    string(REPLACE ".cpp" "" test_file ${test_file})

    add_executable(${test_file} EXCLUDE_FROM_ALL ${src})
    target_link_libraries(${test_file} reactor_share)
    set_target_properties(${test_file}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        COMMAND ${test_file})
endforeach(src ${reactor_test_src})
