#ifndef __LIBRARY_INTF_H_INCLUDED
#define __LIBRARY_INTF_H_INCLUDED


class library_t;

class library_intf_t
{
public:
    library_intf_t( const char *lib_name );
    ~library_intf_t( );

    bool load( );
    virtual bool is_loaded( ) const;

protected:
    virtual bool load_functions( ) = 0;

protected:
    library_t *m_lib;
    std::string m_name;
    bool m_loaded;
};

#endif // __LIBRARY_INTF_H_INCLUDED
