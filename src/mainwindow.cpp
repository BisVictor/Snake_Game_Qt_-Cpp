#include "mainwindow.h"

#include <QFile>
#include <cstdlib>
#include <ctime>

#include "gui/desktop/frontend.h"

using namespace s21;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  gameTimer = new QTimer(this);
  connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
  gameTimer->start(gameInfo.speed);

  // Инициализация таймера ускорения
  boostTimer = new QTimer(this);
  boostTimer->setSingleShot(true);  // Сработает только один раз
  normalSpeed = gameInfo.speed;     // Сохраняем обычную скорость

  connect(boostTimer, &QTimer::timeout, this, [this]() {
    // Возвращаем обычную скорость
    gameInfo.speed = normalSpeed;
    gameTimer->start(gameInfo.speed);
  });

  gameInfo.pause = 1;
  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();
  gameInfo.x = 5;
  gameInfo.y = 13;
  gameInfo.n_tail = 4;
  gameInfo.prev_key = QT_KEY_UP;
  // currentState = nullptr;

  initialization();
  setFixedSize(FIELD_WIDTH * 20 + 200, FIELD_HEIGHT * 20);
}

MainWindow::~MainWindow() {
  free_tail_arrays(&gameInfo);
  free_memory_field(&gameInfo);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);
}

// MEMORY FUNCTIONS

void MainWindow::free_tail_arrays(GameInfo_t* gameInfo) {
  delete[] gameInfo->tail_x;
  delete[] gameInfo->tail_y;
  gameInfo->tail_x = nullptr;
  gameInfo->tail_y = nullptr;
}

void MainWindow::allocate_memory_for_field(GameInfo_t* gameInfo) {
  if (gameInfo->field) free_memory_field(gameInfo);

  gameInfo->field = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->field[i] = new int[FIELD_WIDTH]();
  }

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gameInfo->field[i][j] = 1;
      }
    }
  }
}

void MainWindow::filling_playing_field(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      gameInfo->snake[i][j] = 0;
    }
  }
}

void MainWindow::allocate_memory_for_next(GameInfo_t* gameInfo) {
  if (gameInfo->next) free_memory_next(gameInfo);

  gameInfo->next = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->next[i] = new int[FIELD_WIDTH]();
  }

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gameInfo->next[i][j] = 1;
      }
    }
  }
}

void MainWindow::allocate_memory_for_snake(GameInfo_t* gameInfo) {
  if (gameInfo->snake) free_memory_snake(gameInfo);

  gameInfo->snake = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gameInfo->snake[i] = new int[FIELD_WIDTH]();
  }
  filling_playing_field(gameInfo);
}

void MainWindow::free_memory_field(GameInfo_t* gameInfo) {
  if (!gameInfo->field) return;

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->field[i];
  }
  delete[] gameInfo->field;
  gameInfo->field = nullptr;
}

void MainWindow::free_memory_next(GameInfo_t* gameInfo) {
  if (!gameInfo->next) return;

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->next[i];
  }
  delete[] gameInfo->next;
  gameInfo->next = nullptr;
}

void MainWindow::free_memory_snake(GameInfo_t* gameInfo) {
  if (!gameInfo->snake) return;

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    delete[] gameInfo->snake[i];
  }
  delete[] gameInfo->snake;
  gameInfo->snake = nullptr;
}

void MainWindow::initialization() {
  allocate_memory_for_field(&gameInfo);
  allocate_memory_for_field(&currentState);
  allocate_memory_for_next(&gameInfo);
  allocate_memory_for_snake(&gameInfo);
  generate_apple_in_field(&gameInfo, &apple_height, &apple_width);
}

void MainWindow::reset() {
  free_tail_arrays(&gameInfo);
  free_memory_field(&gameInfo);
  free_memory_field(&currentState);
  free_memory_next(&gameInfo);
  free_memory_snake(&gameInfo);

  gameInfo.tail_x = new int[100]();
  gameInfo.tail_y = new int[100]();

  gameInfo.x = 5;
  gameInfo.y = 13;
  gameInfo.n_tail = 4;
  gameInfo.prev_key = QT_KEY_UP;
  gameInfo.first_run = false;
  gameInfo.score = 0;
  gameInfo.level = 1;
  // gameInfo.speed = 400;
  gameInfo.pause = 1;
  state = GameState::MENU;
  normalSpeed = 400;

  gameInfo.speed = normalSpeed;
  gameTimer->start(gameInfo.speed);
  boostTimer->stop();

  initialization();
}

// END MEMORY FUNCTIONS

// CONTROLLER FUNCTIONS

