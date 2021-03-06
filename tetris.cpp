#include "tetris.h"

int main(void) {
   initGame();
   mainLoop();
   closeGame();
   return EXIT_SUCCESS;
}

void mainLoop() {
   int i = 25;
   int ch = getch();
   struct itimerval tout_val;

   // Our game clock. This alarm is synonymous to game speed.
   tout_val.it_value.tv_sec = 0;
   tout_val.it_value.tv_usec = 250000;
   tout_val.it_interval.tv_sec = 0;
   tout_val.it_interval.tv_usec = 250000;
   setitimer(ITIMER_REAL, &tout_val, 0);
   signal(SIGALRM, alarm_wakeup);

   while(ch != 'x') {
      // Handle arrow keys first
      if (ch == '\033') {
         getch(); // Get rid of slash
         switch (getch()) {
            case 'A': rotate();    redraw(); break; // Up
            case 'B': move(DOWN);  redraw(); break; // Down
            case 'C': move(RIGHT); redraw(); break; // Right
            case 'D': move(LEFT);  redraw(); break; // Left
         }
      } else {
         switch(ch) {
           // For now, this is the 'start'
            case 'r' :
               refreshBlock();
               redraw();
               break;
            case ' ':
               space();
               break;
            case 'l':
               break;
            case 'e':
               emptyGrid();
               redraw();
               break;
         }
      }
      ch = getch();
   }
}

Block calculateIdeal(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
   Block ideal = block;
   ideal.a.x += ax; ideal.a.y += ay;
   ideal.b.x += bx; ideal.b.y += by;
   ideal.c.x += cx; ideal.c.y += cy;
   ideal.d.x += dx; ideal.d.y += dy;
   //printf("Ideal: (%d, %d), (%d, %d), (%d, %d), (%d, %d)", 
   //ideal.a.x, ideal.a.y, ideal.b.x, ideal.b.y, ideal.c.x, ideal.c.y, ideal.d.x, ideal.d.y);
   return ideal;
}

bool fits(Block test) {
   if (test.a.x < 0  || test.b.x < 0  || test.c.x < 0  || test.d.x < 0 ||
       test.a.x > 9  || test.b.x > 9  || test.c.x > 9  || test.d.x > 9 ||
       test.a.y < 0  || test.b.y < 0  || test.c.y < 0  || test.d.y < 0 ||
       test.a.y > 19 || test.b.y > 19 || test.c.y > 19 || test.d.y > 19) {
      return false;
   }
   return true;
}

bool isAvailable(Block test) {
   if (grid[test.a.y][test.a.x] != EMPTY ||
       grid[test.b.y][test.b.x] != EMPTY ||
       grid[test.c.y][test.c.x] != EMPTY ||
       grid[test.d.y][test.d.x] != EMPTY) {
      return false;
   }
   return true;
}

Block shift(Block test, int y, int x) {
   test.a.y += y; test.b.y += y; test.c.y += y; test.d.y += y;
   test.a.x += x; test.b.x += x; test.c.x += x; test.d.x += x;
   return test;
}

/* Rotation (clockwise):
 * To make rotations more lenient (read: fun), allow several positions.
 * Define what your ideal position is, and how far it can stray from that.
 */
