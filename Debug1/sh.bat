@echo off
setlocal

rem Kiểm tra xem chương trình client đã được biên dịch hay chưa
set CLIENT_EXEC=client.exe
if not exist "%CLIENT_EXEC%" (
    echo Client executable not found. Please compile your client code first.
    exit /b 1
)

rem Chạy 10 client với các ID khác nhau, mỗi client trong một cửa sổ Command Prompt riêng
for /L %%i in (1,1,4) do (
    echo Starting client with ID %%i in a new terminal...
    start cmd /k "%CLIENT_EXEC% %%i"
)

echo All clients started.
endlocal
