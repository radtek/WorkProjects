#include "terminal.h"
#include <QtWidgets/QApplication>

#include "ComPort.h"
#include "MessageReciver.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Terminal w;
	w.show();
	return a.exec();
}