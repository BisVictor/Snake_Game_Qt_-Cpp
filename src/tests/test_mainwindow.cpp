#include "test_mainwindow.h"

#include <QFile>
#include <QKeyEvent>  // Добавлено для QKeyEvent
#include <QSignalSpy>
#include <QTemporaryFile>  //для load_high_score
#include <QTextStream>

#include "brick_game/snake/mainwindow.h"

namespace s21 {

void MainWindowTest::SetUp() { mainWindow = new MainWindow(); }

void MainWindowTest::TearDown() { delete mainWindow; }

TEST_F(MainWindowTest, MemoryAllocation) {
  GameInfo_t testInfo;
  MainWindow::allocate_memory_for_field(&testInfo);
  ASSERT_NE(testInfo.field, nullptr);

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    ASSERT_NE(testInfo.field[i], nullptr);
  }

  MainWindow::free_memory_field(&testInfo);
}

TEST_F(MainWindowTest, InitializationTest) {
  mainWindow->initialization();

  ASSERT_NE(mainWindow->gameInfo.field, nullptr);
  ASSERT_NE(mainWindow->gameInfo.next, nullptr);
  ASSERT_NE(mainWindow->gameInfo.snake, nullptr);

  // Проверка, что яблоко сгенерировано в допустимых пределах
  EXPECT_GE(mainWindow->apple_height, 1);
  EXPECT_LT(mainWindow->apple_height, FIELD_HEIGHT - 1);
  EXPECT_GE(mainWindow->apple_width, 1);
  EXPECT_LT(mainWindow->apple_width, FIELD_WIDTH - 1);
}

TEST_F(MainWindowTest, ResetFunctionality) {
  // Изменяем состояние игры
  mainWindow->gameInfo.score = 100;
  mainWindow->gameInfo.level = 5;
  mainWindow->state = GameState::PLAY;

  // Сбрасываем игру
  mainWindow->reset();

  // Проверяем сброс состояния
  EXPECT_EQ(mainWindow->gameInfo.score, 0);
  EXPECT_EQ(mainWindow->gameInfo.level, 1);
  EXPECT_EQ(mainWindow->state, GameState::MENU);
  EXPECT_EQ(mainWindow->gameInfo.x, 5);
  EXPECT_EQ(mainWindow->gameInfo.y, 13);
  EXPECT_EQ(mainWindow->gameInfo.n_tail, 4);
}

TEST_F(MainWindowTest, TimerFunctionality) {
  // Проверка работы таймеров
  EXPECT_TRUE(mainWindow->gameTimer->isActive());
  EXPECT_FALSE(mainWindow->boostTimer->isActive());

  // Проверка начальной скорости
  EXPECT_EQ(mainWindow->gameInfo.speed, 400);
  EXPECT_EQ(mainWindow->normalSpeed, 400);
}

TEST_F(MainWindowTest, GameStateTransitions) {
  // Проверка перехода между состояниями игры
  mainWindow->state = GameState::MENU;

  // Эмулируем нажатие клавиши Enter для старта игры
  QKeyEvent event(QEvent::KeyPress, QT_ENTER_KEY, Qt::NoModifier);
  mainWindow->keyPressEvent(&event);

  EXPECT_EQ(mainWindow->state, GameState::PLAY);
  EXPECT_EQ(mainWindow->gameInfo.pause, 0);
}

// Тест для проверки обработки столкновений
TEST_F(MainWindowTest, CollisionDetection) {
  mainWindow->initialization();

  // Устанавливаем змейку в позицию столкновения со стеной
  mainWindow->gameInfo.x = 0;
  mainWindow->gameInfo.y = 0;

  int collision = mainWindow->collision_check(mainWindow->gameInfo);
  EXPECT_EQ(collision, 1);  // Ожидаем обнаружение столкновения
}

// Тест движения змейки вверх
TEST_F(MainWindowTest, SnakeMovementUp) {
  mainWindow->initialization();

  // Устанавливаем начальную позицию и предыдущую клавишу
  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_UP;

  // Сохраняем предыдущие координаты для проверки хвоста
  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  // Проверяем движение вверх (y уменьшается)
  EXPECT_EQ(mainWindow->gameInfo.x, 5);
  EXPECT_EQ(mainWindow->gameInfo.y, 4);

  // Проверяем, что предыдущая позиция сохранилась в хвост
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);

  // Проверяем, что новая позиция отмечена в массиве snake
  EXPECT_EQ(mainWindow->gameInfo.snake[4][5], 1);
}

