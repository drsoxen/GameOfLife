#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>
#include <cstdlib>
#include <functional>
#include <time.h>

static struct {
    int w, h, c;   // width/height/count(w*h)
    HDC hdc,       // window device context
        odc;       // off screen device context
} App;

constexpr auto LiveColor = RGB(255, 255, 255);
constexpr auto DeadColor = RGB(0,0,0);

const float scaleFactor = 2;

bool *currentArray;
bool *resultArray;

int getCellState(const int& x, const int& y)
{
    if (x < 0 || y < 0 || x > App.w - 3 || y > App.h - 3)
    {
        return false;
    }

    return (int)currentArray[x * App.h + y];
}

bool getAlive(const int& originX, const int& originY, const bool alive)
{
    int neighbourCount = 0;

    neighbourCount += getCellState(originX - 1, originY - 1);
    neighbourCount += getCellState(originX, originY - 1);
    neighbourCount += getCellState(originX + 1, originY - 1);

    neighbourCount += getCellState(originX - 1, originY);
    neighbourCount += getCellState(originX + 1, originY);

    neighbourCount += getCellState(originX - 1, originY + 1);
    neighbourCount += getCellState(originX, originY + 1);
    neighbourCount += getCellState(originX + 1, originY + 1);

    if (alive)
    {
        return neighbourCount == 2 || neighbourCount == 3 ? true : false;
    }
    else
    {
        return neighbourCount == 3 ? true : false;
    }
}

void Tick()
{
    for (int x = 0; x < App.w; x++)
    {
        for (int y = 0; y < App.h; y++)
        {
            resultArray[x * App.h + y] = getAlive(x, y, currentArray[x * App.h + y]);

            if (resultArray[x * App.h + y] != currentArray[x * App.h + y])
            {
                SetPixel(App.odc, x, y, resultArray[x * App.h + y] ? LiveColor : DeadColor);
            }
        }
    }

    memcpy(currentArray, resultArray, App.w * App.h);

    StretchBlt(App.hdc, 0, 0, App.w * scaleFactor, App.h * scaleFactor, App.odc, 0, 0, App.w, App.h, SRCCOPY);
}

int main(int argsc, char** argsv) {
    HWND hwnd;
    App.hdc = GetDC(hwnd = GetConsoleWindow());
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    App.w = (int)(clientRect.right - clientRect.left) / scaleFactor;
    App.h = (int)(clientRect.bottom - clientRect.top) / scaleFactor;
    App.odc = CreateCompatibleDC(App.hdc);
    HBITMAP obm = CreateCompatibleBitmap(App.hdc, App.w, App.h);
    SelectObject(App.odc, obm);
    SelectObject(App.odc, GetStockObject(NULL_PEN));
    SelectObject(App.odc, GetStockObject(BLACK_BRUSH));

    currentArray = new bool[App.w * App.h];
    resultArray = new bool[App.w * App.h];

    srand(time(0));

    for (int i = 0; i < App.w * App.h; i++)
    {
        currentArray[i] = rand() % 2;
    }

    Rectangle(App.odc, 0, 0, App.w, App.h);
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        Tick();
    }

    delete[] currentArray;
    delete[] resultArray;

    DeleteObject(obm);
    DeleteDC(App.odc);
    return 0;
}