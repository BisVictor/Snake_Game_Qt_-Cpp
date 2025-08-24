#include <QApplication>

#include "gui/desktop/frontend.h"
#include "mainwindow.h"

using namespace s21;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Инициализация генератора случайных чисел
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  MainWindow window;
  window.show();

  return app.exec();
}
