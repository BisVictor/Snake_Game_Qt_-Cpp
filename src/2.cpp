// #include <conio.h>

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

enum class GameState { MENU, PLAY, PAUSE, GAME_OVER };

class SnakeGame {
 private:
  GameInfo_t gameInfo;
  int apple_height = 0;
  int apple_width = 0;
  GameState state;
  void allocate_memory_for_field(GameInfo_t* gameInfo);
  void filling_playing_field(GameInfo_t* gameInfo);
  void allocate_memory_for_next(GameInfo_t* gameInfo);
  void allocate_memory_for_snake(GameInfo_t* gameInfo);
  void free_memory_field(GameInfo_t* gameInfo);
  void free_memory_next(GameInfo_t* gameInfo);
  void free_memory_snake(GameInfo_t* gameInfo);

 public:
  SnakeGame();
  ~SnakeGame();
  void run();
};

// @brief Выделение памяти для поля (field)
void SnakeGame::allocate_memory_for_field(GameInfo_t* gameInfo) {
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

void SnakeGame::allocate_memory_for_next(GameInfo_t* gameInfo) {
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

void SnakeGame::filling_playing_field(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      gameInfo->snake[i][j] = 0;  // Внутреннее поле
    }
  }
}

void SnakeGame::allocate_memory_for_snake(GameInfo_t* gameInfo) {
  gameInfo->snake = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->snake[i] = new int[FIELD_WIDTH];
  }
  filling_playing_field(gameInfo);
}

void SnakeGame::free_memory_field(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->field[i];
  }
  delete[] gameInfo->field;
}

void SnakeGame::free_memory_next(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->next[i];
  }
  delete[] gameInfo->next;
}

void SnakeGame::free_memory_snake(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->snake[i];
  }
  delete[] gameInfo->snake;
}

// Конструктор SnakeGame
SnakeGame::SnakeGame(/* args */) {
  // инициализируем начальное состояние игры
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

  gameInfo.x = 5;       // Голова змейки
  gameInfo.y = 13;      // Голова змейки
  gameInfo.n_tail = 4;  // Длина хвоста
  gameInfo.key = -1;
}

SnakeGame::~SnakeGame() {
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;
  free_memory_field(&gameInfo);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);
}
