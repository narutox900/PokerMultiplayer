#include "server.hpp"

Server::Server(int port_number)
{
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port_number);

    //bind to the port
    bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    current_connection = 0;
}

Server::~Server()
{
}

void Server::mainLogic()
{
    listen(listen_socket, max_client);
    printf("Start listening...");
    //start logic
    while (true)
    {
        if (current_connection < max_client)
        {
            client_length[nConnection] = sizeof(cliaddr[nConnection]);
            if ((connfd[nConnection] = accept(listenfd, (struct sockaddr *)&cliaddr[nConnection], &clilen[nConnection])) == -1)
                perror("Error accepting connection!\n");
            else
            {
                int *client_id = (int *)malloc(sizeof(int));
                *client_id = nConnection;
                printf("Connecttion established for client %d!\n", nConnection + 1);

                pthread_create(&client_ids[nConnection], NULL, handleClient, client_id);
                nConnection++;
            }
        }
    }
}