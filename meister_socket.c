#include <stdio.h>
#include "libfuffafo.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <SDL/SDL.h>
#include <opencv/cv.h>
#include <stdlib.h>
#include <memory.h>
#include <jpeglib.h>

//#define PORT (in_port_t)50000
//#define FILENAME "map_capture.bmp"


int main(){
	int sock_my;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	char buf[1024];
	int i,result;
	int sock_cl;
	int size;
	FILE *fp_w;
	socklen_t len;
	fd_set sock_wait;
	struct timeval tv;

	sock_my=socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=INADDR_ANY;

	bind(sock_my,(struct sockaddr *)&addr,sizeof(addr));

	listen(sock_my,5);

	SDL_Event e;
	SDL_Surface *screen;
	

	SDL_Init(SDL_INIT_EVERYTHING);
	screen=SDL_SetVideoMode(1920,1080,32,SDL_HWSURFACE|SDL_FULLSCREEN);

	while(1){
		rstart:
		len=sizeof(client);
		sock_cl=accept(sock_my,(struct sockaddr *)&client,&len);
		FD_ZERO(&sock_wait);
		FD_SET(sock_cl,&sock_wait);
		
		tv.tv_sec=0;
		tv.tv_usec=500000;
		
		if((fp_w=fopen(FILENAME,"wb"))<0){
			printf("ERROR:can't open file...");
			return -1;
		}
//		if(select(sock_my,&sock_wait,NULL,NULL,&tv)==0){
//			return -1;
//		}
		
		while(1){
			size=recv(sock_cl,buf,sizeof(buf),0);

			if(size<=0){
				if((result=select(sock_cl+1,&sock_wait,NULL,NULL,&tv))==0){
					goto rstart;
				}else if(result<0){
					perror("select()");
					goto rstart;
				}else{
					size=recv(sock_cl,buf,sizeof(char),0);
				}
			};

			fwrite(buf,sizeof(char),size,fp_w);

//			printf("%d",size);

		}
		fwrite((char *)0xFFD9,(unsigned int)2,1,fp_w);
		fclose(fp_w);
//		process_cap();

		MyDisplay(screen);
                if(SDL_PollEvent(&e)==1){
                        if(e.type==SDL_KEYDOWN){
                        	printf("pushed key...\n");
				break;
                        }
                }
		close(sock_cl);
	}
	close(sock_my);
	return 0;

}
/*
int MyDisplay(SDL_Surface *screen){

        IplImage *img;

        img=(IplImage *)cvLoadImage(FILENAME,2|-1);

        if(img==NULL){
                printf("ERROR:failed cvLoadImage()...\n");
                return -1;
        }

        static SDL_Rect rt_dest;
        rt_dest.x=0;
        rt_dest.y=0;
        rt_dest.w=img->width;

        SDL_Surface *sdl_img=SDL_CreateRGBSurfaceFrom(
                img->imageData,
                img->width,
                img->height,24,
                img->widthStep,
                0x00ff0000,0x0000ff00,0x000000ff,0
        );

        SDL_BlitSurface(sdl_img,NULL,screen,&rt_dest);
        SDL_UpdateRect(screen,0,0,img->width,img->height);
        SDL_Flip(screen);

        return 0;
}

JSAMPARRAY turnLeft(JSAMPARRAY img,int height,int width)
{

	int i,j;
	
	//回転後用のイメージ
	JSAMPARRAY outImg =(JSAMPARRAY)malloc(sizeof(JSAMPROW)*width);
	
	for (i=0; i<width; i++)
	{
		outImg[i]=(JSAMPROW)calloc(sizeof(JSAMPLE),3*height);
	}
	

	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			outImg[j][i*3+0]=img[i][j*3+0];
			outImg[j][i*3+1]=img[i][j*3+1];
			outImg[j][i*3+2]=img[i][j*3+2];
		}
	}
	
	for ( i = 0; i<height; i++ ) free( img[i] );
	free( img );
	
	return outImg;
}

JSAMPARRAY in(char FileName[],int *widthAddress,int *heightAddress)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *infile;

	JSAMPARRAY img;
	int i, j;
	int width;
	int height;

	// JPEGオブジェクトの初期化
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );

	// ファイルを開く
	infile = fopen(FileName,"rb" );
	jpeg_stdio_src( &cinfo, infile );

	if(infile==NULL) printf("fuckn");

	// ヘッダの読み込み
	jpeg_read_header( &cinfo,TRUE );

	// 展開の開始
	jpeg_start_decompress( &cinfo );

	// 幅と高さの取得
	width = cinfo.output_width;
	height = cinfo.output_height;
	*widthAddress=width;
	*heightAddress=height;

	// イメージを保持するメモリ領域の確保と初期化
	img = (JSAMPARRAY)malloc(sizeof( JSAMPROW ) * height);
	for ( i = 0; i < height; i++ ) {
		img[i] = (JSAMPROW)calloc(sizeof(JSAMPLE),3*width);
	}

	// 全イメージデータを取得	
	while(cinfo.output_scanline<cinfo.output_height)
    {
		jpeg_read_scanlines( &cinfo,
			img + cinfo.output_scanline,
			cinfo.output_height - cinfo.output_scanline
		);
	}

	// 展開の終了
	jpeg_finish_decompress( &cinfo );

	// JPEGオブジェクトの破棄
	jpeg_destroy_decompress( &cinfo );

	// ファイルを閉じる
	fclose( infile );

	return img;
}

//画像を出力する関数（出力するファイル名,画像の横幅,画像の縦幅）
void out(char FileName[],JSAMPARRAY img,int width,int height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *outfile;

	int i;

	// JPEGオブジェクトの初期化
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// ファイルを開く
	outfile = fopen(FileName, "wb" );
	jpeg_stdio_dest( &cinfo, outfile );
	
	
	if(outfile==NULL) printf("fuckn");

	// パラメータの設定
	//heightはちょうどぢゃないとエラーする
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	// デフォルト値の設定
	jpeg_set_defaults( &cinfo );

	// 圧縮の開始
	jpeg_start_compress( &cinfo, TRUE );

	// 全イメージデータを出力
	jpeg_write_scanlines( &cinfo,img,height);

	// 圧縮の終了
	jpeg_finish_compress( &cinfo );

	// JPEGオブジェクトの破棄
	jpeg_destroy_compress( &cinfo );

	// ファイルを閉じる
	fclose( outfile );	

	// イメージデータを保持するメモリ領域を開放
	//確保したメモリ以上解放しようとするとエラー出る
	for ( i = 0; i<height; i++ )
		free( img[i] );
	free( img );
}
void process_cap(void){

	int width,height;
	
	JSAMPARRAY inImg =in(FILENAME,&width,&height);
	inImg = turnLeft(inImg,height,width);
	
	out(FILENAME,inImg,width,height);

	return;

}
*/
