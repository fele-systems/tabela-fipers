cmake_minimum_required( VERSION 3.6 )
project( tabela-fipers )

set(CMAKE_CXX_STANDARD 17)

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)

######################################################
### Libcurlpp

add_library( libcurlpp
    src/curlpp.cpp
)

target_link_libraries( libcurlpp
    fmt::fmt
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

target_include_directories(libcurlpp PRIVATE
    include
)

######################################################
### Libfipe

add_library( libfipe
    src/fipe.cpp
)

target_link_libraries( libfipe
    fmt::fmt
    libcurlpp)

target_compile_options(libfipe PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

######################################################
### TabelaFipes

add_executable( tabela-fiper
    src/main.cpp
)

target_include_directories(tabela-fiper PRIVATE
    include
)

target_link_libraries( tabela-fiper PRIVATE
    libfipe
)