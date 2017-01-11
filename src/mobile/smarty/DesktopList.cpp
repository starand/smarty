#include "DesktopList.h"
#include "ui_DesktopList.h"

#include "DesktopDialog.h"

#include <common/StdAfx.h>
#include <common/client_protocol.h>

#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>


extern char g_szButtonGrayStyle[];
static char g_szButtonStyleSheet[] = "height:120px;";

//--------------------------------------------------------------------------------------------------

DesktopList::DesktopList( client_lib_t *clientlib, QWidget *parent )
    : QDialog( parent )
    , ui( new Ui::DesktopDialog )
    , m_desktop_list( )
    , m_desktopDialog( NULL )
    , m_clientlib( clientlib )
{
    ui->setupUi( this );
    ui->verticalLayout->addStretch( );
    this->setLayout( ui->verticalLayout );
    setWindowTitle( "Desktops List" );

#ifdef Q_OS_ANDROID
    setWindowState( windowState( ) | Qt::WindowMaximized );
#endif

    create_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

DesktopList::~DesktopList( )
{
    destroy_internal_objects( );
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void DesktopList::create_internal_objects( )
{
    ASSERT( m_desktopDialog == NULL );
    ASSERT( m_clientlib != NULL );

    m_desktopDialog = new DesktopDialog( m_clientlib, this );
}

//--------------------------------------------------------------------------------------------------

void DesktopList::destroy_internal_objects( )
{
    ASSERT( m_desktopDialog != NULL );
    delete m_desktopDialog;
    m_desktopDialog = NULL;
}

//--------------------------------------------------------------------------------------------------

void DesktopList::handle_notification( mobile_notification_t *notification )
{
    ASSERT( notification != NULL );

    switch ( notification->type )
    {
    case EMN_DESKTOP_CONNECTED:
        add_desktop( notification->desktop_index, notification->params );
        break;
    case EMN_DESKTOP_DISCONNECTED:
        remove_desktop( notification->desktop_index );
        break;
    case EMN_RECONNECTED:
        clear_desktop_list( );
        break;

    case EMN_MPLAYER_PLAYLIST:
    case EMN_MPLAYER_TRACKCHANGED:
    case EMN_MPLAYER_VOLCHANGED:
        m_desktopDialog->on_notification( notification );
        break;
    default:
        LOG_ERROR( "Incorrect notification type %u", notification->type );
    }
}

//--------------------------------------------------------------------------------------------------

void DesktopList::add_desktop( uint index, const string& name )
{
    desktop_info_t desktop_info;

    desktop_info.index = index;
    desktop_info.name = name;
    desktop_info.button = create_button( desktop_info.name.c_str( ) );

    {
        mutex_locker_t lock( m_desktop_list_lock );
        m_desktop_list[ index ] = desktop_info;

        ui->verticalLayout->insertWidget( 0, desktop_info.button );
    }

    LOG_DEBUG( "Desktop %s (%u) was added", name.c_str( ), index );
}

//--------------------------------------------------------------------------------------------------

void DesktopList::remove_desktop( uint index )
{
    {
        mutex_locker_t lock( m_desktop_list_lock );

        desktop_list_t::iterator iter = m_desktop_list.find( index );
        if ( iter != m_desktop_list.end( ) )
        {
            do_remove_button( iter );
            m_desktop_list.erase( iter );
        }
    }

    LOG_DEBUG( "Desktop %u was removed", index );
}

//--------------------------------------------------------------------------------------------------

void DesktopList::do_remove_button( desktop_list_t::iterator iter )
{
    QPushButton *button = iter->second.button;
    ui->verticalLayout->removeWidget( button );
    delete button;
}

//--------------------------------------------------------------------------------------------------

void DesktopList::clear_desktop_list( )
{
    LOG_DEBUG( "Clearing desktop list" );
    mutex_locker_t lock( m_desktop_list_lock );

    for ( desktop_list_t::iterator iter = m_desktop_list.begin( );
        iter != m_desktop_list.end( ); ++iter )
    {
        do_remove_button( iter );
    }

    m_desktop_list.clear( );
}

//--------------------------------------------------------------------------------------------------

QPushButton *DesktopList::create_button( const char *name )
{
    QPushButton *button = new QPushButton( name );

    button->setStyleSheet( QString( g_szButtonGrayStyle ) + g_szButtonStyleSheet );
    connect( button, SIGNAL( clicked( ) ), SLOT( on_desktop_selected( ) ) );

    return button;
}

//--------------------------------------------------------------------------------------------------

desktop_list_t::iterator DesktopList::find_by_button( QPushButton *button )
{
    desktop_list_t::iterator iter = m_desktop_list.begin( );
    for ( ; iter != m_desktop_list.end( ); ++iter )
    {
        if ( iter->second.button == button )
        {
            return iter;
        }
    }

    return m_desktop_list.end( );
}

//--------------------------------------------------------------------------------------------------

void DesktopList::on_desktop_selected( )
{
    mutex_locker_t lock( m_desktop_list_lock );

    QPushButton *button = ( QPushButton * )this->sender( );
    desktop_list_t::iterator iter = find_by_button( button );
    if ( iter == m_desktop_list.end( ) )
    {
        LOG_ERROR( "Could not found desktop info by button %p", button );
        return;
    }

    m_desktopDialog->set_desktop_info( iter->second );
    m_desktopDialog->exec( );
}

//--------------------------------------------------------------------------------------------------
