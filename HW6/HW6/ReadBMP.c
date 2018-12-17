#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <io.h>
#include <fcntl.h>
#include "ReadBMP.h"

void DecodeData(const char *byte_data, int offset, BMPDATA *ret_bmp_data);
void DecodeInfomation(const char *byte_data, int offset, BMPDATA *ret_bmp_data);

char ReadBMPFile(const char *file_name, BMPDATA *ret_bmp_data)
{
	int fh;
	long file_length;
	void *bmp_file_data;

	fh = open(file_name, _O_RDONLY, _O_BINARY);
	if (fh == -1)
	{
		return -1;
	}

	lseek(fh, 0, SEEK_END);
	file_length = tell(fh);
	lseek(fh, 0, SEEK_SET);

	bmp_file_data = malloc(file_length);
	read(fh, bmp_file_data, file_length);

	close(fh);

	ReadBMPData(bmp_file_data, ret_bmp_data);

	free(bmp_file_data);
	return 0;
}

char ReadBMPData(const void *bmp_file_data, BMPDATA *ret_bmp_data)
{
	char *byte_data;   //바이트 데이터
	int offset;   //오프셋

	byte_data = (char *)bmp_file_data;

	if ((byte_data[0] != 'B') || (byte_data[1] != 'M'))   //매직 넘버 확인
	{
		return -1;
	}

	memcpy(&offset, &byte_data[10], 4);

	DecodeInfomation(byte_data, offset, ret_bmp_data);
	DecodeData(byte_data, offset, ret_bmp_data);

	return 0;
}

void DecodeInfomation(const char *byte_data, int offset, BMPDATA *ret_bmp_data)
{
	if (offset == 26)
	{
		memcpy(&ret_bmp_data->width, &byte_data[18], 2);
		memcpy(&ret_bmp_data->height, &byte_data[20], 2);
		memcpy(&ret_bmp_data->bits_per_pixel, &byte_data[24], 2);
	}
	else
	{
		memcpy(&ret_bmp_data->width, &byte_data[18], 4);
		memcpy(&ret_bmp_data->height, &byte_data[22], 4);
		memcpy(&ret_bmp_data->bits_per_pixel, &byte_data[28], 2);
	}
}

void DecodeData(const char *byte_data, int offset, BMPDATA *ret_bmp_data)
{
	int data_size;

	if (ret_bmp_data->bits_per_pixel == 24)
	{
		data_size = ret_bmp_data->width * ret_bmp_data->height * 3;
		ret_bmp_data->data = malloc(data_size);
		memcpy(ret_bmp_data->data, &byte_data[offset], data_size);

	}
}

