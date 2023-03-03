cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(${CMAKE_PROJECT_NAME}Apps LANGUAGES CXX)

foreach(file ${app_sources})
    string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" app_name ${file})
    add_executable(${app_name} ${file})
    target_compile_features(${app_name} PUBLIC cxx_std_17)
    set(${CMAKE_PROJECT_NAME}_APP_LIB ${CMAKE_PROJECT_NAME})

    target_link_libraries(${app_name} PUBLIC ${${CMAKE_PROJECT_NAME}_APP_LIB})
endforeach()
