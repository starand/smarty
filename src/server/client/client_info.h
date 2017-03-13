#pragma once

#include <string>
#include <list>
#include <time.h>


class socket_t;

struct client_info_t
{
	client_info_t( const std::string& endpoint_, socket_t *client )
		: endpoint( endpoint_ )
		, socket( client )
		, last_time( time( NULL ) )
	{
	}

	bool is_alive( size_t max_time ) const
	{
		return ( size_t )( time( NULL ) - last_time ) < max_time;
	}

	std::string endpoint;
	socket_t *socket;

	time_t last_time;
};

typedef std::list< client_info_t> client_list_t;
