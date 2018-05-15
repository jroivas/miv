#include "terminal.hh"
#include "keyhandling.hh"

int main(int argc, char **argv) {

    editor::Terminal term;
    editor::KeyHandling keyHandling;

    term.enableRawMode();
    term.clearScreen();
    term.tildes();

    editor::Status status = editor::Status::OK;
    while (status == editor::Status::OK) {
        status = keyHandling.processKeyPress();
    }

    return 0;
}