// Тест движения змейки вниз
TEST_F(MainWindowTest, SnakeMovementDown) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_DOWN;

  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.x, 5);
  EXPECT_EQ(mainWindow->gameInfo.y, 6);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);
  EXPECT_EQ(mainWindow->gameInfo.snake[6][5], 1);
}

// Тест движения змейки влево
TEST_F(MainWindowTest, SnakeMovementLeft) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_LEFT;

  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.x, 4);
  EXPECT_EQ(mainWindow->gameInfo.y, 5);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);
  EXPECT_EQ(mainWindow->gameInfo.snake[5][4], 1);
}

// Тест движения змейки вправо
TEST_F(MainWindowTest, SnakeMovementRight) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;

  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.x, 6);
  EXPECT_EQ(mainWindow->gameInfo.y, 5);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);
  EXPECT_EQ(mainWindow->gameInfo.snake[5][6], 1);
}

// Тест движения без предыдущей клавиши (должен остаться на месте)
TEST_F(MainWindowTest, SnakeMovementNoKey) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = 0;  // Нет клавиши

  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  // Позиция не должна измениться
  EXPECT_EQ(mainWindow->gameInfo.x, prev_x);
  EXPECT_EQ(mainWindow->gameInfo.y, prev_y);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);
  EXPECT_EQ(mainWindow->gameInfo.snake[5][5], 1);
}

// Тест движения с неизвестной клавишей
TEST_F(MainWindowTest, SnakeMovementUnknownKey) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = 999;  // Неизвестная клавиша

  int prev_x = mainWindow->gameInfo.x;
  int prev_y = mainWindow->gameInfo.y;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  // Позиция не должна измениться
  EXPECT_EQ(mainWindow->gameInfo.x, prev_x);
  EXPECT_EQ(mainWindow->gameInfo.y, prev_y);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], prev_x);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], prev_y);
  EXPECT_EQ(mainWindow->gameInfo.snake[5][5], 1);
}

// Тест граничных условий - движение от края поля
TEST_F(MainWindowTest, SnakeMovementFromEdge) {
  mainWindow->initialization();

  // Движение вправо от левого края
  mainWindow->gameInfo.x = 0;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;

  mainWindow->snake_movement(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.x, 1);
  EXPECT_EQ(mainWindow->gameInfo.y, 5);
  EXPECT_EQ(mainWindow->gameInfo.snake[5][1], 1);
}

// Тест последовательных движений
TEST_F(MainWindowTest, SnakeMovementSequence) {
  mainWindow->initialization();

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;

  // Движение вправо
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;
  mainWindow->snake_movement(&mainWindow->gameInfo);
  EXPECT_EQ(mainWindow->gameInfo.x, 6);
  EXPECT_EQ(mainWindow->gameInfo.y, 5);

  // Движение вниз
  mainWindow->gameInfo.prev_key = QT_KEY_DOWN;
  mainWindow->snake_movement(&mainWindow->gameInfo);
  EXPECT_EQ(mainWindow->gameInfo.x, 6);
  EXPECT_EQ(mainWindow->gameInfo.y, 6);

  // Движение влево
  mainWindow->gameInfo.prev_key = QT_KEY_LEFT;
  mainWindow->snake_movement(&mainWindow->gameInfo);
  EXPECT_EQ(mainWindow->gameInfo.x, 5);
  EXPECT_EQ(mainWindow->gameInfo.y, 6);

  // Проверяем историю хвоста (должны сохраниться предыдущие позиции)
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0],
            6);  // Последняя позиция перед движением влево
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], 6);
}

// Тест очистки пустого поля
TEST_F(MainWindowTest, ClearAppleEmptyField) {
  mainWindow->initialization();

  // Убедимся, что поле изначально пустое
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      mainWindow->gameInfo.next[i][j] = 0;
    }
  }

  mainWindow->clear_apple(&mainWindow->gameInfo);

  // Проверяем, что все элементы остались нулевыми
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      EXPECT_EQ(mainWindow->gameInfo.next[i][j], 0);
    }
  }
}

