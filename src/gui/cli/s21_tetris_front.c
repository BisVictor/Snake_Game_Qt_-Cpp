#include "tetris.h"

void print_field(GameInfo_t gs) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      int col = j * 2;

      if (i == 0 && j == 0) {
        mvaddch(i, col, ACS_ULCORNER);      // Верхний левый угол
        mvaddch(i, col + 1, ACS_ULCORNER);  // Дублируем символ
      } else if (i == 0 && j == FIELD_WIDTH - 1) {
        mvaddch(i, col, ACS_URCORNER);      // Верхний правый угол
        mvaddch(i, col + 1, ACS_URCORNER);  // Дублируем символ
      } else if (i == FIELD_HEIGHT - 1 && j == 0) {
        mvaddch(i, col, ACS_LLCORNER);      // Нижний левый угол
        mvaddch(i, col + 1, ACS_LLCORNER);  // Дублируем символ
      } else if (i == FIELD_HEIGHT - 1 && j == FIELD_WIDTH - 1) {
        mvaddch(i, col, ACS_LRCORNER);      // Нижний правый угол
        mvaddch(i, col + 1, ACS_LRCORNER);  // Дублируем символ
      } else if (i == 0 || i == FIELD_HEIGHT - 1) {
        mvaddch(i, col, ACS_HLINE);      // Горизонтальные линии (верх и низ)
        mvaddch(i, col + 1, ACS_HLINE);  // Дублируем символ
      } else if (j == 0 || j == FIELD_WIDTH - 1) {
        mvaddch(i, col, ACS_VLINE);      // Вертикальные линии (лево и право)
        mvaddch(i, col + 1, ACS_VLINE);  // Дублируем символ
      } else if (gs.field[i][j] == 1) {
        mvaddch(i, col, ACS_CKBOARD);      // Заполненные блоки
        mvaddch(i, col + 1, ACS_CKBOARD);  // Дублируем символ
      } else {
        mvaddch(i, col, ' ');      // Пустые клетки
        mvaddch(i, col + 1, ' ');  // Дублируем символ
      }
    }
  }
  mvprintw(1, 30, "Game high score");
  mvprintw(2, 30, "%d", gs.high_score);
  refresh();
  mvprintw(5, 30, "             ");
  mvprintw(4, 30, "Game score");
  mvprintw(5, 30, "%d", gs.score);
  mvprintw(7, 30, "Game level");
  mvprintw(8, 30, "%d", gs.level);
  refresh();  // Обновляем экран
}

void print_figures(int figures[FIGURES_HEIGHT][FIGURES_WIDTH]) {
  for (int i = 0; i < FIGURES_HEIGHT; i++) {
    for (int j = 0; j < FIGURES_WIDTH; j++) {
      // Печатаем символы дважды для удвоенной ширины
      int col = j * 2;
      if (figures[i][j] == 1) {
        mvaddch(i + 11, col + 29, ACS_CKBOARD);  // Заполненные блоки
        mvaddch(i + 11, col + 30, ACS_CKBOARD);  // Дублируем символ
      } else {
        mvaddch(i + 11, col + 29, ' ');  // Пустые клетки
        mvaddch(i + 11, col + 30, ' ');  // Дублируем символ
      }
    }
  }
  refresh();  // Обновляем экран
}

int pause_action() {
  mvprintw(10, 5, "Game paused. Press ENTER to resume.");
  refresh();
  nodelay(stdscr, FALSE);

  int user_input;
  while ((user_input = getch()) != '\n') {
  }
  nodelay(stdscr, TRUE);
  mvprintw(10, 5, "                                    ");
  refresh();
  return 0;
}

int game_over(GameInfo_t *gs) {
  mvprintw(10, 5, "Game over. Press ENTER to restart.");
  refresh();
  nodelay(stdscr, FALSE);

  int user_input;
  while ((user_input = getch()) != '\n') {
  }
  nodelay(stdscr, TRUE);
  mvprintw(10, 5, "                                    ");
  refresh();
  free_field(gs);
  allocate_field(gs);
  gs->score = 0;
  gs->level = 0;
  play_game(gs);
  return 0;
}