void rotate() {
   int range[5] = {0, -1, 1, -2, 2};
   std::vector<int> xAlt (range, range + 5);
   std::vector<int> yAlt (range, range + 5);
   Block test, ideal;

   switch(block.type) {
      case I_BLOCK:
         switch(block.state) {
            case 0: // 0 -> 90        ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(2,  2,  1,  1,  0,  0,  -1, -1);
               break;
            case 90: // 90 -> 180     ax, ay, bx, by, cx, cy, dx, dy 
               ideal = calculateIdeal(-1, 1,  0,  0,  1,  -1, 2, -2);
               break;
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy 
               ideal = calculateIdeal(-2, -2, -1, -1, 0,  0,  1,  1);
               break;
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy 
               ideal = calculateIdeal(1, -1,  0,  0,  -1,  1, -2, 2);
               break;
         } break;
      case L_BLOCK:
         switch(block.state) {
            case 0: // 0 -> 90        ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  1,  0,  0,  -1, -1, -2,  0);
               break;
            case 90: // 90 -> 180     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, 1,  0,  0,  1,  -1, 0,  -2);
               break;
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, -1, 0,  0,  1,  1,  2,  0);
               break;
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1, -1,  0,  0,  -1,  1, 0,  2);
               break;
         } break;
      case J_BLOCK:
         switch(block.state) {
            case 0: // 0 -> 90        ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  1,  0,  0,  -1, -1, 0,  -2);
               break;
            case 90: // 90 -> 180     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, 1,  0,  0,  1,  -1, 2,  0);
               break;
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, -1, 0,  0,  1,  1,  0,  2);
               break;
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1, -1,  0,  0,  -1,  1, -2,  0);
               break;
         } break;
      case T_BLOCK:
         switch(block.state) {
            case 0: // 0 -> 90        ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  -1, 0,  0,  -1, 1,  -1, -1);
               break;
            case 90: // 90 -> 180     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  1,  0,  0,  -1, -1, 1,  -1);
               break;
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, 1,  0,  0,  1,  -1, 1,  1);
               break;
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, -1, 0,  0,  1,  1, -1,  1);
               break;
         } break;
      case Z_BLOCK:
         switch(block.state) {
            case 0:   // 0 -> 90
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  0,  0,  1,  -1, 0,  -2, 1);
               break;
            case 90:  // 90 -> 180
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, 0,  0,  -1,  1, 0,  2,  -1);
               break;
         } break;
      case S_BLOCK:
         switch(block.state) {
            case 0:   // 0 -> 90
            case 180: // 180 -> 270   ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(-1, 2,  0,  1,  -1, 0,  0, -1);
               break;
            case 90:  // 90 -> 180
            case 270: // 270 -> 0     ax, ay, bx, by, cx, cy, dx, dy
               ideal = calculateIdeal(1,  -2, 0,  -1,  1, 0,  0,  1);
               break;
         } break;
      case O_BLOCK:
      default: break;
   }

   for (int x : xAlt) {
      for (int y : yAlt) {
         test = shift(ideal, y, x);
         if (fits(test) && isAvailable(test)) {
            block = test;
            block.state = (block.state + 90) % 360;
            return;
         }
      }
   }
}

/* Moves tetromino if the action is allowed */
bool move(direction dir) {
   if (canMove(dir)) {
      switch(dir) {
         case LEFT:  block.a.x--; block.b.x--; block.c.x--; block.d.x--; break;
         case RIGHT: block.a.x++; block.b.x++; block.c.x++; block.d.x++; break;
         case DOWN:  block.a.y++; block.b.y++; block.c.y++; block.d.y++; break;
      }
      return true;
   }
   return false;
}

/* Locks the current block in place and makes a new one
   The logic is currently split up / not implemented. Deal with it. */
