#include "buffer.hh"
#include "tools.hh"
#include <fstream>
#include <utf8.h>

using editor::Buffer;

std::vector<Buffer*> Buffer::buffers;
uint32_t Buffer::index = 0;

Buffer::Buffer() :
    posX(0),
    posY(0),
    row(0),
    tabSize(8),
    tabsToSpaces(false),
    lineEnding("\n")
{
    buffers.push_back(this);
}

Buffer::Buffer(std::string filename) :
    Buffer()
{
    readFile(filename);
}

Buffer::~Buffer()
{
    removeBuffer(this);
}

void Buffer::removeBuffer(Buffer *buf)
{
    for (size_t i = 0; i < buffers.size(); ++i) {
        if (buffers[i] == buf) {
            if (i > 0) index = i - 1;
            else index = 0;
            buffers.erase(buffers.begin() + i);
            return;
        }
    }
}

Buffer *Buffer::getCurrent()
{
    if (buffers.size() == 0) return nullptr;
    return buffers[index];
}

bool Buffer::readFile(std::string filename)
{
    data.erase(data.begin(), data.end());
    fileName = filename;
    std::ifstream fd(filename);
    if (!fd.is_open()) return false;

    std::string tmp;
    while (std::getline(fd, tmp)) data.push_back(tmp);
    fd.close();
    return true;
}

bool Buffer::writeFile(std::string filename)
{
    std::ofstream fd(filename);
    if (!fd.is_open()) return false;
    fileName = filename;

    for (std::string line : data) fd << line + lineEnding;
    fd.close();
    return true;
}

void Buffer::addLine(std::string line)
{
    data.push_back(line);
}

void Buffer::insertLine(std::string line)
{
    data.insert(data.begin() + posY + 1, line);
}

void Buffer::updateLine(std::string line)
{
    while (posY >= data.size()) addLine("");
    data[posY] = line;
}

void Buffer::deleteLine(uint32_t cnt)
{
    uint32_t origY = posY;
    while (cnt > 0 && !data.empty()) {
        data.erase(data.begin() + posY);
        if (posY >= data.size()) posY = data.size() - 1;
        if (posY < origY) break;
        --cnt;
    }
}

const std::string Buffer::line() const
{
    if (posY >= data.size()) return "";
    return data[posY];
}

uint32_t Buffer::lineLength() const
{
    if (posY >= data.size()) return 0;
    return utf8_length(data[posY]);
}

void Buffer::gotoY(uint32_t y)
{
    if (y == 0 || y > data.size()) y = data.size();
    posY = y - 1;
}

const std::vector<std::string> Buffer::copyLines(uint32_t cnt) const
{
    std::vector<std::string> res;
    if (posY >= data.size()) return res;
    uint32_t target = std::min<uint32_t>(posY + cnt, data.size());
    for (uint32_t l = posY; l < target; ++l) {
        res.push_back(data[l]);
    }
    return res;
}

const std::vector<std::string> Buffer::copyLinesUp(uint32_t cnt) const
{
    std::vector<std::string> res;
    if (posY >= data.size()) return res;
    if (cnt > posY) cnt = posY;
    uint32_t src = std::min<uint32_t>(posY - cnt, data.size());
    for (uint32_t l = src; l <= posY; ++l) {
        res.push_back(data[l]);
    }
    return res;
}

void Buffer::sanitizePos()
{
    uint32_t ll = lineLength();
    posX = std::min<uint32_t>(posX, ll > 0 ? ll - 1 : ll);
    posY = std::min<uint32_t>(posY, data.size() - 1);
}

uint32_t Buffer::tabs() const
{
    std::string l = line();
    uint32_t res = 0;
    for (uint32_t p = 0; p < posX; ++p) {
        char c = utf8_at(l, p);
        if (c == '\t') ++res;
    }
    return res;
}

uint32_t Buffer::tabExtra() const
{
    std::string l = line();

    uint32_t res = 0;
    uint32_t pos = 0;
    for (uint32_t p = 0; p < posX; ++p) {
        char c = utf8_at(l, p);
        if (c == '\t') {
            ++pos;
            while (pos % tabSize != 0) {
                ++pos;
                ++res;
            }
        } else {
            ++pos;
        }
    }
    return res;
}

