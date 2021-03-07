#ifndef _BMP_H
#define _BMP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int show_bmp(char *name)
{
	
	//打开显示屏的设备文件
	int fd_lcd = open("/dev/fb0", O_RDWR);
	if(-1 == fd_lcd)
	{
		printf("fb0 open err!\n");
		return -1;
	}
	
	//打开图片文件
	int fd_bmp = open(name, O_RDWR);
	if(-1 == fd_bmp)
	{
		printf("picture open err!\n");
		return -1;
	}
	
	//偏移54字节文件头（剔除文件信息）
	lseek(fd_bmp, 54, SEEK_SET);
	
	//读取文件中的颜色数据
	char buf[800*480*3] = {0};
	read(fd_bmp, buf, 800*480*3);
	
	int lcd_buf_tmp[800*480] = {0};
	int i;
	for(i=0; i<800*480; i++)
		//				B		     G			    R
		lcd_buf_tmp[i] = buf[i*3] | buf[i*3+1]<<8 | buf[i*3+2]<<16;
	
	int * mmap_bmp = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	
	//按行翻转（先左右翻转，再上下翻转）
	int lcd_buf[800*480] = {0};
	int x,y;
	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			mmap_bmp[x+(479-y)*800] = lcd_buf_tmp[x+y*800];
	
	munmap(mmap_bmp, 800*480*4);
	
	if(close(fd_lcd))
	{
		printf("close err!\n");
		return -2;
	}
	if(close(fd_bmp))
	{
		printf("close err!\n");
		return -2;
	}
}

int show_bmp_any(char *name, int x_refe, int y_refe, int wide, int high)
{
	if((x_refe+wide>800) || (y_refe+high>480) || x_refe<0 || y_refe<0)
	{
		printf("err!\n");
		return -1;
	}	
	
	//打开显示屏的设备文件
	int fd_lcd = open("/dev/fb0", O_RDWR);
	if(-1 == fd_lcd)
	{
		printf("fb0 open err!\n");
		return -1;
	}
	
	//打开图片文件
	int fd_bmp = open(name, O_RDWR);
	if(-1 == fd_bmp)
	{
		printf("picture open err!\n");
		return -1;
	}
	
	//偏移54字节文件头（剔除文件信息）
	lseek(fd_bmp, 54, SEEK_SET);
	
	//读取文件中的颜色数据
	char buf[wide*high*3];
	read(fd_bmp, buf, wide*high*3);
	
	int lcd_buf_tmp[wide*high];
	int i;
	for(i=0; i<wide*high; i++)
		//				B		     G			    R
		lcd_buf_tmp[i] = buf[i*3] | buf[i*3+1]<<8 | buf[i*3+2]<<16;
	
	int * mmap_bmp = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	
	//按行翻转（先左右翻转，再上下翻转）
	int x,y;
	int lcd_buf[800*480] = {0};
	for(y=0; y<high; y++)
		for(x=0; x<wide; x++)
			lcd_buf[x+(high-y-1)*wide]= lcd_buf_tmp[x+y*wide];
		
	
	//根据起始位置写入文件数据	
	for(y=0; y<high; y++)
		for(x=0; x<wide; x++)
			mmap_bmp[x+x_refe+(y+y_refe)*800] = lcd_buf[x+y*wide];
	
	munmap(mmap_bmp, 800*480*4);
	
	if(close(fd_lcd))
	{
		printf("fb0 close err!\n");
		return -2;
	}
	if(close(fd_bmp))
	{
		printf("picture close err!\n");
		return -2;
	}
	
}


#endif