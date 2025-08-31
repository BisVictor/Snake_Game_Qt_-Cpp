# Makefile для SnakeGame (Qt проект)

# Переменные
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=c11 
CXX = g++
MOC = /usr/lib/qt5/bin/moc
TARGET = SnakeGame
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DOCDIR = $(PREFIX)/share/doc/$(TARGET)

# Флаги компиляции
CXXFLAGS = -pipe -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC \
           -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH = -I. \
          -I/usr/include/x86_64-linux-gnu/qt5 \
          -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
          -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
          -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
          -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++

# Библиотеки
LIBS = -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread

# Исходные файлы
SOURCES = brick_game/snake/main.cpp brick_game/snake/mainwindow.cpp gui/desktop/frontend.cpp
HEADERS = brick_game/snake/mainwindow.h gui/desktop/frontend.h
MOC_SRC = brick_game/snake/moc_mainwindow.cpp
OBJECTS = $(SOURCES:.cpp=.o) $(MOC_SRC:.cpp=.o)

# Правила по умолчанию
.PHONY: all install uninstall clean dvi dist test

all: $(TARGET)

# Сборка основной программы
$(TARGET): $(OBJECTS)
	$(CXX) -Wl,-O1 -o $@ $^ $(LIBS)

# Генерация moc-файлов
moc_%.cpp: %.h
	$(MOC) $(INCPATH) $< -o $@

# Компиляция .cpp файлов
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

snake_cli:
	g++ snake_cli.cpp -lncurses -o SnakeGame_cli


tetris:
	$(CC) $(CFLAGS) -Ibrick_game/tetris/ -o tetris_game brick_game/tetris/s21_tetris_back.c gui/cli/s21_tetris_front.c brick_game/tetris/main.c -lncurses
	
# Установка
install: all
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	install -d $(DOCDIR)
	install -m 644 README.md $(DOCDIR)

# Удаление
uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(DOCDIR)

# Очистка
clean:
	rm -f $(TARGET) *.o moc_*.cpp SnakeGame_cli tetris_game
	rm -f gui/desktop/frontend.o
	rm -f brick_game/snake/moc_mainwindow.o
	rm -f brick_game/snake/mainwindow.o
	rm -f brick_game/snake/main.o
	rm -rf tests/build
	rm -f SnakeGame-1.0.tar.gz
	rm -f *.aux *.dvi *.log *.out *.toc
	


# Документация (dvi)
dvi:
	@echo "Генерация документации"
	latex documentation.tex
	latex documentation.tex
	xdvi documentation.dvi

# Дистрибутив
dist: clean
	mkdir -p $(TARGET)-1.0
	cp $(SOURCES) $(HEADERS) Makefile $(TARGET)-1.0/
	tar -czf $(TARGET)-1.0.tar.gz $(TARGET)-1.0
	rm -rf $(TARGET)-1.0

# Тестирование
test: all
	@echo "Запуск тестов..."
	./tests/run_tests.sh
	@echo "Тестирование завершено"