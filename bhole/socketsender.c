#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char msg1[] = "Hello there!\n";
char msg2[] = "Bye bye!\n";

int main(){
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        perror("socket");
        exit(1);
    }	
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr("10.1.1.2");// или 10.1.1.1
    //Главное чтобы IP отличался от того что у eth0, чтобы линукс отправлял сообщения в наш интерфейс
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr *)&addr, sizeof(addr));
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    send(sock, msg2, sizeof(msg2), 0);
	//getchar();
    close(sock);

    return 0;
}
