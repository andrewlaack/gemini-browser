#include "../include/browser.hpp"
#include <malloc.h>
#include "../include/gemini-client.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

const int SITE_CACHE_LIMIT = 10;
const int THREAD_NUM = 4;
std::vector<std::thread> threads(THREAD_NUM);
std::vector<std::atomic<bool>> done(THREAD_NUM);

// we do this because this is built against ncurses, would be nice to do away w/ this
// bc ppl use lots of emojis on gemini sites.

void aggressiveCaching(Browser* bPtr, std::vector<Link> targets, int threadIdx) {
    for(int i =  0 ; i < targets.size() && i < SITE_CACHE_LIMIT; ++i) {
        auto& target = targets[i];
        bPtr->justCacheSite(target);
    }
    done[threadIdx] = true;
}

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
    bool handleOpenOther;
    bool handleRedirect;
    std::string redirInput;
    std::string openOtherInput;
    std::string userInput;
};

void drawInputBox(std::string text, std::string userInput) {

    move(LINES/2-1, COLS/4);

    attron(COLOR_PAIR(COLOR_CYAN+1));
    for(int i = 0; i < COLS/2; ++i) {
        addstr("-");
    }

    move(LINES/2 + 1, COLS/4);
    for(int i = 0; i < COLS/2; ++i) {
        addstr("-");
    }
    attroff(COLOR_PAIR(COLOR_CYAN+1));

    int textSize = text.size();
    int userInputSize = userInput.size();
    int width = COLS/2;

    std::string userTextToRender = userInput;

    int delta = width - (textSize + userInputSize);


    if(delta < 0) {
        userTextToRender = userInput.substr(delta*-1, userInput.size());
    } else {
        while(delta != 0) {
            userTextToRender.append(" "); // this makes sure the background doesn't leak through.
            delta -= 1;
        }
    }


    move(LINES/2, COLS/4);
    addstr(text.c_str());
    addstr(userTextToRender.c_str());

}


void draw(DrawState ds) {

    move(0,0);
    clear();

    for(int i = ds.y;i-ds.y+1 < LINES && i < ds.strLs.size(); ++i) {
        move(i - ds.y, 0);
        attron(COLOR_PAIR(ds.strLs[i].second + 1));
        addstr(ds.strLs[i].first.c_str());
        attroff(COLOR_PAIR(ds.strLs[i].second + 1));
    }

    if (ds.handleRedirect) {
        drawInputBox("Follow redirect (y/n): ", ds.redirInput);
    }

    if(ds.handleInput) {
        drawInputBox("input: ", ds.userInput);
    }

    if (ds.handleOpenOther) {
        drawInputBox("Destination / Link Number: ", ds.openOtherInput);
    }

    refresh();

}

int lowestPos(std::vector<std::pair<std::string, int>>& strLs) {
    return strLs.size() - LINES;
}

std::string openPageHandler(DrawState ds) {

    ds.handleOpenOther = true;
    draw(ds);
    std::string acc = "";

    while(true) {
        int sel = getch();

        if(sel == KEY_BACKSPACE) {
            if(acc.size() > 0) {
                acc = acc.substr(0,acc.size() - 1);
            }
            ds.openOtherInput = acc;
            draw(ds);
            continue;
        }

        if(sel ==  27) {
            acc = "";
            break;
        }

        if(sel == '\n' || sel == KEY_ENTER) {
            break;
        }

        acc += std::string {(char)sel};

        ds.openOtherInput = acc;
        draw(ds);

    }

    ds.openOtherInput = acc;
    ds.handleOpenOther = false;
    draw(ds);



    return acc;
}

enum Direction {
    FORWARD,
    BACKWARD
};

Direction handleRedir(DrawState ds) {

    ds.handleRedirect = true;
    ds.redirInput = "";
    draw(ds);

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
        if(sel ==  27) {
            acc = "";
            ds.userInput = acc;
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

    for (auto& d : done) {
        d = true;
    }

    Browser* bPtr = new Browser{};
    Browser& b = *bPtr;

    DrawState ds {};

    initscr();
    set_escdelay(25);
    noecho(); // don't echo user inputs
    cbreak(); // make C-c and C-z work
    curs_set(0); // hide cursor
	keypad(stdscr,TRUE);

    // TODO: Check colors available first
    start_color();
    use_default_colors();
    initColors();

    b.goToSite("about:newtab",true);

    if(argc > 1) {
        // TODO: This should have tests.
        std::string path = "file:///" + std::filesystem::current_path().string() + "/" + argv[1];
        b.goToSite(path,true);
    } else {
        b.goToSite("gemini://tlgs.one",true);
    }

    auto current = b.renderSite();
    removeNonAscii(current);
    current = breakLines(current,COLS);


    int input = 0;
    int y = 0;

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
        } else if (input == 'r'){
            b.refresh();
        } else if(input == 'f') {
            b.goForward();

            current = b.renderSite();
            removeNonAscii(current);
            current = breakLines(current,COLS);


        } else if(input == 'b') {
            b.goBack();

            current = b.renderSite();
            removeNonAscii(current);
            current = breakLines(current,COLS);

        } else if(input == 'o') {
            std::string locationToGo = openPageHandler(ds);
            
            // TODO: Check if this is an int and if it is try to use that link.
            // if it's not, then try to go to domain (should consider having a default search engine too.)

            try {
                b.followLinkNumber(std::stoi(locationToGo));
            } catch (...) {
                if(locationToGo != "") {
                    if(locationToGo.find("gemini://") == -1) {
                        locationToGo = "gemini://" + locationToGo;
                    }
                    b.goToSite(locationToGo, true);
                }
            }

        }

        if(b.getCurrentSite()->getStatusCode() >= 10 && b.getCurrentSite()->getStatusCode() <= 19) {
            std::string inputQuery = handleUserInput(ds);
            if(inputQuery != "?") { // TODO: Better handling
                b.goToSite(inputQuery,true);
                current = b.renderSite();
                removeNonAscii(current);
                current = breakLines(current,COLS);
            } else {
                b.goBack();
                current = b.renderSite();
                removeNonAscii(current);
                current = breakLines(current,COLS);
            }

        } else if (b.getCurrentSite()->getStatusCode() >= 30 && b.getCurrentSite()->getStatusCode() <= 39){
            Direction dir = handleRedir(ds);
            if(dir == BACKWARD) {
                b.goBack();
            } else {
                b.goToSite(b.getCurrentSite()->getMeta(),true);
            }
            current = b.renderSite();
            removeNonAscii(current);
            current = breakLines(current,COLS);

        } else {
            current = b.renderSite();
            removeNonAscii(current);
            current = breakLines(current,COLS);
        }

        y = std::max(0,y);
        y = std::max(0,std::min(y,lowestPos(current)));
        ds.y = y;
        ds.strLs = current;
        draw(ds);
        refresh();


        bool dispatched = false;
        for(int i = 0; i < THREAD_NUM && dispatched == false; ++i) {
            if(done[i]) {
                if(threads[i].joinable()) {
                    threads[i].join();
                }
                auto lls = b.getLinkLines();
                done[i] = false;
                threads[i] = std::thread(aggressiveCaching, bPtr, lls, i);
                dispatched = true;
            }
        }

        input = getch();
    }

    endwin();

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    delete bPtr;
}
