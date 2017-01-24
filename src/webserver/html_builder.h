#ifndef __HTML_BUILDER_H_INCLUDED
#define __HTML_BUILDER_H_INCLUDED


class config_t;
struct device_state_t;

class html_builder_t
{
public:
    html_builder_t( config_t& config );
    ~html_builder_t( );

public:
    void build_headers( size_t len, std::string& headers );
    bool convert_to_html( const device_state_t& state, std::string& html_content );

    void build_favicon_headers( size_t len, std::string& headers );
    void build_jquery_headers( size_t len, std::string& headers );

private:
    config_t& m_config;
};

#endif // __HTML_BUILDER_H_INCLUDED
