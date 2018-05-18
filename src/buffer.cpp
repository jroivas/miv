#include "buffer.hh"
#include "tools.hh"
#include <algorithm>

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

Buffer::~Buffer()
{
    for (size_t i = 0; i < buffers.size(); ++i) {
        if (buffers[i] == this) {
            if (i > 0) index = i - 1;
            else index = 0;
            buffers.erase(buffers.begin() + i);
            break;
        }
    }
}

Buffer *Buffer::getCurrent()
{
    if (buffers.size() == 0) return nullptr;
    return buffers[index];
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

void Buffer::cursorLeft(uint32_t cnt)
{
    if (cnt >= posX) posX = 0;
    else posX -= cnt;
}

void Buffer::cursorRight(uint32_t cnt)
{
    posX = std::min<uint32_t>(posX + cnt, line().length() - 1);
}

void Buffer::cursorUp(uint32_t cnt)
{
    if (cnt >= posY) posY = 0;
    else posY -= cnt;
    uint32_t ll = line().length();
    if (posX >= ll) posX = ll;
}

void Buffer::cursorDown(uint32_t cnt)
{
    posY = std::min<uint32_t>(posY + cnt, data.size() - 1);
    uint32_t ll = line().length();
    if (posX >= ll) posX = ll;
}

void Buffer::backspaceChars(uint32_t cnt)
{
    if (posX == 0) {
        // FIXME delete from prev line
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

const std::string Buffer::viewport(uint32_t width, uint32_t height) const
{
    std::string res;
    if (data.size() == 0) return res;
    uint32_t target = std::min<uint32_t>(height, data.size());
    for (uint32_t i = 0; i < target; ++i) {
        //TODO handle row
        res += data[i];
        if (i < target - 1) {
            res += "\r\n";
        }
    }

    return res;
}
