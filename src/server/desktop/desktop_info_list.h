#ifndef __DESKTOP_INFO_LIST_H_INCLUDED
#define __DESKTOP_INFO_LIST_H_INCLUDED

#include <map>
#include <ctime>


class socket_t;

//--------------------------------------------------------------------------------------------------

struct desktop_info_t
{
    desktop_info_t( socket_t *socket_, const std::string& name_ )
        : socket( socket_ ),
        name( name_ ),
        last_time( time( NULL ) )
    {
    }

    bool is_alive( size_t max_time ) const
    {
        return (size_t)( time( NULL ) - last_time ) < max_time;
    }

    socket_t *socket;
    std::string name;

    time_t last_time;
};


typedef std::map<uint, desktop_info_t *> desktop_info_list_t;

#endif // __DESKTOP_INFO_LIST_H_INCLUDED
