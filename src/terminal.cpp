#include "terminal.hh"

#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>

using editor::Terminal;

static struct termios original_termios;

#define ESCAPE_KEY "\x1b"
static const std::string CMD_CLEAR_SCREEN = ESCAPE_KEY "[2J";
static const std::string CMD_CURSOR_TOPLEFT = ESCAPE_KEY "[H";

Terminal::Terminal() :
    width(80),
    height(24)
{
    getWindowSize();
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
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) die("Can't restore terminal settings");
}

void Terminal::die(std::string s) {
    clearScreen();
    perror(s.c_str());
    exit(1);
}

void Terminal::append(std::string s)
{
    write(STDOUT_FILENO, s.c_str(), s.length());
}

void Terminal::append(char c)
{
    write(STDOUT_FILENO, &c, 1);
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

void Terminal::tildes()
{
    for (int y = 0; y < height; ++y) {
        append("~");
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
