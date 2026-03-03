#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int size_of_new_connection, client_socket, read_size;
	struct sockaddr_in server, client;
	char client_message[2000], HTTP_type[16], HTTP_version[16], response_message[2000], requested_object[128], type[56], object_path[128];
	int server_port = atoi(argv[1]);

	// 1.) Socket()
	int socket_initialization;
	socket_initialization = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_initialization == -1){
		printf("Failed to create socket\n");
	}
	//printf("Socket creation success\n");

	// 2.) Bind() to address and port
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( server_port );

	if( bind(socket_initialization,(struct sockaddr *)&server, sizeof(server)) < 0){
		perror("Failed to bind server");
		return 1;
	}
	//printf("Bind success\n");

	// 3.) Listen() mode
	listen(socket_initialization, 15); //SOMAXCONN let's OS decide size of queue

	// 4.) Accept() connections and process afterward/pick up the phoam
	//printf("Awaiting incoming connections...\n");
	size_of_new_connection = sizeof(struct sockaddr_in);

	while(1){
		client_socket = accept(socket_initialization, (struct sockaddr *)&client, (socklen_t*)&size_of_new_connection);
		if(client_socket < 0){
			perror("Unable to accept new connection");
			continue;
		}
		//printf("Connection accept success\n");

		pid_t pid = fork();
		if(pid < 0){ //Parent: close socket, leave child to continue while u look for more connections
			close(client_socket);
			continue;
		}

		if(pid == 0){
			close(socket_initialization);

			// 5.) Read(), receive the message/listen to caller's message
		while((read_size = recv(client_socket, client_message, 2000, 0)) > 0){
			// 6.) Write()
			client_message[read_size] = '\0';
			//printf("%s\n\n", client_message);
			// ---- Figure out request type -------
			sscanf(client_message, "%3s" "%30s" "%10s", HTTP_type, HTTP_version, HTTP_version);
			//printf("%s\n", HTTP_type);
			//printf("%s\n", HTTP_version);
			if (strcmp(HTTP_type, "GET") != 0){
				sprintf(response_message, 
					"HTTP/1.1 405 Method Not Allowed\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"405 Method Not Allowed"
				);
				write(client_socket, response_message, strlen(response_message));
				close(client_socket);
			}
			if (strcmp(HTTP_version, "HTTP/1.1") != 0){
				sprintf(response_message, 
					"HTTP/1.1 505 HTTP Version Not Supported\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 26\r\n"
					"\r\n"
					"505 HTTP Version Not Supported"
				);
				write(client_socket, response_message, strlen(response_message));
				close(client_socket);
			}
			sscanf(client_message, "GET %s", requested_object);
			//printf("requested_object: %s\n", requested_object);
			strcpy(object_path, "./www");
			strcat(object_path, requested_object);
			//printf("Object path: %s\n", object_path);
			char *type = strrchr(requested_object, '.');
			//printf("%s\n", type);

			FILE *requested_file = fopen(object_path, "r");
			if(!requested_file){
				sprintf(response_message, 
					"HTTP/1.1 404 Not Found\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"404 Not Found"
				);
				write(client_socket, response_message, strlen(response_message));
				close(client_socket);
			}

			if(requested_file){
			char file_buffer[2000000];
			size_t raw_file = fread(file_buffer, 1, 2000000, requested_file);
			fclose(requested_file);

		 	sprintf(response_message, 
					"HTTP/1.1 200 OK\r\n"
					"Content-Length: %zu\r\n"
					"Content-Type: %s\r\n"
					"Connection: keep-alive\r\n"
					"\r\n"
					,
					raw_file,
					type
				);
			write(client_socket, response_message, strlen(response_message));
			write(client_socket, file_buffer, raw_file);
			close(client_socket);
			}
		}
		}

		if(read_size == 0){
			close(client_socket);
			printf("Client disconnected\n");
			fflush(stdout);
		} else if(read_size == -1){
			perror("recv failed");
		}
	}

	return 0;
}
