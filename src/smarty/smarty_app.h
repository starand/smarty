#pragma once

#include <memory>


class config_t;
class driver_module_t;
class server_module_t;
class webserver_module_t;
class logger_initializer_t;

class smarty_app_t
{
public:
    smarty_app_t( );
    ~smarty_app_t( );

public:
    int run( );

private:
    void do_run( );

    bool get_config_filenames( );
    bool load_config( );
    bool load_configs( );

    bool load_driver( );
    bool load_server( );
    bool load_webserver( );
    bool load_plugins( );

    void unload_driver( );
    void unload_server( );
    void unload_webserver( );
    void unload_plugins( );

#ifdef LINUX
public:
    static void install_signal_handlers( );
#endif

private:
    std::shared_ptr< driver_module_t> m_driver;
    std::unique_ptr< server_module_t > m_server;
    std::unique_ptr< webserver_module_t > m_webserver;

    std::string m_binary_dir;

    std::shared_ptr< config_t > m_config;
    std::unique_ptr< logger_initializer_t > m_logger;

public:
    static volatile bool m_stopping;
};
