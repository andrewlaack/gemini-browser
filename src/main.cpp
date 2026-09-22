#include "../include/browser.hpp"
#include <algorithm>
#include <cstddef>
#include <ncurses.h>
#include <utility>
#include <vector>

// we do this because this is buiult against ncurses, would be nice to do away w/ this
// bc ppl use lots of emojis on gemini sites.

void removeNonAscii(std::vector<std::pair<std::string, int>>& strLs) {

    for(std::size_t i = 0; i < strLs.size(); ++i) {

        std::string& s = strLs[i].first;

        std::string out;
        for (int c: s)
            if ((c >= 0x20 && c < 0x7E) || (c == '\t' || c == '\n')) {
                out += c;
            }
        strLs[i].first = out;
    }
    return;
}

void initColors() {
    for (int c = 0; c < COLORS; ++c) {
        init_pair(c + 1, c, -1);
    }
}

void draw(int y, std::vector<std::pair<std::string, int>>& strLs) {
    move(0,0);

    for(int i = y;i-y+1 < LINES && i < strLs.size(); ++i) {
        move(i - y, 0);
        attron(COLOR_PAIR(strLs[i].second + 1));
        addstr(strLs[i].first.c_str());
        attroff(COLOR_PAIR(strLs[i].second + 1));
    }
}

int lowestPos(std::vector<std::pair<std::string, int>>& strLs) {
    return strLs.size() - LINES;
}

int main() {

    Browser b{};

    initscr();
    noecho(); // don't echo user inputs
    cbreak(); // make C-c and C-z work
    curs_set(0); // hide cursor
	keypad(stdscr,TRUE);

    // TODO: Check colors available first
    start_color();
    use_default_colors();
    initColors();

    b.goToSite("gemini://laack.co");

    auto current = b.renderSite();
    removeNonAscii(current);

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
            y = lowestPos(current);
        } else if (input == 0x04){
            y += LINES / 2;
        } else if (input == 0x15) {
            y -= LINES / 2;
        }


        y = std::max(0,y);
        y = std::max(0,std::min(y,lowestPos(current)));

        draw(y, current);


        refresh();
        input = getch();
    }

    endwin();
}
