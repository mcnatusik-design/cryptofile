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
install(FILES packaging/cryptofile.conf DESTINATION /etc/cryptofile)

# --- Настройка тестов (ПР2) ---
include(CTest)
if(BUILD_TESTING)
    add_executable(test_core tests/test_core.c)
    target_link_libraries(test_core PRIVATE core)
    
    add_test(NAME core COMMAND test_core)
    
    # Тест 1: Проверка вывода версии
    add_test(NAME cli_version COMMAND cryptofile --version)
    set_tests_properties(cli_version PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")
    
    # Тест 2: Проверка на некорректный аргумент (должен вернуть ошибку)
    add_test(NAME cli_bad_args COMMAND cryptofile --no-such-option)
    set_tests_properties(cli_bad_args PROPERTIES WILL_FAIL TRUE)
endif()
