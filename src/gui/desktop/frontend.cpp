#include "frontend.h"

namespace RenderUtils {

// Объявляем статические переменные внутри namespace
namespace {
QMap<int, QString> statusMessages = {
    {0, "Play"},
    {1, "Pause (Press Enter to start)"},
    {2, "Game Over"},
};

QFont overlayFont("Arial", 16);
}  // namespace

void drawGameField(QPainter& painter, const s21::GameInfo_t& state) {
  for (int i = 0; i < s21::FIELD_HEIGHT; i++) {
    for (int j = 0; j < s21::FIELD_WIDTH; j++) {
      QRect cell(j * 20, i * 20, 20, 20);

      if (i == 0 || i == s21::FIELD_HEIGHT - 1 || j == 0 ||
          j == s21::FIELD_WIDTH - 1) {
        painter.fillRect(cell, Qt::gray);
      } else if (state.field && state.field[i][j] == 1) {
        painter.fillRect(cell, Qt::darkGreen);
      } else if (state.next && state.next[i][j] > 0) {
        painter.fillRect(cell, Qt::red);
      } else {
        painter.fillRect(cell, Qt::white);
      }
    }
  }
}

void drawGameUI(QPainter& painter, const s21::GameInfo_t& state) {
  painter.setPen(Qt::black);

  painter.drawText(250, 20, "Game status:");
  painter.drawText(250, 40, statusMessages.value(state.pause, "Unknown"));
  painter.drawText(250, 60, "Game level:");
  painter.drawText(250, 80, QString::number(state.level));
  painter.drawText(250, 100, "Game high score:");
  painter.drawText(250, 120, QString::number(state.high_score));
  painter.drawText(250, 140, "Game score:");
  painter.drawText(250, 160, QString::number(state.score));
  painter.drawText(250, 180, "Game speed:");
  painter.drawText(250, 200, QString::number(state.speed));
  painter.drawText(250, 220, "Game pause:");
  painter.drawText(250, 240, QString::number(state.pause));
}

void drawGameOverlay(QPainter& painter, int pauseState, int width, int height) {
  painter.fillRect(0, 0, width, height, QColor(0, 0, 0, 150));
  painter.setPen(Qt::white);
  painter.setFont(overlayFont);

  QString message = (pauseState == 1) ? "PAUSED\nPress Enter to continue"
                                      : "GAME OVER\nPress Enter to restart";

  painter.drawText(QRect(0, 0, width, height), Qt::AlignCenter, message);
}

void drawAll(QPainter& painter, const s21::GameInfo_t& currentState, int width,
             int height) {
  painter.setRenderHint(QPainter::Antialiasing);

  drawGameField(painter, currentState);
  drawGameUI(painter, currentState);

  if (currentState.pause == 1 || currentState.pause == 2) {
    drawGameOverlay(painter, currentState.pause, width, height);
  }
}

}  // namespace RenderUtils