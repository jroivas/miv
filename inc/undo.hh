#pragma once

#include <cstdint>

namespace editor {

enum class ActionScope
{
    Inline,
    MultiLine
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

private:
    ActionScope scope;
    ActionType type;

    uint32_t preX;
    uint32_t preY;
    uint32_t postX;
    uint32_t postY;
};

}
