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
  Action,
  None  // Ничего не нажато
} UserAction_t;

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 12

const int ESCAPE_KEY{27};
const int ENTER_KEY{10};
const int SPACE_KEY{32};

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
  GameState state;
  int apple_height = 0;
  int apple_width = 0;

  void allocate_memory_for_field(GameInfo_t* gameInfo);
  void filling_playing_field(GameInfo_t* gameInfo);
  void allocate_memory_for_next(GameInfo_t* gameInfo);
  void allocate_memory_for_snake(GameInfo_t* gameInfo);
  void free_memory_field(GameInfo_t* gameInfo);
  void free_memory_next(GameInfo_t* gameInfo);
  void free_memory_snake(GameInfo_t* gameInfo);
  void generate_apple_in_field(GameInfo_t* gameInfo, int* apple_height,
                               int* apple_width);
  void controller(GameInfo_t* gameInfo);
  bool fix_buttons(UserAction_t newDir, UserAction_t prevDir);
  int collision_check(GameInfo_t gameInfo);
  void snake_movement(GameInfo_t* gameInfo);
  void clear_apple(GameInfo_t* gameInfo);
  GameInfo_t updateCurrentState(GameInfo_t gameInfo);
  UserAction_t get_signal(int user_input);
  void initialization();

 public:
  SnakeGame();
  ~SnakeGame();
  void run();
};

UserAction_t SnakeGame::get_signal(int user_input) {
  UserAction_t key_code = Start;

  switch (user_input) {
    case KEY_DOWN:
      return Down;
    case KEY_UP:
      return Up;
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case ESCAPE_KEY:
      return Terminate;
    case ENTER_KEY:
      return Pause;
    case SPACE_KEY:
      return Action;
    default:
      return None;
  }

  return key_code;
}

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

void SnakeGame::generate_apple_in_field(GameInfo_t* gameInfo, int* apple_height,
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
    bool overlaps_tail = false;
    for (int i = 0; i < gameInfo->n_tail; i++) {
      if (*apple_height == gameInfo->tail_y[i] &&
          *apple_width == gameInfo->tail_x[i]) {
        overlaps_tail = true;
        break;
      }
    }

    if (!overlaps_tail && gameInfo->field[*apple_height][*apple_width] == 0 &&
        gameInfo->snake[*apple_height][*apple_width] == 0) {
      valid_position = true;
    }
  }
  gameInfo->next[*apple_height][*apple_width] = 1;
}

bool SnakeGame::fix_buttons(UserAction_t newDir, UserAction_t prevDir) {
  // Не разрешаем разворот в противоположную сторону
  if ((newDir == Up && prevDir == Down) || (newDir == Down && prevDir == Up))
    return false;
  if ((newDir == Left && prevDir == Right) ||
      (newDir == Right && prevDir == Left))
    return false;
  return true;
}

void SnakeGame::controller(GameInfo_t* gameInfo) {
  int ch = getch();                                 // Считываем ввод
  UserAction_t action = SnakeGame::get_signal(ch);  // Преобразуем

  if (action == None) return;  // Ничего не нажато

  if (action == Terminate) {
    gameInfo->pause = 2;  // Завершение игры
    return;
  }

  if (action == Pause) {
    gameInfo->pause = 1;  // Пауза
    return;
  }

  if (action == Action) {
    gameInfo->pause = 0;  // играть
    return;
  }

  // Направление движения
  if (action == Up || action == Down || action == Left || action == Right) {
    // Если движение валидное, обновляем
    if (SnakeGame::fix_buttons(action,
                               SnakeGame::get_signal(gameInfo->prev_key))) {
      gameInfo->prev_key = ch;
      gameInfo->key = ch;
      gameInfo->pause = 0;
    }
  }
}

int SnakeGame::collision_check(GameInfo_t gameInfo) {
  int collision = 0;
  if (gameInfo.x <= 0 || gameInfo.x >= FIELD_WIDTH - 1 || gameInfo.y <= 0 ||
      gameInfo.y >= FIELD_HEIGHT - 1 ||
      gameInfo.snake[gameInfo.y][gameInfo.x] == 1) {
    collision = 1;  // Конец игры
  }
  for (int i = gameInfo.n_tail; i > 0; i--) {
    if (gameInfo.x == gameInfo.tail_x[i] && gameInfo.y == gameInfo.tail_y[i] &&
        gameInfo.n_tail > 4) {
      collision = 1;  // Конец игры
    }
  }

  return collision;
}

void SnakeGame::snake_movement(GameInfo_t* gameInfo) {
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

void SnakeGame::clear_apple(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      gameInfo->next[i][j] = 0;
    }
  }
}

GameInfo_t SnakeGame::updateCurrentState(GameInfo_t gameInfo) {
  GameInfo_t game_info_update;
  SnakeGame::allocate_memory_for_field(&game_info_update);

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

void SnakeGame::initialization() {
  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  allocate_memory_for_field(&gameInfo);
  allocate_memory_for_next(&gameInfo);
  allocate_memory_for_snake(&gameInfo);

  generate_apple_in_field(&gameInfo, &apple_height, &apple_width);
}

// Конструктор SnakeGame
SnakeGame::SnakeGame(/* args */) {
  // инициализируем начальное состояние игры
  state = GameState::MENU;
  gameInfo.next = nullptr;
  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 1;
  gameInfo.pause = 1;  // пауза, 0 - игра, 2 - конец игры

  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();
  gameInfo.x = 5;       // Голова змейки
  gameInfo.y = 13;      // Голова змейки
  gameInfo.n_tail = 4;  // Длина хвоста
  gameInfo.key = -1;
}

// Деструктор SnakeGame
SnakeGame::~SnakeGame() {
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;
  free_memory_field(&gameInfo);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);
}

void SnakeGame::run() {
  // initscr();
  cbreak();
  noecho();
  while (true) {
    switch (state) {
      case GameState::MENU:
        system("cls");
        std::cout << "==== SNAKE GAME ====\n";
        std::cout << "Press ENTER to start\n";
        if (std::cin.get() == 10) state = GameState::PLAY;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        break;
      case GameState::PLAY:
        SnakeGame::initialization();
        while (waitHalfSecond()) {
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

          for (int i = gameInfo.n_tail - 1; i > 0; i--) {
            gameInfo.tail_x[i] = gameInfo.tail_x[i - 1];
            gameInfo.tail_y[i] = gameInfo.tail_y[i - 1];
          }

          print_field_n(updateCurrentState(gameInfo));
        }

        getch();   // Ждём нажатия перед выходом
        endwin();  // Завершение ncurses
      default:
        break;
    }
  }
}

int main() {
  SnakeGame game;
  game.run();  // Запуск игры
  return 0;
}
