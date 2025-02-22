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
  int** snake;
  int** next;
  int* tail_x;
  int* tail_y;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void print_field(GameInfo_t gameInfo) {
  setlocale(LC_ALL, "");  // Включение поддержки Unicode
  clear();

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (gameInfo.field[i][j] > 0) {
        printw("##");
      } else {
        printw("  ");
      }
    }
    printw("\n");
  }

  refresh();
}
// @brief выделение памяти
void allocate_memory_for_field(GameInfo_t* gameInfo) {
  gameInfo->field = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->field[i] = new int[FIELD_WIDTH];
  }

  // Заполнение игрового поля
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gameInfo->field[i][j] = 1;  // Граница
      } else {
        gameInfo->field[i][j] = 0;  // Внутреннее поле
      }
    }
  }
}

void allocate_memory_for_snake(GameInfo_t* gameInfo) {
  gameInfo->snake = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->snake[i] = new int[FIELD_WIDTH];
  }

  // Заполнение игрового поля
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      gameInfo->snake[i][j] = 0;  // Внутреннее поле
    }
  }
}

void free_memory_field(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->field[i];  // Исправлено: правильное количество строк
  }
  delete[] gameInfo->field;
}

void free_memory_snake(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->snake[i];  // Исправлено: правильное количество строк
  }
  delete[] gameInfo->snake;
}

GameInfo_t updateCurrentState(GameInfo_t gameInfo) {
  GameInfo_t game_info_update;
  allocate_memory_for_field(&game_info_update);
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (gameInfo.field[i][j] > 0 || gameInfo.snake[i][j] > 0) {
        game_info_update.field[i][j] = 1;
      } else {
        game_info_update.field[i][j] = 0;
      }
    }
  }
  return game_info_update;
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

  gameInfo.tail_x = new int[FIELD_WIDTH];
  gameInfo.tail_y = new int[FIELD_HEIGHT];
  int n_tail = 0;

  // Выделение памяти для игрового поля
  allocate_memory_for_field(&gameInfo);

  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);

  int i = 13;
  int j = 5;

  while (i > 0 && waitHalfSecond()) {  // Добавили проверку границы
    allocate_memory_for_snake(&gameInfo);
    gameInfo.snake[i][j] = 1;
    print_field(updateCurrentState(gameInfo));

    i--;
    free_memory_snake(&gameInfo);
  }

  getch();  // Ждем нажатия клавиши перед выходом

  endwin();  // Завершаем ncurses

  // Освобождение памяти
  free_memory_field(&gameInfo);
  free_memory_snake(&gameInfo);
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;

  return 0;
}
