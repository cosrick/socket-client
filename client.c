#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

bool Waiting();
void *connection_handler(void *socket_desc);
bool Talking();
void error(char *msg){
	perror(msg);
  	exit(1);
}

int main(int argc, char* argv[]){
	int sd_to_server;
	struct sockaddr_in server;	
	sd_to_server = socket(AF_INET, SOCK_STREAM, 0);	
	if (sd_to_server < 0) 
        error("ERROR opening socket");
    //else if (sd_to_talk < 0) 
        //error("ERROR opening socket");
        
    server.sin_addr.s_addr = inet_addr("140.112.106.9");
    server.sin_family = AF_INET;
    server.sin_port = htons( 18753 );
    int remaddrlen = sizeof(server);
    
    
	int retcode = connect(sd_to_server, (struct sockaddr*) &server , remaddrlen);
	if (retcode < 0 ){
		error("failed");
	}
	int Option;
	int listen_port;
	bool check1 = true;
	bool check2 = true;
	char message[3000];
	memset(message,0,3000);
	char reply[3000];
	memset(reply,0,3000);
	
	
	
	while(true)
    {
        printf("Enter Option:");
        printf("1.Register,");
        printf("2.SIGNIN,");
        printf("3.List,");
        printf("4.Exit,");
        printf("5.Listen,");
        printf("6.Talk: ");
        
        scanf("%d" , &Option);
        
        if(Option == 1){
			printf("Please Input--REGISTER#<IDNAME>: ");
        	scanf("%s", message);
        	if( send(sd_to_server , message , strlen(message) , 0) < 0){
            	puts("Send failed");
            	return 1;
       		}
        	while(true){
        		if( recv(sd_to_server , reply , 2000 , 0) < 0){
            		puts("recv failed");
            		break;
        		}
				puts("Server reply :");
       			puts(reply);
       			if (strcmp(reply,"210 fail \n")  || strcmp(reply, "100 ok \n"))
       				break;
       			memset(reply,0,3000);
        	}
		}else if(Option == 2){
			printf("Please Input--<IDNAME>#<Port>: ");
        	scanf("%s", message);
        	if( send(sd_to_server , message , strlen(message) , 0) < 0){
            	puts("Send failed");
            	return 1;
       		}
        	if( recv(sd_to_server , reply , 2000 , 0) < 0){
            	puts("recv failed");
            	break;
        	}
			puts("Server reply :");
       		puts(reply);	
		}else if(Option == 3){
			message[0] = 'L';
        	message[1] = 'i';
        	message[2] = 's';
        	message[3] = 't';
        		
        	if( send(sd_to_server , message , strlen(message) , 0) < 0){
            	puts("Send failed");
            	return 1;
       		}
        	if( recv(sd_to_server , reply , 2000 , 0) < 0){
            	puts("recv failed");
            	break;
        	}
			puts("Server reply :");
       		puts(reply);
		}else if(Option == 4){
			message[0] = 'E';
        	message[1] = 'x';
        	message[2] = 'i';
        	message[3] = 't';
        	if( send(sd_to_server , message , strlen(message) , 0) < 0){
            	puts("Send failed");
            	return 1;
       		}
        	if( recv(sd_to_server , reply , 2000 , 0) < 0){
            	puts("recv failed");
            	break;
        	}
			printf("Server reply :");
       		puts(reply);
       		break;
		}else if(Option == 5){
			while(check1){
        		check1 = Waiting();
        	}
        	check1 = true;
		}else if(Option == 6){
			while(check2){
        		check2 = Talking();
        	}
        	check2 = true;
			break;	
		}else{
			printf("Wrong Input!!");
		}
		memset(message,0,3000);
		memset(reply,0,3000);
	}
    return 0;

}
bool Waiting(){
	int sd_to_client;
	int port;
    struct sockaddr_in client;
    sd_to_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_to_client < 0) 
     	error("ERROR opening socket");
    printf("Listen for Someone~\n");
    printf("Input a Port: ");
    scanf("%d", &port);
    client.sin_addr.s_addr = INADDR_ANY;
   	client.sin_family = AF_INET;
   	client.sin_port = htons(port+10);
   				
   	if( bind(sd_to_client,(struct sockaddr *)&client , sizeof(client)) < 0){
		perror("bind failed. Error");
        return 0;
    }
    printf("bind done");
    listen (sd_to_client, 3);
    printf("Waiting for incoming connections...\n");
	int c;
	int client_sock;
	c = sizeof(struct sockaddr_in);
	pthread_t tid1;
	while( true ){
		if ((client_sock = accept(sd_to_client, (struct sockaddr *)&client, (socklen_t*)&c))<0)
			break;
		puts("Connection accepted");
    	if( pthread_create( &tid1 ,NULL ,connection_handler ,(void*) &client_sock) < 0){
        	printf("could not create thread");
			return 0;
		}
		printf("Handler assigned");
	}
	close(sd_to_client);
	return false;
	if (client_sock < 0){
		printf("accept failed");
		return 0;
	}
	
}
void *connection_handler(void *socket_desc){
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *re_message , client_message[2000];
    memset(client_message,0,sizeof(client_message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ){
		printf("Other Say: ");
		if (client_message[0] == 'B' && client_message[1] == 'y' && client_message[2] == 'e'){
			re_message = "Bye";
			if( send(sock , re_message , strlen(re_message) , 0) < 0){
           		puts("Send failed");
        		return 0;
       		}
       		break;
		}
		puts(client_message);
		//Send the message back to client
		printf("Input Message: ");
		scanf("%s", re_message);
		memset(client_message,0,sizeof(client_message));
        if( send(sock , re_message , strlen(re_message) , 0) < 0){
            puts("Send failed");
        	return 0;
       	}
    }
    if(read_size == 0){
        puts("Client disconnected");
        fflush(stdout);
    }else if(read_size == -1){
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
}

bool Talking(){
	char IP[200];
	int port;
	char content[2000];
	char re_content[2000];
	memset(content,0,2000);
	memset(re_content,0,2000);
	int sd_to_talk;
    struct sockaddr_in talk;
    sd_to_talk = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_to_talk < 0) 
     	error("ERROR opening socket");
    printf("Connent to Someone~");
    printf("Please Input IP: ");
    scanf("%s", IP);
    puts("Please Input Port: ");
    scanf("%d", &port);
    port += 10;
    
    talk.sin_addr.s_addr = inet_addr(IP);
   	talk.sin_family = AF_INET;
   	talk.sin_port = htons(port);
   				
   	if( connect(sd_to_talk,(struct sockaddr *)&talk , sizeof(talk)) < 0){
		error("Connect Failed");
        return 0;
    }
	while(true){
		printf("Write Something\n");
		scanf("%s", content);
		memset(re_content,0,2000);
		if( send(sd_to_talk , content , strlen(content) , 0) < 0){
            puts("Send failed");
            return 0;
       	}
        if( recv(sd_to_talk , re_content , 2000 , 0) < 0){
            puts("recv failed");
            break;
        }
        puts("Other Say: ");
		puts(re_content);	
	}
	close(sd_to_talk);
	return false;
	
}







