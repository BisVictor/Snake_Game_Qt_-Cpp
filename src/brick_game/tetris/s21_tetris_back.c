// добавить sudo apt-get install libncursesw5-dev

#include "tetris.h"

UserAction_t get_signal(int user_input) {
  UserAction_t key_code = Start;
  if (user_input == KEY_DOWN)
    key_code = Down;
  else if (user_input == KEY_LEFT)
    key_code = Left;
  else if (user_input == KEY_RIGHT)
    key_code = Right;
  else if (user_input == ESCAPE_KEY)
    key_code = Terminate;
  else if (user_input == ENTER_KEY)
    key_code = Pause;
  else if (user_input == SPACE_KEY)
    key_code = Action;
  return key_code;
}

// Функция для получения текущего времени в миллисекундах
long long get_time() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (long long)t.tv_sec * 1000 + t.tv_usec / 1000;
}

// Таймер: проверяет, прошло ли определённое время (в миллисекундах)
bool timer(GameInfo_t *gs) {
  long long current_time = get_time();
  if (current_time - gs->time >= gs->speed) {
    gs->time = current_time;
    return true;  // Прошло время delay
  }
  return false;  // Время еще не прошло
}

// Выделение памяти для игрового поля
void allocate_field(GameInfo_t *gs) {
  gs->field = (int **)malloc(FIELD_HEIGHT * sizeof(int *));
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gs->field[i] = (int *)malloc(FIELD_WIDTH * sizeof(int));
  }

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
        gs->field[i][j] = 1;  // Стены
      } else {
        gs->field[i][j] = 0;  // Пустое пространство
      }
    }
  }
}

// Освобождение памяти для игрового поля
void free_field(GameInfo_t *gs) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    free(gs->field[i]);
  }
  free(gs->field);
  gs->field = NULL;  // Устанавливаем в NULL для безопасности
}

struct Figures random_figure(void) {
  struct Figures figures;
  srand(time(NULL));  // Инициализация генератора случайных чисел
  int random_number = rand() % 7 + 1;

  // Очистим массив фигуры перед началом заполнения
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      figures.fig1[i][j] = 0;  // Заполняем 0 (пробелы)
    }
  }

  switch (random_number) {
    case 1:
      // ####
      figures.fig1[0][0] = 1;
      figures.fig1[0][1] = 1;
      figures.fig1[0][2] = 1;
      figures.fig1[0][3] = 1;
      figures.width = 4;
      figures.height = 1;
      break;
    case 2:
      // #
      // ###
      figures.fig1[0][0] = 1;
      figures.fig1[1][0] = 1;
      figures.fig1[1][1] = 1;
      figures.fig1[1][2] = 1;
      figures.width = 3;
      figures.height = 2;
      break;
    case 3:
      //   #
      // ###
      figures.fig1[0][2] = 1;
      figures.fig1[1][0] = 1;
      figures.fig1[1][1] = 1;
      figures.fig1[1][2] = 1;
      figures.width = 3;
      figures.height = 2;
      break;
    case 4:
      // ##
      // ##
      figures.fig1[0][0] = 1;
      figures.fig1[0][1] = 1;
      figures.fig1[1][0] = 1;
      figures.fig1[1][1] = 1;
      figures.width = 2;
      figures.height = 2;
      break;
    case 5:
      //  ##
      // ##
      figures.fig1[0][1] = 1;
      figures.fig1[0][2] = 1;
      figures.fig1[1][0] = 1;
      figures.fig1[1][1] = 1;
      figures.width = 3;
      figures.height = 2;
      break;
    case 6:
      //  #
      // ###
      figures.fig1[0][0] = 1;
      figures.fig1[1][0] = 1;
      figures.fig1[1][1] = 1;
      figures.fig1[2][0] = 1;
      figures.width = 2;
      figures.height = 3;
      break;
    case 7:
      // ##
      //  ##
      figures.fig1[0][0] = 1;
      figures.fig1[0][1] = 1;
      figures.fig1[1][1] = 1;
      figures.fig1[1][2] = 1;
      figures.width = 3;
      figures.height = 2;
      break;
  }

  return figures;  // Возвращаем структуру
}

void rotate_figure(struct Figures *fig) {
  int rotated[FIGURES_HEIGHT][FIGURES_WIDTH];
  int newWidth = fig->height;
  int newHeight = fig->width;
  for (int i = 0; i < fig->height; i++) {
    for (int j = 0; j < fig->width; j++) {
      rotated[j][fig->height - 1 - i] = fig->fig1[i][j];
    }
  }
  for (int i = 0; i < newHeight; i++) {
    for (int j = 0; j < newWidth; j++) {
      fig->fig1[i][j] = rotated[i][j];
    }
  }
  fig->width = newWidth;
  fig->height = newHeight;
}