void refreshBlock() {
   int type = pseudoRandom();
   block.type = (tetromino)type;
   block.state = 0;

   switch(type) {
      case I_BLOCK:
         block.a.x = 4; block.a.y = -3;
         block.b.x = 4; block.b.y = -2;
         block.c.x = 4; block.c.y = -1;
         block.d.x = 4; block.d.y =  0;
         break;
      case J_BLOCK:
         block.a.x = 5; block.a.y = -2;
         block.b.x = 5; block.b.y = -1;
         block.c.x = 5; block.c.y =  0;
         block.d.x = 4; block.d.y =  0;
         break;
      case L_BLOCK:
         block.a.x = 4; block.a.y = -2;
         block.b.x = 4; block.b.y = -1;
         block.c.x = 4; block.c.y =  0;
         block.d.x = 5; block.d.y =  0;
         break;
      case O_BLOCK:
         block.a.x = 4; block.a.y = -1;
         block.b.x = 4; block.b.y = -0;
         block.c.x = 5; block.c.y = -1;
         block.d.x = 5; block.d.y =  0;
         break;
      case S_BLOCK:
         block.a.x = 5; block.a.y = -1;
         block.b.x = 4; block.b.y = -1;
         block.c.x = 4; block.c.y =  0;
         block.d.x = 3; block.d.y =  0;
         break;
      case Z_BLOCK:
         block.a.x = 3; block.a.y = -1;
         block.b.x = 4; block.b.y = -1;
         block.c.x = 4; block.c.y =  0;
         block.d.x = 5; block.d.y =  0;
         break;
      case T_BLOCK:
         block.a.x = 3; block.a.y = -1;
         block.b.x = 4; block.b.y = -1;
         block.c.x = 5; block.c.y = -1;
         block.d.x = 4; block.d.y =  0;
         break;
      default:
         break;
   }
   // Update Next Tetrominos...
}

// Refreshes the grid
void printGrid() {
   // Empty Grid
   color(DEFAULT);
   for(int i = GRID_START; i <= GRID_END; i++) {
      switch(i) {
         case GRID_START:
            print(i, 0, " _________________________  __________________________________________________  _________________________");
            break;
         case GRID_END:
            print(i, 0, "￣￣￣￣￣￣￣￣￣￣￣￣￣  ￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣  ￣￣￣￣￣￣￣￣￣￣￣￣￣");
            break;
         default:
            print(i, 0, "|                         ||                                                  ||                         |");
            break;
      }
   }

   // Score
   print(SCORE_Y,     SCORE_X, ",-. ,-. ,-. ,-. ,-.");
   print(SCORE_Y + 1, SCORE_X, "`-. |   | | |   |-'   ::");
   print(SCORE_Y + 2, SCORE_X, "`-' `-' `-' '   `-'");

   // Hold
   print(HOLD_Y,     HOLD_X, ".       .    .");
   print(HOLD_Y + 1, HOLD_X, "|-. ,-. |  ,-|");
   print(HOLD_Y + 2, HOLD_X, "| | | | |  | |");
   print(HOLD_Y + 3, HOLD_X, "' ' `-' `' `-'");

   // Level
   print(LEVEL_Y,     LEVEL_X, ".               .");
   print(LEVEL_Y + 1, LEVEL_X, "|  ,-. .  , ,-. |");
   print(LEVEL_Y + 2, LEVEL_X, "|  |-' | /  |-' |");
   print(LEVEL_Y + 3, LEVEL_X, "`' `-' `'   `-' `'");

   // Goal
   print(GOAL_Y,     GOAL_X, "           .");
   print(GOAL_Y + 1, GOAL_X, ",-. ,-. ,-. |");
   print(GOAL_Y + 2, GOAL_X, "| | | | ,-| |");
   print(GOAL_Y + 3, GOAL_X, "`-| `-' `-^ `'");
   print(GOAL_Y + 4, GOAL_X, " ,|");
   print(GOAL_Y + 5, GOAL_X, " `'");

   // Next
   print(NEXT_Y,     NEXT_X, ",-. ,-. . , |-");
   print(NEXT_Y + 1, NEXT_X, "| | |-'  X  |");
   print(NEXT_Y + 2, NEXT_X, "' ' `-' ' ` `'");
}

/* Sets text color according to block type */
void color(int type) {
   attroff(A_BOLD);
   switch(type) {
      case I_BLOCK: attron(COLOR_PAIR(1)); break;
      case J_BLOCK: attron(COLOR_PAIR(2)); break;
      case L_BLOCK: attron(COLOR_PAIR(3)); break;
      case O_BLOCK: attron(COLOR_PAIR(4)); attron(A_BOLD); break;
      case S_BLOCK: attron(COLOR_PAIR(5)); break;
      case Z_BLOCK: attron(COLOR_PAIR(6)); break;
      case T_BLOCK: attron(COLOR_PAIR(7)); break;
      default:      attron(COLOR_PAIR(8)); attron(A_BOLD); break;
   }
}

