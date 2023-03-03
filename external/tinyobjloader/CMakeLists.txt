set(sources src/tiny_obj_loader.cc)
add_library(tinyobjloader ${sources})
target_include_directories(tinyobjloader PUBLIC include)
