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
  int** next;  // яблоко
  int* tail_x;
  int* tail_y;
  int n_tail;
  int key;
  int x;
  int y;
  int prev_key;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  int debug;
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
    printw("\n");
  }
  mvprintw(0, 27, "Game status:");
  const char* game_status = nullptr;
  const char* game_status2 = nullptr;
  if (game_info_update.pause == 0) {
    game_status = "The game has started";
  } else if (game_info_update.pause == 1) {
    game_status = "Pause";
  } else {
    game_status = "Game over";
  }
  mvprintw(1, 27, "%s", game_status);

  if (game_info_update.debug == 0) {
    game_status2 = "Debug == 0";
  } else {
    game_status2 = "Debug != 0";
  }
  // mvprintw(20, 27, "Game status:");
  mvprintw(21, 27, "%s", game_status2);

  refresh();
}

// @brief Выделение памяти для поля (field)
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

void allocate_memory_for_next(GameInfo_t* gameInfo) {
  gameInfo->next = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->next[i] = new int[FIELD_WIDTH];
  }
  // Заполнение игрового поля
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gameInfo->next[i][j] = 1;  // Граница
      } else {
        gameInfo->next[i][j] = 0;  // Внутреннее поле
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
    delete[] gameInfo->field[i];
  }
  delete[] gameInfo->field;
}

void free_memory_next(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->next[i];
  }
  delete[] gameInfo->next;
}

void free_memory_snake(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->snake[i];
  }
  delete[] gameInfo->snake;
}

GameInfo_t updateCurrentState(GameInfo_t gameInfo) {
  GameInfo_t game_info_update;
  allocate_memory_for_field(&game_info_update);

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (gameInfo.field[i][j] > 0 || gameInfo.snake[i][j] > 0 ||
          gameInfo.next[i][j] > 0) {
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
  game_info_update.debug = gameInfo.debug;

  return game_info_update;
}

void snake_movement(GameInfo_t* gameInfo) {
  gameInfo->tail_x[0] = gameInfo->x;  // Координаты головы
  gameInfo->tail_y[0] = gameInfo->y;
  switch (gameInfo->prev_key) {
    case KEY_UP:
      gameInfo->y--;
      break;
    case KEY_DOWN:
      gameInfo->y++;
      break;
    case KEY_LEFT:
      gameInfo->x--;
      break;
    case KEY_RIGHT:
      gameInfo->x++;
      break;
    default:
      break;
  }
  // Новое координаты головы змейки
  gameInfo->snake[gameInfo->y][gameInfo->x] = 1;
}

void fix_buttons(GameInfo_t* gameInfo) {
  if ((gameInfo->key == KEY_UP || gameInfo->key == KEY_DOWN) &&
      (gameInfo->prev_key == KEY_UP || gameInfo->prev_key == KEY_DOWN)) {
    /* do nothing */
  }
  // условия когда повторяется кнопка или кнопка в обратную сторону
  else if ((gameInfo->key == KEY_LEFT || gameInfo->key == KEY_RIGHT) &&
           (gameInfo->prev_key == KEY_LEFT ||
            gameInfo->prev_key == KEY_RIGHT)) {
    /* do nothing */
  } else {
    gameInfo->prev_key = gameInfo->key;
    gameInfo->pause = 0;
  }
}

void controller(GameInfo_t* gameInfo) {
  gameInfo->key = getch();
  if (gameInfo->key != -1) {
    // Проверяем кнопки на повторение
    fix_buttons(gameInfo);
  }
}

int collision_check(GameInfo_t gameInfo) {
  int collision = 0;
  if (gameInfo.x <= 0 || gameInfo.x >= FIELD_WIDTH - 1 || gameInfo.y <= 0 ||
      gameInfo.y >= FIELD_HEIGHT - 1 ||
      gameInfo.snake[gameInfo.y][gameInfo.x] == 1) {
    collision = 1;  // Конец игры
  }
  for (int i = gameInfo.n_tail; i > 0; i--) {
    if (gameInfo.x == gameInfo.tail_x[i]) {
      collision = 1;  // Конец игры
    }
  }

  return collision;
}

void clear_apple(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      gameInfo->next[i][j] = 0;
    }
  }
}

void generate_apple_in_field(GameInfo_t* gameInfo, int* apple_height,
                             int* apple_width) {
  bool valid_position = false;
  static bool initialized = false;
  if (!initialized) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    initialized = true;
  }

  // Генерируем случайные координаты с учётом рамки
  while (!valid_position) {
    *apple_height = 1 + std::rand() % (FIELD_HEIGHT - 2);
    *apple_width = 1 + std::rand() % (FIELD_WIDTH - 2);

    // Проверяем, что на этой позиции нет змейки и нет другого яблока
    if (gameInfo->field[*apple_height][*apple_width] == 0 &&
        gameInfo->snake[*apple_height][*apple_width] == 0) {
      valid_position = true;
    }
  }
  gameInfo->next[*apple_height][*apple_width] = 1;
}

void game_logic(GameInfo_t* gameInfo);

int main() {
  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);  // Включаем поддержку стрелок
  nodelay(stdscr, TRUE);

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

  for (int i = 0; i < 4; i++) {
    gameInfo.tail_x[i] = 5;
    gameInfo.tail_y[i] = 13 + i + 1;
  }

  gameInfo.y = 13;
  gameInfo.x = 5;
  gameInfo.n_tail = 4;
  gameInfo.key = -1;

  int apple_height = 0;
  int apple_width = 0;

  // Выделение памяти для игрового поля и змейки
  allocate_memory_for_field(&gameInfo);
  allocate_memory_for_next(&gameInfo);
  allocate_memory_for_snake(&gameInfo);

  generate_apple_in_field(&gameInfo, &apple_height, &apple_width);

  while (waitHalfSecond()) {  // Добавили проверку границы
    filling_playing_field(&gameInfo);
    controller(&gameInfo);
    if (!collision_check(gameInfo)) {
      snake_movement(&gameInfo);
    } else {
      gameInfo.pause = 2;
    }
    if (gameInfo.y == apple_height && gameInfo.x == apple_width) {
      clear_apple(&gameInfo);
      gameInfo.n_tail++;
      generate_apple_in_field(&gameInfo, &apple_height, &apple_width);
    }

    // generate_apple_in_field(&gameInfo);

    for (int i = gameInfo.n_tail - 1; i > 0; i--) {
      gameInfo.tail_x[i] = gameInfo.tail_x[i - 1];
      gameInfo.tail_y[i] = gameInfo.tail_y[i - 1];
    }

    print_field_n(updateCurrentState(gameInfo));
  }

  getch();   // Ждем нажатия клавиши перед выходом
  endwin();  // Завершаем ncurses */

  // Освобождение памяти
  free_memory_field(&gameInfo);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;

  return 0;
}
