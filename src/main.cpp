#include "../include/browser.hpp"
#include <algorithm>
#include <cmath>
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

struct DrawState {
    int y;
    std::vector<std::pair<std::string, int>> strLs;
    bool handleInput;
    std::string userInput;
};


void draw(DrawState ds) {

    move(0,0);
    clear();

    if(ds.handleInput) {
        addstr("input: ");
        addstr(ds.userInput.c_str());
    } else {
        for(int i = ds.y;i-ds.y+1 < LINES && i < ds.strLs.size(); ++i) {
            move(i - ds.y, 0);
            attron(COLOR_PAIR(ds.strLs[i].second + 1));
            addstr(ds.strLs[i].first.c_str());
            attroff(COLOR_PAIR(ds.strLs[i].second + 1));
        }
    }
}

int lowestPos(std::vector<std::pair<std::string, int>>& strLs) {
    return strLs.size() - LINES;
}

int linkHandler() {

    int acc = 0;
    int num = 0;
    int itr = 0;

    while(num!=-1) {
        int sel = getch();
        num = (sel - 0x30);

        if(sel == '\n' || sel == KEY_ENTER) {
            num = -1;
            continue;
        }

        if(num > 9 || num < 0) {
            continue;
        }
        acc += num * (std::pow(10,itr));
        itr += 1;
    }

    return acc;
}

std::string handleUserInput(DrawState ds) {

    ds.handleInput = true;
    draw(ds);
    std::string acc = "";

    while(true) {
        int sel = getch();
        if(sel == '\n' || sel == KEY_ENTER) {
            break;
        }

        acc += std::string {(char)sel};
        ds.userInput = acc;
        draw(ds);
    }

    ds.handleInput = false;
    draw(ds);
    return "?" + acc;
}


int main(int argc, char** argv) {

    Browser b{};

    DrawState ds {};

    initscr();
    noecho(); // don't echo user inputs
    cbreak(); // make C-c and C-z work
    curs_set(0); // hide cursor
	keypad(stdscr,TRUE);

    // TODO: Check colors available first
    start_color();
    use_default_colors();
    initColors();

    if(argc > 1) {
        b.goToSite(argv[1],true);
    } else {
        b.goToSite("gemini://tlgs.one",true);
    }

    auto current = b.renderSite();
    removeNonAscii(current);

    int input = 0;
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
        } else if(input == 'f') {
            b.goForward();
            current = b.renderSite();
            removeNonAscii(current);

        } else if(input == 'b') {
            b.goBack();
            current = b.renderSite();
            removeNonAscii(current);
        } else if(input == ' ') {
            int linkToFollow = linkHandler();
            if(linkToFollow != -1) {
                b.followLinkNumber(linkToFollow);
                if(b.getCurrentSite()->getStatusCode() >= 10 && b.getCurrentSite()->getStatusCode() <= 19) {
                    std::string inputQuery = handleUserInput(ds);
                    if(inputQuery != "?") { // TODO: Better handling
                        b.goToSite(inputQuery,true);
                        current = b.renderSite();
                        removeNonAscii(current);
                    }

                } else {
                    current = b.renderSite();
                    removeNonAscii(current);
                }
            }

        }

        y = std::max(0,y);
        y = std::max(0,std::min(y,lowestPos(current)));
        ds.y = y;
        ds.strLs = current;

        draw(ds);


        refresh();
        input = getch();
    }

    endwin();
}