// Initializes data we need for a new round
void initGame() {
   int key = 0;

   if ((mainwin = initscr()) == NULL) {
      fprintf(stderr, "Error initialising ncurses.\n");
      exit(EXIT_FAILURE);
   }
   curs_set(0);
   raw();
   noecho();
   start_color();

   init_color(COLOR_BLACK, 0, 0, 0);
   init_color(COLOR_CYAN, 204, 204, 1000); // BLUE... actually
   setColors(false);

   srand (time(NULL));
   emptyGrid();
   for(int i = 0; i < 7; i++) {
      weights[i] = 1;
   }
   printGrid();
   refresh();
}

void emptyGrid() {
   for(int y = 0; y < 20; y++) {
      for(int x = 0; x < 10; x++) {
         grid[y][x] = EMPTY;
      }
   }
}

void space() {
   while(move(DOWN)) { }
   updateGrid();
   redraw();
}

// This should be fine-tuned. Okay for now.
int pseudoRandom() {
   std::vector<int> probabilities;
   std::vector<int>::iterator it;

   for (int i = 0; i < 7; i++) {
      it = probabilities.begin();
      probabilities.insert(it, weights[i], i + 1);
   }

   int result = probabilities[rand() % probabilities.size()];
   for (int i = 0; i < 7; i++) {
      if (i == result - 1) {
         weights[i] = 1;
      } else {
         weights[i] *= 2;
      }
   } 

   return result;
}

void printWeights() {
   for (int i = 0; i < 7; i++) {
      printf("%d, ", weights[i]);
   }
   printf("\n");
}

void clearLines() {
   int linesCleared = 0;

   for(int y = 0; y < 20; y++) {
      for(int x = 0; x < 10; x++) {
         if (grid[y][x] == EMPTY) {
            break;
         } else if (x == 9) {
            linesCleared++;
            for (int y2 = y; y2 > 0; y2--) {
               std::copy(grid[y2 - 1], grid[y2 - 1] + 10, grid[y2]);
            }
         }
      }
   }
}

void setColors(bool full) {
   if (full) {
      init_pair(1, COLOR_BLUE,    COLOR_BLUE);    // I - THis is cyan
      init_pair(2, COLOR_YELLOW,  COLOR_YELLOW);  // J - This is orange
      init_pair(3, COLOR_CYAN,    COLOR_CYAN);    // L - This is blue
      init_pair(4, COLOR_YELLOW,  COLOR_YELLOW);  // O
      init_pair(5, COLOR_GREEN,   COLOR_GREEN);   // S
      init_pair(6, COLOR_RED,     COLOR_RED);     // Z
      init_pair(7, COLOR_MAGENTA, COLOR_MAGENTA); // T
      init_pair(8, COLOR_WHITE,   COLOR_BLACK);   // Default
   } else {
      init_pair(1, COLOR_BLUE,    COLOR_BLACK);  // I
      init_pair(2, COLOR_YELLOW,  COLOR_BLACK);  // J
      init_pair(3, COLOR_CYAN,    COLOR_BLACK);  // L
      init_pair(4, COLOR_YELLOW,  COLOR_BLACK);  // O
      init_pair(5, COLOR_GREEN,   COLOR_BLACK);  // S
      init_pair(6, COLOR_RED,     COLOR_BLACK);  // Z
      init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // T
      init_pair(8, COLOR_WHITE,   COLOR_BLACK);  // Default
   }
}


/* Debugging? Not really useful anymore... printf works fineish */
void print(int y, int x, const char* message) {
   mvaddstr(y, x, message);
}

/* Supposed to do cleanup... */
void closeGame() {
   // Free ?
   delwin(mainwin);
   endwin();
   refresh();
}


