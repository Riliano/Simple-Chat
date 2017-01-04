#include<iostream>
#include<string>
#include<thread>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

void input( char *messege, int *size, bool *done )
{
	std::string temp;
	std::getline( std::cin, temp );
	int i;
	for( i=0;i<temp.length();i++ )
	{
		*(messege+i) = temp[i];
	}
	*size = i;
	*(messege+i) = '\0';
	*done = true;
}

char messege[1024];

int main( int argc, char** argv )
{
	bool imHosting = true;

	SDLNet_Init();

	IPaddress me, they;
	TCPsocket server, client, sock;

	Uint16 port = 2314;

	if( imHosting )
	{
		SDLNet_ResolveHost( &me, NULL, port );
		server = SDLNet_TCP_Open( &me );
	}
	else
		SDLNet_ResolveHost( &they, "192.168.0.101", port );

	
	bool doneWritingMessege = false;
	char myMessege[1024];
	int len;
	std::thread t1( input, messege, &len, &doneWritingMessege );

	std::cout<<SDLNet_GetError()<<std::endl;

	while( true )
	{
		if( imHosting )
		{
			client = SDLNet_TCP_Accept( server );
			char messege[1024];
			if( client )
			{
				SDLNet_TCP_Recv( client, messege, 1024 );
				std::cout<<messege<<std::endl;
			}else
				std::cout<<"Did not recieve"<<std::endl;
		}
		else
		{
				sock = SDLNet_TCP_Open( &they );
			if( sock )
			{
				if( doneWritingMessege )
				{
					t1.join();
					doneWritingMessege = false;
					t1 = std::thread( input, myMessege, &len, &doneWritingMessege ); 
					SDLNet_TCP_Send( sock, myMessege, len );
				}
			}else
				std::cout<<"Did not send"<<std::endl;
		}


		std::cout<<"Working"<<std::endl;
		SDL_Delay(100);
	}
	/*

	char messege[1024];

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
