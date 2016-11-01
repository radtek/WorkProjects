#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QDir>

#include <vector>
#include "File.h"
#include "BufferedFile.h"

#include "WinConsole.h"
#include "TestWidget.h"
#include "Raid5.h"
#include "Orbita.h"

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

#include "ExtentRecovery.h"
#include "FileRecordRecovery.h"
#include "Timestamp.h"
#include "Hikvision.h"
#include "dhfs.h"
#include "WFS.h"

#include <iostream>

#include "G2fdbVolume.h"
#include "utility.h"

class WinApiException : public std::exception
{
private:
	DWORD error_code;
	std::string error_description;
public:
	WinApiException(DWORD os_error_code)
	{
		if (error_code) {
			char *str = NULL;
			if (::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, (LPSTR)&str, 0, NULL)) {
				error_description = str;
				::LocalFree(str);
			} else {
				error_description = "�� ������� �������� �������� ������.";
			}
		}
	}

	virtual const char * what() const noexcept override
	{
		return error_description.c_str();
	}
};

int Test(void)
{
	W32Lib::FileEx file("wewe");
	if (!file.Open()) {
		throw WinApiException(::GetLastError());
	}

	return 0;
}

void TestFind(void)
{
	using namespace G2FDB;

	G2fdbVolume volume("D:\\Work\\40673\\1.bin");
	if (volume.Open()) {
	
		Frame frame;
		LONGLONG frame_offset = 0;
		std::vector<BYTE> frame_data;
		size_t counter = 0;

		while(volume.FindAndReadFrame(frame)) {
		
			counter++;	
		
			frame_offset += 39;
			volume.SetPointer(frame_offset);
		}
		
		int x = 0;
	
	
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//Orbita::Main("\\\\.\\PhysicalDrive0", "K:\\38702\\out");
	//WFS::Main();

	//DWORD raw = 0x103CB6A9;
	//DWORD raw = 0x102C034a;
	//HIKV::TIMESTAMP *t = (HIKV::TIMESTAMP *)&raw;

	//HIKV::TIMESTAMP tst = { 0,0,0,25,2,16 };
	//DWORD *qq = (DWORD *)&tst;
	//qq;

	//DWORD dw = 0x409EB74F;
	//WFS::TIMESTAMP *tm = (WFS::TIMESTAMP *)&dw;

	//Timestamp time;
	//time = (Timestamp)tst.TimeStamp();

	//HIKV::StartRecovering("\\\\.\\PhysicalDrive0", "F:\\40038\\out\\", dvr::Timestamp(2016, 01, 01, 0, 0, 0), dvr::Timestamp());
	//DHFS::StartRecovering("\\\\.\\PhysicalDrive0", "E:\\40623\\out\\", dvr::Timestamp(2016, 01, 01, 0, 0, 0), dvr::Timestamp());

	//FileRecordRecoveryStart();

	//std::string out_file_name = "F:\\39389\\frames_info.txt";
	//HIKV::HikVolume vol("\\\\.\\PhysicalDrive13");
	//if (vol.Open()) {
	//	vol.SaveFramesInfoToFile(out_file_name);	
	//}

	//DHFS::StartRecovering("\\\\.\\PhysicalDrive0", "e:\\40175\\out\\", dvr::Timestamp(2015, 1, 1, 0, 0, 0), dvr::Timestamp(0, 0, 0, 0, 0, 0));

	//DHFS::StartRecovering("\\\\.\\PhysicalDrive1", "F:\\39988\\out\\", Timestamp(2015, 5, 1, 0, 0, 0), Timestamp(2016, 8, 14, 0, 0, 0));
	//DHFS::Volume volume("\\\\.\\PhysicalDrive1");
	//if (volume.Open()) {
	//	volume.SaveFrameInfo("F:\\39988\\frames.txt");
	//	//volume.SaveFrameSequenceInfo("F:\\39988\\sequences.txt");
	//}

	//DHFS::StartRecovering("\\\\.\\PhysicalDrive0", "F:\\40623\\out\\", dvr::Timestamp(2016, 01, 01, 0, 0, 0), dvr::Timestamp());
	//DHFS::Volume volume("\\\\.\\PhysicalDrive0");
	//if (volume.Open()) {
	//	volume.SaveFrameInfo("E:\\40623\\frames.txt");
	//	//volume.SaveFrameSequenceInfo("F:\\40052\\sequences.txt");
	//}

	TestFind();

	w.show();
	return a.exec();
}
