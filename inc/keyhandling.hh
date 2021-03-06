#pragma once

#include <string>
#include <vector>

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

enum class Operation {
    None,
    Command,
    Delete,
    Copy
};

enum class CopyMode {
    Lines,
    Chars
};

class KeyHandling
{
public:
    KeyHandling();
    Status processKeyPress();
    char readKey() const;
    void changeMode(Mode mode);

private:
    bool isNormalMode() const;
    bool isInsertMode() const;

    void processNormalMode();
    void resetNormalMode();
    void processInsertMode();
    void executeCommand();

    void handleCopy();
    void handleDelete();
    void handlePaste();
    void handleCommandEdit();

    uint32_t parseMultiplier(bool forceOne = true);
    void saveFile(std::string fname) const;

    Mode mode;
    char lastChar;

    std::string stack;

    Operation operation;
    Status status;
    CopyMode copyMode;

    std::vector<std::string> copyBuffer;
    std::string copyBufferChars;
};

}
