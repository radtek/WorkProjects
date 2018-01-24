#include "zfs_test.h"
#include <intrin.h>

#include <system_error>
#include <string>
#include <cmath>
#include "sha256.h"
#include "flatcher.h"
#include "lz4.h"
#include "lzjb.h"

//#define VDEV_OFFSET						323510272*512ULL
#define VDEV_OFFSET						323510272*512ULL
//#define VDEV_OFFSET						2048*512ULL
#define VDEV_LABEL_NVLIST_OFFSET		16*1024
#define SECTOR_SIZE_SHIFT				9
#define SECTOR_SIZE						(1 << SECTOR_SIZE_SHIFT)

inline bool	ChecksumEqual(zio_cksum &zc1, zio_cksum &zc2)
{
	return 	(0 == ( ((zc1).word[0] - (zc2).word[0]) | ((zc1).word[1] - (zc2).word[1]) |
		((zc1).word[2] - (zc2).word[2]) | ((zc1).word[3] - (zc2).word[3])) );
}


bool IsValidDnode(dnode_phys_t *dnode)
{
	if (dnode->type == DMU_OT_NONE) {
		return false;
	}

	if (!DMU_OT_IS_VALID(dnode->type)) {
		return false;
	}

	if (dnode->ind_blk_shift < DN_MIN_INDBLKSHIFT) return false;
	if (dnode->ind_blk_shift > 17) return false; // 128 Kb

	if (dnode->nlevels == 0) return false;
	if (dnode->nlevels > 6) return false;

	if (dnode->nblk_ptr > 3) return false;

	for (size_t i = 0; i < dnode->nblk_ptr; i++) {

		if (dnode->blk_ptr[i].props.embedded) {
			if (!zfs_blkptr_verify(dnode->blk_ptr[i])) return false;
		} else if (dnode->blk_ptr[i].dva[0].alloc_size) {
			if (!zfs_blkptr_verify(dnode->blk_ptr[i])) return false;
		}

	}

	if (dnode->checksum >= ZIO_CHECKSUM_FUNCTIONS) {
		return false;
	}

	if (dnode->compress >= ZIO_COMPRESS_FUNCTIONS) {
		return false;
	}

	//	Is dn_used in bytes?  if not, it's in multiples of SPA_MINBLOCKSIZE
	//#define	DNODE_FLAG_USED_BYTES			(1<<0)
	//#define	DNODE_FLAG_USERUSED_ACCOUNTED	(1<<1)
	//
	//	Does dnode have a SA spill blkptr in bonus?
	//#define	DNODE_FLAG_SPILL_BLKPTR			(1<<2)

	if (dnode->flags > 7) {
		return false;
	}

	if (dnode->nblk_ptr == 1) {
		if (dnode->bonus_type == DMU_OT_NONE) {
			return false;
		}

		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}

		if (dnode->bonus_len == 0) {
			return false;
		}
	}

	if (dnode->bonus_type) {
		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}

		if (dnode->bonus_len == 0) {
			return false;
		}	
	}
	if (dnode->bonus_len) {
		if (dnode->bonus_type == DMU_OT_NONE) {
			return false;
		}
		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}
	}


	if (dnode->bonus_len > DN_MAX_BONUSLEN) return false;	

	if (dnode->pad2[0] != 0x00) return false;
	if (dnode->pad2[1] != 0x00) return false;
	if (dnode->pad2[2] != 0x00) return false;
	if (dnode->pad2[3] != 0x00) return false;

	if (dnode->pad3[0] != 0x00) return false;
	if (dnode->pad3[1] != 0x00) return false;
	if (dnode->pad3[2] != 0x00) return false;
	if (dnode->pad3[3] != 0x00) return false;

	return true;
}

void zfs_dnode_recovery(void)
{
	//std::string file_name = "D:\\zfs\\zfs-pool-flat.vmdk";
	std::string file_name = "\\\\.\\PhysicalDrive0";
	W32Lib::FileEx io;
	if (io.Open(file_name.c_str())) {

		size_t buff_size = 256* SECTOR_SIZE;
		std::vector<char> buffer(buff_size, 0x00);

		ULONGLONG offset = VDEV_OFFSET + 0x400000;
		ULONGLONG dnode_offset;
		size_t readed = 0;
		dnode_phys_t *dnode = nullptr;

		io.SetPointer(offset);

		while (readed = io.Read(buffer.data(), buffer.size())) {
			
			size_t sectors = readed >> SECTOR_SIZE_SHIFT;

			for (size_t i = 0; i < sectors; i++) {			
				dnode = (dnode_phys_t *)&buffer[i*SECTOR_SIZE];
				if (IsValidDnode(dnode)) {				
					dnode_offset = offset + i*SECTOR_SIZE;
					if (dnode->type == DMU_OT_PLAIN_FILE_CONTENTS || dnode->type == DMU_OT_ZVOL) {
					
						int x = 0;
						x++;
					
					}				
				}			
			}
			offset += readed;		
		}
	}
}

