#pragma once

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <windows.h>
#include <string>
#include <map>
#include <vector>

namespace dvr
{
	class Timestamp
	{
	public:
		WORD year = 0;
		BYTE month = 0;
		BYTE day = 0;
		BYTE hours = 0;
		BYTE minutes = 0;
		BYTE seconds = 0;

		Timestamp(WORD year_ = 0, BYTE month_ = 0, BYTE day_ = 0, BYTE hours_ = 0, BYTE mins_ = 0, BYTE sec_ = 0);
		void Clear(void);
		ULONGLONG Seconds(void) const;
		std::string ToString(void);
		void ToString(std::string &str);
		std::string Date(char separator = '-');
		std::string Time(char separator = '-');

		bool operator>(const Timestamp &t);
		bool operator>=(const Timestamp &t);
		bool operator<(const Timestamp &t);
		bool operator<=(const Timestamp &t);
		bool operator==(const Timestamp &t);
		ULONGLONG operator-(const Timestamp &t);
		operator bool(void);
	};

	struct RecoveryTask {
		std::string io_name;
		uint64_t io_offset;
		uint64_t io_size;
		std::string output_dir;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
	};

	struct Frame {
		uint64_t offset = 0;
		uint32_t camera = 0;
		dvr::Timestamp time;
		std::vector<uint8_t> buffer;

		void Clear(void);
	};

	struct FrameSequence {
		uint64_t first_frame_offset = 0;
		uint64_t last_frame_offset = 0;
		uint32_t camera = 0;
		size_t frames_count = 0;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
		std::vector<uint8_t> buffer;
		
		void Clear(void);
	};

	class VideoFile 
	{
	private:
		std::string io_path;

		uint64_t first_frame_offset = 0;
		uint64_t last_frame_offset = 0;
		uint32_t camera = 0;
		size_t frames_count = 0;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
	public:
		VideoFile(const std::string &file_path);
		bool AppendFrameSequence(FrameSequence &seq) { return false; }
		std::string Date(void) { return start_time.Date(); }
		std::string Time(void) { return start_time.Time(); }
		uint32_t Camera(void) { return camera; }
		uint64_t Size(void) { return 0; }
	};

	class VideoStorage
	{
	private:
		std::string dir_path;
		size_t max_file_size = 512*1024*1024;

		std::map<std::string, std::map<uint32_t, VideoFile *>> storage;
	public:
		VideoStorage(const std::string &path);
		~VideoStorage(void);
		bool Open(void);
		bool Close(void);
		bool SaveFrameSequence(FrameSequence &seq);
	private:
		VideoFile * CreateNewFile(FrameSequence &seq);
		bool CloseFile(VideoFile *file) {return false;}
		//bool CloseAllFiles(FrameSequence &seq);
		VideoFile * GetVideoFile(const std::string &date, uint32_t camera);
	};


}

#endif