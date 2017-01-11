#ifndef __SERVER_H_INCLUDED
#define __SERVER_H_INCLUDED

#include <common/library_intf.h>
#include <common/server_intf.h>


class library_t;

class server_module_t : public library_intf_t, public server_intf_t
{
public:
    server_module_t( );
    ~server_module_t( );

public: // server_intf_t
    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< smarty_config_t > config );
    virtual ErrorCode stop( );

public:
    ErrorCode load( );
    ErrorCode unload( );

public: // library_intf_t
    virtual bool is_loaded( ) const;

protected:
    virtual bool load_functions( );

private:
    server_intf_t *m_server;
};

#endif // __SERVER_H_INCLUDED
