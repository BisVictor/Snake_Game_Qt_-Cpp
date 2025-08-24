#!/bin/bash

# 1. Компиляция исходных файлов
g++ -c -pipe -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC \
    -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
    -I. -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ \
    -o main.o main.cpp

g++ -c -pipe -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC \
    -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
    -I. -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ \
    -o mainwindow.o mainwindow.cpp

# 2. Генерация moc-файлов
/usr/lib/qt5/bin/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
    --include ./moc_predefs.h \
    -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ \
    -I. \
    -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    mainwindow.h -o moc_mainwindow.cpp

g++ -c -pipe -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC \
    -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
    -I. -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ \
    -o moc_mainwindow.o moc_mainwindow.cpp

# 3. Линковка
g++ -Wl,-O1 -o SnakeGame2 \
    main.o mainwindow.o moc_mainwindow.o \
    /usr/lib/x86_64-linux-gnu/libQt5Widgets.so \
    /usr/lib/x86_64-linux-gnu/libQt5Gui.so \
    /usr/lib/x86_64-linux-gnu/libQt5Core.so \
    -lGL -lpthread

# 4. Очистка временных файлов (опционально)
rm -f *.o moc_*.cpp