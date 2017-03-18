#pragma once

#include <event/timer_event.h>

#include <lock_queue.h>
#include <threading/threading.h>

namespace smarty
{

class timer_t : public thread_base_t
{
public:
    timer_t( );

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

public:
    void add_event( time_event_ptr_t event );

private:
    void check_event_expiration( );

private:
    lock_queue_t< time_event_ptr_t > m_event_queue;
};

} // namespace smarty