// Тест очистки полностью заполненного поля
TEST_F(MainWindowTest, ClearAppleFullField) {
  mainWindow->initialization();

  // Заполняем поле ненулевыми значениями
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      mainWindow->gameInfo.next[i][j] = (i + j) % 2 + 1;  // Чередуем 1 и 2
    }
  }

  mainWindow->clear_apple(&mainWindow->gameInfo);

  // Проверяем, что все элементы стали нулевыми
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      EXPECT_EQ(mainWindow->gameInfo.next[i][j], 0);
    }
  }
}

// Тест уровня 1 (score < 5)
TEST_F(MainWindowTest, ActualLevel1) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 4;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 1);
  EXPECT_EQ(mainWindow->gameInfo.speed, 400);
}

// Тест уровня 2 (score >= 5)
TEST_F(MainWindowTest, ActualLevel2) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 5;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 2);
  EXPECT_EQ(mainWindow->gameInfo.speed, 350);
}

// Тест уровня 3 (score >= 10)
TEST_F(MainWindowTest, ActualLevel3) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 10;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 3);
  EXPECT_EQ(mainWindow->gameInfo.speed, 300);
}

// Тест уровня 4 (score >= 15)
TEST_F(MainWindowTest, ActualLevel4) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 15;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 4);
  EXPECT_EQ(mainWindow->gameInfo.speed, 250);
}

// Тест уровня 5 (score >= 20)
TEST_F(MainWindowTest, ActualLevel5) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 20;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 5);
  EXPECT_EQ(mainWindow->gameInfo.speed, 220);
}

// Тест уровня 6 (score >= 25)
TEST_F(MainWindowTest, ActualLevel6) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 25;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 6);
  EXPECT_EQ(mainWindow->gameInfo.speed, 200);
}

// Тест уровня 7 (score >= 30)
TEST_F(MainWindowTest, ActualLevel7) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 30;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 7);
  EXPECT_EQ(mainWindow->gameInfo.speed, 180);
}

// Тест уровня 8 (score >= 35)
TEST_F(MainWindowTest, ActualLevel8) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 35;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 8);
  EXPECT_EQ(mainWindow->gameInfo.speed, 160);
}

// Тест уровня 9 (score >= 40)
TEST_F(MainWindowTest, ActualLevel9) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 40;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 9);
  EXPECT_EQ(mainWindow->gameInfo.speed, 140);
}

// Тест уровня 10 (score >= 45)
TEST_F(MainWindowTest, ActualLevel10) {
  mainWindow->initialization();

  mainWindow->gameInfo.score = 45;
  mainWindow->actual_level(&mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.level, 10);
  EXPECT_EQ(mainWindow->gameInfo.speed, 120);
}

// Тест загрузки корректного рекорда из файла
TEST_F(MainWindowTest, LoadHighScoreValidFile) {
  // Создаем временный файл с рекордом
  QTemporaryFile tempFile;
  ASSERT_TRUE(tempFile.open());

  QTextStream out(&tempFile);
  out << 12345;
  tempFile.close();

  int highScore = mainWindow->load_high_score(tempFile.fileName());
  EXPECT_EQ(highScore, 12345);
}

// Тест сохранения нового рекорда
TEST_F(MainWindowTest, SaveHighScoreNewRecord) {
  QString testFileName = "test_save_new.txt";

  // Сначала создаем файл с меньшим рекордом
  QFile initialFile(testFileName);
  if (initialFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&initialFile);
    out << 100;
    initialFile.close();
  }

  // Пытаемся сохранить больший рекорд
  mainWindow->save_high_score(testFileName, 200);

  // Проверяем, что рекорд обновился
  int savedScore = mainWindow->load_high_score(testFileName);
  EXPECT_EQ(savedScore, 200);

  QFile::remove(testFileName);
}

TEST_F(MainWindowTest, GameStateMenuChangesStateToPlay) {
  MainWindow window;

  // Вызываем тестируемую функцию
  window.game_state_menu();

  // Проверяем, что состояние изменилось на PLAY
  // Предполагается, что есть метод для получения состояния
  EXPECT_EQ(window.state, 1);
}

TEST_F(MainWindowTest, PauseGame_state) {
  MainWindow window;

  // Вызываем тестируемую функцию
  window.game_state_pause();

  // Проверяем, что состояние изменилось на PLAY
  // Предполагается, что есть метод для получения состояния
  EXPECT_EQ(window.state, 2);
}

