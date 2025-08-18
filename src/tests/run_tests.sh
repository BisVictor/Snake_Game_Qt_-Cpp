#!/bin/bash

# Переходим в директорию со скриптом
cd "$(dirname "$0")"

# Очищаем предыдущую сборку (если нужно)
rm -rf build
mkdir -p build
cd build

# Генерируем Makefile с помощью CMake
echo "Configuring CMake..."
cmake .. || { echo -e "\033[31mCMake generation failed\033[0m"; exit 1; }

# Собираем тесты
echo "Building tests..."
make || { echo -e "\033[31mBuild failed\033[0m"; exit 1; }

# Запускаем тесты
echo "Running tests..."
if ctest --output-on-failure; then
    echo -e "\n\033[32mAll tests passed successfully!\033[0m"
    exit 0
else
    echo -e "\n\033[31mSome tests failed\033[0m"
    exit 1
fi