#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QDir>

#include <iostream>
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
#include "G2fdbVolume.h"
#include "utility.h"
#include "G2fdbRecovery.h"

class WinApiException : public std::exception
{
private:
	DWORD error_code;
	std::string error_description;
public:
	WinApiException(DWORD win_error_code) : error_code(win_error_code)
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

#include "bitstream_reader.h"
#include "h264_ps.h"

void h264_test(void)
{
	SPS sps = { 0 };
	uint8_t buff[] = { 0x67, 0x64, 0x00, 0x28, 0xAC, 0xE8, 0x0E, 0x81, 0x26, 0xC0, 0x44};
	bitstream_reader bs(buff, sizeof(buff));

	size_t bits_count = bs.bits_available();
	size_t size_sps = sizeof(sps);

	int forbidden_zero_bit = bs.f(1);
	int nal_ref_idc = bs.u(2);	
	int nal_unit_type = bs.u(5);

	if (nal_unit_type == 7) {
		ReadSPS(bs, sps);
	}

	int x = 0;

	return;
}

#include "DhfsVolume.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//std::string volume_name = "d:\\work\\DHFS.bin";
	std::string volume_name = "\\\\.\\PhysicalDrive0";
	DHFS::DhfsVolume volume(volume_name);
	if (volume.Open()) {

		DHFS::Frame frame;
		DHFS::FrameSequence sequence;
		size_t file_counter = 0;

		volume.SetPointer(170346847LL);

		bool fd_present = false;

		while (volume.FindAndReadFrameSequence(sequence)) {

			//fd_present = frame.Header()->frame_type == 0xFD ? true : false;
			//sequence.AddFirstFrame(frame);

			//while (volume.ReadFrame(frame)) {
			//	sequence.AddFrame(frame);
			//	if (frame.Header()->frame_type == 0xFD) {
			//		fd_present = true;
			//	}
			//}

			std::string out_dir = "F:\\40774\\test\\";
			std::string file_name = out_dir + std::to_string(file_counter) + ".h264";
			if (!fd_present) {
				file_name += "-x";
			}
			file_name += ".h264";

			W32Lib::FileEx out_file(file_name.c_str());
			if (out_file.Create()) {			
				out_file.Write(sequence.data.data(), sequence.data.size());			
			}
 
			file_counter++;
		}
	}

	w.show();
	return a.exec();
}
