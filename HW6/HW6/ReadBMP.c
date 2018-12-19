/******************************************************************/
/*                1615057 ���ѳ�                  */
/******************************************************************/

#include <stdio.h>    // fopen, fread, fseek, fprintf, fclose �Լ��� ����� ��� ����
#include <stdlib.h>   // malloc, free �Լ��� ����� ��� ����
#include "raytrace.h"

#define PIXEL_SIZE   3    // �ȼ� �� ���� ũ�� 3����Ʈ(24��Ʈ)
#define PIXEL_ALIGN  4    // �ȼ� ������ ���� �� ���� 4�� ��� ũ��� �����

void ReadBMPFile(char* file_name, int flag, int* width, int* height)
{
	FILE *fpBmp;                    // ��Ʈ�� ���� ������
	BITMAPFILEHEADER fileHeader;    // ��Ʈ�� ���� ��� ����ü ����
	BITMAPINFOHEADER infoHeader;    // ��Ʈ�� ���� ��� ����ü ����

	unsigned char *image;    // �ȼ� ������ ������
	int size;                // �ȼ� ������ ũ��
	int padding;             // �ȼ� �������� ���� ũ�Ⱑ 4�� ����� �ƴ� �� ���� ������ ũ��
	int i = 0, map_width, map_height;

	fpBmp = fopen(file_name, "rb");    // ��Ʈ�� ������ ���̳ʸ� ���� ����
	if (fpBmp == NULL) {    // ���� ���⿡ �����ϸ�
		printf(".bmp ���� ����");
		return 0;         // ���α׷� ����
	}

	// ��Ʈ�� ���� ��� �б�. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
	if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
	{
		printf(".bmp ���� ����");
		fclose(fpBmp);
		return 0;
	}

	// ���� �ѹ��� MB�� �´��� Ȯ��(2����Ʈ ũ���� BM�� ��Ʋ ��������� �о����Ƿ� MB�� ��)
	// ���� �ѹ��� ���� ������ ���α׷� ����
	if (fileHeader.bfType != 'MB')
	{
		printf(".bmp ���� ����");
		fclose(fpBmp);
		return 0;
	}

	// ��Ʈ�� ���� ��� �б�. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
	if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
	{
		printf(".bmp ��� ����");
		fclose(fpBmp);
		return 0;
	}

	// 24��Ʈ ��Ʈ���� �ƴϸ� ���α׷� ����
	if (infoHeader.biBitCount != 24)
	{
		printf(".bmp ���� ����");
		fclose(fpBmp);
		return 0;
	}

	size = infoHeader.biSizeImage;    // �ȼ� ������ ũ��
	map_width = infoHeader.biWidth;       // ��Ʈ�� �̹����� ���� ũ��
	*width = map_width;
	map_height = infoHeader.biHeight;     // ��Ʈ�� �̹����� ���� ũ��
	*height = map_height;

	// �̹����� ���� ũ�⿡ �ȼ� ũ�⸦ ���Ͽ� ���� �� ���� ũ�⸦ ���ϰ�
	// 4�� �������� ����. �׸��� 4���� �������� ���ָ� ���� ������ ���� �� ����.
	// ���� ���� ������ 0�̶�� ���� ����� 4�� �ǹǷ� ���⼭ �ٽ� 4�� �������� ����.
	padding = (PIXEL_ALIGN - ((map_width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

	if (size == 0)    // �ȼ� ������ ũ�Ⱑ 0�̶��
	{
		// �̹����� ���� ũ�� * �ȼ� ũ�⿡ ���� ������ �����ָ� ������ ���� �� �� ũ�Ⱑ ����.
		// ���⿡ �̹����� ���� ũ�⸦ �����ָ� �ȼ� �������� ũ�⸦ ���� �� ����.
		size = (map_width * PIXEL_SIZE + padding) * map_height;
	}

	image = malloc(size);    // �ȼ� �������� ũ�⸸ŭ ���� �޸� �Ҵ�.

	// ���� �����͸� �ȼ� �������� ���� ��ġ�� �̵�
	fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

	// ���Ͽ��� �ȼ� ������ ũ�⸸ŭ ����.
	//�б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����.
	if (fread(image, size, 1, fpBmp) < 1)
	{
		printf(".bmp ������ ����");
		fclose(fpBmp);
		return 0;
	}

	fclose(fpBmp);    // ��Ʈ�� ���� �ݱ�

	if(flag == 1)
		colorMap1 = (color**) malloc(sizeof(color*) *map_width);
	else if(flag == 2)
		colorMap2 = (color**)malloc(sizeof(color*) *map_width);

	for (i = 0; i < map_width; i++) {
		if (flag == 1)
			colorMap1[i] = (color*)malloc(sizeof(color) * map_height);
		else if (flag == 2)
			colorMap2[i] = (color*)malloc(sizeof(color) * map_height);
	}

	// �ȼ� �����ʹ� �Ʒ� ���� �������� ����ǹǷ� �Ʒ��ʺ��� �ݺ�
	// ���� ũ�⸸ŭ �ݺ�
	for (int y = map_height - 1; y >= 0; y--)
	{
		// ���� ũ�⸸ŭ �ݺ�
		for (int x = 0; x < map_width; x++)
		{
			// �Ϸķ� �� �迭�� �����ϱ� ���� �ε����� ���
			// (x * �ȼ� ũ��)�� �ȼ��� ���� ��ġ
			// (y * (���� ũ�� * �ȼ� ũ��))�� �ȼ��� �� ��° ������ ���
			// ���� ���� * y�� �ٺ��� ������ ���� ����
			int index = (x * PIXEL_SIZE) + (y * (map_width * PIXEL_SIZE)) + (padding * y);
			int u = map_height - y - 1, r, g, b;

			// ���� �ȼ��� �ּҸ� RGBTRIPLE �����ͷ� ��ȯ�Ͽ� RGBTRIPLE �����Ϳ� ����
			RGBTRIPLE *pixel = (RGBTRIPLE *)&image[index];
			r = pixel->rgbtRed;
			g = pixel->rgbtGreen;
			b = pixel->rgbtBlue;

			// RGBTRIPLE ����ü�� �Ķ�, �ʷ�, �������� ������
			if (flag == 1) {
				colorMap1[x][u].b = b / 255.0;
				colorMap1[x][u].g = g / 255.0;
				colorMap1[x][u].r = r / 255.0;
			}
			else if (flag == 2) {
				colorMap2[x][u].b = b / 255.0;
				colorMap2[x][u].g = g / 255.0;
				colorMap2[x][u].r = r / 255.0;
			}
		}
	}

	free(image);      // �ȼ� �����͸� ������ ���� �޸� ����

}