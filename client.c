#include "socketutil.h"
#include "socketutil.c"

void create_listening_thread(int socketfd);
void recieve_and_print(int socketfd);
void read_userinput(int socketfd);

int main()
{
    int socket_fd = createSocket();

    struct sockaddr_in *address = createAddress("127.0.0.1", 2000);

    int connect_status = connect(socket_fd, address, sizeof(*address));

    if (connect_status == 0)
    {
        printf("Connection successful\n");
    }
    else
    {
        printf("Connection failed\n");
        return 0;
    }

    create_listening_thread(socket_fd);
    read_userinput(socket_fd);

    close(socket_fd);
    return 0;
}

void create_listening_thread(int socketfd)
{
    pthread_t id;
    pthread_create(&id, NULL, recieve_and_print, socketfd);
}

void recieve_and_print(int socketfd)
{
    char buffer[1024];

    while (true)
    {
        ssize_t server_message_size = recv(socketfd, buffer, 1024, 0);

        if (server_message_size > 0)
        {
            buffer[server_message_size] = '\0';
            printf("%s", buffer);
        }

        if (server_message_size == 0)
            break;
    }

    close(socketfd);
}

void read_userinput(int socketfd)
{
    char *name = NULL;
    size_t nameSize = 0;
    printf("Enter your name:\n");
    ssize_t name_char_count = getline(&name, &nameSize, stdin);
    name[name_char_count - 1] = '\0';

    printf("Start typing messages!\n");

    char *user_input = NULL;
    size_t user_input_size = 0;

    char buffer[3000];

    while (true)
    {
        size_t input_count = getline(&user_input, &user_input_size, stdin);

        if (input_count > 0)
        {
            if (strcmp(user_input, "cqrout\n") == 0)
            {
                break;
            }
            else
            {

                sprintf(buffer, "%s:%s", name, user_input);
                send(socketfd, buffer, strlen(buffer), 0);
            }
        }
    }
}