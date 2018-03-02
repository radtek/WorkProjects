#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <string>
#include <vector>
#include "File.h"
#include "dbf.h"
#include <assert.h>

static int valid_symbols_table[] = {
	/*		 00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F		  */
	/*00*/	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  /*00*/
	/*01*/	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  /*01*/
	/*02*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*02*/
	/*03*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*03*/
	/*04*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*04*/
	/*05*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*05*/
	/*06*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*06*/
	/*07*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,  /*07*/
	/* --------------------------------------- 1251 --------------------------------------------- */
	/*08*/	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  /*08*/
	/*09*/	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  /*09*/
	/*0A*/	0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x01,  /*0A*/
	/*0B*/	0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x01,  /*0B*/
	/*0C*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*0C*/
	/*0D*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*0D*/
	/*0E*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,  /*0E*/
	/*0F*/	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01   /*0F*/
};

bool IsValidDbfHeader(dbf::header_t &hdr)
{
	if (hdr.flags != 3) return false;
	if (hdr.month == 0 || hdr.month > 12) return false;
	if (hdr.day == 0 || hdr.day > 31) return false;

	if (hdr.header_size < 32 * 2 + 1) return false;
	if (hdr.header_size % 32 != 1) return false;

	if (hdr.record_size < 2) return false;

	if (hdr.reserved_1 != 0x00) return false;
	for (const auto &val: hdr.reserved_2) {
		if (val != 0 ) {
			return false;
		}
	}
	for (const auto &val : hdr.reserved_3) {
		if (val != 0) {
			return false;
		}
	}

	if (hdr.transaction_incomplete_flag != 0) return false;
	if (hdr.encryption_flag != 0) return false;

	return true;
}

bool IsValidFieldDescriptor(dbf::field_descriptor_t &field_descriptor)
{


	return true;
}

enum class DataType {
	Unknown = 0,
	DbfBegin,
	DbfEnd,
	DbfRecords,
	DbfFields,
	DbfFieldsAndRecords,
};

void TestDbfFile(void)
{
	W32Lib::FileEx io("F:\\43693\\DB examples\\BUCC\\SC12610.DBF");
	if (io.Open()) {
		
		using namespace dbf;

		dbf::header_t hdr;
		io.Read(&hdr, sizeof(hdr));

		bool result = IsValidDbfHeader(hdr);


		size_t fields_count = (hdr.header_size / 32) - 1;
		assert(hdr.header_size % 32 == 1);

		std::vector<field_descriptor_t> fields;
		fields.resize(fields_count);

		io.Read(fields.data(), fields.size() * sizeof(field_descriptor_t));


		std::vector<uint8_t> buff(hdr.record_size);
		for (int i = 0; i < hdr.records_count; i++) {
			
			io.SetPointer((int64_t)hdr.header_size + (int64_t)(i * hdr.record_size));
			io.Read(buff.data(), buff.size());
			
			std::vector<std::string> record(fields_count);
			
			char *fld_ptr = (char *)buff.data() + 1;
			for (int f = 0; f < fields_count; f++) {
			
				record[f] = std::string(fld_ptr, fields[f].size);
				fld_ptr += fields[f].size;
			
			}

			int xx = 0;		
		}
		int x = 0;	
	}	
}

uint8_t MostFrequentSymbol(std::vector<size_t> &frequency_table)
{
	return 0;
}

bool FindAndSaveAllDbfFragments()
{
	std::string file_name = "F:\\43693\\vhd\\1CServ_Backup.vhdx.decryptor@cock.li";
	std::string out_dir = "F:\\43693\\recovery\\extents";
	const size_t block_size = 512;
	std::vector<uint8_t> buffer(256 * block_size);

	W32Lib::FileEx src_file(file_name.c_str());
	if (!src_file.Open()) {
		return false;
	}
	
	size_t readed = 0;
	while (readed = src_file.Read(buffer.data(), buffer.size())) {
	
		size_t blocks = readed / block_size;
		for (int i = 0 ; i < blocks; i++) {
		
			
		
		}
	
	}

	return false;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	TestDbfFile();

	bool result = FindAndSaveAllDbfFragments();

	w.show();
	return a.exec();
}
