#pragma once

#include <string>

namespace editor {

enum class Mode {
    NormalMode,
    InsertMode,
    VisualMode
};

enum class Status {
    OK,
    Quit
};

class KeyHandling
{
public:
    KeyHandling();
    Status processKeyPress();
    char readKey();
    void changeMode(Mode mode);

private:
    bool isNormalMode() const;
    bool isInsertMode() const;
    void processNormalMode();
    void resetNormalMode();
    void processInsertMode();
    void executeCommand();


    Mode mode;
    char lastChar;

    std::string stack;
    bool command;
    bool deleter;

    Status status;
};

}
