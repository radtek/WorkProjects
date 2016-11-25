#ifndef _G2FDB_ONDISK_H
#define _G2FDB_ONDISK_H

#include "windows.h"
#include "Timestamp.h"

//
// DVR Infinity (example: NDR-X2416)
//
// This is G2FDb Magic
// This is G2FDb Storage Magic
//

namespace G2FDB
{

#pragma pack(push)
#pragma pack(1)

	struct TIMESTAMP {
		DWORD seconds : 6;
		DWORD minutes : 6;
		DWORD hours : 5;
		DWORD day : 5;
		DWORD month : 4;
		DWORD year : 6;

		void Timestamp(dvr::Timestamp &t) {
			t.year	= (year + 1970);
			t.month	= month;
			t.day	= day;
			t.hours = hours;
			t.minutes = minutes;
			t.seconds = seconds;
		}
	};

#define FRAME_HEADER_SIZE					((DWORD)0xA1)
#define FRAME_HEADER_SIGNATURE_1			((DWORD)0x00000002)
#define FRAME_HEADER_SIGNATURE_2			((DWORD)0x03419F7D)

	struct FRAME_HEADER {
		DWORD signature;					// FRAME_HEADER_SIGNATURE_1 (0x00000002)
		TIMESTAMP time;						 
		DWORD unk_1;
		DWORD signature_2;					// FRAME_HEADER_SIGNATURE_2 may be signature
		DWORD data_size;
		DWORD unk_3;
		WORD camera;
		WORD unk_4;
		DWORD frame_offset;
		WORD width;
		WORD height;
		WORD str_length;
		BYTE string[91];
		BYTE unk_5[32];
	};

#pragma pack(pop)

}

#endif // _G2FDB_ONDISK_H