#pragma once

#define GET_RAW_FILE_INTERFACE_PROC		_T("GetRawFileInterface")

#define NAME_MAX_LEN					255
#define EXTENSION_MAX_LEN				20


#pragma pack(8)
//typedef struct {
//	TCHAR name[NAME_MAX_LEN];
//	TCHAR extension[EXTENSION_MAX_LEN];
//} RawFileInfo;


class RawFileInterface
{
public:
	virtual TCHAR GetName(void) = 0;							// �������� ���� �����
	virtual TCHAR GetDescription(void) = 0;						// ���������� �������� ���� �����

	virtual TCHAR *GetFileExtension(void) = 0;					// ���������� �����

	virtual DWORD GetMinSize(void) = 0;							// ����������� ������ RAW �����
	virtual DWORD GetMaxSize(void) = 0;							// ������������ ������ RAW �����

	virtual BOOL IsBeginOfFile(BYTE *buffer, DWORD size) = 0;	// 
};