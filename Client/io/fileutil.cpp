#include "fileutil.h"
#include "../xz/Xz.h"
#include "../xz/7zCrc.h"
#include "../xz/XzCrc64.h"

static void *SzAlloc(void *p, size_t size) { p = p; return malloc(size); }
static void SzFree(void *p, void *address) { p = p; free(address); }
static ISzAlloc lzmaAlloc = { SzAlloc, SzFree };

void init_crc_tables()
{
	CrcGenerateTable();
	Crc64GenerateTable();
}

static uint32_t xz_unpack_data(const void* file_buffer,
	const uint64_t file_size, void** buffer, uint64_t* size)
{
	CXzUnpacker state;
	uint64_t uncompressed_size, dst_idx, src_idx;
	SizeT dst_size, src_size;
	uint32_t err;
	ECoderStatus status;

	err = XzUnpacker_Create(&state, &lzmaAlloc);

	*buffer = NULL;
	*size = 0;

	if (err != SZ_OK)
	{
		return err;
	}

	dst_idx = 0;
	src_idx = 0;

	uncompressed_size = 0;

	do
	{
		uncompressed_size += 0x40000;

		*buffer = realloc(*buffer, uncompressed_size);

		dst_size = uncompressed_size - dst_idx;
		src_size = file_size - src_idx;

		err = XzUnpacker_Code(&state, (Byte*)*buffer + dst_idx,
			&dst_size, (Byte*)file_buffer + src_idx,
			&src_size, CODER_FINISH_ANY, &status);

		src_idx += src_size;
		dst_idx += dst_size;
	}
	while ((err == SZ_OK) && (status == CODER_STATUS_NOT_FINISHED));

	XzUnpacker_Free(&state);

	if (err == SZ_OK)
	{
		*size = dst_idx;
		*buffer = realloc(*buffer, dst_idx+1);
		(*(char **)buffer)[dst_idx] = 0;
	}
	else
	{
		free(*buffer);
		*buffer = NULL;
	}

	return err;
}

uint32_t file_read(FILE* file, const uint64_t file_size, void** buffer, uint64_t* size)
{
	void* file_buffer;
	uint32_t result;

	*size = 0;
	*buffer = NULL;

	if (file_size == 0)
		return 1;

	file_buffer = malloc(file_size+1);
	if (!file_buffer)
		return 1;

	fseek(file, 0, SEEK_SET);
	if (fread(file_buffer, file_size, 1, file) != 1)
	{
		free(file_buffer);
		return 1;
	}

	if (file_size > XZ_SIG_SIZE
		&& memcmp(file_buffer, XZ_SIG, XZ_SIG_SIZE) == 0)
	{
		result = xz_unpack_data(file_buffer, file_size,
			buffer, size);
		free(file_buffer);
		return result;
	}

	((char *)file_buffer)[file_size] = 0;
	*size = file_size;
	*buffer = file_buffer;

	return 0;
}

uint32_t xz_file_read(FILE* file, void** buffer, uint64_t* size)
{
	void* file_buffer;
	uint64_t file_size;
	uint32_t result;

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	*size = 0;
	*buffer = 0;

	if (file_size <= XZ_SIG_SIZE)
		return 1;

	file_buffer = malloc(file_size);
	if (!file_buffer)
		return 1;

	fseek(file, 0, SEEK_SET);
	if (fread(file_buffer, file_size, 1, file) != 1
		|| memcmp(file_buffer, XZ_SIG, XZ_SIG_SIZE) != 0)
	{
		free(file_buffer);
		return 1;
	}

	result = xz_unpack_data(file_buffer, file_size, buffer, size);
	free(file_buffer);

	return result;
}

