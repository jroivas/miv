#include "keyhandling.hh"
#include "terminal.hh"

#include <unistd.h>
#include <iostream>

static const char KEY_NONE = 0x0;
static const char KEY_ENTER = 0xA;
static const char KEY_RETURN = 0xD;
static const char KEY_ESC = 0x1b;

using editor::KeyHandling;

KeyHandling::KeyHandling() :
    mode(Mode::NormalMode),
    command(false)
{
}

char KeyHandling::readKey()
{
    char c = 0;
    int cnt;
    while ((cnt = read(STDIN_FILENO, &c, 1)) != 1) {
        if (cnt == -1 && errno != EAGAIN) Terminal::die("Read failed");
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

void KeyHandling::executeCommand()
{
    std::cout << "STACK: " << stack << "\r\n";
    if (stack.find('q') != std::string::npos) {
        status = editor::Status::Quit;
    }
}

void KeyHandling::processNormalMode()
{
    std::cout <<"a " << (int)lastChar << " " << lastChar << "\r\n";
    if (!command && lastChar == ':') {
        command = true;
    } else if (lastChar == KEY_ENTER || lastChar == KEY_RETURN) {
        if (command) executeCommand();
        stack = "";
        command = false;
    } else if (command) {
        stack += lastChar;
    } else if (lastChar == 'i') {
        mode = Mode::InsertMode;
    }
}

void KeyHandling::processInsertMode()
{
    if (lastChar == KEY_ESC) {
        std::cout << "nn\r\n";
        mode = Mode::NormalMode;
    } else {
        Terminal::append(lastChar);
        //std::cout << lastChar;
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
