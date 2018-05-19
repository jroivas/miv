#pragma once

#include <string>
#include <vector>
#include <termios.h>

namespace editor {

class Terminal
{

public:
    static Terminal *get();

    void enableRawMode();
    static void disableRawMode();
    void die(std::string s);

    void clearScreen();
    void cursorTopLeft();
    std::string cursorLastRow();

    void append(std::string);
    void append(char);
    void appendData(std::string);
    void appendData(char);
    void removeData();
    void appendTemp(std::string s);
    void appendTemp(char c);
    void removeTemp();
    void setStatus(std::string s);
    void setError(std::string s);

    void refresh();
    void refreshPost();

    void resetTemp();

    std::string data;
    static std::string cursorPos(int x, int y);
    void flush();

private:
    Terminal();
    std::string renderLines(const std::vector<std::string> &lines) const;

    void setInputFlags();
    void setOutputFlags();
    void setLocalFlags();
    void setTimeout();
    void getWindowSize();
    void flushBuffer();
    void flushData();
    void flushTemp();
    void flushStatus();
    void flushInfo();
    void relocateCursor();

    struct termios raw;

    int width;
    int height;

    std::string buffer;
    std::string temp;
    std::string status;
    uint32_t statusTime;
};

}
