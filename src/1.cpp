#include <ncurses.h>

#include <chrono>
#include <iostream>
#include <thread>

const int FIELD_WIDTH = 12;
const int FIELD_HEIGHT = 22;

bool waitHalfSecond() {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  return true;
}

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void screen(GameInfo_t gameInfo) {
  clear();

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (gameInfo.field[i][j] > 0) {
        printw("▓▓");
      } else {
        printw("░░");
      }
    }
    printw("\n");
  }

  refresh();
}

int main() {
  GameInfo_t gameInfo;

  // Инициализация полей структуры
  gameInfo.next = nullptr;
  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 1;
  gameInfo.pause = 0;

  // Выделение памяти для игрового поля
  gameInfo.field = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo.field[i] = new int[FIELD_WIDTH];
  }

  // Заполнение игрового поля
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gameInfo.field[i][j] = 1;  // Граница
      } else {
        gameInfo.field[i][j] = 0;  // Внутреннее поле
      }
    }
  }

  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);

  int i = 10;
  int j = 5;

  while (i >= 0 && waitHalfSecond()) {  // Добавили проверку границы
    screen(gameInfo);
    gameInfo.field[i][j] = 1;
    i--;
  }

  getch();  // Ждем нажатия клавиши перед выходом

  endwin();  // Завершаем ncurses

  // Освобождение памяти
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo.field[i];  // Исправлено: правильное количество строк
  }
  delete[] gameInfo.field;

  return 0;
}