void SaveDataToFile(W32Lib::FileEx &io, dnode_phys_t &dnode, std::string out_file_path)
{

	W32Lib::FileEx out_file(out_file_path.c_str());

	if (out_file.Create()) {

		size_t readed = 0;
		size_t data_block_size = dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE;
		std::vector<char> buffer;
		buffer.reserve(data_block_size);
		for (uint64_t i = 0; i <= dnode.max_blk_id; i++) {
			
			buffer.clear();
			readed = ReadDataBlock2(io, dnode, i, buffer);
			if (readed != data_block_size) {			
				buffer.resize(data_block_size);				
			}
			out_file.Write(buffer.data(), data_block_size);

		}
	
	}
}

#include "ZfsObjectDataSaver.h"

void zfs_test(void)
{
	W32Lib::FileEx io;
	//std::string volume_file_name = "D:\\zfs\\zfs-pool-flat.vmdk";
	std::string volume_file_name = "\\\\.\\PhysicalDrive1";
	if (!io.Open(volume_file_name.c_str())) {
		return;
	}

	std::auto_ptr<uint8_t> vd_label_buff(new uint8_t[sizeof(vdev_label_t)]);
	vdev_label_t *label = (vdev_label_t *)vd_label_buff.get();

	NVList nvlist("vdev label");

	io.SetPointer(VDEV_OFFSET);
	io.Read(vd_label_buff.get(), sizeof(vdev_label_t));

	if (label->vl_vdev_phys.vp_zbt.zec_magic != ZEC_MAGIC) {
		// ��� ��� ��� �� ������.
		int x = 0;
	}

	//zio_cksum_t old = label->vl_vdev_phys.vp_zbt.zec_cksum;
	//zio_cksum_t chksum = {0};

	//memset(&label->vl_vdev_phys.vp_nvlist, 0x00, sizeof(label->vl_vdev_phys.vp_zbt.zec_cksum));
	//zio_checksum_SHA256( &label->vl_vdev_phys.vp_nvlist, sizeof( label->vl_vdev_phys.vp_nvlist ), &chksum );


	DecodeXdrNVList(nvlist, (uint8_t *)label->vl_vdev_phys.vp_nvlist + 4, sizeof(label->vl_vdev_phys.vp_nvlist));

	uberblock_t *ub = nullptr;

	int max_tgx = 0;
	int ub_idx = 0;

	for (int i = 0; i < 128; ++i) {
		ub = (uberblock_t *)(label->vl_uberblock + i * 1024);
		if (ub->magic == UBERBLOCK_MAGIC) {
			if (ub->txg > max_tgx) {
				max_tgx = ub->txg;
				ub_idx = i;
			}			
		}	
	}

	ub = (uberblock_t *)(label->vl_uberblock + ub_idx * 1024);

	//vdev_sector_size = 1 << 0x0C;

	bool result = false;
	MetaObjecSet mos = { 0 };
	result = ReadMOS(io, ub->rootbp, mos);

	Dataset root_dataset = { 0 };
	result = ReadDataset(io, mos.objset, mos.root_dataset_obj, root_dataset);



	//
	// Recovery 43558
	//
	
	std::string out_dir = "F:\\43558\\v3\\";
	std::vector<char> buff;
	size_t readed = 0;

	std::map<std::string, uint64_t> vhd_dir;
	dnode_phys_t vhd_dir_dnode = root_dataset.objset[0x0998];
	assert(vhd_dir_dnode.type == DMU_OT_DIRECTORY_CONTENTS);

	TraversingZapObject(io, vhd_dir_dnode,
		[&vhd_dir](const uint64_t &value, const char* name) {
		vhd_dir.emplace(std::string(name), value);
		return true;
	});

	vhd_dir.erase("28548ee1-ffbf-4b2a-a399-f6ad34e2190b.vhd");
	vhd_dir.erase("27fc4259-4f66-4a30-a969-a8c5e542677f.vhd");
	vhd_dir.erase("25ea76a3-a87a-48f6-9578-6c481fc2a8be.vhd");
	vhd_dir.erase("26d82b9a-791f-407d-b66a-778a0c7db9d5.vhd");
	vhd_dir.erase("2422362c-bfde-4d79-b33c-ce55ae664bb9.vhd");
	vhd_dir.erase("22b37058-25e7-4e17-a544-ce8c5cd60c30.vhd");
	vhd_dir.erase("223b9e88-22f7-4bab-a5a3-5d3ec45813f3.vhd");
	vhd_dir.erase("2219a8ad-05d2-4752-9537-a566f8dfc9e0.vhd");
	vhd_dir.erase("21e54f46-36d5-436d-9546-1dd168fbaf80.vhd");
	vhd_dir.erase("1e617fe3-e8d5-4e98-9bab-031b16178379.vhd");
	vhd_dir.erase("21836121-c1a5-4a71-bc85-877159d3a85c.vhd");
	vhd_dir.erase("1baed32f-4fe8-4661-9577-981c368ac4e1.vhd");
	vhd_dir.erase("1c7e4132-0da5-4ae2-aab4-0c8d50b3a717.vhd");
	vhd_dir.erase("17c0ab48-78b0-4dde-9f27-63da9dbbc26e.vhd");
	vhd_dir.erase("19b397f6-272f-4cdc-a4a6-ed83249a96b8.vhd");
	vhd_dir.erase("16e70995-83b0-480b-a323-a08ae9224d9e.vhd");
	vhd_dir.erase("14fb9dd4-486b-44c6-9ff4-c77982a799b3.vhd");
	vhd_dir.erase("10ecdace-5e51-403f-a8d9-9c9829c6dfb4.vhd");
	vhd_dir.erase("13a8548a-e90e-43fc-b126-82c81b9c9f90.vhd");
	vhd_dir.erase("13b2593f-687e-444d-9afd-185e99c8cbb1.vhd");
	vhd_dir.erase("122d843c-5e6d-4c95-a338-4585265cddb4.vhd");
	vhd_dir.erase("0ed1a7f6-b352-4ebd-ac6f-32e6f369a97c.vhd");
	vhd_dir.erase("0d845c88-11b9-45d8-99dd-e682dec99863.vhd");
	vhd_dir.erase("0d7714cd-b948-4be9-b126-26a902499bcb.vhd");
	vhd_dir.erase("0d58f567-b4de-40f2-9b77-fd2ccd43d490.vhd");
	vhd_dir.erase("0b844046-1988-4505-b69d-a5af29d31b12.vhd");
	vhd_dir.erase("0a05cea4-dca5-4160-98e4-6d14b4808c18.vhd");
	vhd_dir.erase("09c944c3-db07-4a4a-8af9-0e6d01d46467.vhd");
	vhd_dir.erase("098b7d80-8374-4dbe-ab05-df8909e353be.vhd");
	vhd_dir.erase("0973b9c1-05ed-41a9-8d33-f17e1a35ebf4.vhd");
	vhd_dir.erase("08977e50-0a99-48af-9f92-e8edcce692bb.vhd");
	vhd_dir.erase("07a3a704-ffd4-481b-9e01-d5ea4b1af9d8.vhd");
	vhd_dir.erase("02df7eee-fa24-40ae-84d9-173b2a73cfdc.vhd");
	vhd_dir.erase("03a06a35-80db-49e7-9931-6dc9aed44d53.vhd");
	vhd_dir.erase("04a3c239-a854-421c-92a7-5fd266c8c5d0.vhd");
	vhd_dir.erase("06aaaf12-0736-47bd-8abf-f5d21ac0c66d.vhd");
	vhd_dir.erase("034a1127-1c8f-433e-b48e-735f5fb9b5d8.vhd");
	vhd_dir.erase("02b41b1e-8ca8-441c-a0d5-366d3666dc79.vhd");
	vhd_dir.erase("02af08c4-a5c1-4c17-9f8c-071d075259b3.vhd");
	vhd_dir.erase("01e7093a-d5d3-43f1-aa9c-d7ca10cb34d4.vhd");
	vhd_dir.erase("01fc7485-00aa-4109-a80e-f90d5536db6c.vhd");
	vhd_dir.erase("002ecd5c-6078-47ac-a860-c7f17f97f898.vhd");

	for (auto &entry : vhd_dir) {		
		std::string name = entry.first;
		uint64_t object_id = entry.second & 0x00FFFFFFFFFFFFFF;
		dnode_phys_t dnode = root_dataset.objset[object_id];
		if (dnode.type == DMU_OT_PLAIN_FILE_CONTENTS) {

			std::string file_path = out_dir + name;

			ZfsObjectDataSaver saver(io, dnode, file_path);
			int result = saver.Run();

			//SaveDataToFile(io, dnode, file_path);		
		}
	}


	
	int x = 0;
}

