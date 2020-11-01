#include "windows/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MainWindow window;

	window.resize(350, 500);
	window.setWindowTitle("ET-3400 Emulator");
	window.show();

	return app.exec();
}