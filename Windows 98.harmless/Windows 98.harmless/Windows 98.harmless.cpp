#pragma comment(lib, "winmm.lib")
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Глобальные переменные размеров экрана
int screenWidth = 0; int screenHeight = 0;

void RefreshScreenClean() { InvalidateRect(NULL, NULL, TRUE); UpdateWindow(GetDesktopWindow()); }

COLORREF ConvertHslToRgb(float h, float s, float l) {
    if (s == 0) return RGB((int)(l * 255), (int)(l * 255), (int)(l * 255));
    float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    float p = 2.0f * l - q;
    float t[3] = { h + 1.0f / 3.0f, h, h - 1.0f / 3.0f };
    int rgb[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; i++) {
        if (t[i] < 0) t[i] += 1.0f;
        if (t[i] > 1.0f) t[i] -= 1.0f;
        if (t[i] < 1.0f / 6.0f) rgb[i] = (int)((p + (q - p) * 6.0f * t[i]) * 255);
        else if (t[i] < 1.0f / 2.0f) rgb[i] = (int)(q * 255);
        else if (t[i] < 2.0f / 3.0f) rgb[i] = (int)((p + (q - p) * (2.0f / 3.0f - t[i]) * 6.0f) * 255);
        else rgb[i] = (int)(p * 255);
    }
    return RGB(rgb[0], rgb[1], rgb[2]);
}

// Глобальные переменные для анимации и графики
int progressPercent = 0;
HBITMAP hBackgroundBmp = NULL;

