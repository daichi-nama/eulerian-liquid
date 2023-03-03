set(sources src/stb.cpp)
add_library(stb ${sources})
target_include_directories(stb PUBLIC include)
