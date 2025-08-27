#include "tetris.h"

int main() {
  GameInfo_t gs = {NULL, NULL, 0, 0, 0, 0, 0, 5, 1, get_time()};

  // Инициализация поля
  allocate_field(&gs);

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  setlocale(LC_ALL, "");
  curs_set(0);
  gs.score = 0;
  gs.high_score = 0;

  play_game(&gs);
  getch();

  endwin();

  free_field(&gs);
  return 0;
}