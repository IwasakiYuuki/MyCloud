        #include <stdio.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <fcntl.h>
        #include <unistd.h>

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

        while(1){
        len=sizeof(client);
        sock_cl=accept(sock_my,(struct sockaddr *)&client,&len);

        if((fp_w=fopen(FILENAME,"wb"))<0){
        printf("ERROR:can't open file...");
        return 0;
        }

        while(1){
        size=read(sock_cl,buf,sizeof(buf));
        if(size<=0)break;
        fwrite(buf,sizeof(char),size,fp_w);
        }
        close(sock_cl);
        fclose(fp_w);
        }
        close(sock_my);
        return 0;
        }
