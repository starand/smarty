#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "LogViewDialog.h"
#include "ConfigDialog.h"
#include "DesktopList.h"

#include <QMessageBox>
#include <QFile>
#include <QGestureEvent>
#include <QPushButton>
#include <QSizePolicy>

#include <common/StdAfx.h>
#include <common/client_lib.h>
#include <common/config_parser.h>
#include <common/client_protocol.h>
#include <common/enums.h>

#include <strutils.h>

#ifdef WINDOWS
#   include <windows.h>
#endif


char g_szButtonGrayStyle[] =
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2d2d2d, "
"stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525);";

gui_updater_t *g_gui_updater = NULL;

//--------------------------------------------------------------------------------------------------

/*static */
void gui_updater_t::light_changed_callback( uchar state )
{
    if ( g_gui_updater != NULL )
    {
        g_gui_updater->on_light_changed( state );
    }
}

//--------------------------------------------------------------------------------------------------

/*static */
void gui_updater_t::notification_callback( mobile_notification_t *notification )
{
    if ( g_gui_updater != NULL )
    {
        g_gui_updater->on_notification( notification );
    }
}

//--------------------------------------------------------------------------------------------------

void gui_updater_t::on_notification( mobile_notification_t *notification )
{
    mobile_notification_t *local_notification = new mobile_notification_t( );
    local_notification->desktop_index = notification->desktop_index;
    local_notification->params = notification->params;
    local_notification->type = notification->type;

    emit mobile_notification_signal( local_notification );
}

//--------------------------------------------------------------------------------------------------
// MainWindow implementation
//--------------------------------------------------------------------------------------------------

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow )
    , m_clientlib( NULL )
    , m_gui_updater( )
    , m_buttons( NULL )
    , m_buttons_count( 0 )
    , m_config( NULL )
    , m_viewLogDialog( NULL )
    , m_configDialog( NULL )
    , m_desktopDialog( NULL )
    , m_bInitialized( false )
    , m_context_menu( )
    , m_button_idx( 0 )
{
    ui->setupUi( this );
    setWindowTitle( "Smarty [starand]" );
    create_internal_objects( );

    setAttribute( Qt::WA_AcceptTouchEvents );
    grabGesture( Qt::PanGesture );
    grabGesture( Qt::SwipeGesture );

    g_gui_updater =& m_gui_updater;
    connect( g_gui_updater, SIGNAL( light_changed_signal( uchar ) ),
        SLOT( update_light_buttons( uchar ) ) );
    connect( g_gui_updater, SIGNAL( mobile_notification_signal( mobile_notification_t * ) ),
        SLOT( process_notification( mobile_notification_t * ) ) );

#ifdef Q_OS_ANDROID
    setWindowState( windowState( ) | Qt::WindowMaximized );
#endif
}

//--------------------------------------------------------------------------------------------------

