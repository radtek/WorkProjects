#ifndef _FRAME_H
#define _FRAME_H

#include <windows.h>
#include "Timestamp.h"
#include "BufferedFile.h"
#include <string>
#include <vector>

namespace DHFS
{
#define FRAME_HEADER_MAGIC			((DWORD)0x56414844)		// 'DHAV'
#define FRAME_FOOTER_MAGIC			((DWORD)0x76616864)		// 'dhav'

#define FRAME_MAX_SIZE				((DWORD)32*1024*1024)

	using namespace dvr;

#pragma pack(push)
#pragma pack(1)

	typedef struct _TIMESTAMP {
		DWORD seconds:6;
		DWORD minutes:6;
		DWORD hours:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;
		void Timestamp(dvr::Timestamp &t) {
			t.year = 2000 + year;
			t.month = month;
			t.day = day;
			t.hours = hours;
			t.minutes = minutes;
			t.seconds = seconds;
		}
		dvr::Timestamp Timestamp(void);
	} TIMESTAMP;

	typedef struct _FRAME_HEADER {
		DWORD magic;				// FRAME_HEADER_MAGIC
		WORD frame_type;
		WORD camera;
		DWORD sync_counter;
		DWORD frame_size;			// Frame size
		TIMESTAMP time;

		DWORD HeaderSize(void) {
			switch (frame_type) {
			case 0x00FC:
				return 0x20;
			case 0x00FD:
			case 0x00F1:
				return 0x28;
			case 0x00F0:
				return 0x24;
			default :
				return 0;
			}		
		}
	} FRAME_HEADER;

	typedef struct _FRAME_FOOTER {
		DWORD magic;					// FRAME_FOOTER_MAGIC
		DWORD frame_size;
	} FRAME_FOOTER;

	typedef struct _VOLUME_HEADER {
		BYTE unknown_1[12];				// 0x00
		DWORD unknown_2;				// 0X55
		TIMESTAMP start_time;
		TIMESTAMP end_time;

		DWORD current_cluster;			// ���� �� ����� ��������� ��������� ���������� ���������������
										// ������ ����� ��� ����������� ������� ��������� �������,  
										// ������� ����� ������� ��� ������ ����� ������.

		BYTE unknown_3[16];				// 0x00
		DWORD sector_size;				// ������ ������� � ������. ������ - 512 ����.
		DWORD cluster_size;				// ������ �������� � ��������. ����� 4096 �������� = 2 ��.
		DWORD unknown_4;				// 0x00
		DWORD unknown_5;				// 0x40
		DWORD unknown_6;				// 0x7900
		DWORD vol_header_offset;		// 0x22
		DWORD record_table_offset;		// 0xbb
		DWORD first_cluster_offset;		// �������� ������� ��������, � ��������
		DWORD clusters_count;			// ���-�� ���������.
	} VOLUME_HEADER;

#pragma pack(pop)

	typedef struct _FrameInfo {
		LONGLONG offset;
		Timestamp timestamp;
		DWORD camera;
		DWORD flag;
		DWORD counter;
		DWORD frame_size;
		DWORD data_size;
		void Clear(void) {memset(this, 0x00, sizeof(_FrameInfo));}
		void ToString(std::string &info_str);
	} FrmInfo;

	typedef struct _FrameSequenceInfo {
		FrmInfo start_frame;
		FrmInfo end_frame;
		DWORD frame_counter;
		DWORD start_sync_counter;
		DWORD end_sync_counter;
		DWORD size;
		void Clear(void);
		DWORD Camera(void) { return start_frame.camera; }
		void SetFirstFrame(FrmInfo &frame);
		bool AppendFrame(FrmInfo &frame);
	} FrameSequenceInfo;

	class Volume
	{
	private:
		BufferedFile io;
	public:
		Volume(const std::string &volume_file_name);
		bool Open();
		void SetPointer(const LONGLONG &pointer);
		bool IsValidHeader(FRAME_HEADER &header);
		bool ReadFrame(std::vector<BYTE> &buffer, FrmInfo &info);
		bool FindAndReadNextFrame(std::vector<BYTE> &buffer, FrmInfo &info);
		bool NextFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequnence_info);
		void Test(void);
		void SaveFrameInfo(const std::string &out_file);
		void SaveFrameSequenceInfo(const std::string &out_file);
	};
}

#endif //_DHFS