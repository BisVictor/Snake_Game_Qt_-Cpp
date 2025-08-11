// #include <conio.h>

#include <ncurses.h>

#include <array>
#include <chrono>
#include <fstream>  //file
#include <iostream>
#include <thread>  // bool wait()

const int FIELD_WIDTH = 12;
const int FIELD_HEIGHT = 22;

typedef struct {
  int** field;
  int** snake;
  int** next;  // яблоко
  int* tail_x;
  int* tail_y;
  int n_tail;
  int key;
  int prev_key;
  int x;
  int y;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  bool first_run;
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

bool wait(GameInfo_t* gameInfo) {
  std::this_thread::sleep_for(std::chrono::milliseconds(gameInfo->speed));
  return true;
}

// #define FIELD_HEIGHT 22
// #define FIELD_WIDTH 12

class KeyLogger {
 private:
  static const int MAX_KEYS = 3;           // Храним последние 10 клавиш
  std::array<int, MAX_KEYS> key_buffer{};  // Массив нажатий
  int current_index = 0;

 public:
  // Возвращает true, если нажатие не повторяется сразу
  bool log_key(int ch) {
    // Проверка на дублирование последнего нажатия
    if (current_index > 0 && key_buffer[(current_index - 1) % MAX_KEYS] == ch) {
      return false;  // Повтор — залипание
    }

    key_buffer[current_index % MAX_KEYS] = ch;
    current_index++;

    return true;  // Успешное нажатие
  }
};

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
  mvprintw(0, 27, "Game status: ");
  switch (game_info_update.pause) {
    case 0:
      game_status = "Play";
      break;
    case 1:
      game_status = "Pause (Press Enter to start)";
      break;
    case 2:
      game_status = "Game Over";
      break;
    default:
      game_status = "Unknown";
      break;
  }
  // print game status
  mvprintw(1, 27, "%s", game_status);
  // print game score
  mvprintw(2, 27, "%s", "Game level:");
  mvprintw(3, 27, "%d", game_info_update.level);
  // print game score
  mvprintw(4, 27, "%s", "Game high score:");
  mvprintw(5, 27, "%d", game_info_update.high_score);
  // print game score
  mvprintw(6, 27, "%s", "Game score:");
  mvprintw(7, 27, "%d", game_info_update.score);

  refresh();
}

enum class GameState { MENU, PLAY, PAUSE, GAME_OVER, EXIT };

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
  void controller(GameInfo_t* gameInfo, GameState* state);
  void handle_action(UserAction_t action, GameInfo_t* gameInfo,
                     GameState* state);
  int get_key_code(UserAction_t action);
  bool fix_buttons(UserAction_t newDir, UserAction_t prevDir);
  int collision_check(GameInfo_t gameInfo);
  void snake_movement(GameInfo_t* gameInfo);
  void clear_apple(GameInfo_t* gameInfo);
  GameInfo_t updateCurrentState(GameInfo_t gameInfo);
  UserAction_t get_signal(int user_input);
  void initialization();
  void reset();
  void game_state_menu(GameState* state, GameInfo_t* gameInfo);
  void game_state_play(GameState* state, GameInfo_t* gameInfo);
  void game_state_game_over(GameState* state, GameInfo_t* gameInfo);
  void actual_level(GameInfo_t* gameInfo);
  int load_high_score(const std::string& filename);
  void save_high_score(const std::string& filename, int score);

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

int SnakeGame::get_key_code(UserAction_t action) {
  switch (action) {
    case Up:
      return KEY_UP;
    case Down:
      return KEY_DOWN;
    case Left:
      return KEY_LEFT;
    case Right:
      return KEY_RIGHT;
    default:
      return -1;
  }
}

void SnakeGame::handle_action(UserAction_t action, GameInfo_t* gameInfo,
                              GameState* state) {
  if (action == Terminate) {
    *state = GameState::EXIT;
    return;
  }

  if (action == Pause && gameInfo->pause == 0) {
    gameInfo->pause = 1;
    *state = GameState::PAUSE;
    return;
  } else if (action == Pause && gameInfo->pause == 1) {
    gameInfo->pause = 0;
    *state = GameState::PLAY;
    return;
  } else if (action == Pause && gameInfo->pause == 2) {
    gameInfo->pause = 0;
    *state = GameState::PLAY;
    return;
  } else if (action == Terminate &&
             (gameInfo->pause == 1 || gameInfo->pause == 2)) {
    gameInfo->pause = 3;
    return;
  }

  if (action == Up || action == Down || action == Left || action == Right) {
    if (SnakeGame::fix_buttons(action,
                               SnakeGame::get_signal(gameInfo->prev_key))) {
      gameInfo->prev_key = SnakeGame::get_key_code(action);
      gameInfo->key = SnakeGame::get_key_code(action);
    }
  }
  if (action == Action) {
    gameInfo->speed = 50;
  }
}

