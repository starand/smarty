#pragma once

#include <ctime>


class light_object_t
{
public:
    light_object_t( );

public:
    time_t get_turnoff_timeout( );
    void set_turnoff_time( time_t time );
    void clear_turnoff_time( );

private:
    time_t m_turnoff_time;
};
