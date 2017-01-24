#ifndef __WEBSERVER_H_INCLUDED
#define __WEBSERVER_H_INCLUDED

#include <common/library_intf.h>
#include <common/webserver_intf.h>


class library_t;


class webserver_module_t : public library_intf_t, public web_server_intf_t
{
public:
    webserver_module_t( );
    ~webserver_module_t( );

public: // web_server_intf_t
    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< config_t > config );
    virtual ErrorCode stop( );

public:
    ErrorCode load( );
    ErrorCode unload( );

public: // library_intf_t
    virtual bool is_loaded( ) const;

protected:
    virtual bool load_functions( );

private:
    web_server_intf_t *m_server;
};

#endif // __WEBSERVER_H_INCLUDED
