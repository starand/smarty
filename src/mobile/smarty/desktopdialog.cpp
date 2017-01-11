#include "DesktopDialog.h"
#include "ui_DesktopDialog.h"

#include <common/StdAfx.h>
#include <common/client_lib.h>
#include <common/client_protocol.h>


extern char g_szButtonGrayStyle[];

//--------------------------------------------------------------------------------------------------

DesktopDialog::DesktopDialog( client_lib_t *clientlib, QWidget *parent )
    : QDialog( parent )
    , ui( new Ui::DesktopControl )
    , m_desktopInfo( )
    , m_clientlib( clientlib )
    , m_musicPlayer( )
{
    ui->setupUi( this );
    setWindowTitle( "Control Desktop" );

    ui->closeButton->setStyleSheet( g_szButtonGrayStyle );
    ui->shutdownButton->setStyleSheet( g_szButtonGrayStyle );
    ui->screenOffButton->setStyleSheet( g_szButtonGrayStyle );
    ui->lockBtn->setStyleSheet( g_szButtonGrayStyle );
    ui->soundOffBtn->setStyleSheet( g_szButtonGrayStyle );
    ui->musicButton->setStyleSheet( g_szButtonGrayStyle );

    /*
    ui->lockBtn->setIcon(QIcon(QPixmap(":android/res/drawable/lock.png")));
    ui->lockBtn->setIconSize(QSize(200, 200));

    ui->screenOffButton->setIcon(QIcon(QPixmap(":android/res/drawable/displayoff.png")));
    ui->screenOffButton->setIconSize(QSize(200, 200));

    ui->soundOffBtn->setIcon(QIcon(QPixmap(":android/res/drawable/soundoff.png")));
    ui->soundOffBtn->setIconSize(QSize(200, 200));
    */

#ifdef Q_OS_ANDROID
    setWindowState( windowState( ) | Qt::WindowMaximized );
#endif

    setWindowTitle( "Desktop Control" );
}

//--------------------------------------------------------------------------------------------------

DesktopDialog::~DesktopDialog( )
{
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::set_desktop_info( const desktop_info_t & desktop_info )
{
    m_desktopInfo = desktop_info;
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_notification( mobile_notification_t *notification )
{
    if ( notification == NULL ) return;

    switch ( notification->type )
    {
    case EMN_MPLAYER_PLAYLIST:
    case EMN_MPLAYER_TRACKCHANGED:
    case EMN_MPLAYER_VOLCHANGED:
        m_musicPlayer.on_notification( notification );
        break;
    default:
        LOG_TRACE( "Notification retrieved: %u -> %s",
            notification->type, notification->params.c_str( ) );
    }
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::execute_command( EDESKTOPCOMMAND cmd, const std::string& params/*=""*/ ) const
{
    desktop_command_request_t command;
    command.command = cmd;
    command.desktop_index = m_desktopInfo.index;
    command.params = params;

    m_clientlib->desktop_command( command );
    LOG_DEBUG( "Desktop command triggered : %u %u", command.command, command.desktop_index );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_closeButton_clicked( )
{
    close( );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_screenOffButton_clicked( )
{
    execute_command( EDC_TURN_OFF_DISPLAY );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_shutdownButton_clicked( )
{
    execute_command( EDC_TURN_OFF, TF_HIBERNATE );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_lockBtn_clicked( )
{
    execute_command( EDC_LOCK_DESKTOP );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_soundOffBtn_clicked( )
{
    execute_command( EDC_SOUND_OFF );
}

//--------------------------------------------------------------------------------------------------

void DesktopDialog::on_musicButton_clicked( )
{
    m_musicPlayer.loadPlayList( m_desktopInfo.index, *m_clientlib );
    m_musicPlayer.exec( );
}

//--------------------------------------------------------------------------------------------------
