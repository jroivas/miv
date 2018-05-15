#pragma once

#include <string>
#include <termios.h>

namespace editor {

class Terminal {

public:
    Terminal();

    void enableRawMode();
    static void disableRawMode();
    static void die(std::string s);

    static void clearScreen();
    static void cursorTopLeft();

    void tildes();
    static void append(std::string);
    static void append(char);

private:
    void setInputFlags();
    void setOutputFlags();
    void setLocalFlags();
    void setTimeout();
    void getWindowSize();
    
    struct termios raw;

    int width;
    int height;
};

}