bool IsHole(const blkptr_t &bp)
{
	uint64_t *word = (uint64_t *)&bp.dva[0];
	if (!bp.props.embedded) {
		if ((word[0] == 0) && (word[1] == 0)) {
			return true;
		}
	}
	return false;
}

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer)
{
	int result = 0;
	size_t decompressed_data_size = 0;
	size_t origin_size = buffer.size();
	size_t readed_size = 0;
	std::vector<char> compressed_data;
	char *dst = nullptr;
	char *src = (char *)&blkptr;
	dnode_phys_t *dnode = nullptr;

	if (blkptr.props.embedded) {

		blk_props_emb_t *props = (blk_props_emb_t *)&blkptr.props;

		size_t logical_size = props->logical_size + 1;
		size_t physical_size = props->physical_size + 1;

		if (props->compression == ZIO_COMPRESS_OFF) {
			buffer.resize(origin_size + logical_size);
			dst = &buffer[origin_size];
		} else {
			compressed_data.resize(physical_size);
			dst = compressed_data.data();

			decompressed_data_size = logical_size;
			buffer.resize(origin_size + decompressed_data_size);
		}

		assert(props->physical_size <= 6*8 + 3*8 + 5*8);

		if (props->physical_size <= 6*8) {
			memcpy(dst, src, props->physical_size);
		} else {
			memcpy(dst, src, 6*8);
			if (props->physical_size <= 6*8 + 3*8) {
				memcpy(&dst[6*8], &src[6*8 + 8], props->physical_size - 6*8);			
			} else {
				memcpy(&dst[6*8], &src[6*8 + 8], 3*8);
				memcpy(&dst[6*8 + 3*8], &src[6*8 + 8 + 3*8 + 8], props->physical_size - 6*8 - 3*8);
			}		
		}

		readed_size = props->physical_size;

	} else {

		//
		// TODO: reading gang block.
		//
		assert(blkptr.dva[0].gang_flag == 0);

		size_t data_size = 0;

		if (IsHole(blkptr)) {
			data_size = (blkptr.props.logical_size + 1) * SPA_MINBLOCKSIZE;
			buffer.resize(origin_size + data_size);
			std::memset(&buffer[origin_size], 0x00, data_size);		
			return true;
		} else {
			data_size = blkptr.dva[0].alloc_size * SPA_MINBLOCKSIZE;

			readed_size = data_size;

			if (blkptr.props.compression == ZIO_COMPRESS_OFF) {
				buffer.resize(origin_size + data_size);
				dst = &buffer[origin_size];
			}
			else {
				compressed_data.resize(data_size);
				dst = compressed_data.data();

				decompressed_data_size = (blkptr.props.logical_size + 1) * SPA_MINBLOCKSIZE;
				buffer.resize(origin_size + decompressed_data_size);
			}

			if (!io.SetPointer(VDEV_OFFSET + VDEV_LABEL_START_SIZE + blkptr.dva[0].offset * SPA_MINBLOCKSIZE)) {
				return false;
			}

			if (!io.Read(dst, data_size)) {
				return false;
			}		
		}
	}

	bool checksummed = false;
	zio_cksum_t calculated_chksum = { 0 };

	switch (blkptr.props.checksum) {
	case ZIO_CHECKSUM_OFF:
		break;

	case ZIO_CHECKSUM_ON:
	case ZIO_CHECKSUM_ZILOG:
	case ZIO_CHECKSUM_FLETCHER_2:
		fletcher_2_native(dst, readed_size, &calculated_chksum);
		checksummed = true;
		break;

	case ZIO_CHECKSUM_ZILOG2:
	case ZIO_CHECKSUM_FLETCHER_4:
		fletcher_4_native(dst, readed_size, &calculated_chksum);
		checksummed = true;
		break;

	case ZIO_CHECKSUM_LABEL:
	case ZIO_CHECKSUM_GANG_HEADER:
	case ZIO_CHECKSUM_SHA256:
		zio_checksum_SHA256(dst, readed_size, &calculated_chksum);
		checksummed = true;
		break;

	default:
		break;	
	}

	if (checksummed) {
		assert(ChecksumEqual(blkptr.checksum, calculated_chksum));
	}

	//bool chksum_ok = ChecksumEqual(blkptr.checksum, calculated_chksum);

	switch (blkptr.props.compression) {
	
	case ZIO_COMPRESS_OFF:
		return true;

	case ZIO_COMPRESS_LZJB:
		result = lzjb_decompress(compressed_data.data(), &buffer[origin_size], compressed_data.size(), decompressed_data_size, 0);
		return true;

	case ZIO_COMPRESS_LZ4:
		result = lz4_decompress_zfs(compressed_data.data(), &buffer[origin_size], compressed_data.size(), decompressed_data_size, 0);
		assert(result == 0);
		return true;
	
	default :
		throw std::system_error(std::error_code(), "ReadBlock(): Unsupported compression.");
		return false;	
	}

	return false;
}

bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer)
{
	assert(dnode.nlevels);

	size_t ptr_per_block = (1 << dnode.ind_blk_shift) / sizeof(blkptr_t);
	size_t level = dnode.nlevels - 1;
	size_t blkptr_idx = 0;
	//blkptr_t blkptr = {0};
	//uint64_t bnum = block_num;

	std::vector<char> tmp;
	tmp.reserve(dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE);

	uint64_t blocks_per_ptr = std::pow(ptr_per_block, level);
	uint64_t blocks_per_level = blocks_per_ptr * dnode.nblk_ptr;

	assert(block_num < blocks_per_level);
	assert(block_num <= dnode.max_blk_id);

	blkptr_t blkptr = dnode.blk_ptr[block_num / blocks_per_ptr];
	uint64_t bnum = block_num % blocks_per_ptr;

	while (level) {

		assert(blkptr.props.level == level);

		tmp.clear();
		ReadBlock(io, blkptr, tmp);

		if (level == 1) {
			assert(bnum < ptr_per_block);
			blkptr_idx = bnum;
		} else {
			blocks_per_ptr = std::pow(ptr_per_block, level);
			blkptr_idx = bnum / blocks_per_ptr;
			bnum = bnum % blocks_per_ptr;		
		}

		blkptr = ((blkptr_t *)tmp.data())[blkptr_idx];

		level--;
	}

	assert(blkptr.props.level == 0);

	return ReadBlock(io, blkptr, buffer);
}


