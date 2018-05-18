#include "terminal.hh"
#include "keyhandling.hh"
#include "buffer.hh"
#include <iostream>

int main(int argc, char **argv)
{
    std::string src;
    if (argc > 1) src = argv[1];
    editor::Buffer buffer(src);
    editor::Terminal *term = editor::Terminal::get();
    editor::KeyHandling keyHandling;

    term->enableRawMode();
    term->clearScreen();
    term->tildes();

    editor::Status status = editor::Status::OK;
    while (status == editor::Status::OK) {
        term->refresh();
        status = keyHandling.processKeyPress();
    }
    term->clearScreen();
    term->flush();

    return 0;
}
