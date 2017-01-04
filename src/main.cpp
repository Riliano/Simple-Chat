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
	*size = i+1;
	*(messege+i) = '\0';
	*done = true;
}

char messege[1024];

int main( int argc, char** argv )
{

	bool doneWriting = false;
	int len;
	std::thread writing( input, messege, &len, &doneWriting );

	SDLNet_Init();

	TCPsocket client, server, local;
	IPaddress me, them;
	char target[16];
	std::cin>>target;
	Uint16 port = 1234;

	SDLNet_ResolveHost( &me, NULL, port );
	SDLNet_ResolveHost( &them, target, port );

	server = SDLNet_TCP_Open( &me );

	while( true )
	{
		client = SDLNet_TCP_Accept( server );
		if( client )
		{
			char msg[1024];
			SDLNet_TCP_Recv( client, msg, 1024 );
			std::cout<<msg<<std::endl;
		}

		if( doneWriting )
		{
			local = SDLNet_TCP_Open( &them );
			if( local )
			{
				SDLNet_TCP_Send( local, messege, len );
			}else
				std::cout<<"Messege failed to send"<<std::endl;
			writing.join();
			doneWriting = false;
			writing = std::thread( input, messege, &len, &doneWriting );
		}
		SDL_Delay( 200 );
	}

}
