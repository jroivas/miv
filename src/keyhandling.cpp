#include "keyhandling.hh"
#include "terminal.hh"
#include "buffer.hh"
#include "tools.hh"

#include <unistd.h>

static const char KEY_NONE = 0x0;
static const char KEY_ENTER = 0xA;
static const char KEY_RETURN = 0xD;
static const char KEY_ESC = 0x1b;
static const char KEY_BACKSPACE = 0x7f;

using editor::KeyHandling;

KeyHandling::KeyHandling() :
    mode(Mode::NormalMode),
    operation(Operation::None)
{
}

char KeyHandling::readKey()
{
    char c = 0;
    int cnt;
    while ((cnt = read(STDIN_FILENO, &c, 1)) != 1) {
        if (cnt == -1 && errno != EAGAIN) Terminal::get()->die("Read failed");
    }
    return c;
}

void KeyHandling::changeMode(Mode m)
{
    mode = m;
}

bool KeyHandling::isNormalMode() const
{
    return mode == Mode::NormalMode;
}

bool KeyHandling::isInsertMode() const
{
    return mode == Mode::InsertMode;
}

void KeyHandling::saveFile(std::string fname)
{
    if (!fname.empty()) {
        if (editor::Buffer::getCurrent()->writeFile(fname)) {
            Terminal::get()->setStatus("\"" + fname + "\" was written");
        } else {
            Terminal::get()->setError("Could not write \"" + fname + "\"");
        }
    }
    else Terminal::get()->setError("Invalid file name: " + stack);
}

void KeyHandling::executeCommand()
{
    if (substrSafe(stack, 0, 2) == "q") {
        status = editor::Status::Quit;
    } else if (substrSafe(stack, 0, 2) == "w ") {
        saveFile(editor::trim_copy(substrSafe(stack, 2)));
    } else if (substrSafe(stack, 0, 1) == "w") {
        if (editor::Buffer::getCurrent()->hasFilename()) {
            saveFile(editor::Buffer::getCurrent()->filename());
        } else Terminal::get()->setError("No file name");
        if (substrSafe(stack, 1, 1) == "q") status = editor::Status::Quit;
    } else Terminal::get()->setError("Unknown command: " + stack);
}

void KeyHandling::resetNormalMode()
{
    stack = "";
    operation = Operation::None;
    Terminal::get()->resetTemp();
}

uint32_t KeyHandling::parseMultiplier()
{
    uint32_t res = 0;
    bool first = true;
    uint32_t i = 0;
    for (char c : stack) {
        if (c >= '0' && c <= '9') {
            if (!first) {
                res *= 10;
            }
            first = false;
            res += c - ('0');
        } else {
            if (res != 0) {
                stack = substrSafe(stack, i);
                return res;
            }
        }
        ++i;
    }
    if (res == 0) res = 1;
    stack = "";
    return res;
}

