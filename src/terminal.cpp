#include "terminal.hh"
#include "buffer.hh"

#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>

using editor::Terminal;

static struct termios original_termios;
static Terminal *terminal = nullptr;

#define ESCAPE_KEY "\x1b"
static const std::string CMD_CLEAR_SCREEN = ESCAPE_KEY "[2J";
static const std::string CMD_CURSOR_TOPLEFT = ESCAPE_KEY "[H";
static const std::string CMD_CURSOR_HIDE = ESCAPE_KEY "[?25l";
static const std::string CMD_CURSOR_SHOW = ESCAPE_KEY "[?25h";
static const std::string CMD_REMOVE_TILL_END = ESCAPE_KEY "[K";

static const int reservedLinesBottom = 1;

Terminal* Terminal::get()
{
    if (terminal == nullptr) {
        terminal = new Terminal();
    }
    return terminal;
}

Terminal::Terminal() :
    width(80),
    height(24)
{
    getWindowSize();
}

std::string Terminal::cursorPos(int x, int y)
{
    return std::string(ESCAPE_KEY "[") + std::to_string(y) + ";" + std::to_string(x) + "H";
}


void Terminal::enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) die("Can't read terminal attributes");
    atexit(&Terminal::disableRawMode);
    raw = original_termios;

    setInputFlags();
    setOutputFlags();
    raw.c_cflag |= (CS8);
    setLocalFlags();
    setTimeout();

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("Can't set terminal attributes");
}

void Terminal::setInputFlags()
{
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
}

void Terminal::setOutputFlags()
{
    raw.c_oflag &= ~(OPOST);
}

void Terminal::setLocalFlags()
{
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
}

void Terminal::setTimeout()
{
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
}

void Terminal::disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) Terminal::get()->die("Can't restore terminal settings");
}

void Terminal::die(std::string s) {
    clearScreen();
    perror(s.c_str());
    exit(1);
}

void Terminal::append(std::string s)
{
    buffer += s;
}

void Terminal::append(char c)
{
    buffer += c;
}

void Terminal::appendData(std::string s)
{
    data += s;
}

void Terminal::appendData(char c)
{
    data += c;
}

void Terminal::clearScreen()
{
    append(CMD_CLEAR_SCREEN);
    cursorTopLeft();
}

void Terminal::cursorTopLeft()
{
    append(CMD_CURSOR_TOPLEFT);
}

std::string Terminal::cursorLastRow()
{
    return cursorPos(1, height);
}

void Terminal::tildes()
{
    cursorTopLeft();
    for (int y = 0; y < height - reservedLinesBottom; ++y) {
        append("~" + CMD_REMOVE_TILL_END);
        if (y < height - 1) append("\r\n");
    }
    cursorTopLeft();
}

void Terminal::getWindowSize()
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) return;

    width = ws.ws_col;
    height = ws.ws_row;
}

void Terminal::flush()
{
    flushBuffer();
}

void Terminal::flushBuffer()
{
    write(STDOUT_FILENO, buffer.c_str(), buffer.length());
}

void Terminal::flushData()
{
    std::string bf = editor::Buffer::getCurrent()->viewport(width, height);
    // FIXME ranges and out of bounds
    bf += cursorPos(editor::Buffer::getCurrent()->x() + 1, editor::Buffer::getCurrent()->y() + 1);
    write(STDOUT_FILENO, bf.c_str(), bf.length());
}

void Terminal::flushTemp()
{
    write(STDOUT_FILENO, temp.c_str(), temp.length());
}

void Terminal::appendTemp(std::string s)
{
    temp += s;
}

void Terminal::appendTemp(char c)
{
    temp += c;
}

void Terminal::removeData()
{
    data = data.substr(0, data.length() - 1);
}

void Terminal::removeTemp()
{
    temp = temp.substr(0, temp.length() - 1);
}

void Terminal::resetTemp()
{
    temp = "";
}

void Terminal::refresh()
{
    buffer = "";
    append(CMD_CURSOR_HIDE);

    tildes();

    append(CMD_CURSOR_SHOW);

    flushBuffer();
    cursorTopLeft();
    flushData();
    flushTemp();
}
