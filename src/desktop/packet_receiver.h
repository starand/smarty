#ifndef __COMMAND_HANDLER_H_INCLUDED
#define __COMMAND_HANDLER_H_INCLUDED

#include <music_player.h>
#include <volume_tracker.h>

#include <string>
#include <vector>


class socket_t;
class packet_sender_t;
class config_t;

struct desktop_command_request_t;
struct mobile_notification_t;
enum EMOBILENOTIFICATION;

class packet_receiver_t : public musicplayer_tracker_inf_t, volume_tracker_inf_t
{
public:
    packet_receiver_t( packet_sender_t& packet_sender, config_t& config );
    ~packet_receiver_t( );

private:
    void create_internal_objects( );
    void load_playlist( );
    void destroy_internal_objects( );

public: // musicplayer_tracker_inf_t implementation
    virtual void on_track_changed( size_t track );

public: // volume_tracker_inf_t implementation
    virtual void on_volume_changed( size_t level );

public:
    void process_commands( );

    void set_desktop_index( uint index );

private:
    bool process_command( const desktop_command_request_t& request );

    void process_turn_display( bool turn_on ) const;
    void process_lock_desktop( ) const;
    void process_sound_off( ) const;

    void process_mplayer_getlist( );
    void add_file_to_playlist( const std::string& file, std::string& playlist ) const;

    void process_mplayer_playtrack( const std::string& track );
    void play_track( int track );
    void process_mplayer_pause( );

    void process_volume_changed( const std::string& level ) const;

    void handle_shutdown( const string& params ) const;

    void send_noticiation( EMOBILENOTIFICATION type, const std::string& params = "" ) const;

private:
    string m_endpoint;

    packet_sender_t& m_packet_sender;
    config_t& m_config;

    std::vector<std::string> m_playlist;
    music_player_t *m_music_player;
    volume_tracker_t *m_volume_tracker;

    uint m_desktop_index;
};

#endif // __COMMAND_HANDLER_H_INCLUDED
