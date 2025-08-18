cmake_minimum_required(VERSION 3.10)
project(SnakeGameTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(GTest REQUIRED)

# Указываем правильные пути относительно расположения CMakeLists.txt
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")  # Переходим на уровень выше (в src)
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Основной код приложения (как библиотека)
add_library(SnakeGameLib STATIC 
    "${SOURCE_DIR}/mainwindow.cpp"
    "${SOURCE_DIR}/main.cpp"
)

target_include_directories(SnakeGameLib PUBLIC 
    "${SOURCE_DIR}"
)

target_link_libraries(SnakeGameLib 
    Qt5::Widgets
)

# Исполняемый файл для тестов
add_executable(SnakeGameTests
    "${TESTS_DIR}/main_test.cpp"
    "${TESTS_DIR}/test_mainwindow.cpp"
    "${TESTS_DIR}/test_mainwindow.h"
)

target_include_directories(SnakeGameTests PUBLIC 
    "${SOURCE_DIR}"
    "${TESTS_DIR}"
)

target_link_libraries(SnakeGameTests
    SnakeGameLib
    Qt5::Widgets
    Qt5::Test
    GTest::GTest
    GTest::Main
)

enable_testing()
add_test(NAME SnakeGameTests COMMAND SnakeGameTests)

# Включение coverage флагов для всех целей
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
endif()