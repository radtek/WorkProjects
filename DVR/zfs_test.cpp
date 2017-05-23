#include "zfs_test.h"
#include <memory>
#include <string>

#define VDEV_OFFSET						2048*512
#define VDEV_LABEL_NVLIST_OFFSET		16*1024

void zfs_test(void)
{
	W32Lib::FileEx io;

	//if (!io.Open("D:\\zol-0.6.1\\vdev1")) {
	if (!io.Open("J:\\VM\\zfs-flat.vmdk")) {
		return;
	}

	NVList nvlist("vdev label");
	std::vector<unsigned char> nvlist_buff(112*1024, 0);

	io.SetPointer(VDEV_OFFSET + VDEV_LABEL_NVLIST_OFFSET);
	io.Read(nvlist_buff.data(), nvlist_buff.size());

	DecodeXdrNVList(nvlist, nvlist_buff.data() + 4, nvlist_buff.size());


	uberblock_t *ub = nullptr;
	std::vector<unsigned char> ub_buff(1024, 0);
	int max_tgx = 0;
	int ub_idx = 0;

	for (int i = 0; i < 128; ++i) {

		io.SetPointer(VDEV_OFFSET + VDEV_LABEL_UBERBLOCKS_OFFSET + i*1024);
		io.Read(ub_buff.data(), ub_buff.size());

		ub = (uberblock_t *)ub_buff.data();
		if (ub->magic == UBERBLOCK_MAGIC) {

			if (ub->txg > max_tgx) {
				max_tgx = ub->txg;
				ub_idx = i;
			}
			
			int x = 0;
		}
	
	}

	io.SetPointer(VDEV_OFFSET + VDEV_LABEL_UBERBLOCKS_OFFSET + ub_idx * 1024);
	io.Read(ub_buff.data(), ub_buff.size());

	ub = (uberblock_t *)ub_buff.data();


	std::auto_ptr<objset_phys_t> objset(new objset_phys_t());

	io.SetPointer(VDEV_OFFSET + VDEV_DATA_OFFSET + ub->rootbp.dva[0].offset * 512);
	io.Read(&(*objset), sizeof(objset_phys_t));

	int z = 0;
}
