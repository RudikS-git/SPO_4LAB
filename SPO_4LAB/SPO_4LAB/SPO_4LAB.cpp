#include "framework.h"
#include "SPO_4LAB.h"
#include "AnswerInfo.h"
#include <cmath>
#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

int Width, Height;
const int MARGIN = 100;
HFONT hFont;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void SetWindowSize(int _Width, int _Height);

// Вывести график функции, точки приближения на графике, количество итераций, значение корня.
double Function(double x);
AnswerInfo SolveEquation(HDC& hdc, double a, double b, double epsilon, double nPixPerX, double nPixPerY, double kMidX, double kMidY);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPO4LAB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPO4LAB));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPO4LAB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;//COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SPO4LAB);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            hFont = CreateFontA(20, 10, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, "Times New Roman");
        }
        break;

    case WM_SIZE:
        {
            RECT Rect;
            GetClientRect(hWnd, &Rect);
            SetWindowSize(Rect.right - Rect.left, Rect.bottom - Rect.top);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT Rect;
            HDC hdc, hCmpDC;
            HBITMAP hBmp;

            GetClientRect(hWnd, &Rect);
            hdc = BeginPaint(hWnd, &ps);

            hCmpDC = CreateCompatibleDC(hdc);

            COLORREF color = 0x00FFFFFF;
            SetTextColor(hdc, color);

            COLORREF colorBk = 0x00292626;
            SetBkColor(hdc, colorBk);

            hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
            SelectObject(hCmpDC, hBmp);

            LOGBRUSH br;
            br.lbStyle = BS_SOLID;
            br.lbColor = 0x00292626;
            HBRUSH brush;
            brush = CreateBrushIndirect(&br);// кисть
            FillRect(hCmpDC, &Rect, brush);
            DeleteObject(brush);

            // Копируем изображение из теневого контекста на экран
            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top,
                hCmpDC, 0, 0, SRCCOPY);

            // Удаляем ненужные системные объекты
            DeleteDC(hCmpDC);
            DeleteObject(hBmp);
            hCmpDC = NULL;

            LPPOINT lppt;
            HPEN hpen = CreatePen(PS_SOLID, 1, RGB(117, 117, 117)); // кисть для оси, сетка
            SelectObject(hdc, hpen);

            int xMin = 1;
            int xMax = 3;
            double t = xMin; // step
            double y;

            int yMin = -200;
            int yMax = 200;

            double nPixPerX = (double)(Width - 2 * MARGIN) / (xMax - xMin);
            double nPixPerY = (double)(Height - 2 * MARGIN) / (yMax - yMin);

            double kMidX;
            double kMidY;

            // GRID
            const double stepX = 0.1; // 0.1 - шаг
            int gridStep = stepX * nPixPerX;

            const int stepY = 40; // 40 - шаг
            int gridStepY = stepY * nPixPerY;

            if (gridStep == 0 || gridStepY == 0)
            {
                break;
            }

            int countGridHeight = ((Width - 2 * MARGIN) / gridStep); // count grid x
            int countGridWidth = ((Height - 2 * MARGIN) / gridStepY); // count grid y
            std::string text;

            if (yMin < 0 && yMax > 0)
            {
                kMidY = -(double)yMin / (yMax - yMin);

                // стрелки OX
                MoveToEx(hdc, (Width - MARGIN), MARGIN + (Height - 2 * MARGIN) * kMidY, NULL);
                LineTo(hdc, (Width - MARGIN) - 5, (MARGIN + (Height - 2 * MARGIN) * kMidY) + 5);
                MoveToEx(hdc, (Width - MARGIN), MARGIN + (Height - 2 * MARGIN) * kMidY, NULL);
                LineTo(hdc, (Width - MARGIN) - 5, (MARGIN + (Height - 2 * MARGIN) * kMidY) - 5);
                
                int cordMid = MARGIN + (Height - 2 * MARGIN) * abs(kMidY);
                // GRID Y RIGHT
                for (double i = stepY, j = 1; i <= yMax; i += stepY, j++) // y [-*,+*]
                {
                    MoveToEx(hdc, MARGIN, gridStepY * -j + cordMid, NULL);
                    LineTo(hdc, Width - MARGIN, gridStepY * -j + cordMid);

                    text = std::to_string(round(i * 10) / 10);
                    const char* _text = text.c_str();
                    TextOutA(hdc, MARGIN - 30, gridStepY * -j + cordMid, _text, 3);
                }

                // GRID Y LEFT
                for (double i = 0, j = 0; i >= yMin; i -= stepY, j++) // y [-*,+*]
                {
                    MoveToEx(hdc, MARGIN, gridStepY * j + cordMid, NULL);
                    LineTo(hdc, Width - MARGIN, gridStepY * j + cordMid);

                    text = std::to_string(round(i * 10) / 10);
                    const char* _text = text.c_str();
                    TextOutA(hdc, MARGIN - 30,
                        gridStepY * j + cordMid, _text, 4);
                    
                }
            }
            else
            {
                kMidY = abs((double)yMin / (yMax - yMin));

                if (kMidY == 1)
                {
                    kMidY = 0;
                }
                else if (kMidY == 0)
                {
                    kMidY = 1;
                }

                // стрелки OX
                if (yMin < 0 && yMax <= 0) // верху ox
                {
                    MoveToEx(hdc, (Width - MARGIN), MARGIN, NULL);
                    LineTo(hdc, (Width - MARGIN) - 5, MARGIN + 5);
                    MoveToEx(hdc, (Width - MARGIN), MARGIN, NULL);
                    LineTo(hdc, (Width - MARGIN) - 5, MARGIN - 5);
                }
                else // внизу ox
                {
                    MoveToEx(hdc, (Width - MARGIN), (Height - MARGIN), NULL);
                    LineTo(hdc, (Width - MARGIN) - 5, (Height - MARGIN) + 5);
                    MoveToEx(hdc, (Width - MARGIN), (Height - MARGIN), NULL);
                    LineTo(hdc, (Width - MARGIN) - 5, (Height - MARGIN) - 5);
                }

                for (int i = yMax, j = 0; i >= yMin; i -= stepY, j++) // y [-*,+*]
                {
                    MoveToEx(hdc, MARGIN, MARGIN + j * gridStepY, NULL);
                    LineTo(hdc, (Width - MARGIN), MARGIN + j * gridStepY);
                    text = std::to_string(i);
                    const char* _text = text.c_str();
                    TextOutA(hdc, MARGIN - 30,
                        MARGIN + j * gridStepY - 5, _text, strlen(_text));
                }
            }

            if (xMin >= 0 || xMax <= 0)
            {
                kMidX = -(double)xMin / (xMax - xMin);
                
                if (yMax > 0)
                {
                    // стрелки OY
                    MoveToEx(hdc, MARGIN, MARGIN, NULL);
                    LineTo(hdc, MARGIN - 5, MARGIN + 5);
                    MoveToEx(hdc, MARGIN, MARGIN, NULL);
                    LineTo(hdc, MARGIN + 5, MARGIN + 5); 
                }
                else
                {
                    // стрелки OY
                    MoveToEx(hdc, MARGIN, MARGIN, NULL);
                    LineTo(hdc, MARGIN - 5, MARGIN + 5);
                    MoveToEx(hdc, MARGIN, MARGIN, NULL);
                    LineTo(hdc, MARGIN + 5, MARGIN + 5);
                }
                

                // GRID
                for (int i = 0; i <= countGridHeight; i++) // x [-*, -*] or [+*, +*]
                {
                    MoveToEx(hdc, MARGIN + gridStep * i, MARGIN, NULL);
                    LineTo(hdc, MARGIN + gridStep * i, Height - MARGIN);

                    double value = round(((gridStep * i) / nPixPerX + xMin) * 10) / 10;
                    text = std::to_string(value);
                    const char* _text = text.c_str();
                    TextOutA(hdc, MARGIN + gridStep * i,
                                  Height - MARGIN + 5, _text, 3);

                }
            }
            else
            {
                kMidX = abs((double)xMin / (xMax - xMin));

                // стрелки OY
                MoveToEx(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX, MARGIN, NULL);
                LineTo(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX - 5, MARGIN + 5);

                MoveToEx(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX, MARGIN, NULL);
                LineTo(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX + 5, MARGIN + 5);

                int cordMid = MARGIN + (Width - 2 * MARGIN) * abs(kMidX);
                // GRID X RIGHT
                for (double i = stepX, j = 1; i <= xMax + stepX; i += stepX, j++) // x [-*, +*]
                {
                    MoveToEx(hdc, gridStep * j + cordMid, MARGIN, NULL);
                    LineTo(hdc, gridStep * j + cordMid, Height - MARGIN);

                    text = std::to_string(round(i * 10) / 10);
                    const char* _text = text.c_str();
                    TextOutA(hdc, gridStep * j + cordMid,
                        Height - MARGIN + 5, _text, 3);
                }

                // GRID X LEFT
                for (double i = 0, j = 0; i >= xMin; i -= stepX, j++) // x [-*, +*]
                {
                    MoveToEx(hdc, gridStep* -j + cordMid, MARGIN, NULL);
                    LineTo(hdc, gridStep* -j + cordMid, Height - MARGIN);

                    text = std::to_string(round(i * 10) / 10);
                    const char* _text = text.c_str();
                    TextOutA(hdc, gridStep * -j + cordMid,
                        Height - MARGIN + 5, _text, 4);
                }
            }

            //OY
            MoveToEx(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX, MARGIN, NULL);
            LineTo(hdc, MARGIN + (Width - 2 * MARGIN) * kMidX, (Height - MARGIN));
            // OX
            MoveToEx(hdc, MARGIN, MARGIN + (Height - 2 * MARGIN) * kMidY, NULL);
            LineTo(hdc, (Width - MARGIN), MARGIN + (Height - 2 * MARGIN) * kMidY);

            DeleteObject(hpen);

            HPEN hpenGraph = CreatePen(PS_SOLID, 1, RGB(30, 144, 255));
            SelectObject(hdc, hpenGraph);

            MoveToEx(hdc, MARGIN, 
                          MARGIN + ((Height - 2 * MARGIN) * kMidY) - Function(t) * nPixPerY, NULL);
            
            do
            {
                y = Function(t);

                int cordX = MARGIN + t * nPixPerX + (Width - 2 * MARGIN) * kMidX;
                int cordY = MARGIN + ((Height - 2 * MARGIN) * kMidY) - y * nPixPerY;

                if ((cordX >= MARGIN && cordX <= Width - MARGIN) && (cordY >= MARGIN && cordY <= Height - MARGIN))
                {
                    LineTo(hdc, cordX, cordY);
                }

                t += 0.01;
            } 
            while (t <= xMax);

            DeleteObject(hpenGraph);
            
            //============================================================================
            const double epsilon = 0.01;
            const int a = 1;
            const int b = 3;

            AnswerInfo ansInfo = SolveEquation(hdc, a, b, epsilon, nPixPerX, nPixPerY, kMidX, kMidY);

            SelectObject(hdc, hFont);

            std::string textInfo = "Интервал: [" + std::to_string(a) + ',' + std::to_string(b) + ']';
            const char* textInterval = textInfo.c_str();
            TextOutA(hdc, 10, 5, textInterval, strlen(textInterval));

            textInfo = "Точность: " + std::to_string(epsilon);
            const char* textEpsilon = textInfo.c_str();
            TextOutA(hdc, 10, 25, textEpsilon, strlen(textEpsilon));

            textInfo = "Приблизительный ответ: " + std::to_string(ansInfo.value);
            const char* textValue = textInfo.c_str();
            TextOutA(hdc, 10, 45, textValue, strlen(textValue));

            textInfo = "Кол-во итераций: " + std::to_string(ansInfo.N);
            const char* textCountIter = textInfo.c_str();
            TextOutA(hdc, 10, 65, textCountIter, strlen(textCountIter));
            //============================================================================


            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void SetWindowSize(int _Width, int _Height) 
{
    Width = _Width;
    Height = _Height;
}

