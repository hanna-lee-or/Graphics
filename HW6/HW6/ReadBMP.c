/******************************************************************/
/*                1615057 ���ѳ�                  */
/******************************************************************/

#include <stdio.h>    // fopen, fread, fseek, fprintf, fclose �Լ��� ����� ��� ����
#include <stdlib.h>   // malloc, free �Լ��� ����� ��� ����
#include "raytrace.h"

#define PIXEL_SIZE   3    // �ȼ� �� ���� ũ�� 3����Ʈ(24��Ʈ)
#define PIXEL_ALIGN  4    // �ȼ� ������ ���� �� ���� 4�� ��� ũ��� �����

color** colorMap;
int map_width;
int map_height;

void ReadBMPFile(char* file_name)
{
	FILE *fpBmp;                    // ��Ʈ�� ���� ������
	BITMAPFILEHEADER fileHeader;    // ��Ʈ�� ���� ��� ����ü ����
	BITMAPINFOHEADER infoHeader;    // ��Ʈ�� ���� ��� ����ü ����

	unsigned char *image;    // �ȼ� ������ ������
	int size;                // �ȼ� ������ ũ��
	int padding;             // �ȼ� �������� ���� ũ�Ⱑ 4�� ����� �ƴ� �� ���� ������ ũ��
	int i = 0;

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
	map_height = infoHeader.biHeight;     // ��Ʈ�� �̹����� ���� ũ��

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

	colorMap = (color**) malloc(sizeof(color*) *map_height);

	for (i = 0; i < map_height; i++) {
		colorMap[i] = (color*)malloc(sizeof(color) * map_width);
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
			colorMap[u][x].b = b / 255.0;
			colorMap[u][x].g = g / 255.0;
			colorMap[u][x].r = r / 255.0;

		}
	}

	free(image);      // �ȼ� �����͸� ������ ���� �޸� ����

}