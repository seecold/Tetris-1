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
   tout_val.it_value.tv_sec = 1;
   tout_val.it_value.tv_usec = 0;
   tout_val.it_interval.tv_sec = 1;
   tout_val.it_interval.tv_usec = 0;
   setitimer(ITIMER_REAL, &tout_val, 0);
   signal(SIGALRM, alarm_wakeup);

   while(ch != 'x') {
      // Handle arrow keys first
      if (ch == '\033') {
         getch(); // Get rid of slash
         switch (getch()) {
            case 'A': break;                        // Up
            case 'B': move(DOWN);  redraw(); break; // Down
            case 'C': move(RIGHT); redraw(); break; // Right
            case 'D': move(LEFT);  redraw(); break; // Left
         }
      } else {
         switch(ch) {
           // For now, this is the 'start'
            case 'r' :
               refreshBlock();
               break;
            case 'l':
               break;
            case 'e':
               emptyGrid();
               break;
         }
      }
      ch = getch();
   }
}

/* Moves tetromino if the action is allowed */
void move(direction dir) {
   if (canMove(dir)) {
      switch(dir) {
         case LEFT:  block.a.x--; block.b.x--; block.c.x--; block.d.x--; break;
         case RIGHT: block.a.x++; block.b.x++; block.c.x++; block.d.x++; break;
         case DOWN:  block.a.y++; block.b.y++; block.c.y++; block.d.y++; break;
      }
   }
}

/* Locks the current block in place and makes a new one
   The logic is currently split up / not implemented. Deal with it. */
void refreshBlock() {
   // Random is too random. Stack the odds towards uniformity
   //int type = (rand() % 7) + 1;
   int type = O_BLOCK;
   block.type = (tetromino)type;

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
      print(Y_OFFSET + block.a.y * 2 + 1, X_OFFSET + block.a.x * 5, "|_ _|");
   }

   if (block.b.y >= 0) {
      print(Y_OFFSET + block.b.y * 2,     X_OFFSET + block.b.x * 5, "|' '|");
      print(Y_OFFSET + block.b.y * 2 + 1, X_OFFSET + block.b.x * 5, "|_ _|");
   }

   if (block.c.y >= 0) {
      print(Y_OFFSET + block.c.y * 2,     X_OFFSET + block.c.x * 5, "|' '|");
      print(Y_OFFSET + block.c.y * 2 + 1, X_OFFSET + block.c.x * 5, "|_ _|");
   }

   if (block.d.y >= 0) {
      print(Y_OFFSET + block.d.y * 2,     X_OFFSET + block.d.x * 5, "|' '|");
      print(Y_OFFSET + block.d.y * 2 + 1, X_OFFSET + block.d.x * 5, "|_ _|");
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
   if (canMove(DOWN)) {
      move(DOWN);
   } else {
      setBlock(block.type);
      //Check if a row was completed
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
   refresh();

   //setitimer(ITIMER_REAL, &tout_val,0);
}
