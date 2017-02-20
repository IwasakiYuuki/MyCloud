#include <stdio.h>
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
//#define LED_PIN

//#define DENGER_DISTANCE

double read_dis(int out_pin,int in_pin);
void notification_dis(char *num,char *out,char *in);

int main(){
	double distance;
	char sensor_flag[4]={0,0,0,0};//0:遠距離、1:近距離
	int pin_out[4]={
		FORWORD_OUT_PIN,
		RIGHT_OUT_PIN,
		LEFT_OUT_PIN,
		BEHIND_OUT_PIN
	};	
	int pin_in[4]={
		FORWORD_IN_PIN,
		RIGHT_IN_PIN,
		LEFT_IN_PIN,
		BEHIND_IN_PIN
	};
	
//	notification_dis(sensor_flag,out_pin,in_pin);

	
	distance=read_dis(FORWORD_OUT_PIN,FORWORD_IN_PIN);
	printf("%f",distance);
//ここからはセンサーの距離測定＆判断の処理

//	for(int i=0;i<SENSOR_NUM;i++){
//		if(*sensor_flag==0)
//			printf("%d番目のセンサー異常無し...\n",i);)
//		else
//			printf("%d番目のセンサーの距離が近いです！\n",i);
//			digitalWrite(LED_PIN,1);
//			sleep(1);
//		sensor_flag++;
//	}

//ここまで
	
	return 0;
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

//距離の通知
void notification_dis(int *num,int *out,int *in){
//　シングルスレッドの処理	
	double distance;

	for(int i=0;i<SENSOR_NUM;i++){
		distance=read_dis(out[i],in[i]);
		if(distance<DENGER_DISTANCE)
			*num=1;
		else 
			*num=0;
		num++;
	}
	return;
}
