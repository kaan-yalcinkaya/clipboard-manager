# clipboard-manager
Manage clipboard with command line interface for Windows
# Build
```git clone https://github.com/kaan-yalcinkaya/clipboard-manager.git```<br>
```cd clipboard-manager```<br>
```cmake -B build -G "Visual Studio 17 2022"```<br>
```cmake --build build --config Release```<br>
# Usage
1. copy [-f file]: copyies entered text or text from file to the clipboard.
2. paste [-m mode -f file]: Writes text from the clipboard to the screen or to a file in the specified mode(w/a).
3. clear: cleans the clipboard.
4. list [-m mode -f file]: writes clipboard history to the screen or file in the specified mode(w/a).
5. help: provides informatin about commands.
6. exit: exits the program.
