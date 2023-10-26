© 2023 Microchip Technology Inc. and its subsidiaries.  

 

Subject to your compliance with these terms, you may use this Microchip software and any derivatives of this software. You must retain the above copyright notice with any redistribution of this software and the following disclaimers. It is your responsibility to comply with third party license terms applicable to your use of third party software (including open source software) that may accompany this Microchip software. THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THIS SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <netdb.h>

#define BUF_SIZE 2048

int main (int argc, char **argv)
{
    int result;
    int socketFd;               

    struct addrinfo hints;      
    struct addrinfo *servInfo;  
    struct addrinfo *p;         

    if (argc != 3)
    {
        fprintf(stderr,
            "Usage:  %s <server hostname or address> <port number>\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;          
    hints.ai_socktype = SOCK_STREAM;   
    hints.ai_protocol = IPPROTO_TCP;  
    hints.ai_flags = AI_CANONNAME;   

    result = getaddrinfo(argv[1], argv[2], &hints, &servInfo);

    if (result != 0)
    {
        fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(result));
        exit(EXIT_FAILURE);
    }


    printf("Trying %s...\n", argv[1]);
    p = servInfo;

    while (p != NULL)
    {
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (socketFd >= 0)
        {
            result = connect(socketFd, p->ai_addr, p->ai_addrlen);

            if (result != 0)
            {
                close(socketFd);
            }
            else
            {
                break;
            }
        }

        p = p->ai_next;
    }

    if (NULL == p)
    {
        fprintf(stderr, "Unable to connect to server.\n");
        freeaddrinfo(servInfo);
        exit(EXIT_FAILURE);
    }

    printf("Connected to %s\n", p->ai_canonname);
    freeaddrinfo(servInfo); 

    /***********************************************************************
    * send messages to echo server and receive echos until user sends empty
    * message or the server disconnects.
    ***********************************************************************/

    char buffer[BUF_SIZE];

    ssize_t rcv_len;

    while (1){
	if(rcv_len = recv(socketFd, buffer, BUF_SIZE-1, 0) >0){
		printf("%s\r\n", buffer);
    	}
	else if(rcv_len == 0 && errno == EWOULDBLOCK){
		close(socketFd);
		return EXIT_SUCCESS;
    	}
    }
}
