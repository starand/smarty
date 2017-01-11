#include <common/StdAfx.h>
#include "packet_receiver.h"

#include "packet_sender.h"

#include <common/client_protocol.h>

#include <display.h>
#include <strutils.h>
#include <System.h>
#include <volume_mixer.h>
#include <config.h>
#include <fileutils.h>
#include <type_cast.h>


//--------------------------------------------------------------------------------------------------

packet_receiver_t::packet_receiver_t( packet_sender_t& packet_sender, config_t& config )
    : m_endpoint( )
    , m_packet_sender( packet_sender )
    , m_config( config )
    , m_playlist( )
    , m_music_player( NULL )
    , m_volume_tracker( NULL )
    , m_desktop_index( 0 )
{
    create_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

packet_receiver_t::~packet_receiver_t( )
{
    destroy_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::create_internal_objects( )
{
    ASSERT( m_music_player == NULL );
    m_music_player = new music_player_t( );

    ASSERT( m_volume_tracker == NULL );
    m_volume_tracker = new volume_tracker_t;
    m_volume_tracker->start( );
    m_volume_tracker->add_tracker( this );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::load_playlist( )
{
    auto mp_folder_node = m_config[ "music" ][ "folder" ];
    if ( mp_folder_node.isNull( ) )
    {
        LOG_TRACE( "Music folder not ste" );
        return;
    }

    std::string folder = mp_folder_node.asString( );
    if ( folder.empty( ) )
    {
        LOG_TRACE( "Empty music folder name" );
        return;
    }

    if ( FileUtils::GetFileListRecursive( folder.c_str( ), m_playlist, true, "*.mp3" ) )
    {
        ASSERT( m_music_player != NULL );
        m_music_player->SetTrackList( m_playlist );
        m_music_player->add_tracker( this );
    }
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::destroy_internal_objects( )
{
    m_music_player->remove_tracker( this );
    FREE_POINTER_ASSERT( m_music_player );

    m_volume_tracker->remove_tracker( this );
    m_volume_tracker->stop( );
    m_volume_tracker->wait( );
    FREE_POINTER_ASSERT( m_volume_tracker );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void packet_receiver_t::on_track_changed( size_t track )
{
    send_noticiation( EMN_MPLAYER_TRACKCHANGED, StrUtils::IntToStr( track ) );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void packet_receiver_t::on_volume_changed( size_t level )
{
    send_noticiation( EMN_MPLAYER_VOLCHANGED, StrUtils::IntToStr( level ) );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_commands( )
{
    load_playlist( );

    socket_t *socket = m_packet_sender.get_socket( );
    ASSERT( socket != NULL && socket->is_connected( ) == true );
    m_endpoint = socket->get_remote_endpoint( );

    while ( true )
    {
        RECV_PACKET_SWITCH( socket );
        RECV_PACKET_CASE( desktop_command_request_t, request )
        {
            LOG_DEBUG( "Command request from %s : %u", m_endpoint.c_str( ), (uint)request.command );
            if ( !process_command( request ) )
            {
                return;
            }
        }
        RECV_PACKET_CASE( desktop_heartbeat_request_t, request )
        {
            LOG_TRACE( "Heart beat request retrieved" );

            desktop_heartbeat_response_t *response = new desktop_heartbeat_response_t( );
            response->desktop_index = m_desktop_index;

            m_packet_sender.send_packet( response );
        }
        RECV_PACKET_ERROR( message )
        {
            LOG_ERROR( "Error : %s from %s", message, m_endpoint.c_str( ) );
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::set_desktop_index( uint index )
{
    m_desktop_index = index;
}

//--------------------------------------------------------------------------------------------------

bool packet_receiver_t::process_command( const desktop_command_request_t& request )
{
    switch ( request.command )
    {
    case EDC_TURN_ON_DISPLAY:
        process_turn_display( true );
        break;
    case EDC_TURN_OFF_DISPLAY:
        process_turn_display( false );
        break;
    case EDC_TURN_OFF:
        handle_shutdown( request.params );
        break;
    case EDC_LOCK_DESKTOP:
        process_lock_desktop( );
        break;
    case EDC_SOUND_OFF:
        process_sound_off( );
        break;
    case EDC_MPLAYER_GETLIST:
        process_mplayer_getlist( );
        break;
    case EDC_MPLAYER_PLAYTRACK:
        process_mplayer_playtrack( request.params );
        break;
    case EDC_MPLAYER_PAUSE:
        process_mplayer_pause( );
        break;
    case EDC_MPLAYER_VOLCAHNGED:
        process_volume_changed( request.params );
        break;

    default:
        ASSERT_NOT_IMPLEMENTED( );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_turn_display( bool turn_on ) const
{
    LOG_DEBUG( "Display turned %s request", ( display_t::is_off( ) ? "off" : "on" ) );
    turn_on ? display_t::turn_on( ) : display_t::turn_off( );

    send_noticiation( EMN_DISPLAY_TURNED_ONOFF, turn_on ? "1" : "0" );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_lock_desktop( ) const
{
    LOG_DEBUG( "Lock workstation request" );
    System::LockWorkStation( );

    send_noticiation( EMN_DESKTOP_LOCKED );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_sound_off( ) const
{
    LOG_TRACE( "Sound off request" );
    m_volume_tracker->set_level( 0 );

    send_noticiation( EMN_VOLUME_CHANGED, "0.0" );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_mplayer_getlist( )
{
    ASSERT( m_music_player != NULL );
    LOG_TRACE( "MusicPlayer GetList request" );

    int current_track = m_music_player->GetCurrentTrackNumber( );
    std::string playlist = string( StrUtils::IntToStr( current_track ) ) + "\n";

    size_t count = m_playlist.size( );
    for ( size_t idx = 0; idx < count; ++idx )
    {
        add_file_to_playlist( m_playlist[ idx ], playlist );
    }

    send_noticiation( EMN_MPLAYER_PLAYLIST, playlist );
    on_volume_changed( m_volume_tracker->get_level( ) );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::add_file_to_playlist( const std::string& file, std::string& playlist ) const
{
    size_t pos = file.rfind( "\\" );
    std::string short_name = ( pos != std::string::npos ) ? file.substr( ++pos ) : file;

    playlist += short_name + "\n";
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_mplayer_playtrack( const std::string& track )
{
    LOG_TRACE( "MusicPlayer PlayTrack request: %s", track.c_str( ) );

    int track_index = atoi( track.c_str( ) );
    play_track( track_index );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::play_track( int track )
{
    ASSERT( m_music_player != NULL );

    m_music_player->SetCurrentTrackNumber( (size_t)track );
    m_music_player->ExecuteCommand( EMPC_CUSTOMTRACK );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_mplayer_pause( )
{
    ASSERT( m_music_player != NULL );
    m_music_player->PlayPause( );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::process_volume_changed( const std::string& level ) const
{
    size_t volume = (size_t)atoi( level.c_str( ) );
    if ( !IN_RANGE( volume, 0, 1000 ) )
    {
        volume = 500;
    }

    m_volume_tracker->set_level( volume );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::handle_shutdown( const string& params ) const
{
    LOG_TRACE( "TURN OFF command" );
    // TODO: implement this method

    switch ( params[ 0 ] )
    {
    case 's': // sleep
        System::Sleep( );
        break;
    case 'h': // hibernate
        System::Hibernate( );
        break;
    case 'p': // power off
        System::PowerOff( );
        break;
    }

    send_noticiation( EMN_DESKTOP_SHUTDOWN );
}

//--------------------------------------------------------------------------------------------------

void packet_receiver_t::send_noticiation( EMOBILENOTIFICATION type,
                                          const std::string& params/* = ""*/ ) const
{
    mobile_notification_t *notification = new mobile_notification_t( );

    socket_t *socket = m_packet_sender.get_socket( );
    if ( socket == NULL )
    {
        return;
    }

    notification->desktop_index = m_desktop_index;
    notification->type = type;
    notification->params = params;

    LOG_DEBUG( "Desktop index %u", socket->get_laddr( ) );
    m_packet_sender.send_packet( notification );
}

//--------------------------------------------------------------------------------------------------
