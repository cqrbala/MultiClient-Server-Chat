#include "socketutil.h"
#include "socketutil.c"
#include <stdio.h>

int main()
{
    int server_socket_fd = createSocket();
    struct sockaddr_in *serveradd = createAddress("", 2000);

    int bind_result = bind(server_socket_fd, serveradd, sizeof(*serveradd));

    if (bind_result == 0)
    {
        printf("Server socket bound done\n");
    }
    else
    {
        printf("binding failed\n");
    }

    int listen_result = listen(server_socket_fd, 10);

    struct sockaddr_in connectingclient;
    int connectingclient_addrsize = sizeof(connectingclient);

    int connectingclient_fd = accept(server_socket_fd, &connectingclient, &connectingclient_addrsize);

    char buffer[1024];

    while (true)
    {
        ssize_t user_input_size = recv(connectingclient_fd, buffer, 1024, 0);

        if (user_input_size > 0)
        {
            buffer[user_input_size] = '\0';
            printf("%s\n", buffer);
        }
        else if (user_input_size == 0)
        {
            break;
        }
    }

    shutdown(server_socket_fd, SHUT_RDWR);
    close(connectingclient_fd);
    // return 0;
}