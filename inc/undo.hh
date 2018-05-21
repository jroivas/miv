#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace editor {

enum class ActionScope
{
    Inline,
    MultiLine,
    InsertMode
};

enum class ActionType
{
    Addition,
    Removal,
    Both
};

class UndoableAction
{
public:
    UndoableAction(ActionScope scope, ActionType type);
    void setPrePos(uint32_t x, uint32_t y);
    void setPostPos(uint32_t x, uint32_t y);
    void addLine(std::string line);
    void setLines(std::vector<std::string> line);

private:
    ActionScope scope;
    ActionType type;

    uint32_t preX;
    uint32_t preY;
    uint32_t postX;
    uint32_t postY;

    std::vector<std::string> lines;
};

class UndoNode
{
public:
    UndoNode(UndoableAction action, UndoNode *prev);

    void newNext(UndoNode *node);

    UndoableAction action;
    UndoNode *getNext() const;
    UndoNode *getPrev() const;
    std::vector<UndoNode*> child() { return next; }

private:
    std::vector<UndoNode*> next;
    UndoNode *prev;
};

class UndoTree
{
public:
    UndoTree();
    ~UndoTree();

    void add(UndoableAction action);
    UndoableAction *last();
    UndoableAction undo();
    UndoableAction redo();

private:
    void deleteNodes(UndoNode *n);
    UndoNode *parent;
    UndoNode *current;
};

}
