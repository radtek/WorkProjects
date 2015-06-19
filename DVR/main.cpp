#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"
#include "WFS.h"
#include "File.h"

#include <vector>
#include "BufferedFile.h"

#include "ZPool.h"
#include "VDev.h"

#include <QDir>

#include "WinConsole.h"

using namespace DHFS;

inline void _trace(char *format, ...)
{
	static char buffer[2048] = { 0 };
	DWORD rw = 0;

	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	::WriteConsoleA(::GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &rw, NULL);
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//QString path = "D:\\";
	//QDir base_dir(path);

	//QFileInfoList info_list = base_dir.entryInfoList();
	//foreach(QFileInfo file_info, info_list) {
	//	QString curr_path = file_info.absoluteFilePath();
	//	int x = 0;
	//}
	

	//WFS::Main();

	WinConsole con;
	con.Print(_T("Hello!!!\n"), kRed|kIntensity);
	con.Print(30, 10, _T("Hello there:)"));
	con.Print(30, 11, _T("Hello there:)"), kBlue|kIntensity);
	con.Print(_T("Hello!!!\n"), kGreen | kIntensity);
	con.Print(30, 11, _T("Hello there:)"), kBlue | kIntensity);
	con.Print(_T("Testing\n"));
	w.show();
	return a.exec();
}
