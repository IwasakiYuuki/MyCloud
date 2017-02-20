#include <stdio.h>
#include <sys/ioctl.h>
#include <SDL/SDL.h>
#include <opencv/cv.h>

#define FILENAME "map_capture.jpeg"

int main(){
	IplImage *img;
	
//img=cvLoadImage(FILENAME,CV_LOAD_IMAGE_ANYCOLOR|CV_LOAD_IMAGE_ANYDEPTH);
	img=(IplImage *)cvLoadImage(FILENAME,2|-1);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface *sdl_img=SDL_CreateRGBSurfaceFrom(
		img->imageData,
		img->width,
		img->height,24,
		img->widthStep,
		0x00ff0000,0x0000ff00,0x000000ff,0
	);
	printf("width:%d height:%d\n",img->width,img->height);
	
	SDL_Surface *screen=SDL_SetVideoMode(1920,1080,32,SDL_HWSURFACE|SDL_FULLSCREEN);
	
	printf("SetVideoMode complate...\n");
	SDL_Rect rt_dest;
	rt_dest.x=0;
	rt_dest.y=0;
	rt_dest.w=img->width;
	
	SDL_BlitSurface(sdl_img,NULL,screen,&rt_dest);
	printf("BlitSurface complate...\n");
	SDL_UpdateRect(screen,0,0,img->width,img->height);
	printf("UpdateRects complate...\n");
	SDL_Flip(screen);
	
	printf("finish...\n");
	return 0;
}

static int MyDisplay(SDL_BiltSurface *screen,SDL_Rect rt_dest,IplImage *img){
	
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
		
	if(SDL_BlitSurface(sdl_img,NULL,screen,&rt_dest)!=0){
		printf("ERROR:failed SDL_BlitSurface()...\n");
		return -1;
	}
	if(SDL_UpdateRect(screen,0,0,img->width,img->height)!=0){
		printf("ERROR:failed SDL_UpdateRect()...\n"):
		return -1;
	}
	if(SDL_Flip(screen)!=0){
		printf("ERROR:failed SDL_Flip()...\n");
		return -1;
	}
	
	retunrn 0;
}
