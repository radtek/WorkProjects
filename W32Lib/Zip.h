#define ZIP_MAGIC		"PK"

typedef struct _ZIP_HEADER {
	BYTE	magic[2];					// ZIP_MAGIC
	WORD	sub_code;					// 0x0201 - ������ � ������ sub_code-�� �������� � ����� ����� 
										// 0x0403 - ������ � ������ sub_code-�� �������� � ������ �����,
										//			��� ������ ������ ��������������� �������������� ������ �����
										// 0x0605 - ��������� ������, �� ���� ��� ���� �� ���� ����
	BYTE	unknown[6];
	DWORD	time;						// �������� !!! �����
	DWORD	crc;
	DWORD	packed_size;
	DWORD	real_size;
	WORD	name_length;
	WORD	data_offset;
	BYTE	name[1];
} ZIP_HEADER;