// Тест перехода в состояние PAUSE
TEST_F(MainWindowTest, GameStatePlayToPause) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;

  // Устанавливаем флаг паузы
  mainWindow->gameInfo.pause = 1;

  mainWindow->game_state_play();

  EXPECT_EQ(mainWindow->state, GameState::PAUSE);
  EXPECT_EQ(mainWindow->currentState.pause, 1);
}

// Тест перехода в состояние GAME_OVER
TEST_F(MainWindowTest, GameStatePlayToGameOver) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;

  // Устанавливаем флаг завершения игры
  mainWindow->gameInfo.pause = 2;

  mainWindow->game_state_play();

  EXPECT_EQ(mainWindow->state, GameState::GAME_OVER);
  EXPECT_EQ(mainWindow->currentState.pause, 2);
}

// Тест движения змейки без столкновений
TEST_F(MainWindowTest, GameStatePlayMovementNoCollision) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;
  mainWindow->gameInfo.pause = 0;

  // Устанавливаем начальную позицию и направление
  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;

  int initialX = mainWindow->gameInfo.x;
  int initialY = mainWindow->gameInfo.y;

  mainWindow->game_state_play();

  // Змейка должна переместиться
  EXPECT_EQ(mainWindow->gameInfo.x, initialX + 1);
  EXPECT_EQ(mainWindow->gameInfo.y, initialY);
}

// Тест поедания яблока
TEST_F(MainWindowTest, GameStatePlayEatApple) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;
  mainWindow->gameInfo.pause = 0;

  // Устанавливаем позицию змейки на яблоко
  mainWindow->apple_height = 5;
  mainWindow->apple_width = 5;
  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;

  // Также нужно установить, что на этой позиции есть яблоко
  mainWindow->gameInfo.next[5][5] = 1;

  mainWindow->game_state_play();

  EXPECT_EQ(mainWindow->gameInfo.score, 0);
  EXPECT_EQ(mainWindow->gameInfo.n_tail, 4);
}

// Тест обновления хвоста змейки
TEST_F(MainWindowTest, GameStatePlayTailUpdate) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;
  mainWindow->gameInfo.pause = 0;

  // Устанавливаем несколько сегментов хвоста
  mainWindow->gameInfo.n_tail = 3;
  mainWindow->gameInfo.tail_x[0] = 4;
  mainWindow->gameInfo.tail_y[0] = 5;
  mainWindow->gameInfo.tail_x[1] = 3;
  mainWindow->gameInfo.tail_y[1] = 5;
  mainWindow->gameInfo.tail_x[2] = 2;
  mainWindow->gameInfo.tail_y[2] = 5;

  mainWindow->gameInfo.x = 5;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.prev_key = QT_KEY_RIGHT;

  mainWindow->game_state_play();

  // Хвост должен сдвинуться
  EXPECT_EQ(mainWindow->gameInfo.tail_x[0], 5);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[0], 5);
  EXPECT_EQ(mainWindow->gameInfo.tail_x[1], 5);
  EXPECT_EQ(mainWindow->gameInfo.tail_y[1], 5);
}

TEST_F(MainWindowTest, GameStateGameOverFromPlay) {
  mainWindow->initialization();

  // Устанавливаем состояние PLAY и некоторые игровые данные
  mainWindow->state = GameState::PLAY;
  mainWindow->gameInfo.score = 100;
  mainWindow->gameInfo.x = 10;
  mainWindow->gameInfo.y = 5;
  mainWindow->gameInfo.n_tail = 8;

  // Сохраняем некоторые значения до сброса
  int initialScore = mainWindow->gameInfo.score;
  int initialX = mainWindow->gameInfo.x;
  int initialY = mainWindow->gameInfo.y;
  int initialTail = mainWindow->gameInfo.n_tail;

  mainWindow->game_state_game_over();

  // Проверяем, что игра была сброшена (значения должны измениться)
  EXPECT_NE(mainWindow->gameInfo.score, initialScore);
  EXPECT_NE(mainWindow->gameInfo.x, initialX);
  EXPECT_NE(mainWindow->gameInfo.y, initialY);
  EXPECT_NE(mainWindow->gameInfo.n_tail, initialTail);
}

