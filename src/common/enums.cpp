#include <common/StdAfx.h>
#include "enums.h"


//--------------------------------------------------------------------------------------------------

static const char* g_aszMicrocontrollerCommandNames[ EC__MAX ] =
{
    "STATUS",           // EC_STATUS
    "TURN ON",          // EC_TURNON
    "TURN OFF",         // EC_TURNOFF
    "TURN ALL",         // EC_TURNALL
    "TURN EQUAL",       // EC_TURNEQUAL

    "TURN OFF BUTTON",  // EC_TURNOFFBUTTON
    "TURN ON BUTTON",   // EC_TURNONBUTTON
};

const char* get_mc_command_name( ECOMMAND cmd )
{
    static const char invalid_command[] = "invalid command";
    if ( cmd >= EC__MAX ) return invalid_command;

    return g_aszMicrocontrollerCommandNames[ cmd ];
}

//--------------------------------------------------------------------------------------------------

static const char *g_aszDesktopCommandNames[ EDC__MAX ] =
{
    "display off",		// EDC_TURN_OFF_DISPLAY
    "display on",		// EDC_TURN_ON_DISPLAY

    "sound off",		// EDC_SOUND_OFF

    "refresh desktops",	// EDC_REFRESH_DESKTOPS

    "turn off",			// EDC_TURN_OFF
    "lock desktop",		// EDC_LOCK_DESKTOP

    "get play list",	// EDC_MPLAYER_GETLIST
    "play track",		// EDC_MPLAYER_PLAYTRACK
    "player pause",		// EDC_MPLAYER_PAUSE
    "change volume",	// EDC_MPLAYER_VOLCAHNGED
};

const char *get_desktop_command_name( EDESKTOPCOMMAND cmd )
{
    static const char invalid_command[] = "invalid command";
    if ( cmd >= EDC__MAX ) return invalid_command;

    return g_aszDesktopCommandNames[ cmd ];
}

//--------------------------------------------------------------------------------------------------

static const char *g_aszMobileNotificationNames[ EMN__MAX ] =
{
    "invalid type",			// EMN__MIN

    "desktop connected",	// EMN_DESKTOP_CONNECTED,
    "desktop disconnected",	// EMN_DESKTOP_DISCONNECTED

    "volume changed",		// EMN_VOLUME_CHANGED
    "desktop locked",		// EMN_DESKTOP_LOCKED
    "display off",			// EMN_DISPLAY_TURNED_ONOFF
    "shutdown",				// EMN_DESKTOP_SHUTDOWN

    "reconnected",			// EMN_RECONNECTED

    "play list",			// EMN_MPLAYER_PLAYLIST
    "track changed",		// EMN_MPLAYER_TRACKCHANGED
    "volume changed",		// EMN_MPLAYER_VOLCHANGED
};

const char *get_mobile_notification_name( EMOBILENOTIFICATION type )
{
    if ( type >= EMN__MAX ) type = EMN__MIN;
    return g_aszMobileNotificationNames[ type ];
}

//--------------------------------------------------------------------------------------------------

static const char *g_aszServerCommandNames[ (size_t)SERVERCOMMAND::_MAX_ ] =
{
    "DELAYED", // DELAYED
    "LIGHT", // LIGHT

    "UNKNOWN", // _UNKNOWN_
};

const char* get_server_command_name( SERVERCOMMAND type )
{
    if ( type >= SERVERCOMMAND::_MAX_ ) type = SERVERCOMMAND::_UNKNOWN_;
    return g_aszServerCommandNames[ (size_t)type ];
}

//--------------------------------------------------------------------------------------------------

static const char *g_aszDeviceEventTypeNames[ (size_t)DeviceEventType::_MAX_ ] =
{
    "sensor",   // SENSOR
    "button",   // BUTTON
    "light",    // LIGHT
    "double click", // DOUBLE_CLICK
};

DeviceEventType device_event_type_by_name( const std::string& name )
{
    for ( uint idx = (uint)DeviceEventType::_MIN_; idx < (uint)DeviceEventType::_MAX_; ++idx )
    {
        if ( g_aszDeviceEventTypeNames[ idx ] == name )
        {
            return (DeviceEventType)idx;
        }
    }

    return DeviceEventType::_UNKNOWN_;
}

//--------------------------------------------------------------------------------------------------

static const char *g_aszTargetTypeNames[ (size_t)TargetType::_MAX_ ] =
{
    "light",    // LIGHT
    "button",   // BUTTON
    "sensor",   // SENSOR
};

TargetType target_type_by_name( const std::string& name )
{
    for ( uint idx = (uint)TargetType::_MIN_; idx < (uint)TargetType::_MAX_; ++idx )
    {
        if ( g_aszTargetTypeNames[ idx ] == name )
        {
            return (TargetType)idx;
        }
    }

    return TargetType::_UNKNOWN_;
}


//--------------------------------------------------------------------------------------------------
