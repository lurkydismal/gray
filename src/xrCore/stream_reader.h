#pragma once

class XRCORE_API CStreamReader :
	public IReaderBase
{
private:
	FileHandle	m_file_mapping_handle;
	u32		m_start_offset;
	u32		m_file_size;
	u32		m_archive_size;
	u32		m_window_size;

private:
	u32		m_current_offset_from_start;
	u32		m_current_window_size;
	u8* m_current_map_view_of_file;
	u8* m_start_pointer;
	u8* m_current_pointer;

private:
	void			map(const u32& new_offset);
	IC void			unmap();
	IC void			remap(const u32& new_offset);

private:
	// should not be called
	IC CStreamReader(const CStreamReader& object);
	IC CStreamReader& operator=			(const CStreamReader&);

public:
	IC CStreamReader();

public:
	virtual	void construct(
		const FileHandle& file_mapping_handle,
		const u32& start_offset,
		const u32& file_size,
		const u32& archive_size,
		const u32& window_size
	);
	virtual	void			destroy();

public:
	IC		const FileHandle& file_mapping_handle() const;

	IC		u32		elapsed() const override;
	IC		u32		length() const override;
	IC		void	seek(int offset) override;
	IC		u32		tell() const override;

	IC		void	close();

	void			advance(int offset) override;
	void			r(void* buffer, u32 buffer_size) override;
	CStreamReader* open_chunk(const u32& chunk_id);
	u32				find_chunk(u32 ID, BOOL* bCompressed = 0);

public:
	void			r_stringZ(shared_str& dest);
private:
	typedef IReaderBase			inherited;
};

#include "stream_reader_inline.h"