bool MainWindow::fix_buttons(UserAction_t newDir, UserAction_t prevDir) {
  if ((newDir == Up && prevDir == Down) || (newDir == Down && prevDir == Up))
    return false;
  if ((newDir == Left && prevDir == Right) ||
      (newDir == Right && prevDir == Left))
    return false;
  return true;
}

int MainWindow::get_key_code(UserAction_t action) {
  switch (action) {
    case Up:
      return QT_KEY_UP;
    case Down:
      return QT_KEY_DOWN;
    case Left:
      return QT_KEY_LEFT;
    case Right:
      return QT_KEY_RIGHT;
    default:
      return -1;
  }
}

UserAction_t MainWindow::get_signal(int user_input) {
  switch (user_input) {
    case QT_KEY_DOWN:
      return Down;
    case QT_KEY_UP:
      return Up;
    case QT_KEY_LEFT:
      return Left;
    case QT_KEY_RIGHT:
      return Right;
    case QT_ESCAPE_KEY:
      return Terminate;
    case QT_ENTER_KEY:
      return Pause;
    case QT_SPACE_KEY:
      return Action;
    default:
      return None;
  }
}

void MainWindow::handle_action(UserAction_t action, GameInfo_t* gameInfo) {
  if (action == Terminate) {
    state = GameState::EXIT;
    return;
  } else if (action == Pause) {
    if (gameInfo->pause == 0) {
      gameInfo->pause = 1;
      state = GameState::PAUSE;
    } else if (gameInfo->pause == 1) {
      gameInfo->pause = 0;
      state = GameState::PLAY;
    } else if (gameInfo->pause == 2) {
      reset();
      state = GameState::PLAY;
    }
    return;
  } else if (action == Up || action == Down || action == Left ||
             action == Right) {
    if (gameInfo->pause == 0 &&
        fix_buttons(action, get_signal(gameInfo->prev_key))) {
      gameInfo->prev_key = get_key_code(action);
      gameInfo->key = get_key_code(action);
    }
  } else if (action == Action) {  // Обработка пробела
    if (gameInfo->pause == 0) {
      // Если ускорение уже активно - сбросить и начать заново
      if (boostTimer->isActive()) {
        boostTimer->stop();
      }

      // Устанавливаем ускоренную скорость
      gameInfo->speed = 50;
      gameTimer->start(gameInfo->speed);

      // Запускаем таймер на 0.2 секунду
      boostTimer->start(200);
    }
  }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  UserAction_t action = get_signal(event->key());
  if (action == None) return;
  handle_action(action, &gameInfo);  // Вся логика здесь
}

// END CONTROLLER FUNCTIONS

void MainWindow::generate_apple_in_field(GameInfo_t* gameInfo,
                                         int* apple_height, int* apple_width) {
  bool valid_position = false;
  static bool initialized = false;
  if (!initialized) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    initialized = true;
  }

  while (!valid_position) {
    *apple_height = 1 + std::rand() % (FIELD_HEIGHT - 2);
    *apple_width = 1 + std::rand() % (FIELD_WIDTH - 2);

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

int MainWindow::collision_check(const GameInfo_t& gameInfo) {
  // Проверка границ
  if (gameInfo.x <= 0 || gameInfo.x >= FIELD_WIDTH - 1 || gameInfo.y <= 0 ||
      gameInfo.y >= FIELD_HEIGHT - 1) {
    return 1;
  }

  // Проверка столкновения с хвостом
  for (int i = 0; i < gameInfo.n_tail; i++) {
    if (gameInfo.x == gameInfo.tail_x[i] && gameInfo.y == gameInfo.tail_y[i]) {
      return 1;
    }
  }
  return 0;
}

void MainWindow::snake_movement(GameInfo_t* gameInfo) {
  gameInfo->tail_x[0] = gameInfo->x;
  gameInfo->tail_y[0] = gameInfo->y;
  switch (gameInfo->prev_key) {
    case QT_KEY_UP:
      gameInfo->y--;
      break;
    case QT_KEY_DOWN:
      gameInfo->y++;
      break;
    case QT_KEY_LEFT:
      gameInfo->x--;
      break;
    case QT_KEY_RIGHT:
      gameInfo->x++;
      break;
    default:
      break;
  }
  gameInfo->snake[gameInfo->y][gameInfo->x] = 1;
}

void MainWindow::clear_apple(GameInfo_t* gameInfo) {
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      gameInfo->next[i][j] = 0;
    }
  }
}

