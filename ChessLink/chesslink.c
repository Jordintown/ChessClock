#include <windows.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 400

// Variables globales
char tiempo_jugador_1[16] = "000.0";
char tiempo_jugador_2[16] = "000.0";

// Función para dibujar texto
void DrawTextOnWindow(HDC hdc, int x, int y, const char *text, HFONT font) {
    SelectObject(hdc, font);
    TextOut(hdc, x, y, text, strlen(text));
}

// Función para configurar el puerto serial
HANDLE ConfigurarPuertoSerie(const char *puerto) {
    HANDLE hSerial = CreateFileA(
        puerto,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error al abrir el puerto serie: %ld\n", GetLastError());
        return NULL;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error al obtener configuración del puerto\n");
        CloseHandle(hSerial);
        return NULL;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error al configurar el puerto\n");
        CloseHandle(hSerial);
        return NULL;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    return hSerial;
}

// Bucle principal de la ventana
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HFONT font = CreateFontA(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_SWISS, "Arial");

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);

            // Dibujar los tiempos en la ventana
            DrawTextOnWindow(hdc, 100, 100, tiempo_jugador_1, font);
            DrawTextOnWindow(hdc, 500, 100, tiempo_jugador_2, font);

            DeleteObject(font);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    // Configurar puerto serie
    HANDLE hSerial = ConfigurarPuertoSerie("COM3"); // Cambia "COM3" al puerto correspondiente
    if (!hSerial) {
        return 1;
    }

    // Crear ventana
    const char CLASS_NAME[] = "VentanaTiempos";
    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Tiempos Jugadores",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {
        printf("Error al crear la ventana\n");
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);

    // Bucle de mensajes
    MSG msg = {0};
    DWORD bytesRead;
    char buffer[32];

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) break;

        // Leer datos del puerto serie
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Terminar el string
            sscanf(buffer, "%s %s", tiempo_jugador_1, tiempo_jugador_2);
            InvalidateRect(hwnd, NULL, TRUE); // Redibujar ventana
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hSerial);
    return 0;
}