double Function(double x)
{
    return pow(x, 3) - 5 * pow(x, 2) - 4 * x + 19;
}

AnswerInfo SolveEquation(HDC &hdc, double a, double b, double epsilon, double nPixPerX, double nPixPerY, double kMidX, double kMidY)
{
    int N = 0;
    double c;

    AnswerInfo answerInfo;

    if (Function(a) * Function(b) > 0) // на данном интервале отсутствуют корни
    {
        answerInfo.value = -1;
        return answerInfo;
    }

    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(hdc, brush);

    HPEN hpenGraph = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    SelectObject(hdc, hpenGraph);

    int halfHeight = MARGIN + ((Height - 2 * MARGIN) * kMidY);
    int halfWidth;

    halfWidth = (Width - 2 * MARGIN) * kMidX;

    while (b - a > epsilon)
    {
        c = (a + b) / 2;
      /*  SetPixel(
            hdc,
            MARGIN + c * nPixPerX - (Width - 2 * MARGIN) / 2,
            MARGIN + ((Height - 2 * MARGIN) / 2) - Function(c) * nPixPerY,
            RGB(255, 0, 0)
        );*/

        double y = Function(c);
        double _height = (halfHeight - y * nPixPerY);
        double _width = MARGIN + c * nPixPerX + halfWidth;

        Ellipse(hdc,
            _width - 4,
            _height - 4,
            _width + 4,
            _height + 4
            );


        if (Function(a) * Function(c) < 0)
        {
            b = c;
        }
        else
        {
            a = c;
        }

        N++;
    }

    answerInfo.value = (a + b) / 2;
    answerInfo.N = N;

    DeleteObject(brush);
    DeleteObject(hpenGraph);

    return answerInfo;
}
