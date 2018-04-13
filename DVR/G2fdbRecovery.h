#ifndef _G2FDB_RECOVERY_H
#define _G2FDB_RECOVERY_H

#include <string>
#include "File.h"
#include "dvr.h"
#include "G2fdbVolume.h"

namespace G2FDB
{
	class VideFile
	{
	private:
	public:
		VideFile() {}
		~VideFile() {}
	};

	class VideoStorage
	{
	private:
		std::string base_directory;
		bool CreateDirectory(const std::string &directory);
	public:
		VideoStorage() {}
		~VideoStorage() {}
		bool Open(const std::string &storage_directory);
		bool SaveFrameSequence(FrameSequence &sequence);
	};

	int StartRecovery(const std::string &dvr_volume, const std::string &out_directory, const dvr::Timestamp &start_time = dvr::Timestamp(), const dvr::Timestamp &end_time = dvr::Timestamp());
	int StartRecovery(const dvr::RecoveryTask &task);
	void StartRcoveryByMetadata(const dvr::RecoveryTask &task);
	void TestRecovery(void);
}

#endif // _G2FDB_RECOVERY_H