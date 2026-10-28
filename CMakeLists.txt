cmake_minimum_required(VERSION 3.20)
file(STRINGS VERSION APP_VERSION LIMIT_COUNT 1)
project(cryptofile VERSION ${APP_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(cryptofile src/main.c)
target_compile_definitions(cryptofile PRIVATE APP_VERSION="${PROJECT_VERSION}")

install(TARGETS cryptofile RUNTIME DESTINATION bin)
