#pragma once
#include "Engine.h"

namespace Global
{
	////////////////////////////////////////////////////////
	//	Global variables
	//

	extern const CString strMainWndClass;
	extern const CString strMainWndName;

	////////////////////////////////////////////////////////
	//	Global function
	//

	// �������� ���������� �� ��������� ���� (����� ���� ����� ���� ����)
	inline BOOL IsPathExists(TCHAR *path);
};

