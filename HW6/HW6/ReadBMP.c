/******************************************************************/
/*                1615057 이한나                  */
/******************************************************************/

#include <stdio.h>    // fopen, fread, fseek, fprintf, fclose 함수가 선언된 헤더 파일
#include <stdlib.h>   // malloc, free 함수가 선언된 헤더 파일
#include "raytrace.h"

#define PIXEL_SIZE   3    // 픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN  4    // 픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨

color** colorMap;
int map_width;
int map_height;

void ReadBMPFile(char* file_name)
{
	FILE *fpBmp;                    // 비트맵 파일 포인터
	BITMAPFILEHEADER fileHeader;    // 비트맵 파일 헤더 구조체 변수
	BITMAPINFOHEADER infoHeader;    // 비트맵 정보 헤더 구조체 변수

	unsigned char *image;    // 픽셀 데이터 포인터
	int size;                // 픽셀 데이터 크기
	int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기
	int i = 0;

	fpBmp = fopen(file_name, "rb");    // 비트맵 파일을 바이너리 모드로 열기
	if (fpBmp == NULL) {    // 파일 열기에 실패하면
		printf(".bmp 파일 오류");
		return 0;         // 프로그램 종료
	}

	// 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
	if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
	{
		printf(".bmp 파일 오류");
		fclose(fpBmp);
		return 0;
	}

	// 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
	// 매직 넘버가 맞지 않으면 프로그램 종료
	if (fileHeader.bfType != 'MB')
	{
		printf(".bmp 파일 오류");
		fclose(fpBmp);
		return 0;
	}

	// 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
	if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
	{
		printf(".bmp 헤더 오류");
		fclose(fpBmp);
		return 0;
	}

	// 24비트 비트맵이 아니면 프로그램 종료
	if (infoHeader.biBitCount != 24)
	{
		printf(".bmp 파일 오류");
		fclose(fpBmp);
		return 0;
	}

	size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
	map_width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
	map_height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

	// 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고
	// 4로 나머지를 구함. 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
	// 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함.
	padding = (PIXEL_ALIGN - ((map_width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

	if (size == 0)    // 픽셀 데이터 크기가 0이라면
	{
		// 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴.
		// 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음.
		size = (map_width * PIXEL_SIZE + padding) * map_height;
	}

	image = malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당.

	// 파일 포인터를 픽셀 데이터의 시작 위치로 이동
	fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

	// 파일에서 픽셀 데이터 크기만큼 읽음.
	//읽기에 실패하면 파일 포인터를 닫고 프로그램 종료.
	if (fread(image, size, 1, fpBmp) < 1)
	{
		printf(".bmp 데이터 오류");
		fclose(fpBmp);
		return 0;
	}

	fclose(fpBmp);    // 비트맵 파일 닫기

	colorMap = (color**) malloc(sizeof(color*) *map_height);

	for (i = 0; i < map_height; i++) {
		colorMap[i] = (color*)malloc(sizeof(color) * map_width);
	}

	// 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
	// 세로 크기만큼 반복
	for (int y = map_height - 1; y >= 0; y--)
	{
		// 가로 크기만큼 반복
		for (int x = 0; x < map_width; x++)
		{
			// 일렬로 된 배열에 접근하기 위해 인덱스를 계산
			// (x * 픽셀 크기)는 픽셀의 가로 위치
			// (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
			// 남는 공간 * y는 줄별로 누적된 남는 공간
			int index = (x * PIXEL_SIZE) + (y * (map_width * PIXEL_SIZE)) + (padding * y);
			int u = map_height - y - 1, r, g, b;

			// 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
			RGBTRIPLE *pixel = (RGBTRIPLE *)&image[index];
			r = pixel->rgbtRed;
			g = pixel->rgbtGreen;
			b = pixel->rgbtBlue;

			// RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴
			colorMap[u][x].b = b / 255.0;
			colorMap[u][x].g = g / 255.0;
			colorMap[u][x].r = r / 255.0;

		}
	}

	free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제

}