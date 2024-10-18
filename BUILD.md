supercrack build documentation
=

### Revision ###

* 1.6

### Author ###

See docs/credits.txt

### Building supercrack for Linux and macOS ###

Get a copy of the **supercrack** repository

```
$ git clone https://github.com/supercrack/supercrack.git
```

Run "make"

```
$ make
```

### Install supercrack for Linux ###

The install target is linux FHS compatible and can be used like this:

```
$ make install
```

If the $HOME/.supercrack folder exists, then:

- Session related files go to: $HOME/.supercrack/sessions/
- Cached kernels go to: $HOME/.supercrack/kernels/
- Potfiles go to: $HOME/.supercrack/

Otherwise, if environment variable XDG_DATA_HOME and XDG_CACHE_HOME exists, then:

- Session related files go to: $XDG_DATA_HOME/supercrack/sessions/
- Cached kernels go to: $XDG_CACHE_HOME/supercrack/kernels/
- Potfiles go to: $XDG_DATA_HOME/supercrack/

Otherwise, if environment variable XDG_DATA_HOME exists, then:

- Session related files go to: $XDG_DATA_HOME/supercrack/sessions/
- Cached kernels go to: $HOME/.cache/supercrack
- Potfiles go to: $XDG_DATA_HOME/supercrack/

Otherwise, if environment variable XDG_CACHE_HOME exists, then:

- Session related files go to: $HOME/.local/share/supercrack/sessions/
- Cached kernels go to: $XDG_CACHE_HOME/supercrack/kernels/
- Potfiles go to: $HOME/.local/share/supercrack/

Otherwise:

- Session related files go to: $HOME/.local/share/supercrack/sessions/
- Cached kernels go to: $HOME/.cache/supercrack
- Potfiles go to: $HOME/.local/share/supercrack/

### Building supercrack for Windows (using macOS) ###

Refer to [BUILD_macOS.md](BUILD_macOS.md)

### Building supercrack for Windows (using Windows Subsystem for Linux) ###

Refer to [BUILD_WSL.md](BUILD_WSL.md)

### Building supercrack for Windows (using Cygwin) ###

Refer to [BUILD_CYGWIN.md](BUILD_CYGWIN.md)

### Building supercrack for Windows (using MSYS2) ###

Refer to [BUILD_MSYS2.md](BUILD_MSYS2.md)

### Building supercrack for Windows from Linux ###

```
$ make win
```

=
Enjoy your fresh **supercrack** binaries ;)
