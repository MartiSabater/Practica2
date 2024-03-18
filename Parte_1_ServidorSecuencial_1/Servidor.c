#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

void *AtenderCliente (void *socket)
{
	int socket_conn;
	int *s;
	s= (int *) socket;
	socket_conn = *s;
	char buff[512];
	char buff2[512];
	int ret;
	
	
	int terminar = 0;
	//Entramos en un bucle para atender todas las peticiones de este cliente.
	//hasta que se desconecte
	while (terminar == 0)
	{
		//Ahora recibimos su nombre, que dejamos en buff.
		ret = read(socket_conn, buff, sizeof(buff));
		printf("Rercibido\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		//Escribimos el nombre en la consola
		
		printf ("Se ha conectado: %s\n",buff);
		
		
		char *p = strtok( buff, "/");
		int codigo =  atoi (p);
		char nombre[20];
		//Ya tenemos el codigo de la peticion
		if (codigo != 0){
			p=strtok(NULL, "/");
			
			strcpy(nombre, p);
			//Ya tenemos el nombre
			printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		
		if(codigo == 0)//peticio de desconexion
		{
			terminar = 1;
		}
		else if (codigo ==1) //piden la longitd del nombre
		{	
			sprintf (buff2,"%d,",strlen (nombre));
		}
		
		if(codigo == 2)// quieren saber si el nombre es bonito
		{
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (buff2,"SI,");
			else
				strcpy (buff2,"NO,");
		}
		if(codigo == 3)//quiera saber se es alto
		{					
			p = strtok( NULL, "/");
			float altura =  atof (p);
			printf("%s es el nom, %f es la altura", nombre, altura);
			
			if(altura > 1.70f){
				strcpy (buff2, "1,");
				printf("Eres alto\n");
			}else{
				strcpy (buff2, "0,");
				printf("eres bajo\n");
			}
			
		}
		
		if (codigo != 0)
		{
			printf("%s\n", buff2);
			//Enviamos 
			write (socket_conn, buff2, strlen(buff2));
		}
	}
	// Se acabo el servicio para este cliente
	close(socket_conn); 
	
}






int main(int argc, char *argv[])
{
	int socket_conn, socket_listen, ret;
	struct sockaddr_in serv_adr;
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((socket_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(socket_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(socket_listen, 2) < 0)
		printf("Error en el Listen");
	
	int i;
	 int sockets[100];
	 pthread_t thread;
	i =0; 
	// Atenderemos solo 5clientes
	for(;;)
	{
		printf ("Escuchando\n");
		
		socket_conn = accept(socket_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		
		sockets[i] = socket_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		//Creatr thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente, &sockets[i] ); //Le passas el socket por referencia. eso es lo que quiere decir &
		i++;
	}
	//Para que el servidro no termine de golpe
/*	for(i = 0; i < 5; i++){*/
/*		pthread_join (thread[i], NULL);*/
/*	}*/
}