int64_t ReadDataBlock2(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer)
{
	assert(dnode.nlevels);
	assert(dnode.nblk_ptr);
	assert(dnode.nlevels == dnode.blk_ptr[0].props.level + 1);
	assert(block_num <= dnode.max_blk_id);

	size_t level = dnode.nlevels - 1;
	size_t indblock_size = 1 << dnode.ind_blk_shift;
	size_t data_block_size = dnode.data_blk_sz_sec << SPA_MINBLOCKSHIFT;
	size_t pointers_per_indblock = (indblock_size) / sizeof(blkptr_t);
	uint64_t blocks_per_pointer = std::pow((uint64_t)pointers_per_indblock, (uint64_t)level);

	assert(dnode.max_blk_id < blocks_per_pointer * dnode.nblk_ptr);

	std::vector<char> indblock_buffer;
	indblock_buffer.reserve(indblock_size);

	size_t pointer_index = block_num / blocks_per_pointer;
	uint64_t bnum = block_num % blocks_per_pointer;

	blkptr_t blkptr = dnode.blk_ptr[pointer_index];

	while (level) {

		if(IsHole(blkptr)) {
			buffer.resize(buffer.size() + data_block_size);
			return data_block_size;
		}

		assert(blkptr.props.level == level);

		indblock_buffer.clear();
		if (!ReadBlock(io, blkptr, indblock_buffer)) {
			return -1;
		}
		assert(indblock_buffer.size() == indblock_size);

		level--;

		blocks_per_pointer = std::pow(pointers_per_indblock, level);

		pointer_index = bnum / blocks_per_pointer;
		bnum = bnum % blocks_per_pointer;

		blkptr = ((blkptr_t *)indblock_buffer.data())[pointer_index];
	
	}

	assert(blkptr.props.level == 0);

	size_t origin_size = buffer.size();
	if (ReadBlock(io, blkptr, buffer)) {
		return buffer.size() - origin_size;
	}

	return -1;
}

bool ReadObjectData(W32Lib::FileEx &io, dnode_phys_t &dnode, std::vector<char> &buffer)
{
	buffer.clear();
	for (int i = 0; i <= dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, dnode, i, buffer)) {
			return false;
		}
	}
	return true;
}

