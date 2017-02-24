#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>

#define SENSOR_NUM 2

#define FORWORD_OUT_PIN 3
#define FORWORD_IN_PIN 4
#define BEHIND_OUT_PIN 0
#define BEHIND_IN_PIN 2

#define LED_PIN_1 5
#define LED_PIN_2 21
#define LED_PIN_3 22
#define LED_PIN_4 23


#define DENGER_DISTANCE 80

double read_dis(int out_pin,int in_pin);
void notification_dis(int *num,int *out,int *in);

int main(){
	double distance,distance_2;
	int sensor_flag[SENSOR_NUM];
	int i,flag;
	int pin_out[SENSOR_NUM]={
		FORWORD_OUT_PIN,
		BEHIND_OUT_PIN
	};
	int pin_in[SENSOR_NUM]={
		FORWORD_IN_PIN,
		BEHIND_IN_PIN
	};
	
//	notification_dis(sensor_flag,out_pin,in_pin);

	while(1){	
		flag=0;
		notification_dis(sensor_flag,pin_out,pin_in);
		for(i=0;i<SENSOR_NUM;i++){
			if(sensor_flag[i]==1){
				onLED(LED_PIN_1);
				onLED(LED_PIN_2);
				onLED(LED_PIN_3);
				onLED(LED_PIN_4);
				flag=1;
			}
			if(flag==0){
				offLED();
			}
		}
		sleep(0.05);
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
	while(digitalRead(in_pin)==0){
		pinoff=clock();
	}
	while(digitalRead(in_pin)==1){
		pinon=clock();
		if(((pinon-pinoff)/CLOCKS_PER_SEC)>0.017647)return 300;
	}
	
	return ((pinon-pinoff)/CLOCKS_PER_SEC)*(double)17000;
}

//距離の通知
void notification_dis(int *num,int *out,int *in){
//　シングルスレッドの処理	
	double distance,sum=0;
	int i=0;

	while(i<10){
		sum+=read_dis(out[0],in[0]);
		sum+=read_dis(out[1],in[1]);
		i++;
	}
	sum=sum/20;
	printf("dis  : %f\n",sum);
	for(i=0;i<SENSOR_NUM;i++){
		if(sum<DENGER_DISTANCE)
			*num=1;
		else 
			*num=0;
		num++;
	}
	return;
}

int onLED(int pin){	
	pinMode(pin,OUTPUT);
	digitalWrite(pin,1);
	return 0;	
}

int offLED(void){
	
	if(wiringPiSetup()==-1){
		printf("ERROR\n");
		return -1;
	}
	
	
	digitalWrite(LED_PIN_1,0);
	digitalWrite(LED_PIN_2,0);
	digitalWrite(LED_PIN_3,0);
	digitalWrite(LED_PIN_4,0);
		
	return 0;
}