// 1. ФУНКЦИЯ ОТРИСОВКИ ИНТЕРФЕЙСА (Картинка на весь экран + Шкала по центру + Проценты СТРОГО В ЦЕНТРЕ ПОЛОСЫ)
void DrawInterface(HWND hWnd, HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    int screenWidth = rect.right;
    int screenHeight = rect.bottom;

    // Растягиваем Windows.bmp на весь экран
    if (hBackgroundBmp != NULL) {
        HDC hMemDC = CreateCompatibleDC(hdc);
        SelectObject(hMemDC, hBackgroundBmp);
        BITMAP bmp;
        GetObject(hBackgroundBmp, sizeof(BITMAP), &bmp);
        StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
        DeleteDC(hMemDC);
    }
    else {
        HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, hBlackBrush);
        DeleteObject(hBlackBrush);
    }

    // РАСЧЕТ ЦЕНТРИРОВАНИЯ ШКАЛЫ
    int barWidth = 400;                      // Ширина полосы прогресса в пикселях
    int barHeight = 30;                      // Высота полосы
    int barX = (screenWidth - barWidth) / 2; // Центр экрана по горизонтали
    int barY = (screenHeight - barHeight) / 2 + 100; // Центр экрана по вертикали с небольшим смещением вниз

    // Отрисовка пустой (фоновой) части шкалы — цвет #a8a8a8
    HBRUSH hEmptyBrush = CreateSolidBrush(RGB(168, 168, 168));
    RECT emptyRect = { barX, barY, barX + barWidth, barY + barHeight };
    FillRect(hdc, &emptyRect, hEmptyBrush);
    DeleteObject(hEmptyBrush);

    // Отрисовка Facebook/Windows синей части — цвет #0000a8
    int currentBarWidth = (barWidth * progressPercent) / 100;
    HBRUSH hBlueBrush = CreateSolidBrush(RGB(0, 0, 168));
    RECT progressRect = { barX, barY, barX + currentBarWidth, barY + barHeight };
    FillRect(hdc, &progressRect, hBlueBrush);
    DeleteObject(hBlueBrush);

    // ЛОГИКА ДЛЯ РАМКИ: Черная до 100%, белая при 100%
    HBRUSH hFrameBrush;
    if (progressPercent >= 100) {
        hFrameBrush = CreateSolidBrush(RGB(255, 255, 255)); // Белая рамка при касании края
    }
    else {
        hFrameBrush = CreateSolidBrush(RGB(0, 0, 0));       // Черная рамка во время загрузки
    }
    RECT frameRect = { barX, barY, barX + barWidth, barY + barHeight };
    FrameRect(hdc, &frameRect, hFrameBrush);
    DeleteObject(hFrameBrush);

    // Создаем шрифты для текста
    HFONT hTitleFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Arial");

    // Создаем чёткий жирный шрифт для процентов внутри полосы
    HFONT hPercentFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Arial");

    // --- 1. ВЫВОД ВЕРХНЕГО ТЕКСТА (СТАТИЧНЫЙ, ЧЕРНЫЙ) ---
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); // Чисто черный цвет текста для облаков
    HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);

    std::wstring titleText = L"Windows is installing updates...";
    RECT textRect = { barX, barY - 50, barX + barWidth, barY }; // Прямоугольник строго над шкалой
    DrawText(hdc, titleText.c_str(), -1, &textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

    // --- 2. ВЫВОД ПРОЦЕНТОВ СТРОГО В ЦЕНТРЕ ПОЛОСЫ (БЕЛАЯ ЦИФРА) ---
    SelectObject(hdc, hPercentFont);
    SetTextColor(hdc, RGB(255, 255, 255)); // Белый цвет текста, чтобы выделялся на синей полосе

    wchar_t percentBuffer[16];
    wsprintf(percentBuffer, L"%d%%", progressPercent);

    // Прямоугольник текста идеально совпадает с границами всей шкалы
    RECT percentRect = { barX, barY, barX + barWidth, barY + barHeight };

    // Магические флаги: DT_CENTER (по центру X) + DT_VCENTER (по центру Y) + DT_SINGLELINE
    DrawText(hdc, percentBuffer, -1, &percentRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Очищаем созданные шрифты из оперативной памяти
    SelectObject(hdc, hOldFont);
    DeleteObject(hTitleFont);
    DeleteObject(hPercentFont);
}

// 2. ОБРАБОТЧИК СОБЫТИЙ ОКНА (Логика таймеров, фокуса и выхода)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Загружаем картинку из ресурсов по её ID (101 - стандартный ID вашей Windows.bmp)
        hBackgroundBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(101));

        // Таймер 1: срабатывает каждые 3000 миллисекунд (3 секунды) для процентов
        SetTimer(hWnd, 1, 3000, NULL);

        // Таймер 2: удерживает фокус на окне
        SetTimer(hWnd, 2, 1000, NULL);
        return 0;

    case WM_TIMER:
        if (wParam == 2 || wParam == 1) {
            SetForegroundWindow(hWnd);
            SetActiveWindow(hWnd);
            SetFocus(hWnd);
        }

        // Логика движения шкалы
        if (wParam == 1) {
            if (progressPercent < 99) {
                progressPercent += 1; // Двигаем на 1% каждые 3 секунды
                InvalidateRect(hWnd, NULL, FALSE); // Перерисовываем экран
            }
            // Дошли до 99% — останавливаем рост и выдаем ошибку
            else if (progressPercent == 99) {
                KillTimer(hWnd, 1); // Отключаем таймер прогресса

                // Выводим окно ошибки
                MessageBoxW(hWnd,
                    L"System updating is failing, setup cannot copy IO.SYS.",
                    L"Windows Setup Error",
                    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

                // После того как нажали ОК — доводим шкалу до 100%
                progressPercent = 100;
                InvalidateRect(hWnd, NULL, FALSE); // Перерисовка (рамка станет белой!)

                // Запускаем Таймер 3 на безопасный выход из программы через 1 секунду
                SetTimer(hWnd, 3, 1000, NULL);
            }
        }

        // Таймер 3: Действие при завершении прогресса (Закрытие программы вместо BSOD)
        if (wParam == 3) {
            KillTimer(hWnd, 3);
            KillTimer(hWnd, 2);
            DestroyWindow(hWnd);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawInterface(hWnd, hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (hBackgroundBmp) DeleteObject(hBackgroundBmp);
        KillTimer(hWnd, 2);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"Win98SimpleFullscreen";

    RegisterClassEx(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Стили WS_POPUP & WS_EX_TOPMOST разворачивают окно без рамок поверх всего
    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST, L"Win98SimpleFullscreen", L"",
        WS_POPUP | WS_VISIBLE,
        0, 0, screenWidth, screenHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd) return 0;

    // Цикл обработки сообщений Windows
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
