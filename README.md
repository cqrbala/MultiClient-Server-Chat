# Overview

This repository contains code for a server and multiple clients where communication between a client and a server happens using socket programming, and support for multiple clients is done using multithreading.

### How to run:

1. Clone the repository 
2. Start by running the server → 

```c
gcc server.c -o server
./server
```

1. Run the client code in multiple machines/terminals →

```c
gcc client.c -o client
./client
```

The below video depicts the functioning of the chat room:

**Note**: Multiple clients are simulated by using multiple command prompts as clients.

https://user-images.githubusercontent.com/102587700/206996263-f7183a98-670a-4a2c-8d9d-f8f575b288c8.mp4


# Code Implementation

## Utility Functions

The following functions’ declarations and definitions are present in *socketutil.h* and *socketutil.c* respectively. These are the functions that are common to both server.c and client.c

1. **createSocket( )**: uses the **socket( )** command to create a socket. 

```c
int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}
```

1. **createAddress( )** : takes the ip address string and port number as parameters to fill the sockaddr_in structure. If the ip string passed is empty, then the address is set to INADDR_ANY indicating that connections on all interfaces are accepted by the server.

```c
struct sockaddr_in *createAddress(char *ip, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));

    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (!strlen(ip))
    {
        address->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }

    return address;
}
```

## Server Implementation

We begin by creating a socket and binding it to the address created by the createAddress function - port 2000 is used for listening to incoming connection requests and the ip address is set to INADDR_ANY to allow connections on all network interfaces.

```c
int server_socket_fd = createSocket();
struct sockaddr_in *serveradd = createAddress("", 2000);

int bind_result = bind(server_socket_fd, serveradd, sizeof(*serveradd));
```

Now we start listening on the port using the builtin **listen( )** function which takes the socket file descriptor and the number of requests allowed to queue up, as parameters.

```c
int listen_result = listen(server_socket_fd, 10);
```

Important function to accept connections: struct connected_socket *accept_connection(int serversocketFD)

This connects to the cilent using the builtin **accept( )** method and the information of the connected client ( struct sockaddr_in ) is stored in a custom built structure called connected_socket which is the following:

```c
struct connected_socket
{
    int connected_socket_fd; // file descriptor of the connected client socket
    struct sockaddr_in address; // address of the connected client
    int error; // set to the error code if connection fails
    bool connection_success; // tells if the connection was a success or no
};
```

Now, in the main function, we call **start_accepting( )**

```c
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
```

The above is a blocking function which constantly looks for connections and if a valid connection is made, then the structure - connected_socket, is added to an array of size 10 since our program allows only upto 10 clients.

Once we get a valid connection, we create a new thread for it using the **create_datarecieving_thread( )** routine.

```c
void create_datarecieveing_thread(struct connected_socket *connected_client_socket)
{
    pthread_t id;
    pthread_create(&id, NULL, get_and_print_data, connected_client_socket->connected_socket_fd);
}
```

The pthread library is used for creating a new thread and the initialization function for it is **get_and_print_data( ).**

```c
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
```

This routine runs an infinite while(true) loop where it recieves the message from the client socket and checks for the number of characters.

If it is equal to zero, then that implies the ending of the client communication and hence we break out of the loop & close the client socket.

If it is greater than zero, we call the **broadcast( )** routine that broadcasts the message to other clients.

```c
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
```

The broadcast routine simply iterates through all the connected clients’ connected_socket structure and sends the message to every client socket other than the one which sent the message to the server in the first place.

## Client Implementation

We begin by creating a client socket and a sockaddr_in structure with ip address as ‘127.0.0.1’ and port 2000. The IP address is that of the local machine as we’re simulating this chat room in one computer and hence the server’s ip address is that of the local machine itself. The port is set to 2000 as that is what the server is listening on.

Then, the **connect( ) function** is used to connect to the server socket with error checking, in the following way:

```c
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
```

Then we listen for messages from the server using **create_listening_thread( ).**

```c
void create_listening_thread(int socketfd)
{
    pthread_t id;
    pthread_create(&id, NULL, recieve_and_print, socketfd);
}
```

We create a new thread because we have to constantly listen for messages from the server which will block the rest of the client code where we have to read command line input from the user as well.

Now the following function runs on the thread created.

```c
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
```

It recieves messages from the server socket using the builtin **recv( )** command and checks the number of characters recieved.

If it is greated than zero, it prints it for the client to see.

If it is equal to zero, then it implies the end of the server socket communication and hence we break out of the infinite loop & close the client socket.

Now, as told before, we have to read the user input on the command line as well, which is then sent to the server to be broadcast to other clients. This is done using the following routine.

```c
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
```

It begins by taking the name of the client so that it can be appended to the message. This allows other clients to see who the message is from, when it gets broadcasted by the server. 

A while(true) loop runs and the **getline( )** command is used to read the input from stdin and we check for the character count of the input.

If it is greater than zero and is “cqrout” (conqueror out :p), then the client wishes to exit. Else we append the name along with the message and send it to the server socket.
