#include "undo.hh"

using editor::UndoableAction;

UndoableAction::UndoableAction(ActionScope s, ActionType t) :
    scope(s),
    type(t)
{
}

void UndoableAction::setPrePos(uint32_t x, uint32_t y)
{
    preX = x;
    preY = y;
}

void UndoableAction::setPostPos(uint32_t x, uint32_t y)
{
    postX = x;
    postY = y;
}

void UndoableAction::setLines(std::vector<std::string> l)
{
    lines = l;
}

void UndoableAction::addLine(std::string line)
{
    lines.push_back(line);
}
