#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <SDL/SDL.h>
#include <opencv/cv.h>


#define PORT (in_port_t)50000
#define FILENAME "map_capture.jpeg"

int main(){
	int sock_my;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	char buf[1024];
	int i;
	int sock_cl;
	int size;
	FILE *fp_w;
	socklen_t len;

	sock_my=socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=INADDR_ANY;

	bind(sock_my,(struct sockaddr *)&addr,sizeof(addr));

	listen(sock_my,5);

	SDL_Event e;
	SDL_Surface *screen;

	while(1){
		len=sizeof(client);
		sock_cl=accept(sock_my,(struct sockaddr *)&client,&len);

		SDL_Init(SDL_INIT_EVERYTHING);
		screen=SDL_SetVideoMode(1920,1080,32,SDL_HWSURFACE|SDL_FULLSCREEN);

		if((fp_w=fopen(FILENAME,"wb"))<0){
			printf("ERROR:can't open file...");
			return 0;
		}

		while(1){
			size=read(sock_cl,buf,sizeof(buf));
			if(size<=0)break;
			fwrite(buf,sizeof(char),size,fp_w);
		}
		MyDisplay(screen);
                if(SDL_PollEvent(&e)==1){
                        if(e.type==SDL_KEYDOWN){
                                return -1;
                        }
                }
		close(sock_cl);
		fclose(fp_w);
	}
	close(sock_my);
	return 0;
}

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

