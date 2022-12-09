#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    char *ip = "142.250.188.46";
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(80);
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

    int connect_status = connect(socket_fd, &address, sizeof(address));

    if (connect_status == 0)
    {
        printf("Connection successful\n");
    }
    else
    {
        printf("retard\n");
    }

    char *message;
    message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";

    send(socket_fd, message, strlen(message), 0);

    char *buffer[1024];
    recv(socket_fd, buffer, 1024, 0);

    printf("%s\n", buffer);

    return 0;
}