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

#if 0
    std::cout << &buffer << "\r\n";
    editor::Buffer *tmp = new editor::Buffer();
    std::cout << &tmp << "\r\n";
    std::cout << editor::Buffer::cnt() << "\r\n";
    std::cout << "curr : " << editor::Buffer::getCurrent() << "\r\n";
    std::cout << editor::Buffer::cnt() << "\r\n";
    {
        editor::Buffer b2;
        std::cout << editor::Buffer::cnt() << "\r\n";
        for (int i = 0; i < 5; ++i) {
            std::cout << "nn " << i << ": "<< editor::Buffer::getCurrent() << "\r\n";
            editor::Buffer::next();
        }
        editor::Buffer::next();
        std::cout << "next1: " << editor::Buffer::getCurrent() << "\r\n";
        editor::Buffer::next();
        std::cout << "next2: " << editor::Buffer::getCurrent() << "\r\n";
    }

    std::cout << editor::Buffer::cnt() << "\r\n";
    delete tmp;
    std::cout << editor::Buffer::cnt() << "\r\n";
#endif

    return 0;
}
