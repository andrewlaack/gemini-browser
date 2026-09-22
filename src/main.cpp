#include "../include/browser.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <iostream>
#include <ncurses.h>

// we do this because this is buiult against ncurses, would be nice to do away w/ this
// bc ppl use lots of emojis on gemini sites.
std::string removeNonAscii(const std::string& s) {
    std::string out;
    for (int c: s)
        if ((c >= 0x20 && c < 0x7E) || (c == '\t' || c == '\n')) {
            out += c;
        }
    return out;
}

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

    b.goToSite("gemini://tlgs.one/");
    auto current = b.renderSite();
    current = removeNonAscii(current);
    auto strLs = stringToList(current);

    int input;
    int y = 0;
    int x = 0;

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
        y = std::max(0,std::min(y,lowestPos(strLs)));

        draw(y, strLs);


        refresh();
        input = getch();
    }

    endwin();
}
