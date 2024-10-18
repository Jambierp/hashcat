# Compiling supercrack for Windows with Windows Subsystem for Linux.

Tested on Windows 10 x64, should also work to build supercrack for Windows on Linux.

I had it tested with WSL2 using Ubuntu_2004.2020.424.0_x64.appx.

Make sure to have the system upgraded after install (otherwise it will fail to find the gcc-mingw-w64-x86-64 package).

### Installation ###

Enable WSL.

Press the win + r key on your keyboard simultaneously and in the "Run" popup window type bash and make sure to install additional dependencies necessary for supercrack compilation
```
sudo apt install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 make git
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

The process may take a while, please be patient. Once it's finished, close WSL.
Press the win + r keys together and (in the "Run" popup window) type cmd, using cd navigate to the supercrack folder e.g.
```
cd "C:\Users\user\supercrack"
```
and start supercrack by typing
```
supercrack.exe
```
