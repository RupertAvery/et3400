#include "util/mutex.hpp"
#include <QDebug>
#include "windows/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

 	//qRegisterMetaType<MemoryLocation>();

	MainWindow window;

	window.setWindowTitle("ET-3400 Emulator");
	window.show();

	return app.exec();
}