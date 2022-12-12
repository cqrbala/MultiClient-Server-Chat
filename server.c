#include "socketutil.h"
#include "socketutil.c"
#include <stdio.h>

struct connected_socket
{
    int connected_socket_fd;
    struct sockaddr_in address;
    int error;
    bool connection_success;
};

struct connected_socket connected_clients[10];
int connected_client_count = 0;

struct connected_socket *accept_connection(int serversocketfd);
void start_accepting(int serversocketfd);
void create_datarecieveing_thread(struct connected_socket *connected_client_socket);
void get_and_print_data(int client_socket_fd);
void broadcast(char *buffer, int client_socket_fd);

int main()
{
    int server_socket_fd = createSocket();
    struct sockaddr_in *serveradd = createAddress("", 2000);

    int bind_result = bind(server_socket_fd, serveradd, sizeof(*serveradd));

    if (bind_result == 0)
    {
        printf("Server socket bound successfully\n");
    }
    else
    {
        printf("Binding failed\n");
        return 0;
    }

    int listen_result = listen(server_socket_fd, 10);

    start_accepting(server_socket_fd);

    shutdown(server_socket_fd, SHUT_RDWR);
}

struct connected_socket *accept_connection(int serversocketFD)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serversocketFD, &clientAddress, &clientAddressSize);

    struct connected_socket *acceptedSocket = malloc(sizeof(struct connected_socket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->connected_socket_fd = clientSocketFD;
    acceptedSocket->connection_success = clientSocketFD != -1;

    if (!acceptedSocket->connection_success)
        acceptedSocket->error = clientSocketFD;

    return acceptedSocket;
}

void start_accepting(int serversocketfd)
{
    while (true)
    {
        struct connected_socket *client_socket = accept_connection(serversocketfd);
        if ((*client_socket).connection_success)
        {
            if (connected_client_count < 10)
            {
                connected_clients[connected_client_count] = *client_socket;
                connected_client_count++;
                create_datarecieveing_thread(client_socket);
            }
        }
    }
}

void create_datarecieveing_thread(struct connected_socket *connected_client_socket)
{
    pthread_t id;
    pthread_create(&id, NULL, get_and_print_data, connected_client_socket->connected_socket_fd);
}

void get_and_print_data(int client_socket_fd)
{
    char buffer[3000];

    while (true)
    {
        ssize_t input_size = recv(client_socket_fd, buffer, 1024, 0);

        if (input_size > 0)
        {
            buffer[input_size] = '\0';
            printf("%s", buffer);

            broadcast(buffer, client_socket_fd);
        }

        if (input_size == 0)
            break;
    }

    close(client_socket_fd);
}

void broadcast(char *buffer, int client_socket_fd)
{
    for (int i = 0; i < connected_client_count; i++)
    {
        if (connected_clients[i].connected_socket_fd != client_socket_fd)
        {
            send(connected_clients[i].connected_socket_fd, buffer, strlen(buffer), 0);
        }
    }
}
