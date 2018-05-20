#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace editor {

class Buffer
{
public:
    Buffer();
    Buffer(std::string filename);
    ~Buffer();

    static Buffer *getCurrent();
    static bool setCurrent(Buffer *);

    bool readFile(std::string filename);
    bool writeFile(std::string filename);
    bool hasFilename() const {
        return !fileName.empty();
    }
    std::string filename() const {
        return fileName;
    }

    void addLine(std::string line);
    void insertLine(std::string line);
    void updateLine(std::string line);
    void deleteLine(uint32_t cnt = 1);
    void append(std::string line);
    void append(char line);

    const std::string line() const;
    uint32_t lineLength() const;
    uint32_t tabs() const;
    uint32_t tabExtra() const;
    const std::vector<std::string> copyLines(uint32_t cnt = 1) const;
    const std::vector<std::string> copyLinesUp(uint32_t cnt = 1) const;

    void cursorLeft(uint32_t cnt = 1);
    void cursorRight(uint32_t cnt = 1);
    void cursorUp(uint32_t cnt = 1);
    void cursorDown(uint32_t cnt = 1);
    void cursorAppend();
    void pageUp(uint32_t pageSize, uint32_t cnt = 1);
    void pageDown(uint32_t pageSize, uint32_t cnt = 1);

    void cursorWord(uint32_t cnt = 1);
    void cursorWordBack(uint32_t cnt = 1);

    void backspaceChars(uint32_t cnt = 1);
    void deleteChars(uint32_t cnt = 1);

    void relocateRow(uint32_t width, uint32_t height);
    const std::vector<std::string> viewport(uint32_t width, uint32_t height) const;

    uint32_t x() const { return posX; }
    uint32_t y() const { return posY; }
    uint32_t size() const { return data.size(); }
    uint32_t y(uint32_t height) const { return posY - row; }
    bool atEnd() const { return posY == data.size() - 1; }
    void gotoY(uint32_t y = 0);

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
    std::string tabsToSpace(const std::string &d) const;

private:
    std::vector<std::string> data;
    uint32_t posX;
    uint32_t posY;
    uint32_t row;
    uint32_t tabSize;
    bool tabsToSpaces;

    void sanitizePos(bool expand = false);

    std::string spaces(uint32_t cnt) const;
    std::string handleSpecial(std::string line) const;

    std::string lineEnding;
    std::string fileName;
    std::string appendBuffer;

    static void removeBuffer(Buffer *);
    static std::vector<Buffer*> buffers;
    static uint32_t index;
};

}
