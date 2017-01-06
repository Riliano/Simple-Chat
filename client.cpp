#include<iostream>
#include<string>
#include<thread>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

void input( char *messege, int *len, bool *done )
{
	std::string temp;
	std::getline( std::cin, temp );
	int i;
	for( i=0;i<temp.length();i++ )
	{
		*(messege+i) = temp[i];
	}
	*(messege+i) = '\0';
	*len = i+1;
	*done = true;
}

int main()
{
	SDLNet_Init();

	IPaddress ip;
	TCPsocket sock;

	char address[17];
	Uint16 port;

	std::cout<<"Enter address: ";
	std::cin>>address;
	std::cout<<"Enter port: ";
	std::cin>>port;

	SDLNet_ResolveHost( &ip, address, port );
	sock = SDLNet_TCP_Open( &ip );

	char name[20];

	if( sock )
	{
		std::cout<<"Connection successful\nEnter nickname: ";
		std::cin>>name;
		SDLNet_TCP_Send( sock, name, 20 );
	}else
	{
		std::cout<<"Connection failed"<<std::endl;
		return 0;
	}

	char msg[200];
	int len;
	bool doneWriting = false;
	std::thread t1( input, msg, &len, &doneWriting );

	SDLNet_SocketSet chk = SDLNet_AllocSocketSet( 1 );
	SDLNet_TCP_AddSocket( chk, sock );

	while( true )
	{
		int active = SDLNet_CheckSockets( chk, 0 );
		if( active > 0 )
		{
			if( SDLNet_SocketReady( sock ) )
			{
				char msg[200];
				SDLNet_TCP_Recv( sock, msg, 200 );
				std::cout<<msg<<std::endl;
			}
		}
		if( doneWriting )
		{
			t1.join();
			SDLNet_TCP_Send( sock, msg, len );
			doneWriting = false;
			t1 = std::thread( input, msg, &len, &doneWriting );

		}
	}
}
