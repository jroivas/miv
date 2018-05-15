#pragma once

#include <string>
#include <termios.h>

namespace editor {

class Terminal {

public:
    static Terminal *get();

    void enableRawMode();
    static void disableRawMode();
    void die(std::string s);

    void clearScreen();
    void cursorTopLeft();
    std::string cursorLastRow();

    void tildes();
    void append(std::string);
    void append(char);
    void appendData(std::string);
    void appendData(char);
    void appendTemp(std::string s);
    void appendTemp(char c);
    void removeTemp();

    void refresh();
    void refreshPost();

    void resetTemp();

    std::string data;
    static std::string cursorPos(int x, int y);
private:
    Terminal();

    void setInputFlags();
    void setOutputFlags();
    void setLocalFlags();
    void setTimeout();
    void getWindowSize();
    void flushBuffer();
    void flushData();
    void flushTemp();
    
    struct termios raw;

    int width;
    int height;

    std::string buffer;
    std::string temp;
};

}
