#include "Orbita.h"
#include <assert.h>

using namespace Orbita;

int Orbita::Main(const std::string &io_name, const std::string &out_dir)
{
	Orbita::Scanner scanner(io_name);
	FRAME_SEQUENCE sequence;
	if (scanner.Open()) {
		sequence.Clear();
		while (scanner.NextFrameSequence(sequence)) {
			
			


			sequence.Clear();
		}
	}

	return 0;
}

Orbita::Scanner::Scanner(const std::string &io_name) : io(io_name, 256*512)
{

}

Orbita::Scanner::~Scanner()
{
}

void Orbita::Scanner::AlignIoPointer(void)
{
	LONGLONG offset = io.Pointer();
	LONGLONG offset_mask = 7;
	offset = (offset & offset_mask) ? ((offset & ~offset_mask) + 8) : offset;
	io.SetPointer(offset);
}

bool Orbita::Scanner::IsValidFrameHeader(HEADER *header)
{
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			return (((HEADER_0dc *)header)->IsValid());
		}
		else {
			return (((HEADER_dc *)header)->IsValid());
		}
	case 'bw':
		return (((HEADER_wb *)header)->IsValid());
	default:
		return false;
	}
}

DWORD Orbita::Scanner::HeaderExtraSize(HEADER *header)
{
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			return (sizeof(HEADER_0dc) - sizeof(HEADER));
		}
		else {
			return (sizeof(HEADER_dc) - sizeof(HEADER));
		}
	case 'bw':
		return (sizeof(HEADER_wb) - sizeof(HEADER));
	default:
		return 0;
	}
}

DWORD Orbita::Scanner::FrameDataSize(HEADER *header)
{
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			return (((HEADER_0dc *)header)->size);
		}
		else {
			return (((HEADER_dc *)header)->size);
		}
	case 'bw':
		return (((HEADER_wb *)header)->size_1);
	default:
		return 0;
	}
}

Timestamp Orbita::Scanner::FrameTimestamp(HEADER *header)
{
	Timestamp t;
	if (header->frame_type == 'cd') {
		if (header->sub_type == 0x00) {
			t = ((HEADER_0dc *)header)->timestamp.Timestamp();
		}	
	}
	return t;
}

bool Orbita::Scanner::NextFrameHeader(LONGLONG &frame_offset)
{
	AlignIoPointer();

	DWORD size_buff = 64;
	LONGLONG *buff = new LONGLONG[size_buff];
	LONGLONG current_offset = io.Pointer();

	while (io.Read((void *)buff, 64 * sizeof(LONGLONG)) == (64 * sizeof(LONGLONG))) {
		for (DWORD i = 0; i < size_buff; i++) {
			HEADER *header = (HEADER *)&buff[i];
			if (header->IsValid()) {
				if (((HEADER_dc *)header)->IsValid() || ((HEADER_wb *)header)->IsValid()) {
					frame_offset = current_offset;
					return (io.SetPointer(frame_offset));
				}			
			}
			current_offset += 8;
		}	
	}
	return false;
}

bool Orbita::Scanner::NextFrameHeaderWithTimestamp(void)
{
	AlignIoPointer();

	DWORD size_buff = 64;
	LONGLONG *buff = new LONGLONG[size_buff];
	LONGLONG current_offset = io.Pointer();

	while (io.Read((void *)buff, 64 * sizeof(LONGLONG)) == (64 * sizeof(LONGLONG))) {
		for (DWORD i = 0; i < size_buff; i++) {
			HEADER *header = (HEADER *)&buff[i];
			if (((HEADER_dc *)header)->IsValid() && (header->sub_type == 0x00)) {
				return (io.SetPointer(current_offset));
			}
			current_offset += 8;
		}
	}
	return false;
}

bool Orbita::Scanner::ReadFrame(std::vector<BYTE> &buffer, FRAME_DESCRIPTOR &frame)
{
	HEADER *header;
	HEADER_dc *header_dc;
	HEADER_0dc *header_0dc;
	HEADER_wb *header_wb;

	DWORD data_size = 0;
	DWORD header_extra_size = 0;
	DWORD frame_start = buffer.size();

	frame.Clear();
	frame.offset = io.Pointer();

	buffer.resize(buffer.size() + sizeof(HEADER));
	if (io.Read(&buffer[frame_start], sizeof(HEADER)) == sizeof(HEADER)) {

		header = (HEADER *)&buffer[frame_start];
		if (header->IsValid()) {
			frame.channel = header->channel;
			frame.frame_type = header->frame_type;
			frame.sub_type = header->sub_type;

			header_extra_size = HeaderExtraSize(header);
			if (header_extra_size) {
			
				buffer.resize(buffer.size() + header_extra_size);
				if (io.Read(&buffer[frame_start + sizeof(HEADER)], header_extra_size) == header_extra_size) {
					
					header = (HEADER *)&buffer[frame_start];
					if (IsValidFrameHeader(header)) {

						data_size = FrameDataSize(header);
						if (data_size) {

							frame.size = sizeof(HEADER) + header_extra_size + data_size;
							frame.timestamp = FrameTimestamp(header);

							buffer.resize(buffer.size() + data_size);
							if (io.Read(&buffer[frame_start + sizeof(HEADER) + header_extra_size], data_size) == data_size) {
								AlignIoPointer();
								return true;
							}
						}
					}
				}			
			}
		}
	}

	buffer.resize(frame_start);
	io.SetPointer(frame.offset + 1);
	AlignIoPointer();

	return false;
}

bool Orbita::Scanner::NextFrameSequence(FRAME_SEQUENCE &sequence)
{
	FRAME_DESCRIPTOR current_frame;

	while (NextFrameHeaderWithTimestamp()) {
		
		sequence.Clear();
		while (ReadFrame(sequence.buffer, current_frame)) {

			if (sequence.frames_count == 0x00) {
				assert(current_frame.timestamp.Seconds());
				sequence.last_timestamp = current_frame.timestamp;
				sequence.first_frame = current_frame;
				sequence.last_frame = current_frame;
				sequence.frames_count++;
			} else {
				if (sequence.IsNextFrame(current_frame)) {								
					sequence.frames_count++;
					if (current_frame.timestamp.Seconds()) {
						sequence.last_timestamp = current_frame.timestamp;					
					}					
				} else {
					sequence.buffer.resize(sequence.buffer.size() - current_frame.size);
					io.Pointer(sequence.last_frame.offset);
					break;
				}
			}
		}	
		if (sequence.frames_count)
			return true;
	}
	return false;
}
