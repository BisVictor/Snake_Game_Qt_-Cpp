#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <gtest/gtest.h>

namespace s21 {
class MainWindow;  // Предварительное объявление

class MainWindowTest : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  s21::MainWindow* mainWindow;
};
}  // namespace s21
#endif  // TEST_MAINWINDOW_H