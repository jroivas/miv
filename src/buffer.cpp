#include "buffer.hh"
#include "tools.hh"
#include <fstream>

using editor::Buffer;

std::vector<Buffer*> Buffer::buffers;
uint32_t Buffer::index = 0;

Buffer::Buffer() :
    posX(0),
    posY(0),
    row(0)
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
    std::ifstream fd(filename);
    if (!fd.is_open()) return false;

    std::string tmp;
    while (std::getline(fd, tmp)) data.push_back(tmp);
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

const std::vector<std::string> Buffer::copyLines(uint32_t cnt)
{
    std::vector<std::string> res;
    if (posY >= data.size()) return res;
    uint32_t target = std::min<uint32_t>(posY + cnt, data.size());
    for (uint32_t l = posY; l < target; ++l) {
        res.push_back(data[l]);
    }
    return res;
}

const std::vector<std::string> Buffer::copyLinesUp(uint32_t cnt)
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

void Buffer::cursorLeft(uint32_t cnt)
{
    if (cnt >= posX) posX = 0;
    else posX -= cnt;
}

void Buffer::cursorRight(uint32_t cnt)
{
    posX = std::min<uint32_t>(posX + cnt, line().length() - 1);
}

void Buffer::cursorAppend()
{
    posX = std::min<uint32_t>(posX + 1, line().length());
}

void Buffer::cursorUp(uint32_t cnt)
{
    if (cnt >= posY) posY = 0;
    else posY -= cnt;
    uint32_t ll = line().length();
    if (posX >= ll) posX = std::min<uint32_t>(posX, ll > 0 ? ll - 1 : 0);
}

void Buffer::cursorDown(uint32_t cnt)
{
    posY = std::min<uint32_t>(posY + cnt, data.size() - 1);
    uint32_t ll = line().length();
    if (posX >= ll) posX = std::min<uint32_t>(posX, ll > 0 ? ll - 1 : 0);
}

void Buffer::backspaceChars(uint32_t cnt)
{
    if (posX == 0) {
        // FIXME TODO delete from prev line
        return;
    }
    std::string l = line();
    updateLine(substrSafe(l, 0, posX - cnt) + substrSafe(l, posX));

    if (posX <= cnt) posX = 0;
    else posX -= cnt;
}

void Buffer::deleteChars(uint32_t cnt)
{
    std::string l = line();
    updateLine(substrSafe(l, 0, posX) + substrSafe(l, posX + cnt));
}

void Buffer::append(std::string d)
{
    std::string l = line();
    updateLine(substrSafe(l, 0, posX) + d + substrSafe(l, posX));
    posX += d.length();
}

void Buffer::append(char d)
{
    std::string l = line();
    updateLine(substrSafe(l, 0, posX) + d + substrSafe(l, posX));
    ++posX;
}

const std::vector<std::string> Buffer::viewport(uint32_t width, uint32_t height)
{
    std::vector<std::string> res;
    if (posY < row) row = posY;
    if (posY >= (row + height)) row = posY - height + 1;

    for (uint32_t i = 0; i < height; ++i) {
        uint32_t filerow = i + row;
        if (filerow >= data.size()) res.push_back("~");
        else res.push_back(data[filerow]);
    }

    return res;
}
