#include "socketutil.h"

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

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