void KeyHandling::processNormalMode()
{
    if (lastChar == KEY_ESC) {
        resetNormalMode();
    } else if (operation == Operation::None && lastChar == ':') {
        operation = Operation::Command;
        stack = "";
        Terminal::get()->appendTemp(Terminal::get()->cursorLastRow());
        Terminal::get()->appendTemp(lastChar);
    } else if (operation == Operation::Copy) {
        if (lastChar == 'y') {
            copyBuffer = editor::Buffer::getCurrent()->copyLines(parseMultiplier());
            copyMode = CopyMode::Lines;
            resetNormalMode();
        } else if (lastChar == 'j') {
            copyBuffer = editor::Buffer::getCurrent()->copyLines(parseMultiplier() + 1);
            copyMode = CopyMode::Lines;
            resetNormalMode();
        } else if (lastChar == 'k') {
            copyBuffer = editor::Buffer::getCurrent()->copyLinesUp(parseMultiplier() + 1);
            copyMode = CopyMode::Lines;
            resetNormalMode();
        } else if (lastChar == 'l') {
            uint32_t p = editor::Buffer::getCurrent()->x();
            copyBufferChars = substrSafe(editor::Buffer::getCurrent()->line(), p, parseMultiplier());
            copyMode = CopyMode::Chars;
            resetNormalMode();
        } else if (lastChar == 'h') {
            uint32_t p = editor::Buffer::getCurrent()->x();
            uint32_t c = parseMultiplier();
            if (p > c + 1) p -= c + 1;
            else {
                c = p;
                p = 0;
            }
            copyBufferChars = substrSafe(editor::Buffer::getCurrent()->line(), p, c);
            copyMode = CopyMode::Chars;
            resetNormalMode();
        }
    } else if (operation == Operation::Delete) {
        if (lastChar == 'd') {
            editor::Buffer::getCurrent()->deleteLine(parseMultiplier());
            resetNormalMode();
        } else if (lastChar == 'l') {
            editor::Buffer::getCurrent()->deleteChars(parseMultiplier());
            resetNormalMode();
        } else if (lastChar == 'h') {
            editor::Buffer::getCurrent()->backspaceChars(parseMultiplier());
            resetNormalMode();
        //} else if (lastChar == '$') {
        } else {
            stack += lastChar;
        }
    } else if (lastChar == KEY_ENTER || lastChar == KEY_RETURN) {
        if (operation == Operation::Command) executeCommand();
        resetNormalMode();
    } else if (operation == Operation::Command) {
        if (lastChar == KEY_BACKSPACE) {
            stack = stack.substr(0, stack.length() - 1);
            Terminal::get()->removeTemp();
        } else {
            stack += lastChar;
            Terminal::get()->appendTemp(lastChar);
        }
    } else if (lastChar == 'h') {
        editor::Buffer::getCurrent()->cursorLeft(parseMultiplier());
    } else if (lastChar == 'l') {
        editor::Buffer::getCurrent()->cursorRight(parseMultiplier());
    } else if (lastChar == 'k') {
        editor::Buffer::getCurrent()->cursorUp(parseMultiplier());
    } else if (lastChar == 'j') {
        editor::Buffer::getCurrent()->cursorDown(parseMultiplier());
    } else if (lastChar == 'y') {
        operation = Operation::Copy;
    } else if (lastChar == '$') {
        editor::Buffer::getCurrent()->cursorRight(editor::Buffer::getCurrent()->line().length());
    } else if (lastChar == '0') {
        editor::Buffer::getCurrent()->cursorLeft(editor::Buffer::getCurrent()->line().length());
    } else if (lastChar == 'p') {
        uint32_t cnt = parseMultiplier();
        for (uint32_t c = 0; c < cnt; ++c) {
            if (copyMode == CopyMode::Lines) {
                for (auto s : copyBuffer) {
                    editor::Buffer::getCurrent()->insertLine(s);
                    editor::Buffer::getCurrent()->cursorDown();
                }
            } else {
                editor::Buffer::getCurrent()->append(copyBufferChars);
            }
        }
    } else if (lastChar == 'd') {
        operation = Operation::Delete;
    } else if (lastChar == 'x') {
        editor::Buffer::getCurrent()->deleteChars(parseMultiplier());
    } else if (lastChar == 'o') {
        editor::Buffer::getCurrent()->insertLine("");
        editor::Buffer::getCurrent()->cursorDown();
        mode = Mode::InsertMode;
    } else if (lastChar == 'O') {
        editor::Buffer::getCurrent()->cursorUp();
        editor::Buffer::getCurrent()->insertLine("");
        editor::Buffer::getCurrent()->cursorDown();
        mode = Mode::InsertMode;
    } else if (lastChar == 'a') {
        editor::Buffer::getCurrent()->cursorAppend();
        mode = Mode::InsertMode;
    } else if (lastChar == 'i') {
        mode = Mode::InsertMode;
    } else {
        stack += lastChar;
    }
}

void KeyHandling::processInsertMode()
{
    if (lastChar == KEY_ESC) {
        mode = Mode::NormalMode;
    } else if (lastChar == KEY_ENTER || lastChar == KEY_RETURN) {
        if (editor::Buffer::getCurrent()->atEnd()) {
            editor::Buffer::getCurrent()->addLine("");
        }
        editor::Buffer::getCurrent()->cursorDown();
    } else if (lastChar == KEY_BACKSPACE) {
        editor::Buffer::getCurrent()->backspaceChars();
/*
    } else if (lastChar == KEY_DEL) {
        editor::Buffer::getCurrent()->deleteChars();
*/
    } else {
        editor::Buffer::getCurrent()->append(lastChar);
    }
}

editor::Status KeyHandling::processKeyPress()
{
    status = editor::Status::OK;
    lastChar = readKey();
    if (lastChar == KEY_NONE) return status;

    if (isNormalMode()) processNormalMode();
    else if (isInsertMode()) processInsertMode();

    return status;
}
