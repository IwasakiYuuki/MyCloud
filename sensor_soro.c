#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>

#define SENSOR_NUM 1

#define FORWORD_OUT_PIN 0
#define FORWORD_IN_PIN 2
#define BEHIND_OUT_PIN 3
#define BEHIND_IN_PIN 4

#define LED_PIN 5

#define DENGER_DISTANCE 150

double read_dis(int out_pin,int in_pin);
void notification_dis(int *num,int *out,int *in);

int main(){
	double distance,distance_2;
	int sensor_flag[SENSOR_NUM];
	int i;
	int pin_out[SENSOR_NUM]={
		FORWORD_OUT_PIN,
	};	
	int pin_in[SENSOR_NUM]={
		FORWORD_IN_PIN,
	};
	
//	notification_dis(sensor_flag,out_pin,in_pin);

	while(1){	
		distance=read_dis(FORWORD_OUT_PIN,FORWORD_IN_PIN);	
		printf("dis : %f \n",distance);
		notification_dis(sensor_flag,pin_out,pin_in);
//		for(i=0;i<SENSOR_NUM;i++){
//			if(sensor_flag[i]==1)onLED(LED_PIN);
//		}
		sleep(1);
	}
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
	
	return ((pinon-pinoff)/CLOCKS_PER_SEC)*(double)17000;
}

//距離の通知
void notification_dis(int *num,int *out,int *in){
//　シングルスレッドの処理	
	double distance;
	int i;
	for(i=0;i<SENSOR_NUM;i++){
		distance=read_dis(out[i],in[i]);
		if(distance<DENGER_DISTANCE)
			num[i]=1;
		else 
			num[i]=0;
//		num++;
	}
	return;
}

int onLED(int pin){
	
	if(wiringPiSetup()==-1){
		printf("ERROR\n");
		return -1;
	}
	
	pinMode(pin,OUTPUT);
	digitalWrite(pin,1);
	
	return 0;	
}

