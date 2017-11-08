#include "Editor.hpp"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Editor w;
	a.installEventFilter(&w);
	w.show();
	return a.exec();
}