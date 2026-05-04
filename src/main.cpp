#include "windows/mainwindow.h"
#include "util/log.h"
#include <QApplication>
#include <QTimer>

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
#endif


int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetUnhandledExceptionFilter(crashHandler);
    timeBeginPeriod(1);
#endif
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication app(argc, argv);

	std::vector<std::string> args(argv + 1, argv + argc);
	std::string addr, speed, label, path;

	MainWindow window;

    for (auto i = args.begin(); i != args.end(); ++i) {
        if (*i == "--log-level") {
            Logger::setLevelFromString(*++i);
        }
        else if (*i == "-h" || *i == "--help") {
            std::cout << "Heathkit ET-3400 Emulator\n" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options] [file]\n" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -m <monitor rom>     Load monitor ROM from file" << std::endl;
            std::cout << "  -a <start address>   Set start address for execution (hex)" << std::endl;
            std::cout << "  -s <speed>           Set clock speed in Hz" << std::endl;
            std::cout << "  -d                   Show debugger on startup" << std::endl;
            std::cout << "  -l <label>           Load labels from file" << std::endl;
            std::cout << "  --log-level <level>  Set log level (error, warn, info, debug)" << std::endl;
            std::cout << "  <file>               Load RAM contents from file" << std::endl;
            return 0;
        }
        else if (*i == "-a") {
            addr = *++i;
            window.setAddress(addr);
        }
        else if (*i == "-m") {
            path = *++i;
            window.setROM(path);
        }
        else if (*i == "-s") {
            speed = *++i;
            window.setSpeed(speed);
        }
        else if (*i == "-d") {
            window.setShowDebugger(true);
        }
        else if (*i == "-l") {
            label = *++i;
            window.setLabel(label);
        }
        else {
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