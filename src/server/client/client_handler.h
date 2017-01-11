#pragma once


namespace smarty
{

//--------------------------------------------------------------------------------------------------

class client_handler_t
{
public:
    virtual ~client_handler_t( ) { }

    virtual bool make_handshake( ) = 0;
    virtual void process_client( ) = 0;
};


//--------------------------------------------------------------------------------------------------

}