MainWindow::~MainWindow( )
{
    g_gui_updater = NULL;

    delete ui;
    destroy_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::create_internal_objects( )
{
    ASSERT( m_clientlib == NULL );
    m_clientlib = new client_lib_t( );

    ASSERT( m_viewLogDialog == NULL );
    m_viewLogDialog = new LogViewDialog( this );
    m_viewLogDialog->setModal( true );

    ASSERT( m_configDialog == NULL );
    m_configDialog = new ConfigDialog( this );
    m_configDialog->setModal( true );

    ASSERT( m_desktopDialog == NULL );
    m_desktopDialog = new DesktopList( m_clientlib, this );
    m_desktopDialog->setModal( true );
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::create_buttons_array( )
{
    ASSERT( m_buttons == NULL ); ASSERT( m_buttons_count == 0 ); ASSERT( m_config == NULL );

    if ( !load_config( ) )
    {
        return false;
    }

    m_buttons_count = m_config->get_lights_count( );
    ASSERT( m_buttons_count > 0 && m_buttons_count <= 8 );

    m_buttons = new ButtonInfo[ m_buttons_count ];
    ASSERT( m_buttons != NULL );

    //m_buttons = [m_buttons_count];
    for ( size_t button_idx = 0; button_idx < m_buttons_count; ++button_idx )
    {
        ButtonInfo& bi = m_buttons[ button_idx ];
        bi.button = new QPushButton( );

        light_info_t light_info;
        m_config->get_light_info( button_idx, light_info );

        bi.button->setText( light_info.name.c_str( ) );
        bi.button->setFocusPolicy( Qt::NoFocus );
        bi.automatic_set = light_info.automatic_set;

        int hor_pos = button_idx % 2;
        int vert_pos = button_idx / 2;
        ui->gridLayout->addWidget( bi.button, vert_pos, hor_pos );

        QSizePolicy size_policy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        bi.button->setSizePolicy( size_policy );

        connect( bi.button, SIGNAL( clicked( ) ), SLOT( on_light_button_clicked( ) ) );
        connect( bi.button, SIGNAL( pressed( ) ), SLOT( on_light_button_pressed( ) ) );
        connect( bi.button, SIGNAL( released( ) ), SLOT( on_light_button_released( ) ) );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::load_config( )
{
    const string config_str = m_clientlib->get_config( );
    config_parser_t *config = new config_parser_t( config_str );

    if ( !config->parse( ) )
    {
        show_error( "Could not parse config : " + QString( config_str.c_str( ) ) );
        return false;
    }

    m_config = config;
    return true;
}

//--------------------------------------------------------------------------------------------------

void MainWindow::destroy_internal_objects( )
{
    destroy_buttons_array( );

    ASSERT( m_clientlib != NULL );
    FREE_POINTER( m_clientlib );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::destroy_buttons_array( )
{
    if ( m_buttons )
    {
        for ( size_t idx = 0; idx < m_buttons_count; ++idx )
        {
            ASSERT( m_buttons[ idx ].button != nullptr );
            delete m_buttons[ idx ].button;
        }
        delete[] m_buttons;
        m_buttons = NULL;
    }


    if ( m_config )
    {
        delete m_config;
        m_config = NULL;
    }
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::initialize( )
{
    setDarkStyle( );

    ASSERT( m_configDialog != NULL );
    if ( !m_configDialog->load_config( ) )
    {
        LOG_DEBUG( "Config file does not exists" );
        ERESULT result = m_configDialog->showModal( );
        ASSERT( result != ER_NONE );

        // check if config was saved
        if ( result == ER_CANCELLED || !m_configDialog->config_exists( ) )
        {
            LOG_ERROR( "Unable to continue running without config file" );
            return false;
        }
    }

    ASSERT( m_clientlib != NULL );
    if ( !m_clientlib->initialize( gui_updater_t::light_changed_callback,
        gui_updater_t::notification_callback ) )
    {
        show_error( "Could not load client library" );
        return false;
    }

    LOG_DEBUG( "Connection to %s:%u with password %s", m_configDialog->get_server_address( ),
        m_configDialog->get_server_port( ), m_configDialog->get_server_password( ) );

    string error;
    if ( !m_clientlib->connect( m_configDialog->get_server_address( ),
        m_configDialog->get_server_port( ),
        m_configDialog->get_server_password( ), error ) )
    {
        if ( QMessageBox::Yes ==
            QMessageBox::question( this, "Connection error",
            QString( error.c_str( ) ) + "\nChange config?",
            QMessageBox::Yes | QMessageBox::No ) )
        {
            m_configDialog->showModal( );
        }

        return false;
    }

    if ( !create_buttons_array( ) )
    {
        return false;
    }

    m_bInitialized = true;
    return m_bInitialized;
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::setDarkStyle( )
{
    QFile File( ":/stylesheet.qss" );
    if ( !File.open( QFile::ReadOnly ) )
    {
        LOG_ERROR( "stylesheet.qss was not loaded" );
        return false;
    }

    QString StyleSheet = QLatin1String( File.readAll( ) );
    qApp->setStyleSheet( StyleSheet );

    return true;
}

//--------------------------------------------------------------------------------------------------

void MainWindow::showNotification( const QString& message )
{
    show_error( message );
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::event( QEvent *event )
{
    if ( event->type( ) == QEvent::Gesture )
    {
        return gestureEvent( static_cast<QGestureEvent*>( event ) );
    }
    return QWidget::event( event );
}

//--------------------------------------------------------------------------------------------------

bool MainWindow::gestureEvent( QGestureEvent *event )
{
    if ( QGesture *swipe = event->gesture( Qt::SwipeGesture ) )
    {
        QMessageBox::information( this, "Gesture event", "swipe" );
        //swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    }
    else if ( QGesture *pan = event->gesture( Qt::PanGesture ) )
    {
        QMessageBox::information( this, "Gesture event", "pan" );
        //panTriggered(static_cast<QPanGesture *>(pan));
    }
    if ( QGesture *pinch = event->gesture( Qt::PinchGesture ) )
    {
        QMessageBox::information( this, "Gesture event", "pinch" );
        //pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    return true;
}

//--------------------------------------------------------------------------------------------------

void MainWindow::update_light_buttons( uchar state )
{
    set_status( "State : " + QString::number( state ) );

    // idx -- for button which are visible on screen
    // this indexes match light info records from config
    // each info record contains information about button name and button pin
    // such pin should be used to get state for current button on screen
    for ( size_t idx = 0; idx < m_buttons_count; ++idx )
    {
        size_t btn_idx = get_pin_for_button( idx );
        m_buttons[ idx ].state = ( state & ( 1 << btn_idx ) ) > 0;
    }

    repaint_buttons( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::repaint_buttons( )
{
    for ( size_t idx = 0; idx < m_buttons_count; ++idx )
    {
        if ( m_buttons[ idx ].disabled )
        {
            m_buttons[ idx ].button->setStyleSheet(
                m_buttons[ idx ].state ? "background-color: #A80000;"
                : "background-color: #400000;" );
        }
        else
        {
            m_buttons[ idx ].button->setStyleSheet(
                m_buttons[ idx ].state ? "background-color: #b46c16;" : g_szButtonGrayStyle );
        }
    }
}

//--------------------------------------------------------------------------------------------------

void MainWindow::process_notification( mobile_notification_t *notification )
{
    if ( !notification )
    {
        LOG_ERROR( "NULL notification pointer" );
        return;
    }

    m_desktopDialog->handle_notification( notification );
    delete notification;
}

//--------------------------------------------------------------------------------------------------

size_t MainWindow::get_pin_for_button( size_t btn_idx ) const
{
    // TODO : Create in-memory parsed collection

    light_info_t light_info;
    m_config->get_light_info( btn_idx, light_info );

    return light_info.pin - 1;
}

//--------------------------------------------------------------------------------------------------

void MainWindow::show_error( const QString& error )
{
    if ( error.length( ) )
    {
        LOG_ERROR( error.toStdString( ).c_str( ) );
        QMessageBox::information( this, "Error", error );
    }
}

//--------------------------------------------------------------------------------------------------

void MainWindow::set_status( const QString& status )
{
    ui->textEdit->setHtml( "<p style='color:green; font-weight:bold;'>" + status + "</p>" );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::show_context_menu( )
{
    m_context_menu.reset( new QMenu( this ) );
    m_context_menu->setStyleSheet( "QMenu::item { margin: 30px; }" );

    if (  m_buttons[ m_button_idx ].automatic_set )
    {
        switch ( m_buttons[ m_button_idx ].mode )
        {
            case LampMode::STANDARD:
            {
                auto disableAction = m_context_menu->addAction( "Automatic mode");
                connect( disableAction, SIGNAL( triggered( ) ), this, SLOT( set_automatic_mode( ) ) );
                m_buttons[ m_button_idx ].mode = LampMode::AUTOMATIC;
                break;
            }
            case LampMode::AUTOMATIC:
            {
                auto disableAction = m_context_menu->addAction( "Standard mode");
                connect( disableAction, SIGNAL( triggered( ) ), this, SLOT( set_standard_mode( ) ) );
                m_buttons[ m_button_idx ].mode = LampMode::STANDARD;
                break;
            }
            default:
                ASSERT( false && "Not supported mode" );
        }

        m_context_menu->addAction( "" )->setSeparator( true );
    }

    auto disableAction = m_context_menu->addAction(
        m_buttons[ m_button_idx ].disabled ? "Enable button" : "Disable button" );
    connect( disableAction, SIGNAL( triggered( ) ), this, SLOT( disableButton( ) ) );

    m_context_menu->addAction( "" )->setSeparator( true );

    auto delayedOff10Action = m_context_menu->addAction( "Turn off in 10s" );
    connect( delayedOff10Action, SIGNAL( triggered( ) ), this, SLOT( delayedOff10( ) ) );
    m_context_menu->addAction( "" )->setSeparator( true );
    auto delayedOff30Action = m_context_menu->addAction( "Turn off in 30s" );
    connect( delayedOff30Action, SIGNAL( triggered( ) ), this, SLOT( delayedOff30( ) ) );
    m_context_menu->addAction( "" )->setSeparator( true );
    auto delayedOff60Action = m_context_menu->addAction( "Turn off in 1 min" );
    connect( delayedOff60Action, SIGNAL( triggered( ) ), this, SLOT( delayedOff60( ) ) );
    m_context_menu->addAction( "" )->setSeparator( true );
    auto delayedOff300Action = m_context_menu->addAction( "Turn off in 5 min" );
    connect( delayedOff300Action, SIGNAL( triggered( ) ), this, SLOT( delayedOff300( ) ) );
    m_context_menu->addAction( "" )->setSeparator( true );
    auto delayedOff600Action = m_context_menu->addAction( "Turn off in 10 min" );
    connect( delayedOff600Action, SIGNAL( triggered( ) ), this, SLOT( delayedOff600( ) ) );
    m_context_menu->addAction( "" )->setSeparator( true );

    m_context_menu->exec( QCursor::pos() );
}

//--------------------------------------------------------------------------------------------------

size_t MainWindow::getMenuHeight( ) const
{
    return ui->menuBar->height( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionView_Log_triggered( )
{
    m_viewLogDialog->load_logfile( );
    m_viewLogDialog->showMaximized( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionExit_triggered( )
{
    close( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionChange_config_triggered( )
{
    m_configDialog->exec( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionDesktop_s_triggered( )
{
    // TODO: add desktops command handling
    m_desktopDialog->exec( );
}

//--------------------------------------------------------------------------------------------------

size_t MainWindow::get_button_index( QObject *button )
{
    ASSERT( button != nullptr );
    ASSERT( m_buttons != nullptr );

    for ( size_t idx = 0; idx < m_buttons_count; ++idx )
    {
        if ( m_buttons[ idx ].button == (QPushButton *)button )
        {
            return idx;
        }
    }

    ASSERT( false && "Buttons index not found" );
    return (size_t)-1;
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_light_button_pressed( )
{
    size_t button_idx = get_button_index( this->sender( ) );
    m_buttons[ button_idx ].press_time = time( nullptr );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_light_button_released( )
{
    m_button_idx = get_button_index( this->sender( ) );
    size_t button_pin = get_pin_for_button( m_button_idx );

    auto press_delay = time( nullptr ) - m_buttons[ m_button_idx ].press_time;
    if ( press_delay > 0 )
    {
        show_context_menu( );
    }
    else
    {
        uchar light_on = 1 << button_pin;
        m_clientlib->turn_light( m_buttons[ m_button_idx ].state
                                 ? EC_TURNOFF : EC_TURNON, light_on );

        LOG_TRACE( "Button [%u] pressed. Light is %s.", m_button_idx, ( light_on ? "on" : "off" ) );
    }
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionDisbale_Buttons_triggered( )
{
    m_clientlib->turn_light( EC_TURNOFFBUTTON, 0xFF );

    for ( size_t idx = 0; idx < m_buttons_count; ++idx )
    {
        m_buttons[ idx ].disabled = true;
    }

    repaint_buttons( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::on_actionEnable_Buttons_triggered( )
{
    m_clientlib->turn_light( EC_TURNONBUTTON, 0xFF );

    for ( size_t idx = 0; idx < m_buttons_count; ++idx )
    {
        m_buttons[ idx ].disabled = false;
    }

    repaint_buttons( );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::disableButton( )
{
    size_t button_pin = get_pin_for_button( m_button_idx );
    m_buttons[ m_button_idx ].disabled = !m_buttons[ m_button_idx ].disabled;

    auto cmd = m_buttons[ m_button_idx ].disabled ? EC_TURNOFFBUTTON : EC_TURNONBUTTON;
    m_clientlib->turn_light( cmd, 1 << button_pin );

    repaint_buttons( );

    LOG_TRACE( "Button [%u] %s.", m_button_idx,
        ( m_buttons[ m_button_idx ].disabled ? "disabled" : "enabled" ) );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff10( )
{
    delayedOff( 10 );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff30( )
{
    delayedOff( 30 );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff60( )
{
    delayedOff( 60 );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff300( )
{
    delayedOff( 300 );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff600( )
{
    delayedOff( 600 );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::delayedOff( uint timeout )
{
    ASSERT( timeout != 0 );

    server_command_request_t cmd;
    cmd.command = SERVERCOMMAND::DELAYED;
    size_t button_pin = get_pin_for_button( m_button_idx );
    StrUtils::FormatString( cmd.params, "%u:%u", 1 << button_pin, timeout );

    m_clientlib->server_command( cmd );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::set_automatic_mode( )
{
    set_mode( LampMode::AUTOMATIC );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::set_standard_mode( )
{
    set_mode( LampMode::STANDARD );
}

//--------------------------------------------------------------------------------------------------

void MainWindow::set_mode( LampMode mode )
{
    server_command_request_t cmd;
    cmd.command = SERVERCOMMAND::SET_LIGHT_MODE;
    size_t button_pin = get_pin_for_button( m_button_idx );
    StrUtils::FormatString( cmd.params, "%u:%u", button_pin, static_cast< uint >( mode ) );

    m_clientlib->server_command( cmd );
}

//--------------------------------------------------------------------------------------------------
