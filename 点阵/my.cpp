#include<iostream>

#include<opencv/cv.h>

#include"opencv2/opencv.hpp"

#include<opencv/cxcore.h>

#include<opencv/highgui.h>

#include<math.h>

using namespace cv;

using namespace std;

void paint_chinese(Mat& image,int x_offset,int y_offset,unsigned long offset);

void paint_ascii(Mat& image,int x_offset,int y_offset,unsigned long offset);

void put_text_to_image(int x_offset,int y_offset,String image_path,char* logo_path);

int main(){

    String image_path="tupian.jpeg";//图片路径

    char* logo_path=(char*)"logo.txt";//学号姓名路径

    put_text_to_image(20,300,image_path,logo_path);

    return 0;

}



void paint_ascii(Mat& image,int x_offset,int y_offset,unsigned long offset){

    //绘制的起点坐标

	Point p;

	p.x = x_offset;

	p.y = y_offset;

	 //存放ascii字膜

	char buff[16];           

	//打开ascii字库文件

	FILE *ASCII;

	if ((ASCII = fopen("Asci0816.zf", "rb")) == NULL){

		printf("Can't open ascii.zf,Please check the path!");

		//getch();

		exit(0);

	}

	fseek(ASCII, offset, SEEK_SET);

	fread(buff, 16, 1, ASCII);

	int i, j;

	Point p1 = p;

	for (i = 0; i<16; i++)                  //十六个char

	{

		p.x = x_offset;

		for (j = 0; j < 8; j++)              //一个char八个bit

		{

			p1 = p;

			if (buff[i] & (0x80 >> j))    /*测试当前位是否为1*/

			{

				/*

					由于原本ascii字膜是8*16的，不够大，

					所以原本的一个像素点用4个像素点替换，

					替换后就有16*32个像素点

					ps：感觉这样写代码多余了，但目前暂时只想到了这种方法

				*/

				circle(image, p1, 0, Scalar(0, 0, 255), -1);

				p1.x++;

				circle(image, p1, 0, Scalar(0, 0, 255), -1);

				p1.y++;

				circle(image, p1, 0, Scalar(0, 0, 255), -1);

				p1.x--;

				circle(image, p1, 0, Scalar(0, 0, 255), -1);

				

			}						

			p.x+=2;            //原来的一个像素点变为四个像素点，所以x和y都应该+2

		}

		p.y+=2;

	}

}



void paint_chinese(Mat& image,int x_offset,int y_offset,unsigned long offset){//在图片上画汉字

    Point p;

    p.x=x_offset;

    p.y=y_offset;

    FILE *HZK;

    char buff[72];//72个字节，用来存放汉字的

    if((HZK=fopen("HZKs2424.hz","rb"))==NULL){

        printf("Can't open HZKf2424.hz,Please check the path!");

        exit(0);//退出

    }

    fseek(HZK, offset, SEEK_SET);/*将文件指针移动到偏移量的位置*/

    fread(buff, 72, 1, HZK);/*从偏移量的位置读取72个字节，每个汉字占72个字节*/

    bool mat[24][24];//定义一个新的矩阵存放转置后的文字字膜

    int i,j,k;

    for (i = 0; i<24; i++)                 /*24x24点阵汉字，一共有24行*/

	{

		for (j = 0; j<3; j++)                /*横向有3个字节，循环判断每个字节的*/

			for (k = 0; k<8; k++)              /*每个字节有8位，循环判断每位是否为1*/

				if (buff[i * 3 + j] & (0x80 >> k))    /*测试当前位是否为1*/

				{

					mat[j * 8 + k][i] = true;          /*为1的存入新的字膜中*/

				}

				else {

					mat[j * 8 + k][i] = false;

				}

	}

    for (i = 0; i < 24; i++)

	{

		p.x = x_offset;

		for (j = 0; j < 24; j++)

		{		

			if (mat[i][j])

				circle(image, p, 1, Scalar(255, 0, 0), -1);		  //写(替换)像素点

			p.x++;                                                //右移一个像素点

		}

		p.y++;                                                    //下移一个像素点

	}

}



void put_text_to_image(int x_offset,int y_offset,String image_path,char* logo_path){//将汉字弄上图片
//x和y就是第一个字在图片上的起始坐标
    //通过图片路径获取图片

    Mat image=imread(image_path);

    int length=18;//要打印的字符长度

    unsigned char qh,wh;//定义区号，位号

    unsigned long offset;//偏移量

    unsigned char hexcode[30];//用于存放记事本读取的十六进制,记得要用无符号

    FILE* file_logo;

    if ((file_logo = fopen(logo_path, "rb")) == NULL){

		printf("Can't open txtfile,Please check the path!");

		//getch();

		exit(0);

	}

    fseek(file_logo, 0, SEEK_SET);

    fread(hexcode, length, 1, file_logo);

    int x =x_offset,y = y_offset;//x,y:在图片上绘制文字的起始坐标

    for(int m=0;m<length;){

        if(hexcode[m]==0x23){

            break;//读到#号时结束

        }

        else if(hexcode[m]>0xaf){

            qh=hexcode[m]-0xaf;//使用的字库里是以汉字啊开头，而不是以汉字符号开头

            wh=hexcode[m+1] - 0xa0;//计算位码

            offset=(94*(qh-1)+(wh-1))*72L;

            paint_chinese(image,x,y,offset);

            /*

            计算在汉字库中的偏移量

            对于每个汉字，使用24*24的点阵来表示的

            一行有三个字节，一共24行，所以需要72个字节来表示

            如赵字

            区位码是5352

            十六进制位3534

            机内码就是d5d4

            d5-af=38（十进制），因为是从汉字啊开始的，所以减去的是af而不是a0，38+15等于53与区码相对应

            d4-a0=52

            */

            m=m+2;//一个汉字的机内码占两个字节，

            x+=24;//一个汉字为24*24个像素点，由于是水平放置，所以是向右移动24个像素点

        }

        else{//当读取的字符为ASCII码时

        wh=hexcode[m];

        offset=wh*16l;//计算英文字符的偏移量

        paint_ascii(image,x,y,offset);

        m++;//英文字符在文件里表示只占一个字节，所以往后移一位就行了

        x+=16;
 }



    }

    cv::imshow("image", image);

    cv::waitKey();

}