// @brief Тонкий контроллер
void SnakeGame::controller(GameInfo_t* gameInfo, GameState* state) {
  int ch;
  KeyLogger logger;

  if (gameInfo->first_run) {
    ch = '\n';
    gameInfo->first_run = false;
  } else {
    ch = getch();
  }
  // Обработка нажатия
  if (logger.log_key(ch)) {
  } else {
    // Залипание — игнорировать
  }

  UserAction_t action = SnakeGame::get_signal(ch);
  if (action == None) return;
  handle_action(action, gameInfo, state);
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
          gameInfo.next[i][j] > 0 || (gameInfo.y == i && gameInfo.x == j)) {
        game_info_update.field[i][j] = 1;
      } else {
        game_info_update.field[i][j] = 0;
      }
    }
  }

  for (int i = 0; i < 100; i++) {
    if (gameInfo.tail_x[i] > 0 && gameInfo.tail_y[i] > 0) {
      game_info_update.field[gameInfo.tail_y[i]][gameInfo.tail_x[i]] = 1;
    }
  }
  game_info_update.pause = gameInfo.pause;
  game_info_update.level = gameInfo.level;
  game_info_update.high_score = gameInfo.high_score;
  game_info_update.score = gameInfo.score;

  return game_info_update;
}

void SnakeGame::initialization() {
  // Инициализация ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);  // неблокирующая версия getch()

  allocate_memory_for_field(&gameInfo);
  allocate_memory_for_next(&gameInfo);
  allocate_memory_for_snake(&gameInfo);

  generate_apple_in_field(&gameInfo, &apple_height, &apple_width);
}

void SnakeGame::reset() {
  // Освобождение выделенной памяти
  delete[] gameInfo.tail_x;
  delete[] gameInfo.tail_y;
  free_memory_field(&gameInfo);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);

  // Сброс параметров игры
  gameInfo.next = nullptr;
  gameInfo.score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 400;
  gameInfo.pause = 1;
  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();
  gameInfo.x = 5;
  gameInfo.y = 13;
  gameInfo.n_tail = 4;
  gameInfo.prev_key = KEY_UP;
  gameInfo.first_run = false;
  state = GameState::MENU;

  // Повторная инициализация
  initialization();
}

// @brief Актуальный уровень и score
void SnakeGame::actual_level(GameInfo_t* gameInfo) {
  if (gameInfo->score >= 5 && gameInfo->score < 10) {
    gameInfo->level = 2;
    gameInfo->speed = 350;
  } else if (gameInfo->score >= 10 && gameInfo->score < 15) {
    gameInfo->level = 3;
    gameInfo->speed = 300;
  } else if (gameInfo->score >= 15 && gameInfo->score < 20) {
    gameInfo->level = 4;
    gameInfo->speed = 250;
  } else if (gameInfo->score >= 20 && gameInfo->score < 25) {
    gameInfo->level = 5;
    gameInfo->speed = 220;
  } else if (gameInfo->score >= 25 && gameInfo->score < 30) {
    gameInfo->level = 6;
    gameInfo->speed = 200;
  } else if (gameInfo->score >= 30 && gameInfo->score < 35) {
    gameInfo->level = 7;
    gameInfo->speed = 180;
  } else if (gameInfo->score >= 35 && gameInfo->score < 40) {
    gameInfo->level = 8;
    gameInfo->speed = 160;
  } else if (gameInfo->score >= 40 && gameInfo->score < 45) {
    gameInfo->level = 9;
    gameInfo->speed = 140;
  } else if (gameInfo->score >= 45) {
    gameInfo->level = 10;
    gameInfo->speed = 120;
  } else {
    gameInfo->level = 1;
    gameInfo->speed = 400;  // Базовая скорость
  }
}

