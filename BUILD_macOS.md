# Compiling supercrack for Windows with macOS.

Tested on macOS 12.6.6 M1.

Make sure to have the HomeBrew upgraded.

### Installation ###

```
brew install mingw-w64
git clone https://github.com/supercrack/supercrack
git clone https://github.com/win-iconv/win-iconv
cd win-iconv/
patch < ../supercrack/tools/win-iconv-64.diff
sudo make install
cd ../
```

### Building ###

You've already cloned the latest master revision of supercrack repository above, so switch to the folder and type "make win" to start compiling supercrack
```
cd supercrack/
make win
```

The process may take a while, please be patient.