bool ReadDataset(W32Lib::FileEx &io, std::vector<dnode_phys_t> objset, uint64_t os_object, Dataset &dataset)
{
	assert(os_object < objset.size());

	dnode_phys_t *dataset_dnode = &objset[os_object];
	assert(dataset_dnode->type == DMU_OT_DSL_DIR);
	assert(dataset_dnode->bonus_type == DMU_OT_DSL_DIR);
	dsl_dir_phys_t *dir_phys = (dsl_dir_phys_t *)dataset_dnode->bonus;


	std::vector<char> buff;
	for (int i = 0; i <= objset[dir_phys->dd_child_dir_zapobj].max_blk_id; i++) {
		if (!ReadDataBlock(io, objset[dir_phys->dd_child_dir_zapobj], i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> &child_dir = dataset.child_dir;
	TraversingMicroZapEntries(buff,
		[&child_dir](const uint64_t &value, const char* name) {
		child_dir.emplace(std::string(name), value);
		return true;
	});


	//
	// Read properties ZAP Object
	//
	dnode_phys_t &props_dnode = objset[dir_phys->dd_props_zapobj];
	assert(props_dnode.type == DMU_OT_DSL_PROPS);

	buff.clear();
	for (int i = 0; i <= props_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, props_dnode, i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> props;
	TraversingMicroZapEntries(buff,
		[&props](const uint64_t &value, const char* name) {
		props.emplace(std::string(name), value);
		return true;
	});


	//
	// Read Dataset`s objset
	//
	dnode_phys_t &active_dataset_dnode = objset[dir_phys->dd_head_dataset_obj];
	assert(active_dataset_dnode.bonus_type == DMU_OT_DSL_DATASET);
	dsl_dataset_phys_t *dataset_phys = (dsl_dataset_phys_t *)active_dataset_dnode.bonus;


	buff.clear();
	ReadBlock(io, dataset_phys->ds_bp, buff);
	objset_phys_t ds_objset = *(objset_phys_t *)buff.data();

	int64_t readed = 0;

	buff.clear();
	for (int i = 0; i <= ds_objset.os_meta_dnode.max_blk_id; i++) {
		readed = ReadDataBlock2(io, ds_objset.os_meta_dnode, i, buff);
		if (readed == -1) {
			return false;
		}
	}

	dataset.objset.resize(buff.size() / sizeof(dnode_phys_t));
	std::memcpy(dataset.objset.data(), buff.data(), dataset.objset.size() * sizeof(dnode_phys_t));

	//
	// Read "master node"
	//
	dnode_phys_t &master_node_dnode = dataset.objset[1];
	assert(master_node_dnode.type == DMU_OT_MASTER_NODE);

	buff.clear();
	for (int i = 0; i <= master_node_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, master_node_dnode, i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> master_node_attr;
	TraversingMicroZapEntries(buff,
		[&master_node_attr](const uint64_t &value, const char* name) {
		master_node_attr.emplace(std::string(name), value);
		return true;
	});

	//
	// Read "ROOT"
	//
	dnode_phys_t root_dnode = dataset.objset[master_node_attr["ROOT"]];
	assert(root_dnode.type == DMU_OT_DIRECTORY_CONTENTS);

	TraversingZapObject(io, root_dnode,
		[&dataset](const uint64_t &value, const char* name) {
		dataset.root_dir.emplace(std::string(name), value);
		return true;
	});

	return true;
}

bool ReadMOS(W32Lib::FileEx & io, blkptr_t &blkptr, MetaObjecSet &mos)
{
	assert(blkptr.props.type == DMU_OT_OBJSET);

	std::vector<char> buff;
	if (!ReadBlock(io, blkptr, buff)) {
		return false;
	}
	
	if (((objset_phys_t *)buff.data())->os_type != DMU_OST_META) {
		return false;
	}

	mos.objset_phys = *(objset_phys_t *)buff.data();

	//
	// ������� ������ 32-� dnode.
	//
	size_t dnode_count = mos.objset_phys.os_meta_dnode.max_blk_id <= 32 ? mos.objset_phys.os_meta_dnode.max_blk_id : 32;
	buff.clear();
	for (int i = 0; i < dnode_count; i++) {
		int64_t readed = ReadDataBlock2(io, mos.objset_phys.os_meta_dnode, i, buff);
		if (readed != mos.objset_phys.os_meta_dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE) {
			int x = 0;
		}
	}

	mos.objset.resize(buff.size()/sizeof(dnode_phys_t));
	std::memcpy(mos.objset.data(), buff.data(), mos.objset.size() * sizeof(dnode_phys_t));

	dnode_phys_t &obj_dir_dnode = mos.objset[DMU_POOL_DIRECTORY_OBJECT];
	if (obj_dir_dnode.type != DMU_OT_OBJECT_DIRECTORY) {
		return false;
	}

	TraversingZapObject(io, obj_dir_dnode,
		[&mos](const uint64_t &value, const char* name) {
		if (std::strcmp(name, DMU_POOL_ROOT_DATASET) == 0) {
			mos.root_dataset_obj = value;
		}
		mos.object_dir.emplace(std::string(name), value);
		return true;
	});


	return true;
}

bool zfs_blkptr_verify(const blkptr_t &bp)
{
	bool result = true;
	std::string description = "blkptr has: ";

	if (!DMU_OT_IS_VALID(bp.props.type)) {
		description += " Invalid DMU Object TYPE;";
		result = false;
	}

	if (bp.props.compression >= ZIO_COMPRESS_FUNCTIONS ) {
		description += " Invalid COMPRESS;";
		result = false;
	}

	if (bp.props.logical_size > SPA_MAXBLOCKSIZE) {
		description += "Invalid LSIZE;";
	}

	if (bp.props.physical_size > SPA_MAXBLOCKSIZE) {
		description += "Invalid PSIZE;";
		result = false;
	}

	if (bp.props.embedded) {
		blk_props_emb_t *props = (blk_props_emb_t *)&bp.props;
		if (props->etype > NUM_BP_EMBEDDED_TYPES) {
			description += " Invalid EMBEDDED TYPE;";
			result = false;
		}
		if (props->physical_size > 6 * 8 + 3 * 8 + 5 * 8) {
			result = false;
		}
	} else {
		if (bp.props.checksum >= ZIO_CHECKSUM_FUNCTIONS) {
			description += " Invalid CHECKSUM;";
			result = false;
		}
	}

	/*
	* Pool-specific checks.
	*
	* Note: it would be nice to verify that the blk_birth and
	* BP_PHYSICAL_BIRTH() are not too large.  However, spa_freeze()
	* allows the birth time of log blocks (and dmu_sync()-ed blocks
	* that are in the log) to be arbitrarily large.
	*/

	//for (int i = 0; i < BP_GET_NDVAS(bp); i++) {

	//	uint64_t vdevid = DVA_GET_VDEV(&bp->blk_dva[i]);
	//	vdev_t *vd;
	//	uint64_t offset, asize;
	//	if (vdevid >= spa->spa_root_vdev->vdev_children) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);
	//	}
	//	vd = spa->spa_root_vdev->vdev_child[vdevid];
	//	if (vd == NULL) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);
	//	}
	//	if (vd->vdev_ops == &vdev_hole_ops) {
	//		zfs_panic_recover("blkptr at %p DVA %u has hole "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);

	//	}
	//	if (vd->vdev_ops == &vdev_missing_ops) {
	//		/*
	//		* "missing" vdevs are valid during import, but we
	//		* don't have their detailed info (e.g. asize), so
	//		* we can't perform any more checks on them.
	//		*/
	//		continue;
	//	}

	//	offset = DVA_GET_OFFSET(&bp->blk_dva[i]);
	//	asize = DVA_GET_ASIZE(&bp->blk_dva[i]);
	//	if (BP_IS_GANG(bp))
	//		asize = vdev_psize_to_asize(vd, SPA_GANGBLOCKSIZE);
	//	if (offset + asize > vd->vdev_asize) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"OFFSET %llu",
	//			bp, i, (longlong_t)offset);
	//	}
	//}

	return result;
}

int ByteSizeToShiftSize(uint64_t byte_size)
{
	if (byte_size) {
		for (size_t shift = 0; shift < sizeof(uint64_t) * 8; shift++) {
			if (byte_size & (uint64_t)1) { return shift; }
			byte_size >>= 1;
		}
	}
	return -1;
}

bool TraversingZapObject(W32Lib::FileEx &io, dnode_phys_t &dnode, std::function<bool(const uint64_t&, const char*)> callback)
{
	std::vector<char> buffer;
	buffer.reserve(dnode.data_blk_sz_sec*SPA_MINBLOCKSIZE);

	if (!ReadDataBlock(io, dnode, 0, buffer)) {
		return false;
	}

	zap_phys *zap_hdr = (zap_phys *)buffer.data();

	if (zap_hdr->block_type == ZBT_MICRO) {	
		buffer.clear();
		for (int i = 0; i <= dnode.max_blk_id; i++) {
			if (!ReadDataBlock(io, dnode, i, buffer)) {
				return false;
			}
		}
		return TraversingMicroZapEntries(buffer, callback);
	} else if ((zap_hdr->block_type == ZBT_HEADER) && (zap_hdr->magic == ZAP_MAGIC)) {
		return TraversingFatZapEntries(io, dnode, callback);	
	}

	return false;
}

bool TraversingFatZapEntries(W32Lib::FileEx &io, dnode_phys_t &dnode, std::function<bool(const uint64_t&, const char*)> callback)
{
	size_t block_size = dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE;
	std::vector<char> buffer;
	buffer.reserve(block_size);

	if (!ReadDataBlock(io, dnode, 0, buffer)) { return false; }

	zap_phys_t zap_phys = *(zap_phys_t *)buffer.data();

	if (zap_phys.block_type != ZBT_HEADER) { return false;	}
	if (zap_phys.magic != ZAP_MAGIC) { return false; }

	//
	// �� ������ ������ ���������� ��������� ������ ���������� ������� ����������
	//

	assert(zap_phys.ptrtbl.start_blk == 0);
	assert(zap_phys.ptrtbl.num_blks == 0);

	size_t block_size_shift = ByteSizeToShiftSize(block_size);
	size_t num_hash_entries = ZAP_LEAF_HASH_NUMENTRIES(block_size_shift);
	size_t num_chunks = ZAP_LEAF_NUMCHUNKS(block_size_shift);

	std::vector<char> leaf_buffer;
	leaf_buffer.reserve(block_size);

	for (int block = 1; block <= dnode.max_blk_id; block++) {
	
		leaf_buffer.clear();
		if (!ReadDataBlock(io, dnode, block, leaf_buffer)) { return false; }
	
		zap_leaf_phys *leaf = (zap_leaf_phys *)leaf_buffer.data();

		if ((leaf->hdr.block_type != ZBT_LEAF) || (leaf->hdr.magic != ZAP_LEAF_MAGIC)) {
			continue;
		}

		zap_leaf_chunk *chunks = (zap_leaf_chunk *)(leaf_buffer.data() + 2 * ZAP_LEAF_CHUNKSIZE + 2 * num_hash_entries);

		for (int i = 0; i < num_hash_entries; i++) {

			if ((leaf->hash[i] != CHAIN_END) && (leaf->hash[i] < num_chunks)) {
			
				zap_leaf_chunk::zap_leaf_entry *entry = (zap_leaf_chunk::zap_leaf_entry *)&chunks[leaf->hash[i]];
				while (entry != nullptr) {
					if (entry->type != ZAP_CHUNK_ENTRY) {
						continue;
					}
					if (entry->name_length > ZAP_MAXNAMELEN) {
						continue;
					}
					if ((entry->value_intlen != 1) && (entry->value_intlen != 2) && (entry->value_intlen != 4) && (entry->value_intlen != 8)) {
						continue;
					}
					if (entry->value_intlen * entry->value_numints > ZAP_MAXVALUELEN) {
						continue;
					}

					std::string name = "";
					name.reserve(ZAP_MAXNAMELEN);
					size_t name_len = entry->name_length;
					if (entry->name_chunk >= num_chunks) {
						continue;
					}

					zap_leaf_chunk::zap_leaf_array *chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[entry->name_chunk];

					while (name_len) {
						if (chunk_array->type != ZAP_CHUNK_ARRAY) {
							break;
						}
						if (name_len > ZAP_LEAF_ARRAY_BYTES) {
							name += std::string((const char *)chunk_array->array, ZAP_LEAF_ARRAY_BYTES);
							name_len -= ZAP_LEAF_ARRAY_BYTES;
							if ((chunk_array->next == CHAIN_END) || (chunk_array->next > num_chunks)) {
								break;
							}
							chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[chunk_array->next];
						} else {
							name += std::string((const char *)chunk_array->array, name_len);
							break;
						}
					}

					//
					// ���� � �������� � ������, value ����� ���� ��������, �� ��� � ����� ���������.
					// ���� �������� ��������� ������ ������ ��������. 
					//
					assert(entry->value_numints == 1);

					if (entry->value_chunk >= num_chunks) {
						break;
					}

					chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[entry->value_chunk];
					if (chunk_array->type != ZAP_CHUNK_ARRAY) {
						break;
					}

					uint64_t value = 0;
					switch (entry->value_intlen) {
					case 1:
						value = *(uint8_t *)chunk_array->array;
						break;
					case 2:
						value = _byteswap_ushort(*(uint16_t *)chunk_array->array);
						break;
					case 4:
						value = _byteswap_ulong(*(uint32_t *)chunk_array->array);
						break;
					case 8:
						value = _byteswap_uint64(*(uint64_t *)chunk_array->array);
						break;
					default:
						break;
					}

					if (!callback(value, name.c_str())) {
						return true;
					}
										
					if ((entry->next != CHAIN_END) && (entry->next < num_chunks)) {
						entry = (zap_leaf_chunk::zap_leaf_entry *)&chunks[entry->next];
					} else {
						entry = nullptr;
					}
				}
			}		
		}
	}
	return true;
}
