#include "dhfs.h"

using namespace dvr;

int DHFS::StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp & end_time)
{
	Volume volume(dhfs_volume);
	if (volume.Open()) {

		//volume.SetPointer(846552576LL*512);

		//volume.SetPointer(294400*512LL);

		std::vector<BYTE> sequence;
		FrameSequenceInfo sequence_info;
		FileStorage storage(32, out_directory);

		sequence.resize(100*1024*1024);

		while (volume.NextFrameSequence(sequence, sequence_info)) {

			if (start_time.Seconds()) {
				if (sequence_info.start_frame.timestamp.Seconds() < start_time.Seconds()) {
					if (sequence_info.end_frame.timestamp.Seconds() < start_time.Seconds()) {
						continue;
					}
				}
			}

			if (end_time.Seconds()) {
				if (sequence_info.start_frame.timestamp.Seconds() > end_time.Seconds()) {
					continue;
				}
			}

			storage.SaveFrameSequence(sequence, sequence_info);
		}
	}

	return 0;
}
