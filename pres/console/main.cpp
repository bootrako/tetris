#include <thread>
#include <windows.h>

using namespace std;

int main(int argc, const char** argv)
{
    HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(consoleHandle);

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);

    int screenWidth = screenBufferInfo.dwSize.X;
    int screenHeight = screenBufferInfo.dwSize.Y;

    char* screen = new char[screenWidth * screenHeight];
    memset(screen, 0, screenWidth * screenHeight);

    int speed = 10;
    int framesTillMove = speed;

    int playerX = 5;
    int playerY = 0;

    bool gameRunning = true;
    while (gameRunning)
    {
        this_thread::sleep_for(50ms);

        framesTillMove -= 1;
        if (framesTillMove == 0)
        {
            playerY += 1;
            if (playerY >= screenHeight)
            {
                gameRunning = false;
                break;
            }

            framesTillMove = speed;
        }

        memset(screen, 0, screenWidth * screenHeight);

        screen[playerY * screenWidth + playerX] = 'P';

        DWORD bytesWritten = 0;
        WriteConsoleOutputCharacter(consoleHandle, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    CloseHandle(consoleHandle);
    delete[] screen;

    return 0;
}