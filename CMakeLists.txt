cmake_minimum_required(VERSION 3.20)
file(STRINGS VERSION APP_VERSION LIMIT_COUNT 1)
project(cryptofile VERSION ${APP_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

add_library(core STATIC src/core.c)
target_include_directories(core PUBLIC src)
target_link_libraries(core PUBLIC PkgConfig::SODIUM)

add_executable(cryptofile src/main.c)
target_compile_definitions(cryptofile PRIVATE APP_VERSION="${PROJECT_VERSION}")
target_link_libraries(cryptofile PRIVATE core)

install(TARGETS cryptofile RUNTIME DESTINATION bin)
