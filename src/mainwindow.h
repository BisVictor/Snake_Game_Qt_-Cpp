#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>

// Предварительное объявление тестового класса
class MainWindowTest;

namespace s21 {

constexpr int FIELD_WIDTH = 10;
constexpr int FIELD_HEIGHT = 20;
constexpr int QT_KEY_UP = Qt::Key_Up;
constexpr int QT_KEY_DOWN = Qt::Key_Down;
constexpr int QT_KEY_LEFT = Qt::Key_Left;
constexpr int QT_KEY_RIGHT = Qt::Key_Right;
constexpr int QT_ENTER_KEY = Qt::Key_Return;
constexpr int QT_SPACE_KEY = Qt::Key_Space;
constexpr int QT_ESCAPE_KEY = Qt::Key_Escape;

enum UserAction_t { Up, Down, Left, Right, Terminate, Pause, Action, None };
enum GameState { MENU, PLAY, PAUSE, GAME_OVER, EXIT };

struct GameInfo_t {
  int** field = nullptr;
  int** snake = nullptr;
  int** next = nullptr;
  int* tail_x = nullptr;
  int* tail_y = nullptr;
  int x = 0;
  int y = 0;
  int n_tail = 0;
  int pause = 0;
  int level = 1;
  int high_score = 0;
  int score = 0;
  int speed = 400;
  int key = 0;
  int prev_key = 0;
  bool first_run = true;
};

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  friend class MainWindowTest;
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  // private:
  QTimer* boostTimer;  // Таймер для временного ускорения
  int normalSpeed;     // Для хранения обычной скорости
  QTimer* gameTimer;
  GameInfo_t currentState;
  GameInfo_t gameInfo;
  GameState state;
  int apple_height = 0;
  int apple_width = 0;

  static void allocate_memory_for_field(GameInfo_t* gameInfo);
  static void allocate_memory_for_next(GameInfo_t* gameInfo);
  static void allocate_memory_for_snake(GameInfo_t* gameInfo);
  static void free_memory_field(GameInfo_t* gameInfo);
  static void free_memory_next(GameInfo_t* gameInfo);
  static void free_memory_snake(GameInfo_t* gameInfo);
  static void free_tail_arrays(GameInfo_t* gameInfo);
  static void filling_playing_field(GameInfo_t* gameInfo);

  void initialization();
  void reset();
  void generate_apple_in_field(GameInfo_t* gameInfo, int* apple_height,
                               int* apple_width);
  bool fix_buttons(UserAction_t newDir, UserAction_t prevDir);
  int get_key_code(UserAction_t action);
  UserAction_t get_signal(int user_input);
  void handle_action(UserAction_t action, GameInfo_t* gameInfo);
  int collision_check(const GameInfo_t& gameInfo);
  void snake_movement(GameInfo_t* gameInfo);
  void clear_apple(GameInfo_t* gameInfo);
  GameInfo_t updateCurrentState(const GameInfo_t& gameInfo);
  void actual_level(GameInfo_t* gameInfo);
  int load_high_score(const QString& filename);
  void save_high_score(const QString& filename, int score);
  void initTimers();

  void game_state_menu();
  void game_state_pause();
  void game_state_play();
  void game_state_game_over();
  void updateGame();

  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

  void drawGameField(QPainter& painter, const GameInfo_t& state);
  void drawGameUI(QPainter& painter, const GameInfo_t& state);
  void drawGameOverlay(QPainter& painter, const GameState& state);
};
}  // namespace s21

#endif  // MAINWINDOW_H
