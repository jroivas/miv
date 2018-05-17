#include "keyhandling.hh"
#include "terminal.hh"

#include <unistd.h>

static const char KEY_NONE = 0x0;
static const char KEY_ENTER = 0xA;
static const char KEY_RETURN = 0xD;
static const char KEY_ESC = 0x1b;
static const char KEY_BACKSPACE = 0x7f;

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

void KeyHandling::executeCommand()
{
    //std::cout << "STACK: " << stack << "\r\n";
    if (stack.find('q') != std::string::npos) {
        status = editor::Status::Quit;
    }
}

void KeyHandling::resetNormalMode()
{
    stack = "";
    command = false;
    Terminal::get()->resetTemp();
}

void KeyHandling::processNormalMode()
{
    if (lastChar == KEY_ESC) {
        resetNormalMode();
    } else if (!command && lastChar == ':') {
        command = true;
        Terminal::get()->appendTemp(Terminal::get()->cursorLastRow());
        Terminal::get()->appendTemp(lastChar);
    } else if (lastChar == KEY_ENTER || lastChar == KEY_RETURN) {
        if (command) executeCommand();
        resetNormalMode();
    } else if (command) {
        if (lastChar == KEY_BACKSPACE) {
            stack = stack.substr(0, stack.length() - 1);
            Terminal::get()->removeTemp();
        } else {
            stack += lastChar;
            Terminal::get()->appendTemp(lastChar);
        }
    } else if (lastChar == 'i') {
        mode = Mode::InsertMode;
    }
}

void KeyHandling::processInsertMode()
{
    if (lastChar == KEY_ESC) {
        mode = Mode::NormalMode;
    } else if (lastChar == KEY_ENTER || lastChar == KEY_RETURN) {
        Terminal::get()->appendData("\r\n");
    } else if (lastChar == KEY_BACKSPACE) {
        Terminal::get()->removeData();
    } else {
        Terminal::get()->appendData(lastChar);
    }
}

editor::Status KeyHandling::processKeyPress()
{
    status = editor::Status::OK;
    lastChar = readKey();
    if (lastChar == KEY_NONE) return status;

    /*
    std::cout << "K " << (int)lastChar << "\r\n";
    */
    if (isNormalMode()) processNormalMode();
    else if (isInsertMode()) processInsertMode();

    return status;
}
