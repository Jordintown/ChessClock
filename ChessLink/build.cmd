@echo off
echo ChessLink Compiler
pause
gcc -o chesslink chesslink.c -luser32 -lgdi32 -lkernel32
pause