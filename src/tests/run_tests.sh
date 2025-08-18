#!/bin/bash

# Переходим в директорию со скриптом
cd "$(dirname "$0")"

# Проверяем необходимые инструменты
command -v gcov >/dev/null 2>&1 || { echo >&2 "Error: gcov not found. Install gcc first."; exit 1; }
command -v lcov >/dev/null 2>&1 || { echo >&2 "Error: lcov not found. Install lcov first."; exit 1; }
command -v genhtml >/dev/null 2>&1 || { echo >&2 "Error: genhtml not found. Install lcov first."; exit 1; }

# Очищаем предыдущую сборку
echo -e "\033[34mCleaning previous build...\033[0m"
rm -rf build
mkdir -p build
cd build

# Генерируем Makefile с поддержкой coverage
echo -e "\033[34mConfiguring CMake with coverage...\033[0m"
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fprofile-arcs -ftest-coverage" -DCMAKE_EXE_LINKER_FLAGS="-lgcov --coverage" .. || { 
    echo -e "\033[31mCMake generation failed\033[0m"; exit 1; 
}

# Собираем проект
echo -e "\033[34mBuilding tests with coverage...\033[0m"
make || { echo -e "\033[31mBuild failed\033[0m"; exit 1; }

# Запускаем тесты
echo -e "\033[34mRunning tests...\033[0m"
./SnakeGameTests || { echo -e "\033[31mTests failed\033[0m"; exit 1; }

# Собираем информацию о покрытии
echo -e "\033[34mGenerating coverage report...\033[0m"

# 1. Собираем сырые данные
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1 || {
    echo -e "\033[31mlcov failed to capture coverage data\033[0m"; exit 1
}

# 2. Фильтруем системные файлы и тестовые файлы
lcov --remove coverage.info \
    '/usr/*' \
    '*/tests/*' \
    '*/googletest/*' \
    '*/SnakeGameLib_autogen/*' \
    '*/moc_*.cpp' \
    '*/qrc_*.cpp' \
    --output-file coverage_filtered.info --rc lcov_branch_coverage=1 || {
    echo -e "\033[31mlcov failed to filter coverage data\033[0m"; exit 1
}

# 3. Генерируем HTML-отчет
genhtml coverage_filtered.info --output-directory coverage_report --branch-coverage || {
    echo -e "\033[31mgenhtml failed to generate report\033[0m"; exit 1
}

# 4. Показываем краткую статистику
echo -e "\n\033[34mCoverage summary:\033[0m"
lcov --summary coverage_filtered.info --rc lcov_branch_coverage=1

# 5. Открываем отчет в браузере (опционально)
if command -v xdg-open >/dev/null 2>&1; then
    xdg-open coverage_report/index.html
elif command -v open >/dev/null 2>&1; then
    open coverage_report/index.html
fi

exit 0