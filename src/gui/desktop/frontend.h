#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>

namespace RenderUtils {
void drawGameField(QPainter& painter, const s21::GameInfo_t& state);
void drawGameUI(QPainter& painter, const s21::GameInfo_t& state);
void drawGameOverlay(QPainter& painter, int pauseState, int width, int height);
void drawAll(QPainter& painter, const s21::GameInfo_t& currentState, int width,
             int height);
}  // namespace RenderUtils

#endif  // FRONTEND_H