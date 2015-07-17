#include "DvrExt3.h"


bool dvrext::IsIndirectBlock(const EXT_BLOCK<EXT_BLOCK_SIZE> &block)
{
	DWORD max_valid_pointer_index = 0xFFFFFFFF;
	DWORD pointers_per_block = block.DwordsCount();

	/*
	// 
	// ��������� �� ����� �� ������ ��������� �� ������� �������.
	// ���� ����� ����������� - ���� �� �������� IndirectBlock-��
	//
	for (DWORD i = 0; i < pointers_per_block; ++i) {
		if (block.dword[i] <= EXT_MAX_BLOCK_NUMBER) {
			if (block.dword[i] != 0x00) {
				max_valid_pointer_index = i;
			}
		}
	}
	
	if (max_valid_pointer_index == 0xFFFFFFFF) {
		return false;
	}

	// 
	// ������ IndirectBlcok-� �� ����� ���� ��� ���������� ���������,
	// �� ����������� �������� ���������.
	//
	for (DWORD i = 0; i <= max_valid_pointer_index; ++i) {
		if (block.dword[i]) {
			for (DWORD j = 0; j <= max_valid_pointer_index; ++j) {
				if (block.dword[i] == block.dword[j]) {
					if (i != j) {
						return false;
					} else {
						continue;
					}
				}
			}
		}
	}
	*/

	std::set<DWORD> set;
	std::set<DWORD>::iterator it;
	std::pair<std::set<DWORD>::iterator, bool> ret;
	for (DWORD i = 0; i < pointers_per_block; ++i) {
		if (block.dword[i] <= EXT_MAX_BLOCK_NUMBER) {
			if (block.dword[i]) {
				ret = set.insert(block.dword[i]);
				if (ret.second == false) {
					if (*ret.first == block.dword[i]) {
						return false;
					}
				}
			}
		} else {
			return false;
		}
	}

	return true;
}

int dvrext::RecoverAllIndirectBlocksToFolder(W32Lib::FileEx ext_volume, LONGLONG volume_offset, std::string out_folder)
{


	return 0;
}

int dvrext::Run()
{
	std::string out_dir = "F:\\38023\\iblocks\\";
	std::string out_file_name = "";

	EXT_BLOCK<EXT_BLOCK_SIZE> block;
	block.Erase();

	LONGLONG file_couner = 0;
	LONGLONG start_block = 27792465;
	LONGLONG block_number = start_block;
	BufferedFile ext_volume("\\\\.\\PhysicalDrive2");
	if (ext_volume.Open()) {
		ext_volume.SetPointer(EXT_VOLUME_OFFSET + start_block * EXT_BLOCK_SIZE);
		for (; block_number <= EXT_MAX_BLOCK_NUMBER; ++block_number) {
			if (ext_volume.Read(block.raw, block.Size()) == block.Size()) {
				if (IsIndirectBlock(block)) {
					out_file_name = out_dir + std::to_string(block_number);
					W32Lib::FileEx file(out_file_name.c_str());
					if (file.Create()) {
						file.Write(block.raw, block.Size());
						++file_couner;
					}
				}			
			} else {
				return -1;
			}
		}	
	}

	return 0;
}
