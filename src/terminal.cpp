#include "terminal.hh"
#include "buffer.hh"

#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>

using editor::Terminal;

static struct termios original_termios;
static Terminal *terminal = nullptr;
static bool enabled = false;

#define ESCAPE_KEY "\x1b"
static const std::string CMD_CLEAR_SCREEN = ESCAPE_KEY "[2J";
static const std::string CMD_CURSOR_TOPLEFT = ESCAPE_KEY "[H";
static const std::string CMD_CURSOR_HIDE = ESCAPE_KEY "[?25l";
static const std::string CMD_CURSOR_SHOW = ESCAPE_KEY "[?25h";
static const std::string CMD_REMOVE_TILL_END = ESCAPE_KEY "[K";

static const std::string NEWLINE = "\r\n";
static const int  STATUS_DEFAULT_TIME = 5;

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
    height(24),
    statusTime(0)
{
    getWindowSize();
}

std::string Terminal::cursorPos(int x, int y)
{
    return std::string(ESCAPE_KEY "[") + std::to_string(y) + ";" + std::to_string(x) + "H";
}


void Terminal::enableRawMode()
{
    enabled = true;
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
    if (!enabled) return;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) Terminal::get()->die("Can't restore terminal settings");
    enabled = false;
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

void Terminal::setStatus(std::string s)
{
    status = s;
    statusTime = STATUS_DEFAULT_TIME;
}

void Terminal::setError(std::string s)
{
    setStatus("*** ERROR: " + s);
}

std::string Terminal::cursorLastRow()
{
    return cursorPos(1, height);
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

std::string Terminal::renderLines(const std::vector<std::string> &lines) const
{
    std::string res = CMD_CURSOR_TOPLEFT;
    size_t target = lines.size();
    for (size_t i = 0; i < target; ++i) {
        res += lines[i];
        res += CMD_REMOVE_TILL_END;
        if (i < target - 1) res += NEWLINE;
    }
    res += CMD_CURSOR_SHOW;
    return res;
}

void Terminal::flushData()
{
    editor::Buffer::getCurrent()->relocateRow(width, height - reservedLinesBottom);
    std::string bf = renderLines(editor::Buffer::getCurrent()->viewport(width, height - reservedLinesBottom));
    bf += cursorPos(editor::Buffer::getCurrent()->x() + 1, editor::Buffer::getCurrent()->y(height - reservedLinesBottom) + 1);
    write(STDOUT_FILENO, bf.c_str(), bf.length());
}

void Terminal::flushTemp()
{
    if (temp.empty()) return;
    std::string temp2 = temp  + CMD_REMOVE_TILL_END;
    write(STDOUT_FILENO, temp2.c_str(), temp2.length());
}

void Terminal::flushStatus()
{
    if (statusTime == 0) return;
    std::string statusData = cursorLastRow() + status + CMD_REMOVE_TILL_END;
    write(STDOUT_FILENO, statusData.c_str(), statusData.length());
    if (statusTime > 0) --statusTime;
    if (statusTime == 0) status = "";
}

void Terminal::flushInfo()
{
    std::string info;
    info += std::to_string(editor::Buffer::getCurrent()->x() + 1);
    info += ",";
    info += std::to_string(editor::Buffer::getCurrent()->y() + 1);
    info += " ";
    uint32_t cnt = editor::Buffer::getCurrent()->size();
    if (cnt == 0) info += "0";
    else info += std::to_string((editor::Buffer::getCurrent()->y() + 1) * 100 / cnt);
    info += "%";
    info = cursorPos(width - info.length(), height - 1) + info;
    write(STDOUT_FILENO, info.c_str(), info.length());
}


int Terminal::getHeight() const
{
    return height - reservedLinesBottom - 1;
}

void Terminal::relocateCursor()
{
    if (!temp.empty()) return;
    uint32_t tabs = editor::Buffer::getCurrent()->tabExtra();
    std::string pos = cursorPos(editor::Buffer::getCurrent()->x() + tabs + 1, editor::Buffer::getCurrent()->y(height - reservedLinesBottom) + 1);
    write(STDOUT_FILENO, pos.c_str(), pos.length());
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
    flushBuffer();
    cursorTopLeft();
    flushData();
    flushTemp();
    flushStatus();
    flushInfo();

    buffer = "";
    append(CMD_CURSOR_SHOW);
    flushBuffer();
    relocateCursor();
}