int collision(struct Figures figures, GameInfo_t gs, int y, int x) {
  int collision_temp = 0;
  for (int i = 0; i < figures.height; i++) {
    for (int j = 0; j < figures.width; j++) {
      if (figures.fig1[i][j] == 1 && gs.field[y + i][x + j] == 1) {
        collision_temp = 1;  // Коллизия
      }
    }
  }
  return collision_temp;  // Нет коллизии
}

int side_collision(struct Figures figures, GameInfo_t gs, int y, int x,
                   int button) {
  int collision_temp = 0;
  for (int i = 0; i < figures.height; i++) {
    for (int j = 0; j < figures.width; j++) {
      if (figures.fig1[i][j] == 1 && gs.field[y + i][x + j + 1] == 1 &&
          x + figures.width != 11 && button == Right) {
        collision_temp = 1;  // Коллизия
      } else if (figures.fig1[i][j] == 1 && gs.field[y + i][x + j - 1] == 1 &&
                 x != 1 && button == Left) {
        collision_temp = 1;  // Коллизия
      }
    }
  }
  return collision_temp;  // Нет коллизии справа
}

void figure_on_field(struct Figures figures, GameInfo_t gs, int y, int x) {
  for (int i = 0; i < figures.height; i++) {
    for (int j = 0; j < figures.width; j++) {
      if (figures.fig1[i][j] == 1) {
        gs.field[y + i][x + j] = figures.fig1[i][j];
      }
    }
  }
}

void clear_from_field(struct Figures figures, GameInfo_t gs, int y, int x) {
  for (int i = 0; i < figures.height; i++) {
    for (int j = 0; j < figures.width; j++) {
      if (figures.fig1[i][j] == 1) {
        gs.field[y + i][x + j] = 0;
      }
    }
  }
}

int count_filled_lines(GameInfo_t gs) {
  int count = 0;
  int line_number = 0;

  for (int i = 1; i < FIELD_HEIGHT - 1; i++) {
    count = 0;
    for (int j = 1; j < (FIELD_WIDTH - 1); j++) {
      if (gs.field[i][j] == 1) {
        count++;
      }
    }

    if (count == FIELD_WIDTH - 2) {
      line_number = i;
    }
  }

  return line_number;  // Возвращаем результат, если найдена заполненная линия
}

void clean_lines(GameInfo_t gs, int line_number) {
  for (int i = line_number; i > 1; i--) {
    for (int j = 1; j < FIELD_WIDTH - 1; j++) {
      gs.field[i][j] = gs.field[i - 1][j];
    }
  }

  for (int j = 1; j < FIELD_WIDTH - 1; j++) {
    gs.field[1][j] = 0;
  }
}

void adding_score(int count_lines, GameInfo_t *gs) {
  switch (count_lines) {
    case 1:
      gs->score += 100;
      break;
    case 2:
      gs->score += 300;
      break;
    case 3:
      gs->score += 700;
      break;
    case 4:
      gs->score += 1500;
      break;
  }
}

void switch_levels(GameInfo_t *gs) {
  gs->level = gs->score / 600;
  switch (gs->level) {
    case 0:
      gs->speed = 360;
      break;
    case 1:
      gs->speed = 360;
      break;
    case 2:
      gs->speed = 340;
      break;
    case 3:
      gs->speed = 320;
      break;
    case 4:
      gs->speed = 300;
      break;
    case 5:
      gs->speed = 270;
      break;
    case 6:
      gs->speed = 250;
      break;
    case 7:
      gs->speed = 220;
      break;
    case 8:
      gs->speed = 200;
      break;
    case 9:
      gs->speed = 150;
      break;
    case 10:
      gs->speed = 100;
      break;
    default:
      gs->speed = 100;
      break;
  }
}

void load_high_score(GameInfo_t *gs) {
  FILE *file = fopen("highscore.txt", "r");

  if (file != NULL) {
    if (fscanf(file, "%d", &(gs->high_score)) != 1) {
      gs->high_score = 0;
    }

    fclose(file);
  } else {
    gs->high_score = 0;
  }
}

void adding_high_score(GameInfo_t *gs) {
  if (gs->score > gs->high_score) {
    gs->high_score = gs->score;

    FILE *file = fopen("highscore.txt", "w");
    if (file == NULL) {
      perror("Error opening highscore file");
      return;
    }

    fprintf(file, "%d\n", gs->high_score);

    fclose(file);
  }
}

