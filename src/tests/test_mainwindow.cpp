#include "test_mainwindow.h"

#include <QKeyEvent>  // Добавлено для QKeyEvent

#include "../mainwindow.h"

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