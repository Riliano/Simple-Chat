#include<iostream>
#include<string>
#include<thread>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

void input( char *messege, bool *done )
{
	std::string temp;
	std::getline( std::cin, temp );
	int i;
	for( i=0;i<temp.length();i++ )
	{
		*(messege+i) = temp[i];
	}
	*(messege+i) = '\0';
	*done = true;
}

char messege[1024];

int main()
{
	SDLNet_Init();

	IPaddress me, they;
	TCPsocket server, client, sock;
	Uint16 port = 2314;
	SDLNet_ResolveHost( &me, NULL, port );
	SDLNet_ResolveHost( &they, "127.0.0.1", port );
	server = SDLNet_TCP_Open( &me );

	std::cout<<SDLNet_GetError()<<std::endl;

	while( true )
	{
		client = SDLNet_TCP_Accept( server );
		sock = SDLNet_TCP_Open( &they );
		if( client )
		{
			SDLNet_TCP_Recv(client, messege, 1024);
			std::cout<<messege<<std::endl;
		}else
			std::cout<<"Did not recieve"<<std::endl;
		if( sock )
		{
			SDLNet_TCP_Send( sock, "Hi", 3 );
		}else
			std::cout<<"Did not send"<<std::endl;


		std::cout<<"Working"<<std::endl;
		SDL_Delay(4000);
	}
	/*

	char messege[1024];

	bool done = false;
	std::thread t1( input, messege, &done );
	while( true )
	{
		if( done )
		{
			std::cout<<messege<<std::endl;
			t1.join();
			done = false;
			t1 = std::thread( input, messege, &done );
		}
		else
			std::cout<<"No messege yet"<<std::endl;
	}
	*/
}
