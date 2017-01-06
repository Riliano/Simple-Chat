#include<iostream>
#include<string>
#include<thread>
#include<vector>
#include<cstring>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

TCPsocket server;

struct client_t
{
	TCPsocket socket;
	char name[20];
};

//std::vector<client_t> activeClients;
client_t activeClients[200];
int num_clients;

void input( char *messege, int *size, bool *done )
{
	std::string temp;
	std::getline( std::cin, temp );
	int i;
	for( i=0;i<temp.length();i++ )
	{
		*(messege+i) = temp[i];
	}
	*size = i+1+16;
	*(messege+i) = '\0';
	*done = true;
}

void AddClient( TCPsocket &sock )
{
	char name[20];
	name[0] = 0;
	SDLNet_TCP_Recv( sock, name, 20 );
	if( name[0] == 0 )
	{
		SDLNet_TCP_Close( sock );
		return;
	}

	for( int i=0;i<num_clients;i++ )
	{
		if( strcmp( activeClients[i].name, name ) == 0 )
		{
			SDLNet_TCP_Send( sock, "User by that name is alredy connected", 38 );
			SDLNet_TCP_Close( sock );
			return;
		}
	}

	activeClients[num_clients].socket = sock;
	int i;
	for( i=0;name[i]!='\0';i++ )
		activeClients[num_clients].name[i] = name[i];
	activeClients[num_clients].name[i+1] = '\0';
	num_clients++;
}
void RemoveClient( int who )
{
	SDLNet_TCP_Close( activeClients[who].socket );
	std::swap( activeClients[who], activeClients[num_clients-1] );
	num_clients--;
}

SDLNet_SocketSet makeSocketSet()
{
	SDLNet_SocketSet set = SDLNet_AllocSocketSet( num_clients+1 );

	SDLNet_TCP_AddSocket( set, server );
	for( int i=0;i<num_clients;i++ )
		SDLNet_TCP_AddSocket( set, activeClients[i].socket );

	return set;
}

int main( int argc, char** argv )
{
	SDLNet_Init();
//server info
	IPaddress ip;
	Uint16 port;
	std::cout<<"Enter Port: ";
	std::cin>>port;
//input stuff
	char serverMessege[216] = "SERVER MESSEGE: ";
	int serverMessegeLen;
	bool doneWriting = false;
	std::thread writing( input, serverMessege+16, &serverMessegeLen,  &doneWriting );
//start server
	SDLNet_ResolveHost( &ip, NULL, port );
	server = SDLNet_TCP_Open( &ip );

	std::cout<<"Begin hosting on port: "<<port<<std::endl;

	while( true )
	{
		//check for activity
		SDLNet_SocketSet allSocks = makeSocketSet();
		int activeSockets = SDLNet_CheckSockets( allSocks, 1 );
		//check for new clients
		if( SDLNet_SocketReady( server ) )
		{
			activeSockets--;
			TCPsocket newSocket = SDLNet_TCP_Accept( server );
			if( newSocket )
				AddClient( newSocket );
		}
		for( int i=0;i<num_clients and activeSockets > 0;i++ )
		{
			if( SDLNet_SocketReady( activeClients[i].socket ) )
			{
				activeSockets--;
				char msg[100];
				int res = SDLNet_TCP_Recv( activeClients[i].socket, msg, 100 );
				if( res <= 0 )
					RemoveClient( i );
				else
				{
					char finalMsg[120];
					int newMsgLen;
					for( newMsgLen=0;activeClients[i].name[newMsgLen]!='\0';newMsgLen++ )
					{
						finalMsg[newMsgLen] = activeClients[i].name[newMsgLen];
					}
					finalMsg[newMsgLen] = ':';
					newMsgLen++;
					finalMsg[newMsgLen] = ' ';
					newMsgLen++;
					for( int j=0;j<res;j++ )
						finalMsg[j+newMsgLen] = msg[j];
					newMsgLen+=res;

					std::cout<<finalMsg<<std::endl;
					for( int j=0;j<num_clients;j++ )
						SDLNet_TCP_Send( activeClients[j].socket, finalMsg, newMsgLen );
				}
			}
		}
		if( doneWriting )
		{
			writing.join();
			for( int i=0;i<num_clients;i++ )
				SDLNet_TCP_Send( activeClients[i].socket, serverMessege, serverMessegeLen );
			doneWriting = false;
			writing = std::thread( input, serverMessege+16, &serverMessegeLen,  &doneWriting );
		}
		SDL_Delay( 200 );
	}

}