void copy_matrix(int height, int width, int **src_matrix, int **dest_matrix) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      dest_matrix[i][j] = src_matrix[i][j];
    }
  }
}

int move_down(struct Figures figures, GameInfo_t gs, int *y, int *x) {
  int count = 0;

  figure_on_field(figures, gs, *y, *x);
  print_field(updateCurrentState(&gs));
  clear_from_field(figures, gs, *y, *x);
  (*y)++;
  count++;
  return count;
}

GameInfo_t updateCurrentState(GameInfo_t *gs) {
  GameInfo_t gi;
  gi.score = gs->score;
  gi.level = gs->level;
  gi.speed = gs->speed;
  gi.high_score = gs->high_score;
  gi.pause = gs->pause;

  // Выделяем память для игрового поля GameInfo_t и копируем данные
  gi.field = (int **)malloc(FIELD_HEIGHT * sizeof(int *));
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    gi.field[i] = (int *)malloc(FIELD_WIDTH * sizeof(int));
    for (int j = 0; j < FIELD_WIDTH; j++) {
      gi.field[i][j] = gs->field[i][j];
    }
  }

  copy_matrix(FIELD_HEIGHT, FIELD_WIDTH, gs->field, gi.field);

  return gi;
}

enum GameState state;

void play_game(GameInfo_t *gs) {
  struct Figures figures = random_figure();
  struct Figures next_figures = random_figure();
  state = PLAY;
  gs->level = 0;
  int user_input = Start;
  nodelay(stdscr, TRUE);
  int count = 0;
  int y = 1;        // Начальная позиция Y
  int x = 5;        // Начальная позиция X
  gs->speed = 400;  // 400 миллисекунд
  int count_lines = 0;
  UserAction_t action = get_signal(user_input);
  load_high_score(gs);

  while (action != Terminate) {
    switch_levels(gs);
    user_input = getch();
    adding_score(count_lines, gs);
    adding_high_score(gs);
    count_lines = 0;
    if (user_input > 0) {
      action = get_signal(user_input);
      shift(user_input, &figures, gs, &y, &x);
      if (action == Down) {
        gs->speed = 1;
      }
    }
    if (action == Pause) {
      state = PAUSE;
      gs->pause = 1;
      pause_action();
      action = Start;
    }
    if (timer(gs)) {  // 1000 миллисекунд = 1 секунда
      if (!collision(figures, *gs, y + 1, x)) {
        move_down(figures, *gs, &y, &x);  // Двигаем фигуру вниз
        print_figures(
            next_figures.fig1);  // печать "следующей" фиг в окне справа
      } else {
        figure_on_field(figures, *gs, y, x);  // Устанавливаем фигуру на поле
        print_field(updateCurrentState(gs));
        figures = next_figures;  // Текущая фигура становится следующей
        next_figures = random_figure();
        y = 1;  // Сбрасываем позицию для новой фигуры
        x = 5;
      }
    }
    while (count_filled_lines(*gs)) {
      clean_lines(*gs, count_filled_lines(*gs));
      count_lines++;
    }
    // print_info(gs);
    if (collision(figures, *gs, y, x)) {
      if (count < 1) {
        state = GAME_OVER;
        gs->pause = 2;
        game_over(gs);
      }
    }
  }
}

int shift(int user_input, struct Figures *figures, GameInfo_t *gs, int *y,
          int *x) {
  int action;
  action = get_signal(user_input);

  if (action == Left) {  // Если нажата кнопка влево
    if (*x < 2) {
      *x = 2;
    }
    if (side_collision(*figures, *gs, *y, *x, Left)) {
      return 0;
    }
    if (*x > 1) {
      (*x)--;
    }
  } else if (action == Right) {  // Если нажата кнопка вправо
    if (*x + figures->width > FIELD_WIDTH - 1) {
      *x = FIELD_WIDTH - 1;
    }
    if (side_collision(*figures, *gs, *y, *x, Right)) {
      return 0;
    }
    if (*x + figures->width < FIELD_WIDTH - 1) {  // Проверка на границы
      (*x)++;
    }
  } else if (action == Action)  // Space
  {
    if (!collision(*figures, *gs, *y, *x)) {
      rotate_figure(figures);
    } else {
      return 0;
    }

    while ((*x + figures->width > FIELD_WIDTH - 1)) {
      (*x)--;
    }
    while (collision(*figures, *gs, *y, *x)) {
      (*x)--;
    }
  }
  return 0;
}
