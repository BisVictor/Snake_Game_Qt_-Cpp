#ifndef TETRIS_H
#define TETRIS_H

#include <locale.h>  //setlocale(LC_ALL, "");
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>    // time(NULL)
#include <unistd.h>  // sleep()

// Определения для клавиш
#define ESCAPE_KEY 27
#define ENTER_KEY 10
#define SPACE_KEY 32

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 12

#define FIGURES_HEIGHT 4
#define FIGURES_WIDTH 5

struct Figures {
  int fig1[FIGURES_HEIGHT][FIGURES_WIDTH];
  int width;   // Ширина фигуры
  int height;  // Высота фигуры
};

// Перечисление возможных действий
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  int pos_x;
  int pos_y;
  long long time;
} GameInfo_t;

enum GameState { MENU, PLAY, PAUSE, GAME_OVER, EXIT };

UserAction_t get_signal(int user_input);
long long get_time();
bool timer(GameInfo_t *gs);
void allocate_field(GameInfo_t *gs);
// void allocate_field(** int field) ;
void free_field(GameInfo_t *gs);
void print_figures(int figures[FIGURES_HEIGHT][FIGURES_WIDTH]);
struct Figures random_figure(void);
void rotate_figure(struct Figures *fig);
int collision(struct Figures figures, GameInfo_t gs, int y, int x);
int side_collision(struct Figures figures, GameInfo_t gs, int y, int x,
                   int button);
void figure_on_field(struct Figures figures, GameInfo_t gs, int y, int x);
void clear_from_field(struct Figures figures, GameInfo_t gs, int y, int x);
int shift(int user_input, struct Figures *figures, GameInfo_t *gs, int *y,
          int *x);
int count_filled_lines(GameInfo_t gs);
void clean_lines(GameInfo_t gs, int line_number);
int move_down(struct Figures figures, GameInfo_t gs, int *y, int *x);
void adding_score(int count_lines, GameInfo_t *gs);
void switch_levels(GameInfo_t *gs);
int pause_action();
int game_over(GameInfo_t *gs);
void load_high_score(GameInfo_t *gs);
void adding_high_score(GameInfo_t *gs);
GameInfo_t updateCurrentState(GameInfo_t *gs);
void play_game(GameInfo_t *gs);
void copy_matrix(int height, int width, int **src_matrix, int **dest_matrix);

void print_field(GameInfo_t gs);
void print_info(GameInfo_t *gs);

#endif