/* Prints all blocks currently locked on our Grid + the active piece */
void printBlocks() {
   // Print Falling Block
   color(block.type);
   if (block.a.y >= 0) {
      print(Y_OFFSET + block.a.y * 2,     X_OFFSET + block.a.x * 5, "|' '|");
      print(Y_OFFSET + block.a.y * 2 + 1, X_OFFSET + block.a.x * 5, "|_a_|");
   }

   if (block.b.y >= 0) {
      print(Y_OFFSET + block.b.y * 2,     X_OFFSET + block.b.x * 5, "|' '|");
      print(Y_OFFSET + block.b.y * 2 + 1, X_OFFSET + block.b.x * 5, "|_b_|");
   }

   if (block.c.y >= 0) {
      print(Y_OFFSET + block.c.y * 2,     X_OFFSET + block.c.x * 5, "|' '|");
      print(Y_OFFSET + block.c.y * 2 + 1, X_OFFSET + block.c.x * 5, "|_c_|");
   }

   if (block.d.y >= 0) {
      print(Y_OFFSET + block.d.y * 2,     X_OFFSET + block.d.x * 5, "|' '|");
      print(Y_OFFSET + block.d.y * 2 + 1, X_OFFSET + block.d.x * 5, "|_d_|");
   }

   // Print Fallen Blocks
   for(int y = 0; y < 20; y++) {
      for(int x = 0; x < 10; x++) {
         if (grid[y][x] != EMPTY) {
            color(grid[y][x]);
            print(Y_OFFSET + y * 2,     X_OFFSET + x * 5, "|' '|");
            print(Y_OFFSET + y * 2 + 1, X_OFFSET + x * 5, "|_ _|");
         }
      }
   }
}

/* Check whether the current piece can be moved */
bool canMove(direction dir) {
   int y = 0, x = 0;
   switch(dir) {
      case DOWN : y =  1; break;
      case LEFT : x = -1; break;
      case RIGHT: x =  1; break;
   }

   // Check y boundaries
   if (block.a.y + y == 20 ||
       block.b.y + y == 20 ||
       block.c.y + y == 20 ||
       block.d.y + y == 20) {
      return false;
   }

   // Check x boundaries
   if (block.a.x + x < 0 || block.a.x + x > 9 ||
       block.b.x + x < 0 || block.b.x + x > 9 ||
       block.c.x + x < 0 || block.c.x + x > 9 ||
       block.d.x + x < 0 || block.d.x + x > 9) {
      return false;
   }

   // Check grid for obstacles
   if ((block.a.y + y >= 0 && grid[block.a.y + y][block.a.x + x] != EMPTY) ||
       (block.b.y + y >= 0 && grid[block.b.y + y][block.b.x + x] != EMPTY) ||
       (block.c.y + y >= 0 && grid[block.c.y + y][block.c.x + x] != EMPTY) ||
       (block.d.y + y >= 0 && grid[block.d.y + y][block.d.x + x] != EMPTY)) {
      return false;
   }

   return true;
}

/* Locks our current block into the grid */
void setBlock(int type) {
   grid[block.a.y][block.a.x] = type;
   grid[block.b.y][block.b.x] = type;
   grid[block.c.y][block.c.x] = type;
   grid[block.d.y][block.d.x] = type;
}

/* Called from our alarm, decide how a forced decent affects our grid */
void updateGrid() {
   if (!move(DOWN)) {
      setBlock(block.type);
      clearLines();
      refreshBlock();
   }
}

void redraw() {
   printGrid();
   printBlocks();
   refresh();
}

void alarm_wakeup (int i) {
   // Might be useful when changing speeds
   //struct itimerval tout_val;
   //tout_val.it_value.tv_sec = 0;
   //tout_val.it_value.tv_usec = 500000;
   //tout_val.it_interval.tv_sec = 0;
   //tout_val.it_interval.tv_usec = 500000;

   printGrid();
   updateGrid();
   printBlocks();
   //printWeights();
   refresh();

   //setitimer(ITIMER_REAL, &tout_val,0);
}
