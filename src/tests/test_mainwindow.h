#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <gtest/gtest.h>

class MainWindow;  // Предварительное объявление

class MainWindowTest : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  MainWindow* mainWindow;
};

#endif  // TEST_MAINWINDOW_H