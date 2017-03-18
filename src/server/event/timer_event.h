#pragma once

#include <event/event.h>

#include <memory>


class timer_event_t : public smarty::event_t
{
public:
    timer_event_t( uint mode );

public: // event_t
    virtual void on_event( );

public:
    bool expired( ) const;
};

typedef shared_ptr< timer_event_t > time_event_ptr_t;
