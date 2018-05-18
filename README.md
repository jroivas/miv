# My Improved Vi

Since original [vi](https://en.wikipedia.org/wiki/Vi) editor is quite limited,
there's need for improved version of it.

Yet popular and fairly good alternative is [Vim](https://en.wikipedia.org/wiki/Vim_(text_editor))
but some might say it has too much features, and has it's own limitations.
There's some tries to modernize it, such as [Neovim](https://neovim.io/),
but for my opinion they just make some things too compliated.

My Improved Vi - or MIV - is totally new implementation of Vi kind of editor.
It should be modern, written with modern C++ and targeted for modern systems (latest Linux distributions).
It should work great with my current Vim workflows, and support most of the useful features.


## License

[MIT license](LICENSE)


## Current features

- Normal mode, insert mode
- Basic movement with `h`, `j`, `k`, `l`
- Insert, append `i`, `a`
- New line `o`, `O`
- Delete lines `dd`
- Delete characters `dh`, `dl`
- Copy lines `yy`, `yj`, `yk`
- Copy characters `yh`, `yl`, `x`
- Paste copied line or characters
- Reading files contents to buffer from command line
- Quitting! (`:q`)

## Design

- Utilize C++ strings, vectors, etc. as much as possible
- Separate buffers, terminal handling and key press logic

## TODO

- Multiple buffers
- More basic features
- More commands

## Building

You need [ninja](https://ninja-build.org/) and [meson](http://mesonbuild.com/)

    mkdir build
    cd build
    meson ..
    ninja

After that you have miv binary in your src folder, to start it:

    src/miv

Or to start editing a file:

    src/miv ../src/buffer.cpp
