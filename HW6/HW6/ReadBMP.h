#ifndef READBMP_H
#define READBMP_H

typedef struct BMPDATA
{
	short bits_per_pixel;
	int width;
	int height;
	unsigned char *data;
}BMPDATA;

#ifdef __cplusplus
extern "C"
{
#endif

char ReadBMPFile(const char *file_name, BMPDATA *ret_bmp_data);
char ReadBMPData(const void *bmp_file_data, BMPDATA *ret_bmp_data);

#ifdef __cplusplus
}
#endif


#endif
