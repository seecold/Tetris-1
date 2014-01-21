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
   tetromino type = O_BLOCK; //MAKE RANDOM
   block.type = type;

   switch(type) {
      case I_BLOCK:
         block.a.x = 4; block.a.y = -3;
         block.b.x = 4; block.b.y = -2;
         block.c.x = 4; block.c.y = -1;
         block.d.x = 4; block.d.y =  0;
         break;
      default:
      case O_BLOCK:
         block.a.x = 4; block.a.y = -1;
         block.b.x = 4; block.b.y = -0;
         block.c.x = 5; block.c.y = -1;
         block.d.x = 5; block.d.y =  0;
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
            print(0, i, " _________________________  __________________________________________________  _________________________");
            break;
         case GRID_END:
            print(0, i, "￣￣￣￣￣￣￣￣￣￣￣￣￣  ￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣  ￣￣￣￣￣￣￣￣￣￣￣￣￣");
            break;
         default:
            print(0, i, "|                         ||                                                  ||                         |");
            break;
      }
   }

   // Score
   print(SCORE_X, SCORE_Y,     ",-. ,-. ,-. ,-. ,-.");
   print(SCORE_X, SCORE_Y + 1, "`-. |   | | |   |-'   ::");
   print(SCORE_X, SCORE_Y + 2, "`-' `-' `-' '   `-'");

   // Hold
   print(HOLD_X, HOLD_Y,     ".       .    .");
   print(HOLD_X, HOLD_Y + 1, "|-. ,-. |  ,-|");
   print(HOLD_X, HOLD_Y + 2, "| | | | |  | |");
   print(HOLD_X, HOLD_Y + 3, "' ' `-' `' `-'");

   // Level
   print(LEVEL_X, LEVEL_Y,     ".               .");
   print(LEVEL_X, LEVEL_Y + 1, "|  ,-. .  , ,-. |");
   print(LEVEL_X, LEVEL_Y + 2, "|  |-' | /  |-' |");
   print(LEVEL_X, LEVEL_Y + 3, "`' `-' `'   `-' `'");

   // Goal
   print(GOAL_X, GOAL_Y,     "           .");
   print(GOAL_X, GOAL_Y + 1, ",-. ,-. ,-. |");
   print(GOAL_X, GOAL_Y + 2, "| | | | ,-| |");
   print(GOAL_X, GOAL_Y + 3, "`-| `-' `-^ `'");
   print(GOAL_X, GOAL_Y + 4, " ,|");
   print(GOAL_X, GOAL_Y + 5, " `'");

   // Next
   print(NEXT_X, NEXT_Y,     ",-. ,-. . , |-");
   print(NEXT_X, NEXT_Y + 1, "| | |-'  X  |");
   print(NEXT_X, NEXT_Y + 2, "' ' `-' ' ` `'");
}

/* Sets text color according to block type */
void color(int type) {
   switch(type) {
      case I_BLOCK: attron(COLOR_PAIR(1)); break;
      case J_BLOCK: attron(COLOR_PAIR(2)); break;
      case L_BLOCK: attron(COLOR_PAIR(3)); break;
      case O_BLOCK: attron(COLOR_PAIR(4)); break;
      case S_BLOCK: attron(COLOR_PAIR(5)); break;
      case Z_BLOCK: attron(COLOR_PAIR(6)); break;
      case T_BLOCK: attron(COLOR_PAIR(7)); break;
      default:      attron(COLOR_PAIR(8)); break;
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

   // Testing colors. Most are fine, but orange isn't supported (and yellow
   // looks orange...) Look into custom colors next.
   init_pair(1, COLOR_BLUE, COLOR_BLACK);     // I
   init_pair(2, COLOR_BLUE, COLOR_BLACK);     // J
   //init_pair(3, COLOR_ORANGE, COLOR_BLACK); // L
   init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // O
   init_pair(5, COLOR_GREEN, COLOR_BLACK);    // S
   init_pair(6, COLOR_RED, COLOR_BLACK);      // Z
   init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // T
   init_pair(8, COLOR_WHITE, COLOR_BLACK);    // Default

   for(int y = 0; y < 20; y++) {
      for(int x = 0; x < 10; x++) {
         grid[x][y] = EMPTY;
      }
   }
   printGrid();
   refresh();
}

/* Debugging? Not really useful anymore... printf works fineish */
void print(int x, int y, const char* message) {
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
      print(X_OFFSET + block.a.x * 5, Y_OFFSET + block.a.y * 2,     "|' '|");
      print(X_OFFSET + block.a.x * 5, Y_OFFSET + block.a.y * 2 + 1, "|_ _|");
   }

   if (block.b.y >= 0) {
      print(X_OFFSET + block.b.x * 5, Y_OFFSET + block.b.y * 2,     "|' '|");
      print(X_OFFSET + block.b.x * 5, Y_OFFSET + block.b.y * 2 + 1, "|_ _|");
   }

   if (block.c.y >= 0) {
      print(X_OFFSET + block.c.x * 5, Y_OFFSET + block.c.y * 2,     "|' '|");
      print(X_OFFSET + block.c.x * 5, Y_OFFSET + block.c.y * 2 + 1, "|_ _|");
   }

   if (block.d.y >= 0) {
      print(X_OFFSET + block.d.x * 5, Y_OFFSET + block.d.y * 2,     "|' '|");
      print(X_OFFSET + block.d.x * 5, Y_OFFSET + block.d.y * 2 + 1, "|_ _|");
   }

   // Print Fallen Blocks
   for(int y = 0; y < 20; y++) {
      for(int x = 0; x < 10; x++) {
         if (grid[x][y] != EMPTY) {
            color(grid[x][y]);
            print(X_OFFSET + x * 5, Y_OFFSET + y * 2,     "|' '|");
            print(X_OFFSET + x * 5, Y_OFFSET + y * 2 + 1, "|_ _|");
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
   if ((block.a.y + y >= 0 && grid[block.a.x + x][block.a.y + y] != EMPTY) ||
       (block.b.y + y >= 0 && grid[block.b.x + x][block.b.y + y] != EMPTY) ||
       (block.c.y + y >= 0 && grid[block.c.x + x][block.c.y + y] != EMPTY) ||
       (block.d.y + y >= 0 && grid[block.d.x + x][block.d.y + y] != EMPTY)) {
      return false;
   }

   return true;
}

/* Locks our current block into the grid */
void setBlock(int type) {
   grid[block.a.x][block.a.y] = type;
   grid[block.b.x][block.b.y] = type;
   grid[block.c.x][block.c.y] = type;
   grid[block.d.x][block.d.y] = type;
}

/* Called from our alarm, decide how a forced decent affects our grid */
void updateGrid() {
   if (canMove(DOWN)) {
      move(DOWN);
   } else {
      setBlock(block.type);
      //Check if a row was completed
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
