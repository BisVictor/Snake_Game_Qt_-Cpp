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
  int prev_key;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 12

const int ESCAPE_KEY{27};
const int ENTER_KEY{10};
const int SPACE_KEY{32};

UserAction_t get_signal(int user_input) {
  UserAction_t key_code = Start;
  if (user_input == KEY_DOWN)
    key_code = Down;
  else if (user_input == KEY_UP)
    key_code = Up;
  else if (user_input == KEY_LEFT)
    key_code = Left;
  else if (user_input == KEY_RIGHT)
    key_code = Right;
  else if (user_input == ESCAPE_KEY)
    key_code = Terminate;
  else if (user_input == ENTER_KEY)
    key_code = Pause;
  else if (user_input == SPACE_KEY)
    key_code = Action;
  return key_code;
}

void print_field_n(GameInfo_t game_info_update) {
  setlocale(LC_ALL, "");  // Включение поддержки Unicode
  clear();

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (game_info_update.field[i][j] > 0) {
        printw("##");
      } else {
        printw("  ");
      }
    }
    // std::cout << "ss" << std::endl;
    printw("\n");
  }
  // const char* text = "Start Game";
  mvprintw(0, 27, "Game status:");
  const char* game_status = nullptr;
  if (game_info_update.pause == 0) {
    game_status = "The game has started";
  } else {
    game_status = "Pause";
  }

  mvprintw(1, 27, "%s", game_status);

  refresh();
}

void print_field(GameInfo_t game_info_update) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (game_info_update.field[i][j] > 0) {
        printf("##");
      } else {
        printf("  ");
      }
    }
    // printf("\n");
  }
  printf("\n");
}
// @brief Выделение памяти
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

void filling_playing_field(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      gameInfo->snake[i][j] = 0;  // Внутреннее поле
    }
  }
}

void allocate_memory_for_snake(GameInfo_t* gameInfo) {
  gameInfo->snake = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->snake[i] = new int[FIELD_WIDTH];
  }
  filling_playing_field(gameInfo);

  // Заполнение игрового поля
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

  for (int i = 0; i < 100; i++) {
    // std::cout << gameInfo.tail_x[i] << "and" << gameInfo.tail_y[i];
    if (gameInfo.tail_x[i] > 0 && gameInfo.tail_y[i] > 0) {
      game_info_update.field[gameInfo.tail_y[i]][gameInfo.tail_x[i]] = 1;
    }
  }
  game_info_update.pause = gameInfo.pause;
  game_info_update.high_score = gameInfo.high_score;
  game_info_update.level = gameInfo.level;
  game_info_update.score = gameInfo.score;

  return game_info_update;
}

/* GameInfo_t start_screen(GameInfo_t gameInfo) {
  mvprintw(5, 5, "Start the game");
} */

int main() {
  GameInfo_t gameInfo;

  // Инициализация полей структуры
  gameInfo.next = nullptr;
  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 1;
  gameInfo.pause = 1;

  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();

  // Выделение памяти для игрового поля
  allocate_memory_for_field(&gameInfo);
  allocate_memory_for_snake(&gameInfo);

  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);  // Включаем поддержку стрелок
  nodelay(stdscr, TRUE);

  int y = 18;
  int x = 5;
  int n_tail = 4;
  int user_input = Start;
  gameInfo.prev_key = -1;
  int key = 0;

  while (y > 0 && waitHalfSecond()) {  // Добавили проверку границы

    filling_playing_field(&gameInfo);
    // gameInfo.pause = 1;
    key = getch();
    if (key != -1) {  // Если клавиша была нажата
      gameInfo.prev_key = key;
      gameInfo.pause = 0;
    }

    for (int i = n_tail - 1; i > 0; i--) {
      gameInfo.tail_x[i] = gameInfo.tail_x[i - 1];
      gameInfo.tail_y[i] = gameInfo.tail_y[i - 1];
    }

    gameInfo.tail_x[0] = x;  // Координаты новой головы
    gameInfo.tail_y[0] = y;
    switch (gameInfo.prev_key) {
      case KEY_UP:
        y--;
        break;
      case KEY_DOWN:
        y++;
        break;
      case KEY_LEFT:
        x--;
        break;
      case KEY_RIGHT:
        x++;
        break;
      default:
        break;
    }

    gameInfo.snake[y][x] = 1;
    print_field_n(updateCurrentState(gameInfo));
  }

  getch();   // Ждем нажатия клавиши перед выходом
  endwin();  // Завершаем ncurses */

  // Освобождение памяти
  free_memory_field(&gameInfo);
  free_memory_snake(&gameInfo);
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;

  return 0;
}
