#include "terminal.hh"
#include "keyhandling.hh"
#include <iostream>

int main(int argc, char **argv) {

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

    return 0;
}
