#include "../include/browser.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <iostream>
#include <ncurses.h>

void draw(int y, std::vector<std::string>& strLs) {
    move(0,0);
    for(int i = y;i-y+1 < LINES && i < strLs.size(); ++i) {
        addstr((strLs[i] + '\n').c_str());
        move(i-y+1,0);
    }
}

int lowestPos(std::vector<std::string>& strLs) {
    return strLs.size() - LINES;
}

int main() {

    Browser b{};

    initscr();
    noecho(); // don't echo user inputs
    cbreak(); // make C-c and C-z work
    curs_set(0); // hide cursor
	keypad(stdscr,TRUE);

    endwin();
    b.goToSite("gemini://tlgs.one");
    auto current = b.renderSite();
    std::cout << b.getCurrentSite()->getBody() << std::endl;
    std::cout << "OTHER OTHER" << std::endl;
    std::cout << current << std::endl;
    return 0;
    auto strLs = stringToList(current);

    int input;
    int y = 0;
    int x = 0;
    bool first = true;

    while( input != 'q') {
        if(input == KEY_DOWN) {
            y += 1;
        } else if (input == KEY_UP){
            y -= 1;
        } else if (input == 'g'){
            y = 0;
        } else if (input == 'G'){
            y = lowestPos(strLs);
        } else if (input == 0x04){
            y += LINES / 2;
        } else if (input == 0x15) {
            y -= LINES / 2;
        }


        y = std::max(0,y);
        y = std::min(y,lowestPos(strLs));

        draw(y, strLs);


        refresh();
        input = getch();
    }

    endwin();
}