TEST_F(MainWindowTest, PaintEventBasicCall) {
  mainWindow->initialization();

  // Создаем mock событие отрисовки
  QRect exposedRect(0, 0, 800, 600);
  QPaintEvent event(exposedRect);

  // Устанавливаем начальное состояние
  mainWindow->currentState =
      mainWindow->updateCurrentState(mainWindow->gameInfo);

  // Вызываем paintEvent - не должно быть исключений или ошибок
  EXPECT_NO_THROW(mainWindow->paintEvent(&event));
}

TEST_F(MainWindowTest, UpdateGameCallsActualLevel) {
  mainWindow->initialization();

  // Сохраняем начальные значения
  int initialSpeed = mainWindow->gameInfo.speed;
  int initialLevel = mainWindow->gameInfo.level;

  // Устанавливаем любое состояние
  mainWindow->state = GameState::MENU;
  mainWindow->gameInfo.score = 10;  // Чтобы actual_level что-то делал

  mainWindow->updateGame();

  // actual_level должен быть вызван и возможно изменить speed/level
  // Проверяем что хотя бы одно значение изменилось
  bool changed = (mainWindow->gameInfo.speed != initialSpeed) ||
                 (mainWindow->gameInfo.level != initialLevel);
  EXPECT_TRUE(changed);
}

TEST_F(MainWindowTest, UpdateGameSequenceDifferentStates) {
  mainWindow->initialization();

  // Первый вызов - MENU
  mainWindow->state = GameState::MENU;
  mainWindow->updateGame();

  // Второй вызов - PLAY
  mainWindow->state = GameState::PLAY;
  mainWindow->updateGame();

  // Третий вызов - PAUSE
  mainWindow->state = GameState::PAUSE;
  mainWindow->updateGame();

  // Не должно быть ошибок при последовательных вызовах
  SUCCEED();
}

TEST_F(MainWindowTest, FixButtonsOppositeVertical) {
  EXPECT_FALSE(mainWindow->fix_buttons(Up, Down));
  EXPECT_FALSE(mainWindow->fix_buttons(Down, Up));
}

TEST_F(MainWindowTest, FixButtonsOppositeHorizontal) {
  EXPECT_FALSE(mainWindow->fix_buttons(Left, Right));
  EXPECT_FALSE(mainWindow->fix_buttons(Right, Left));
}

TEST_F(MainWindowTest, FixButtonsSameDirections) {
  EXPECT_TRUE(mainWindow->fix_buttons(Up, Up));
  EXPECT_TRUE(mainWindow->fix_buttons(Down, Down));
  EXPECT_TRUE(mainWindow->fix_buttons(Left, Left));
  EXPECT_TRUE(mainWindow->fix_buttons(Right, Right));
}

TEST_F(MainWindowTest, GetKeyCodeUp) {
  int keyCode = mainWindow->get_key_code(Up);
  EXPECT_EQ(keyCode, QT_KEY_UP);
}

// Тест преобразования действия Down
TEST_F(MainWindowTest, GetKeyCodeDown) {
  int keyCode = mainWindow->get_key_code(Down);
  EXPECT_EQ(keyCode, QT_KEY_DOWN);
}

// Тест преобразования действия Left
TEST_F(MainWindowTest, GetKeyCodeLeft) {
  int keyCode = mainWindow->get_key_code(Left);
  EXPECT_EQ(keyCode, QT_KEY_LEFT);
}

// Тест преобразования действия Right
TEST_F(MainWindowTest, GetKeyCodeRight) {
  int keyCode = mainWindow->get_key_code(Right);
  EXPECT_EQ(keyCode, QT_KEY_RIGHT);
}

TEST_F(MainWindowTest, GetKeyCodeNone) {
  int keyCode = mainWindow->get_key_code(None);
  EXPECT_EQ(keyCode, -1);
}

// Тест преобразования QT_KEY_DOWN в Down
TEST_F(MainWindowTest, GetSignalDown) {
  UserAction_t action = mainWindow->get_signal(QT_KEY_DOWN);
  EXPECT_EQ(action, Down);
}

// Тест преобразования QT_KEY_UP в Up
TEST_F(MainWindowTest, GetSignalUp) {
  UserAction_t action = mainWindow->get_signal(QT_KEY_UP);
  EXPECT_EQ(action, Up);
}