// @brief Считываем high core игры
int SnakeGame::load_high_score(const std::string& filename) {
  std::ifstream file(filename);  // Открываем файл на чтение
  int high_score = 0;

  if (file.is_open()) {  // Убедимся, что файл открылся
    file >> high_score;  // Считываем число из файла
    file.close();
  }

  return high_score;  // Возвращаем считанное значение (или 0 по умолчанию)
}

// @brief Сохраняем high core игры
void SnakeGame::save_high_score(const std::string& filename, int score) {
  int current_high = SnakeGame::load_high_score(filename);

  if (score > current_high) {
    std::ofstream file(filename);
    if (file.is_open()) {
      file << score;
      file.close();
    }
  }
}

// @brief Меню игры
void SnakeGame::game_state_menu(GameState* state, GameInfo_t* gameInfo) {
  std::cout << "==== SNAKE GAME ====\n";
  std::cout << "Press ENTER to start\n";
  std::cout << static_cast<int>(*state) << std::endl;
  while (*state == GameState::MENU) {
    SnakeGame::controller(gameInfo, state);
  }
}

// @brief Запуск игры
void SnakeGame::game_state_play(GameState* state, GameInfo_t* gameInfo) {
  SnakeGame::initialization();
  gameInfo->high_score = SnakeGame::load_high_score("hs_log.txt");
  while (wait(gameInfo) &&
         (*state == GameState::PLAY || *state == GameState::PAUSE)) {
    SnakeGame::actual_level(gameInfo);

    SnakeGame::filling_playing_field(gameInfo);
    SnakeGame::controller(gameInfo, state);

    if (gameInfo->pause == 1) {  // Pause
      print_field_n(SnakeGame::updateCurrentState(*gameInfo));
      continue;
    } else if (gameInfo->pause == 2) {  // Quit
      *state = GameState::GAME_OVER;
      break;
    }

    if (!SnakeGame::collision_check(*gameInfo)) {
      SnakeGame::snake_movement(gameInfo);
    } else {
      gameInfo->pause = 2;
    }

    if (gameInfo->y == apple_height && gameInfo->x == apple_width) {
      SnakeGame::clear_apple(gameInfo);
      gameInfo->n_tail++;
      SnakeGame::generate_apple_in_field(gameInfo, &apple_height, &apple_width);
      gameInfo->score += 1;
      if (gameInfo->score > gameInfo->high_score) {
        gameInfo->high_score = gameInfo->score;
        SnakeGame::save_high_score("hs_log.txt", gameInfo->high_score);
      }
    }

    for (int i = gameInfo->n_tail - 1; i > 0; i--) {
      gameInfo->tail_x[i] = gameInfo->tail_x[i - 1];
      gameInfo->tail_y[i] = gameInfo->tail_y[i - 1];
    }

    print_field_n(SnakeGame::updateCurrentState(*gameInfo));
  }
  getch();   // Ждём нажатия перед выходом
  endwin();  // Завершение ncurses
}

// @brief Конец игры
void SnakeGame::game_state_game_over(GameState* state, GameInfo_t* gameInfo) {
  std::cout << "==== SNAKE GAME OVER ====\n";
  std::cout << "Your score: " << gameInfo->score << "\n";
  std::cout << "Press ENTER to restart\n";
  while (*state == GameState::GAME_OVER) {
    SnakeGame::controller(gameInfo, state);
    if (*state == GameState::PLAY) {
      SnakeGame::reset();
    }
  }
}

// Конструктор SnakeGame
SnakeGame::SnakeGame(/* args */) {
  // инициализируем начальное состояние игры
  state = GameState::MENU;
  gameInfo.next = nullptr;
  gameInfo.first_run = true;
  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 400;
  gameInfo.pause = 1;   //  0 - игра, 1- пауза, 2 - конец игры
  gameInfo.x = 5;       // Голова змейки
  gameInfo.y = 13;      // Голова змейки
  gameInfo.n_tail = 4;  // Длина хвоста
  gameInfo.key = KEY_UP;
  gameInfo.prev_key = KEY_UP;

  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();
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
  while (state != GameState::EXIT) {
    switch (state) {
      case GameState::MENU:
        SnakeGame::game_state_menu(&state, &gameInfo);
        break;
      case GameState::PLAY:
        SnakeGame::game_state_play(&state, &gameInfo);
        break;
      case GameState::PAUSE:
        print_field_n(updateCurrentState(gameInfo));
        controller(&gameInfo, &state);
        break;
      case GameState::GAME_OVER:
        SnakeGame::game_state_game_over(&state, &gameInfo);
        break;

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
