#ifndef __ENUMS_H_INCLUDED
#define __ENUMS_H_INCLUDED

//--------------------------------------------------------------------------------------------------

enum EPACKETMAGIC
{
    EPM__MIN,

    EPM_MOBILE_HANDSHAKE_REQUEST = EPM__MIN,
    EPM_MOBILE_HANDSHAKE_RESPONSE,
    EPM_MOBILE_NOTIFICATION,
    EPM_MOBILE_HEARTBEAT_REQUEST,
    EPM_MOBILE_HEARTBEAT_RESPONSE,

    EPM_STATE_REQUEST,
    EPM_COMMAND_REQUEST,

    EPM_COMMAND_DISCONNECT,

    EPM_STATE_CHANGE_NOTIFICATION,

    EPM_CONFIG_UPDATE_REQUEST,
    EPM_CONFIG_UPDATE_RESPONSE,

    EPM_DESKTOP_HANDSHAKE_REQUEST,
    EPM_DESKTOP_HANDSHAKE_RESPONSE,
    EPM_DESKTOP_COMMAND_REQUEST,
    EPM_DESKTOP_HEARTBEAT_REQUEST,
    EPM_DESKTOP_HEARTBEAT_RESPONSE,

    EPM_SERVER_COMMAND_REQUEST,

    EPM_UPDATE_MODES_REQUESTS,
    EPM_MODES_UPDTE_NOTIFICATION,

    EPM__MAX,
    EPM_ERROR = EPM__MAX,
    EPM_INVALIDMAGIC,
};

//--------------------------------------------------------------------------------------------------

enum ESTATE
{
    ES_OK,
    ES_INVALID_PASSWORD,
    ES_INCORRECT_VERSION,
    ES_ERROR,
};

//--------------------------------------------------------------------------------------------------
// micro controller commands

#include <common/mc_enum.h>

const char* get_mc_command_name( ECOMMAND cmd );

//--------------------------------------------------------------------------------------------------

enum EDESKTOPCOMMAND
{
    EDC__MIN,

    EDC_TURN_OFF_DISPLAY = EDC__MIN,
    EDC_TURN_ON_DISPLAY,

    EDC_SOUND_OFF,

    EDC_REFRESH_DESKTOPS,

    EDC_TURN_OFF,
    EDC_LOCK_DESKTOP,

    EDC_MPLAYER_GETLIST,
    EDC_MPLAYER_PLAYTRACK,
    EDC_MPLAYER_PAUSE,
    EDC_MPLAYER_VOLCAHNGED,

    EDC__MAX,
    EDC_ERROR = EDC__MAX
};

const char *get_desktop_command_name( EDESKTOPCOMMAND cmd );

//--------------------------------------------------------------------------------------------------

enum EMOBILENOTIFICATION
{
    EMN__MIN,

    EMN_DESKTOP_CONNECTED,
    EMN_DESKTOP_DISCONNECTED,

    EMN_VOLUME_CHANGED,
    EMN_DESKTOP_LOCKED,
    EMN_DISPLAY_TURNED_ONOFF,
    EMN_DESKTOP_SHUTDOWN,

    EMN_RECONNECTED,

    EMN_MPLAYER_PLAYLIST,
    EMN_MPLAYER_TRACKCHANGED,
    EMN_MPLAYER_VOLCHANGED,

    EMN__MAX,
};

const char *get_mobile_notification_name( EMOBILENOTIFICATION type );

//--------------------------------------------------------------------------------------------------

enum class SERVERCOMMAND
{
    _MIN_,

    DELAYED = _MIN_,
    LIGHT,

    _UNKNOWN_,
    _MAX_
};

const char* get_server_command_name( SERVERCOMMAND type );

//--------------------------------------------------------------------------------------------------

#define TF_HIBERNATE  "h"
#define TF_SLEEP      "s"
#define TF_POWEROFF   "p"

//--------------------------------------------------------------------------------------------------

enum class TriggerState
{
    LOW,
    HIGH,
};

//--------------------------------------------------------------------------------------------------

enum class LampState
{
    LOW,
    HIGH
};

//--------------------------------------------------------------------------------------------------

enum class DeviceEventType
{
    _MIN_,

    SENSOR = _MIN_,
    BUTTON,
    LIGHT,
    DOUBLE_CLICK,

    _MAX_,
    _UNKNOWN_ = _MAX_
};

DeviceEventType device_event_type_by_name( const std::string& name );

//--------------------------------------------------------------------------------------------------

enum class TargetType
{
    _MIN_,

    LIGHT = _MIN_,
    BUTTON,
    SENSOR,

    _MAX_,
    _UNKNOWN_ = _MAX_
};

TargetType target_type_by_name( const std::string& name );

//--------------------------------------------------------------------------------------------------

#endif // __ENUMS_H_INCLUDED
