#include "undo.hh"

using editor::UndoableAction;
using editor::UndoNode;
using editor::UndoTree;

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

UndoTree::UndoTree() :
    current(nullptr)
{
}

UndoNode::UndoNode(UndoableAction a, UndoNode *p) :
    action(a),
    prev(p)
{
    if (prev != nullptr) {
        prev->newNext(this);
    }
}

void UndoNode::newNext(UndoNode *n)
{
    next.push_back(n);
}

UndoNode *UndoNode::getPrev() const
{
    return prev;
}

UndoNode *UndoNode::getNext() const
{
    if (next.empty()) return nullptr;
    return next.back();
}