void Buffer::cursorLeft(uint32_t cnt)
{
    if (cnt >= posX) posX = 0;
    else posX -= cnt;
}

void Buffer::cursorRight(uint32_t cnt)
{
    posX += cnt;
    sanitizePos();
}

void Buffer::cursorAppend()
{
    ++posX;
    sanitizePos();
}

void Buffer::cursorUp(uint32_t cnt)
{
    if (cnt >= posY) posY = 0;
    else posY -= cnt;
    sanitizePos();
}

void Buffer::cursorDown(uint32_t cnt)
{
    posY += cnt;
    sanitizePos();
}

void Buffer::pageUp(uint32_t pageSize, uint32_t cnt)
{
    if (row >= pageSize * cnt) {
        row -= pageSize * cnt;
        posY = row;
        posY += pageSize;
    } else {
        row = 0;
        posY = 0;
    }
    sanitizePos();
}

void Buffer::pageDown(uint32_t pageSize, uint32_t cnt)
{
    posY += pageSize * cnt - 1;
    sanitizePos();
    row = posY;
}

void Buffer::cursorWord(uint32_t cnt)
{
    std::string nowline = line();
    uint32_t ll = utf8_length(nowline);
    if (posX >= ll) {
        ++posY;
        posX = 0;
        nowline = line();
    }
    static const std::string delimiters = " ,.:;\\/-\t";
    for (uint32_t p = posX + 1; p < ll; ++p) {
        char n = utf8_at(nowline, p);
        if (delimiters.find(n) != std::string::npos) {
            posX = p;
            if (n == ' ') ++posX;
            sanitizePos();
            return;
        }
    }
    ++posY;
    posX = 0;
}

void Buffer::backspaceChars(uint32_t cnt)
{
    if (posX == 0) {
        // FIXME TODO delete from prev line
        return;
    }
    std::string l = line();
    updateLine(substrSafe(l, 0, posX > cnt ? posX - cnt : 0) + substrSafe(l, posX));

    if (posX <= cnt) posX = 0;
    else posX -= cnt;
    sanitizePos();
}

void Buffer::deleteChars(uint32_t cnt)
{
    std::string l = line();
    updateLine(substrSafe(l, 0, posX) + substrSafe(l, posX + cnt));
    sanitizePos();
}

void Buffer::append(std::string d)
{
    std::string l = line();
    updateLine(substrSafe(l, 0, posX) + d + substrSafe(l, posX));
    posX += utf8_length(d);
    sanitizePos();
}

void Buffer::append(char d)
{
    appendBuffer += d;
    if (!utf8_valid(appendBuffer)) {
        // Reset after too many failed chars
        if (appendBuffer.length() > 4) append(appendBuffer);
        return;
    }
    append(appendBuffer);
    appendBuffer = "";
}

void Buffer::relocateRow(uint32_t width, uint32_t height)
{
    if (posY < row) row = posY;
    if (posY >= (row + height)) row = posY - height;
}

std::string Buffer::spaces(uint32_t cnt) const
{
    std::string res;
    for (uint32_t c = 0; c < cnt; ++c) res += ' ';
    return res;
}

std::string Buffer::handleSpecial(std::string line) const
{
    std::string res;
    uint32_t pos = 0;
    uint32_t ll = utf8_length(line);
    for (uint32_t p = 0; p < ll; ++p) {
        std::string s = utf8_at_str(line, p);
        if (s == "\t") {
            res += ' ';
            ++pos;
            while (pos % tabSize != 0) {
                ++pos;
                res += ' ';
            }
        } else {
            ++pos;
            res += s;
        }
    }
    return res;
}

const std::vector<std::string> Buffer::viewport(uint32_t width, uint32_t height) const
{
    std::vector<std::string> res;
    for (uint32_t i = 0; i < height; ++i) {
        uint32_t filerow = i + row;
        if (filerow >= data.size()) res.push_back("~");
        else res.push_back(handleSpecial(data[filerow]));
    }

    return res;
}
