#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace editor {

class Buffer
{
public:
    Buffer();
    ~Buffer();

    static Buffer *getCurrent();

    void addLine(std::string line);
    void insertLine(std::string line);
    void updateLine(std::string line);
    void deleteLine(uint32_t cnt = 1);
    void append(std::string line);
    void append(char line);
    const std::string line() const;

    void cursorLeft(uint32_t cnt = 1);
    void cursorRight(uint32_t cnt = 1);
    void cursorUp(uint32_t cnt = 1);
    void cursorDown(uint32_t cnt = 1);

    void backspaceChars(uint32_t cnt = 1);
    void deleteChars(uint32_t cnt = 1);

    const std::string viewport(uint32_t width, uint32_t height) const;

    uint32_t x() const { return posX; }
    uint32_t y() const { return posY; }
    bool atEnd() { return posY == data.size() - 1; }

    static uint32_t cnt() {
        return buffers.size();
    }

    static void prev() {
        if (buffers.size() == 0) return;
        if (index == 0) index = buffers.size() - 1;
        else index = index - 1;
    }

    static void next() {
        if (buffers.size() == 0) return;
        index = (index + 1) % buffers.size();
    }

    static Buffer* newBuffer() {
        return new Buffer();
    }

private:
    std::vector<std::string> data;
    uint32_t posX;
    uint32_t posY;
    uint32_t row;

    static std::vector<Buffer*> buffers;
    static uint32_t index;
};

}
