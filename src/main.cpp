#include "windows/mainwindow.h"
#include "util/log.h"
#include <QApplication>
#include <QTimer>
#include <iostream>
#include "common/default.h"

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>

LONG WINAPI crashHandler(EXCEPTION_POINTERS *ep)
{
    HANDLE file = CreateFileA("crash.dmp", GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION info{GetCurrentThreadId(), ep, FALSE};
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                          file, MiniDumpNormal, &info, nullptr, nullptr);
        CloseHandle(file);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

static void attachConsole()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
}

static void nudgeConsolePrompt()
{
    HANDLE hCon = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hCon == INVALID_HANDLE_VALUE)
        return;
    INPUT_RECORD ir[2] = {};
    ir[0].EventType = KEY_EVENT;
    ir[0].Event.KeyEvent.bKeyDown = TRUE;
    ir[0].Event.KeyEvent.wRepeatCount = 1;
    ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    ir[0].Event.KeyEvent.uChar.AsciiChar = '\r';
    ir[1] = ir[0];
    ir[1].Event.KeyEvent.bKeyDown = FALSE;
    DWORD written;
    WriteConsoleInputA(hCon, ir, 2, &written);
    CloseHandle(hCon);
}
#endif

static void printHelp(const char *argv0)
{
    std::cout << "Heathkit ET-3400 Emulator\n"
              << std::endl;
    std::cout << "Usage: " << argv0 << " [options] [file]\n"
              << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -m <path>            Load monitor ROM from file" << std::endl;
    // std::cout << "  -a <start address>   Set start address for execution (hex)" << std::endl;
    std::cout << "  -s <speed>           Set clock speed:" << std::endl;
    std::cout << "                          n% - Percent of default clock speed (" << (DEFAULT_CLOCK_RATE / 1000) << "KHz)" << std::endl;
    std::cout << "                          n[k|M]Hz - speed in Hz, kHz or MHz" << std::endl;
    std::cout << "  -d                   Show debugger on startup" << std::endl;
    std::cout << "  -l <path>            Load labels from file" << std::endl;
#ifdef _DEBUG
    std::cout << "  --log-level <level>  Set log level (error, warn, info, debug)" << std::endl;
#endif
    std::cout << "  <file>               Load RAM contents from file" << std::endl;
    std::cout.flush();
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    SetUnhandledExceptionFilter(crashHandler);
    timeBeginPeriod(1);
#endif

    // Handle --help and --log-level before constructing any Qt objects
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
#ifdef _WIN32
            attachConsole();
#endif
            printHelp(argv[0]);
#ifndef _DEBUG
            nudgeConsolePrompt();
#endif
            return 0;
        }
        if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc)
        {
            Logger::setLevelFromString(argv[++i]);
        }
    }

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication app(argc, argv);

    app.setStyleSheet(
        "QToolButton  { font-size: 10pt; }"
        "QComboBox    { font-size: 10pt; }"
        "QButton      { font-size: 10pt; }"
        "QPushButton  { font-size: 10pt; }"
        "QGroupBox    { font-size: 10pt; }"
        "QLineEdit    { font-size: 10pt; }"
        "QListView    { font-size: 10pt; }"
        "QRadioButton { font-size: 10pt; }"
        "QMenuBar     { font-size: 10pt; }"
        "QMenu        { font-size: 10pt; }"
        "QLabel       { font-size: 10pt; }"
    );

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string addr, speed, label, path;

    MainWindow window;

    for (auto i = args.begin(); i != args.end(); ++i)
    {
        if (*i == "--log-level")
        {
            ++i; // already applied above, skip the value
        }
        else if (*i == "-a")
        {
            addr = *++i;
            window.setAddress(addr);
        }
        else if (*i == "-m")
        {
            path = *++i;
            window.setROM(path);
        }
        else if (*i == "-s")
        {
            speed = *++i;
            window.setSpeed(speed);
        }
        else if (*i == "-d")
        {
            window.setShowDebugger(true);
        }
        else if (*i == "-l")
        {
            label = *++i;
            window.setLabel(label);
        }
        else
        {
            path = *i;
            window.setRAM(path);
        }
    }

    window.setWindowTitle("ET-3400 Emulator");
    window.show();

    QTimer::singleShot(0, &window, &MainWindow::start);

    int result = app.exec();
#ifdef _WIN32
    timeEndPeriod(1);
#endif
    return result;
}