// Тест преобразования QT_KEY_LEFT в Left
TEST_F(MainWindowTest, GetSignalLeft) {
  UserAction_t action = mainWindow->get_signal(QT_KEY_LEFT);
  EXPECT_EQ(action, Left);
}

// Тест преобразования QT_KEY_RIGHT в Right
TEST_F(MainWindowTest, GetSignalRight) {
  UserAction_t action = mainWindow->get_signal(QT_KEY_RIGHT);
  EXPECT_EQ(action, Right);
}

// Тест преобразования QT_ESCAPE_KEY в Terminate
TEST_F(MainWindowTest, GetSignalTerminate) {
  UserAction_t action = mainWindow->get_signal(QT_ESCAPE_KEY);
  EXPECT_EQ(action, Terminate);
}

// Тест преобразования QT_ENTER_KEY в Pause
TEST_F(MainWindowTest, GetSignalPause) {
  UserAction_t action = mainWindow->get_signal(QT_ENTER_KEY);
  EXPECT_EQ(action, Pause);
}

// Тест преобразования QT_SPACE_KEY в Action
TEST_F(MainWindowTest, GetSignalAction) {
  UserAction_t action = mainWindow->get_signal(QT_SPACE_KEY);
  EXPECT_EQ(action, Action);
}

// Тест преобразования неизвестной клавиши в None
TEST_F(MainWindowTest, GetSignalUnknownKey) {
  UserAction_t action = mainWindow->get_signal(999);  // Неизвестная клавиша
  EXPECT_EQ(action, None);
}

TEST_F(MainWindowTest, HandleActionTerminate) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;

  mainWindow->handle_action(Terminate, &mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->state, GameState::EXIT);
}

TEST_F(MainWindowTest, HandleActionPauseFromPlay) {
  mainWindow->initialization();
  mainWindow->state = GameState::PLAY;
  mainWindow->gameInfo.pause = 0;

  mainWindow->handle_action(Pause, &mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.pause, 1);
  EXPECT_EQ(mainWindow->state, GameState::PAUSE);
}

TEST_F(MainWindowTest, HandleActionPauseFromPause) {
  mainWindow->initialization();
  mainWindow->state = GameState::PAUSE;
  mainWindow->gameInfo.pause = 1;

  mainWindow->handle_action(Pause, &mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.pause, 0);
  EXPECT_EQ(mainWindow->state, GameState::PLAY);
}

TEST_F(MainWindowTest, HandleActionPauseFromGameOver) {
  mainWindow->initialization();
  mainWindow->state = GameState::GAME_OVER;
  mainWindow->gameInfo.pause = 2;
  mainWindow->gameInfo.score = 100;  // Устанавливаем какой-то счет

  mainWindow->handle_action(Pause, &mainWindow->gameInfo);

  EXPECT_EQ(mainWindow->gameInfo.pause, 1);
  EXPECT_EQ(mainWindow->state, GameState::PLAY);
  EXPECT_EQ(mainWindow->gameInfo.score, 0);  // Счет должен сброситься
}

TEST_F(MainWindowTest, HandleActionValidDirection) {
  mainWindow->initialization();
  mainWindow->gameInfo.pause = 0;
  mainWindow->gameInfo.prev_key = QT_KEY_UP;  // Двигались вверх

  // Пытаемся повернуть направо (допустимое направление)
  mainWindow->handle_action(Right, &mainWindow->gameInfo);

  // Клавиша должна обновиться
  EXPECT_EQ(mainWindow->gameInfo.prev_key, QT_KEY_RIGHT);
  EXPECT_EQ(mainWindow->gameInfo.key, QT_KEY_RIGHT);
}
TEST_F(MainWindowTest, HandleActionBoostActivation) {
  mainWindow->initialization();
  mainWindow->gameInfo.pause = 0;
  int initialSpeed = mainWindow->gameInfo.speed;

  // Убедимся, что таймеры не активны
  if (mainWindow->boostTimer->isActive()) {
    mainWindow->boostTimer->stop();
  }

  mainWindow->handle_action(Action, &mainWindow->gameInfo);

  // Скорость должна измениться на ускоренную
  EXPECT_EQ(mainWindow->gameInfo.speed, 50);
  // Таймер ускорения должен быть активен
  EXPECT_TRUE(mainWindow->boostTimer->isActive());
}

}  // namespace s21