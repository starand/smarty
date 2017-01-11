#ifndef __REQUEST_EXECUTOR_H_INCLUDED
#define __REQUEST_EXECUTOR_H_INCLUDED


class driver_intf_t;
struct device_command_t;
struct device_state_t;

class request_executor_t
{
public:
    request_executor_t( driver_intf_t& driver );
    ~request_executor_t( );

public:
    void execute( const device_command_t& command, device_state_t& state );

private:
    driver_intf_t& m_driver;
};

#endif // __REQUEST_EXECUTOR_H_INCLUDED

/*
Proposed request format:

{
"light" : {
"cmd" : 0|1|2,		// 0 - off, 1 - on, 2 - status
"param" : 1 - 7,	// room number
},

"desktop" : {
"cmd" : "command format should be stated later" ...
}
}


Response format:

{
"light" : {
"status" : 0xFF,
}
}
*/