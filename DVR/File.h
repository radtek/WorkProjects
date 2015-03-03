#ifndef _FILE
#define _FILE

#include <windows.h>
#include <tchar.h>

namespace W32Lib
{
	class IFile
	{
	public:
		virtual BOOL Open(void) = 0;
		virtual BOOL Open(const char *file_name) = 0;
		virtual BOOL Create(void) = 0;
		virtual BOOL Create(const char *file_name) = 0;
		virtual BOOL IsOpen() = 0;
		virtual void Close(void) = 0;
		virtual DWORD Read(void *buffer, DWORD count) = 0;
		virtual DWORD Write(void *buffer, DWORD count) = 0;
		virtual BOOL SetPointer(LONGLONG pointer, DWORD dwMoveMethod) = 0;
		virtual BOOL GetPointer(LONGLONG *pointer) = 0;
		virtual ~IFile() {}
	};

	class File : public IFile
	{
		protected:
			HANDLE hFile;
			char name[MAX_PATH];

		public :
			File() : hFile(INVALID_HANDLE_VALUE) {memset(name, 0x00, sizeof(name));}
			File(const char *file_name) : hFile(INVALID_HANDLE_VALUE) {_tcscpy_s(name, MAX_PATH, file_name);}

			~File() {Close();}

			BOOL Open();
			BOOL Open(const char *file_name);

			BOOL Create();
			BOOL Create(const char *file_name);

			BOOL IsOpen() {return (hFile != INVALID_HANDLE_VALUE) ? TRUE : FALSE;}

			void Close(void);

			DWORD Read(void *buffer, DWORD count);
			DWORD Write(void *buffer, DWORD count);

			BOOL SetPointer(LONGLONG pointer, DWORD dwMoveMethod = FILE_BEGIN);
			BOOL GetPointer(LONGLONG *pointer);

			const char* GetName() {return name;}
			const HANDLE GetHandle(void) {return hFile;}
	};

	class FileEx : public File
	{
		public :
			FileEx() {};
			FileEx(const char *file_name) : File(file_name) {}

			~FileEx() {}; 

			using File::Open;
			using File::Create;
			using File::SetPointer;

			BOOL Open(DWORD dwDesiredAccess, DWORD dwShareMode);
			BOOL Open(const char *file_name, DWORD dwDesiredAccess, DWORD dwShareMode);

			BOOL Create(DWORD dwDesiredAccess, DWORD dwShareMode);
			BOOL Create(const char *file_name, DWORD dwDesiredAccess, DWORD dwShareMode);

			BOOL Create(DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
			BOOL Create(const char *file_name, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
			
			BOOL SetPointer(LONGLONG pointer, LONGLONG *new_pointer, DWORD dwMoveMethod = FILE_BEGIN);
			
			// ���������� ������ �����. ���� ��������� ������ - ���������� '-1'.
			LONGLONG GetSize(void);
			
			BOOL SetEndOfFile() {return ::SetEndOfFile(hFile);}
			BOOL SetFileSize(LONGLONG new_size);

			BOOL Rename(const char *new_name);

			// �-��� ������������ ����� � ������� ������� ��������� ���������.
			// ���� ������ ������� �� �������� ��������� ��������������� �� ������
			// ������ � ����������� ������� ����� ��������� ��� ������.
			// ���� ������ �� ������� �� �������� ��������� �� ���������� � �-��� ���������� -1.
			LONGLONG Find(BYTE *str, DWORD size);

			BOOL Copy(IFile *source_file);
	};

	BOOL CmpMemBlck(BYTE *blck1, BYTE *blck2, DWORD size);
}

#endif