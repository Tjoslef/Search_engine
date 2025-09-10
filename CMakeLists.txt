cmake_minimum_required(VERSION 3.10)
project(Searcher)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(linenoise-ng)
find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)
# Define your main program target
add_executable(Searcher
    src/main.cpp
    src/search.cpp
    src/tokenizer.cpp
    src/input_processing.cpp
)
target_link_libraries(Searcher PRIVATE ${POPPLER_LIBRARIES})
target_include_directories(Searcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise-ng/include
    ${POPPLER_INCLUDE_DIRS}
)


target_link_libraries(Searcher PRIVATE linenoise ${POPPLER_CFLAGS_OTHER})
