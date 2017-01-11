#ifndef __CLIENT_HEARTBEAT_H_INCLUDED
#define __CLIENT_HEARTBEAT_H_INCLUDED

#include <threading.h>


class client_heartbeat_t : public thread_base_t
{
public:
	client_heartbeat_t();
	~client_heartbeat_t();

public:
	virtual void do_run();
	virtual void do_stop();

protected:
	virtual void check_liveness() = 0;
	
	size_t get_timeout() const;

private:
	event_t m_heartbeat_event;
	size_t m_heartbeat_timeout; // in seconds
};

#endif // __CLIENT_HEARTBEAT_H_INCLUDED
