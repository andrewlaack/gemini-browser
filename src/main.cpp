#include "../include/browser.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

// we do this because this is built against ncurses, would be nice to do away w/ this
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
    bool handleLinkFollow;
    bool handleRedirect;
    std::string redirInput;
    std::string linkInput;
    std::string userInput;
};


void draw(DrawState ds) {

    move(0,0);
    clear();

    if(ds.handleInput) {
        addstr("input: ");
        addstr(ds.userInput.c_str());
    } else if (ds.handleRedirect) {
        addstr("Follow redirect (y/n): ");
        addstr(ds.redirInput.c_str());
    } else if (ds.handleLinkFollow) {
        for(int i = ds.y;i-ds.y+1 < LINES && i < ds.strLs.size(); ++i) {
            move(i - ds.y, 0);
            attron(COLOR_PAIR(ds.strLs[i].second + 1));
            addstr(ds.strLs[i].first.c_str());
            attroff(COLOR_PAIR(ds.strLs[i].second + 1));
        }

        move(LINES-1, 0);
        addstr("Following: ");
        addstr(ds.linkInput.c_str());


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

int linkHandler(DrawState ds) {

    ds.handleLinkFollow = true;
    draw(ds);

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
        acc = (acc * 10) + num;
        itr += 1;

        ds.linkInput = std::to_string(acc);
        draw(ds);

    }
    ds.handleLinkFollow = false;

    return acc;
}

enum Direction {
    FORWARD,
    BACKWARD
};

Direction handleRedir(DrawState ds) {

    ds.handleRedirect = true;
    ds.redirInput = "";

    while(true) {
        int input = getch();
        if(input == 'y') {
            ds.redirInput = "y";
            draw(ds);
            ds.handleRedirect = false;
            return FORWARD;
        }
        if(input == 'n') {
            ds.redirInput = "n";
            draw(ds);
            ds.handleRedirect = false;
            return BACKWARD;
        }
    }
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
        if(sel == KEY_BACKSPACE) {
            if(acc.size() > 0) {
                acc = acc.substr(0,acc.size() - 1);
            }
            ds.userInput = acc;
            draw(ds);
            continue;
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
        b.goToSite("gemini://tlgs.one/search/2?test",true);
    }

    auto current = b.renderSite();
    removeNonAscii(current);

    int input = 0;
    int y = 0;
    int x = 0;

    // this is the main loop.

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
            while(!(b.getCurrentSite()->getStatusCode() >= 20 && b.getCurrentSite()->getStatusCode() <= 29)) {
                b.goForward();
            }

            current = b.renderSite();
            removeNonAscii(current);

        } else if(input == 'b') {
            b.goBack();
            while(!(b.getCurrentSite()->getStatusCode() >= 20 && b.getCurrentSite()->getStatusCode() <= 29)) {
                b.goBack();
            }

            current = b.renderSite();
            removeNonAscii(current);
        } else if(input == ' ') {
            int linkToFollow = linkHandler(ds);
            if(linkToFollow != -1) {
                b.followLinkNumber(linkToFollow);
            }

        }


        if(b.getCurrentSite()->getStatusCode() >= 10 && b.getCurrentSite()->getStatusCode() <= 19) {
            std::string inputQuery = handleUserInput(ds);
            if(inputQuery != "?") { // TODO: Better handling
                b.goToSite(inputQuery,true);
                current = b.renderSite();
                removeNonAscii(current);
            }

        } else if (b.getCurrentSite()->getStatusCode() >= 30 && b.getCurrentSite()->getStatusCode() <= 39){
            Direction dir = handleRedir(ds);
            if(dir == BACKWARD) {
                b.goBack();
                while(!(b.getCurrentSite()->getStatusCode() >= 20 && b.getCurrentSite()->getStatusCode() <= 29)) {
                    b.goBack();
                }
            } else {
                b.goToSite(b.getCurrentSite()->getMeta(),true);
            }

        } else {
            current = b.renderSite();
            removeNonAscii(current);
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
