#ifndef __CONTROLLER_WRAPPER_H_INCLUDED
#define __CONTROLLER_WRAPPER_H_INCLUDED

#include <common/driver_intf.h>
#include <common/errors.h>
#include <common/library_intf.h>


class driver_module_t : public library_intf_t, public driver_intf_t
{
public:
    driver_module_t( );
    ~driver_module_t( );

public:
    ErrorCode load( );
    ErrorCode unload( );

public: // library_intf_t
    virtual bool is_loaded( ) const;

protected:
    virtual bool load_functions( );

public: // driver_intf_t
    virtual ErrorCode execute_command( const device_command_t& command );
    virtual ErrorCode get_state( device_state_t& state ) const;

    virtual ErrorCode add_observer( device_observer_t& observer );
    virtual ErrorCode remove_observer( device_observer_t& observer );

private:
    create_driver_funct_t m_create_driver;
    free_driver_funct_t m_free_driver;

    driver_intf_t *m_driver;
};

#endif // __CONTROLLER_WRAPPER_H_INCLUDED
