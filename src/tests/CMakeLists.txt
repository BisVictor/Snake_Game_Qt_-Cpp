cmake_minimum_required(VERSION 3.10)
project(SnakeGameTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(GTest REQUIRED)

# Указываем корневую директорию проекта
set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SNAKE_DIR "${PROJECT_ROOT_DIR}/brick_game/snake")
set(GUI_DIR "${PROJECT_ROOT_DIR}/gui/desktop")

# Основной код приложения (как библиотека)
add_library(SnakeGameLib STATIC 
    "${SNAKE_DIR}/mainwindow.cpp"
    "${SNAKE_DIR}/main.cpp"
    "${GUI_DIR}/frontend.cpp"
)

# Добавляем ВСЕ необходимые include-директории
target_include_directories(SnakeGameLib PUBLIC 
    "${PROJECT_ROOT_DIR}"        # Корень src/
    "${SNAKE_DIR}"               # brick_game/snake/
    "${GUI_DIR}"                 # gui/desktop/
    "${CMAKE_CURRENT_SOURCE_DIR}" # tests/
)

target_link_libraries(SnakeGameLib 
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
)

# Исполняемый файл для тестов
add_executable(SnakeGameTests
    "${CMAKE_CURRENT_SOURCE_DIR}/main_test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_mainwindow.cpp"
)

# Добавляем include-директории для тестов
target_include_directories(SnakeGameTests PUBLIC 
    "${PROJECT_ROOT_DIR}"
    "${SNAKE_DIR}"
    "${GUI_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(SnakeGameTests
    SnakeGameLib
    Qt5::Widgets`
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