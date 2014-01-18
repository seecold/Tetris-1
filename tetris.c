#include "tetris.h"

int main(void) {
   WINDOW * mainwin;
   /*  Initialize ncurses  */
   if ((mainwin = initscr()) == NULL) {
      fprintf(stderr, "Error initialising ncurses.\n");
      exit(EXIT_FAILURE);
   }


   printGrid();
   refresh();
   sleep(30);

   delwin(mainwin);
   endwin();
   refresh();

   return EXIT_SUCCESS;
}

void printGrid() {
   int i;

   // Empty Grid
   for(i = GRID_START; i <= GRID_HEIGHT; i++) {
      switch(i) {
         case GRID_START:
            mvaddstr(i, 0, " _________________________  __________________________________________________  _________________________");
            break;
         case GRID_HEIGHT:
            mvaddstr(i, 0, "|_________________________||__________________________________________________||_________________________|");
            break;
         default:
            mvaddstr(i, 0, "|                         ||                                                  ||                         |");
            break;
      }
   }

   // Score
   mvaddstr(SCORE_X,     SCORE_Y, ",-. ,-. ,-. ,-. ,-.");
   mvaddstr(SCORE_X + 1, SCORE_Y, "`-. |   | | |   |-'   ::");
   mvaddstr(SCORE_X + 2, SCORE_Y, "`-' `-' `-' '   `-'");

   // Hold
   mvaddstr(HOLD_X,     HOLD_Y, ".       .    .");
   mvaddstr(HOLD_X + 1, HOLD_Y, "|-. ,-. |  ,-|");
   mvaddstr(HOLD_X + 2, HOLD_Y, "| | | | |  | |");
   mvaddstr(HOLD_X + 3, HOLD_Y, "' ' `-' `' `-'");

   // Level
   mvaddstr(LEVEL_X,     HOLD_Y, ".               .");
   mvaddstr(LEVEL_X + 1, HOLD_Y, "|  ,-. .  , ,-. |");
   mvaddstr(LEVEL_X + 2, HOLD_Y, "|  |-' | /  |-' |");
   mvaddstr(LEVEL_X + 3, HOLD_Y, "`' `-' `'   `-' `'");

   // Goal
   mvaddstr(GOAL_X,     GOAL_Y, "           .");
   mvaddstr(GOAL_X + 1, GOAL_Y, ",-. ,-. ,-. |");
   mvaddstr(GOAL_X + 2, GOAL_Y, "| | | | ,-| |");
   mvaddstr(GOAL_X + 3, GOAL_Y, "`-| `-' `-^ `'");
   mvaddstr(GOAL_X + 4, GOAL_Y, " ,|");
   mvaddstr(GOAL_X + 5, GOAL_Y, " `'");        


   // Next
   mvaddstr(NEXT_X,     NEXT_Y, ",-. ,-. . , |-");
   mvaddstr(NEXT_X + 1, NEXT_Y, "| | |-'  X  |");
   mvaddstr(NEXT_X + 2, NEXT_Y, "' ' `-' ' ` `'");
}      
