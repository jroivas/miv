#include "undo.hh"

using editor::UndoableAction;
using editor::UndoNode;
using editor::UndoTree;

UndoableAction::UndoableAction(ActionScope s, ActionType t) :
    scope(s),
    type(t),
    preX(0),
    preY(0),
    postX(0),
    postY(0)
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
    parent(nullptr),
    current(nullptr)
{
}

UndoTree::~UndoTree()
{
    deleteNodes(parent);
}

#include <iostream>
void UndoTree::deleteNodes(UndoNode *node)
{
    if (node == nullptr) return;
    for (UndoNode *n : node->child()) {
        deleteNodes(n);
    }
    delete node;
}

void UndoTree::add(UndoableAction action)
{
    current = new UndoNode(action, current);
    if (parent == nullptr) parent = current;
}

UndoableAction UndoTree::undo()
{
    current = current->getPrev();
    return current->action;
}

UndoableAction UndoTree::redo()
{
    current = current->getNext();
    return current->action;
}

UndoableAction *UndoTree::last()
{
    if (current == nullptr) return nullptr;
    return &current->action;
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

void UndoTree::dump(UndoNode *n) const
{
    if (n == nullptr) return;
    std::cout << "--\n";
    std::cout << "Action:\n";

    std::string scope = "Inline";
    std::cout << " Scope: " << n->action.getScopeName() << "\n";
    std::cout << "  Type: " << n->action.getTypeName() << "\n";
    std::cout << " Start: " << n->action.getPreY() << "," << n->action.getPreX() << " (line,pos)\n";
    std::cout << "   End: " << n->action.getPostY() << "," << n->action.getPostX() << " (line,pos)\n";
    std::cout << " Lines:\n";
    for (std::string l : n->action.getLines()) {
        std::cout << " " << l << "\n";
    }

    for (UndoNode *c : n->child()) {
        dump(c);
    }
}

void UndoTree::dump() const
{
    dump(parent);
}
