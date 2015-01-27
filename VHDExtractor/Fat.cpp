#include "Fat.h"
#include <cstring>



bool fat::IsValidDirEntry(char *buff)
{
	DIR_ENTRY *entry = (DIR_ENTRY *)buff;
	DIR_ENTRY_LONG *entry_long = (DIR_ENTRY_LONG *)buff;

	// ������ � ��������� ������� ����� 0x00 ��� 0xE5 ������ �� �����������.
	// ��������� ��� ��� ������.
	unsigned char first_byte = buff[0];
	if ((first_byte == 0x00) || (first_byte == 0xE5)) {
		return true;
	}

	// ��� ������� ���� DIR_ENTRY::attributes  ������ ���� ����� "0".
	if (entry->attributes & (~kAttributesMask)) {
		return false;
	}

	// ���� ���� �������� kLongName ����������� �� ��������� ���� ������ ���� ��������.
	if ((entry->attributes & fat::kLongName) == fat::kLongName) {
		if (entry->attributes & ~fat::kLongName) {
			return false;
		}
	}

	if (entry->attributes == fat::kLongName) {
	//
	// �������� ���������� ������� ������.
	//

		// ��������� ���������� �������� ����� �������� 0x4X ��� 0x0X (��� X - ����� �����).
		if (!(((entry->name[0] >> 4) == 0x00) || ((entry->name[0] >> 4) == 0x04))) {
			return false;
		}

		return true;
	} else {
	//
	// �������� ���������� �������� ������.
	//

		// ���� ������ �������� ������ ����
		if (entry->attributes & fat::kVolumeID) {
			// ��� ��������� �������� ������ ���� ��������.
			if ((entry->attributes & ~fat::kVolumeID)) {
				return false;
			}
			// ����� ������� �������� ������ ���� ����� - "0".
			if ((entry->first_cluster_lo != 0x00) || (entry->first_cluster_hi != 0x00)) {
				return false;
			}
		}

		// ���� ������ �������� ���������
		if (entry->attributes & fat::kDirectory) {
			// ���� DIR_ENTRY::file_size ������ ��������� ����
			if (entry->file_size != 0x00) {
				return false;
			}

			// �������� �� ����� ����� ���� ������� ��������.
			char self_ref_name[11] = {0x2E,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
			if (memcmp(entry->name, self_ref_name, 11) == 0x00) {
				return true;
			}
			char parent_ref_name[11] = {0x2E,0x2E,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
			if (memcmp(entry->name, parent_ref_name, 11) == 0x00) {
				return true;
			}
		}

		// �������� �� ���������� �������� � ����� (DIR_ENTRY::name).
		for (int x = 0; x < sizeof(entry->name); x++) {
			if (entry->name[x] < 0x20) {
				if ((x == 0x00) && (entry->name[x] == 0x05)) {
					continue;
				}
				return false;
			} else if (entry->name[x] == 0x20) {
				// ������ � ����� 0x20 (������) �� ����� ���� ������ �������� �����.
				if (x == 0x00) {
					return false;
				}
			} else {
				if (entry->name[x] == 0x22) {
					return false;
				} else if ((entry->name[x] >= 0x2A) && (entry->name[x] <= 0x2C)) {
					return false;
				} else if ((entry->name[x] >= 0x2E) && (entry->name[x] <= 0x2F)) {
					return false;
				} else if ((entry->name[x] >= 0x3A) && (entry->name[x] <= 0x3F)) {
					return false;
				} else if ((entry->name[x] >= 0x5B) && (entry->name[x] <= 0x5D)) {
					return false;
				} else if (entry->name[x] == 0x7C) {
					return false;
				}
			}
		}
		return true;
	}
}
