#include <stdio.h>
#include <sys/ioctl.h>
#include <SDL/SDL.h>
#include <opencv/cv.h>
#include <time.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>

#define SENSOR_NUM 4

#define FORWORD_OUT_PIN 0
#define FORWORD_IN_PIN 2
/*
#define RIGHT_OUT_PIN
#define RIGHT_IN_PIN
#define LEFT_OUT_PIN
#define LEFT_IN_PIN
#define BEHIND_OUT_PIN
#define BEHIND_IN_PIN
*/
//#define LED_PIN (int)

//#define DENGER_DISTANCE

int out_pin[SENSOR_NUM];
int in_pin[SENSOR_NUM];

void Init_pins(void){
	
	int i=0;

	for(i=0;i<SENSOR_NUM;i++){
		out_pin[i]=0;
		in_pin[i]=0;
	}
	return;
}

int MyDisplay(SDL_Surface *screen,SDL_Rect rt_dest,IplImage *img){
	
	img=(IplImage *)cvLoadImage(FILENAME,2|-1);
	
	if(img==NULL){
		printf("ERROR:failed cvLoadImage()...\n");
		return -1;
	}
	
	SDL_Surface *sdl_img=SDL_CreateRGBSurfaceFrom(
		img->imageData,
		img->width,
		img->height,24,
		img->widthStep,
		0x00ff0000,0x0000ff00,0x000000ff,0
	);
		
	SDL_BlitSurface(sdl_img,NULL,screen,&rt_dest)
	SDL_UpdateRect(screen,0,0,img->width,img->height)
	SDL_Flip(screen)
	
	retunrn 0;
}

//距離の通知
void notification_dis(int *num,int *out,int *in){
//　シングルスレッドの処理	
	double distance;
	int i=0;

	for(i=0;i<SENSOR_NUM;i++){
		distance=read_dis(out[i],in[i]);
		if(distance<DENGER_DISTANCE)
			*num=1;
		else 
			*num=0;
		num++;
	}
	return;
}

//距離測定関数
double read_dis(int out_pin,int in_pin){
	
	double pinoff,pinon;
	//GPIOの初期化
	if(wiringPiSetup()==-1){
		printf("error : can not set up GPIO...\n");
		return -1;
	}
	
	pinMode(out_pin,OUTPUT);
	pinMode(in_pin,INPUT);
	//信号発信
	digitalWrite(out_pin,0);
	delay(0.00001);
	digitalWrite(out_pin,1);

	//信号受信、時間測定
	pinoff=clock();
	while(digitalRead(in_pin)==0)pinoff=clock();
	while(digitalRead(in_pin)==1)pinon=clock();
	
	return ((pinon-pinoff)/CLOCKS_PER_SEC)*(double)17000
}

//LED点灯
int  onLED(int pin){
	
	if(wiirngPiSetup()==-1){
		printf("error : can not set up GPIO...\n");
		return -1;
	}
	
	pinMode(pin,OUTPUT);
	digitalWrite(pin,1);
	delay(1);
	digitalWrite(pin,0);
	
	return 0;
}