GameInfo_t MainWindow::updateCurrentState(const GameInfo_t& gameInfo) {
  GameInfo_t game_info_update;
  game_info_update.tail_x = nullptr;
  game_info_update.tail_y = nullptr;
  game_info_update.field = nullptr;
  game_info_update.snake = nullptr;
  game_info_update.next = nullptr;

  allocate_memory_for_field(&game_info_update);

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
  game_info_update.speed = gameInfo.speed;

  return game_info_update;
}

void MainWindow::actual_level(GameInfo_t* gameInfo) {
  // Не меняем скорость, если активно временное ускорение
  if (boostTimer->isActive()) {
    return;
  }

  int newSpeed;
  int newLevel;

  // Определяем уровень и скорость в зависимости от счета
  if (gameInfo->score >= 45) {
    newLevel = 10;
    newSpeed = 120;
  } else if (gameInfo->score >= 40) {
    newLevel = 9;
    newSpeed = 140;
  } else if (gameInfo->score >= 35) {
    newLevel = 8;
    newSpeed = 160;
  } else if (gameInfo->score >= 30) {
    newLevel = 7;
    newSpeed = 180;
  } else if (gameInfo->score >= 25) {
    newLevel = 6;
    newSpeed = 200;
  } else if (gameInfo->score >= 20) {
    newLevel = 5;
    newSpeed = 220;
  } else if (gameInfo->score >= 15) {
    newLevel = 4;
    newSpeed = 250;
  } else if (gameInfo->score >= 10) {
    newLevel = 3;
    newSpeed = 300;
  } else if (gameInfo->score >= 5) {
    newLevel = 2;
    newSpeed = 350;
  } else {
    newLevel = 1;
    newSpeed = 400;
  }

  // Если уровень или скорость изменились
  if (newLevel != gameInfo->level || newSpeed != gameInfo->speed) {
    gameInfo->level = newLevel;
    gameInfo->speed = newSpeed;
    normalSpeed = newSpeed;  // Обновляем базовую скорость

    // Перезапускаем таймер только если игра не на паузе
    if (gameInfo->pause == 0) {
      gameTimer->start(gameInfo->speed);
    }
  }
}

int MainWindow::load_high_score(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return 0;
  }

  QTextStream in(&file);
  int high_score = 0;
  in >> high_score;
  file.close();
  return high_score;
}

void MainWindow::save_high_score(const QString& filename, int score) {
  int current_high = load_high_score(filename);
  if (score <= current_high) {
    return;
  }

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QTextStream out(&file);
  out << score;
  file.close();
}

void MainWindow::game_state_menu() { state = GameState::PLAY; }

void MainWindow::game_state_pause() {
  state = GameState::PAUSE;
  currentState.pause = 1;
}

void MainWindow::game_state_play() {
  gameInfo.high_score = load_high_score("hs_log.txt");
  filling_playing_field(&gameInfo);

  actual_level(&gameInfo);

  if (gameInfo.pause == 1) {
    state = GameState::PAUSE;
    currentState.pause = 1;
    return;
  } else if (gameInfo.pause == 2) {
    state = GameState::GAME_OVER;
    currentState.pause = 2;
    update();
    return;
  }

  if (!collision_check(gameInfo)) {
    snake_movement(&gameInfo);
  } else {
    gameInfo.pause = 2;
    currentState = updateCurrentState(gameInfo);
    update();
    return;
  }

  if (gameInfo.y == apple_height && gameInfo.x == apple_width) {
    clear_apple(&gameInfo);
    gameInfo.n_tail++;
    generate_apple_in_field(&gameInfo, &apple_height, &apple_width);
    gameInfo.score += 1;

    if (gameInfo.score > gameInfo.high_score) {
      gameInfo.high_score = gameInfo.score;
      save_high_score("hs_log.txt", gameInfo.high_score);
    }
  }

  for (int i = gameInfo.n_tail - 1; i > 0; i--) {
    gameInfo.tail_x[i] = gameInfo.tail_x[i - 1];
    gameInfo.tail_y[i] = gameInfo.tail_y[i - 1];
  }

  currentState = updateCurrentState(gameInfo);
  update();
}

void MainWindow::game_state_game_over() {
  if (state == GameState::PLAY) {
    reset();
    update();
  }
}

void MainWindow::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  RenderUtils::drawAll(painter, currentState, width(), height());
}

void MainWindow::updateGame() {
  actual_level(&gameInfo);
  switch (state) {
    case GameState::PLAY:
      game_state_play();
      break;
    case GameState::MENU:
      game_state_menu();
      break;
    case GameState::PAUSE:
      game_state_pause();
      break;
    case GameState::GAME_OVER:
      game_state_game_over();
      break;
    case GameState::EXIT:
      close();
      break;
  }
}
