    FATS移植流程

1. 首先先裁剪配置ffconf.h
2. 把diskio的四个函数全部实现，就是让这四个函数调用你自己实现的SD卡初始化函数，SD卡写函数，SD卡读  函数，获取SD卡状态函数
    还要#define DEV_SD  0     因为我们需要知道设备号
3. 实现ff.c里的RTC时间获取函数


接下来就可以使用FATS操作我们的SD卡啦。


下面是使用FATS操作SD的流程




static FATFS fs;        // 文件系统对象
FIL file;        // 文件对象
FRESULT fr;      // FatFs 函数结果
void sd_app(void)
{
	uint8_t buf[]="你好FATS，我是xxx\r\n";  
	uint8_t buf1[128]=0;
	UINT bw;
	UINT br;
	if(disk_initialize(0)!=0) printf("SD初始化失败\r\n");
	if(f_mount(&fs,"0:",1)!=0) printf("SD卡挂载失败\r\n");  //
	if(f_open(&file,"ghs.txt",FA_CREATE_ALWAYS |FA_WRITE)!=0) printf("SD卡文件打开失败\r\n");
	
	if(f_write(&file,buf,strlen(buf),&bw)!=0) printf("SD卡文件写入失败\r\n"); 
	
	if(f_close(&file)!=0) printf("SD卡文件关闭失败\r\n"); 
	
	if(f_open(&file,"ghs.txt",FA_READ)!=0) printf("SD卡文件打开失败\r\n"); 
	
	if(f_read(&file,buf1,sizeof(buf1)-1,&br)!=0) printf("SD卡文件读出失败\r\n"); 
	
	if(f_close(&file)!=0)  printf("SD卡文件读出失败\r\n"); 
	
	printf("ghs.txt:\r\n");
	printf("%s\r\n",buf1);
	
	
}

        使用FATS操作SD卡流程
1. 调用初始化函数。disk_initialize(0)  参数是设备编号
2. 调用打开文件函数 f_open(&file,"ghs.txt",FA_CREATE_ALWAYS |FA_WRITE)
    FRESULT f_open (FIL* fp,const TCHAR* path, BYTE mode); 函数原型
这个函数是打开文件函数。一共有三个参数，第一个参数是文件控制块，负责保存文件的属性；第二个参数是文件名，第三个是模式（也就是要进行什么 操作），比如我接下来我进行写SD卡的操作，我就要在这个参数填写FA_WRITE，还有FA_CREATE_ALWAYS这个是假如没有这个文件，我就会创建一个
下面就是所有的操作模式
/* File access mode and open method flags (3rd argument of f_open) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

3. 调用f_write(&file,buf,strlen(buf),&bw)  将buf存储的数据保存进SD卡中的ghs.txt，因为我们之前的f_open函数就将文件的信息都写入&file了。

FRESULT f_write (FIL* fp,const void* buff,UINT btw,	UINT* bw) 函数原型
参数1：FIL* fp：这个参数就是文件控制块，参数的数据类型是FIL*。
参数2：const void* buff：这个参数写入数据的指针。
参数3：UINT btw：期望写入的字节数 这个用sizeof算出字节数
参数4：UINT* bw 实际写入的字节数 这个传入一个类型为UINT*的变量。

4. 调用关闭函数f_close(&file) 将文件保存并关闭。任何操作完都要调用这个文件关闭函数，否则就写入就白写了。



读操作和写操作的流程一样
初始化初始一次就可以。先打开文件，再调用读出函数，再调用关闭文件函数。


