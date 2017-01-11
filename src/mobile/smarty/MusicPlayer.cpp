#include "MusicPlayer.h"
#include "ui_musicplayer.h"

#include <common/client_lib.h>

#include <strutils.h>

#include <QListView>
#include <QStandardItem>


extern char g_szButtonGrayStyle[];

//--------------------------------------------------------------------------------------------------

MusicPlayer::MusicPlayer( QWidget *parent )
    : QDialog( parent )
    , ui( new Ui::MusicPlayer )
    , m_tracks_count( 0 )
    , m_current_track( 0 )
{
    ui->setupUi( this );
    setWindowTitle( "Music Player" );

    ui->playButton->setStyleSheet( g_szButtonGrayStyle );
    ui->nextButton->setStyleSheet( g_szButtonGrayStyle );
    ui->prevButton->setStyleSheet( g_szButtonGrayStyle );
    ui->pauseButton->setStyleSheet( g_szButtonGrayStyle );

    ui->plusButton->setStyleSheet( g_szButtonGrayStyle );
    ui->minusButton->setStyleSheet( g_szButtonGrayStyle );

    QSize size = ui->plusButton->size( );
    ui->plusButton->setMinimumWidth( size.height( ) );
    ui->minusButton->setMinimumWidth( size.height( ) );

#ifdef Q_OS_ANDROID
    setWindowState( windowState( ) | Qt::WindowMaximized );
#endif
}

//--------------------------------------------------------------------------------------------------

MusicPlayer::~MusicPlayer( )
{
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::loadPlayList( uint desktop_index, client_lib_t &clientlib )
{
    m_client_lib = &clientlib;
    m_desktop_index = desktop_index;

    send_command( EDC_MPLAYER_GETLIST );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_notification( mobile_notification_t *notification )
{
    switch ( notification->type )
    {
    case EMN_MPLAYER_PLAYLIST:
        refresh_playlist( notification->params );
        break;
    case EMN_MPLAYER_TRACKCHANGED:
        m_current_track = atoi( notification->params.c_str( ) );
        set_selection( );
        break;
    case EMN_MPLAYER_VOLCHANGED:
        int value = atoi( notification->params.c_str( ) );
        ui->horizontalSlider->setValue( value );
        break;
    }
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::refresh_playlist( const std::string &playlist )
{
    QStandardItemModel* ListModel = new QStandardItemModel( );

    size_t start_pos = playlist.find( "\n" );
    if ( start_pos == std::string::npos )
    {
        LOG_ERROR( "Incorrect playlist format" );
        return;
    }

    m_current_track = atoi( playlist.substr( 0, start_pos ).c_str( ) );
    LOG_TRACE( "Current track: %i", m_current_track );

    size_t end_pos = 0;
    while ( std::string::npos != ( end_pos = playlist.find( "\n", start_pos ) ) )
    {
        std::string name = playlist.substr( start_pos, end_pos - start_pos );
        if ( !name.empty( ) )
        {
            QStandardItem* item = new QStandardItem( name.c_str( ) );
            ListModel->appendRow( item );
            ++m_tracks_count;
        }

        start_pos = ++end_pos;
    }

    ui->playList->setModel( ListModel );
    set_selection( );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::send_command( EDESKTOPCOMMAND cmd, const std::string &params/* = ""*/ )
{
    desktop_command_request_t command;

    command.desktop_index = m_desktop_index;
    command.command = cmd;
    command.params = params;

    m_client_lib->desktop_command( command );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::set_selection( )
{
    if ( m_tracks_count > m_current_track )
    {
        QModelIndex model_index = ui->playList->model( )->index( m_current_track, 0 );
        ui->playList->setCurrentIndex( model_index );
    }
    else
    {
        m_current_track = 0;
    }
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_playList_clicked( const QModelIndex &index )
{
    m_current_track = index.row( );
    std::string track = StrUtils::IntToStr( m_current_track );

    send_command( EDC_MPLAYER_PLAYTRACK, track );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_playButton_clicked( )
{
    std::string track = StrUtils::IntToStr( m_current_track );
    send_command( EDC_MPLAYER_PLAYTRACK, track );

    set_selection( );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_pauseButton_clicked( )
{
    send_command( EDC_MPLAYER_PAUSE );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_prevButton_clicked( )
{
    if ( --m_current_track == -1 ) m_current_track = m_tracks_count - 1;
    on_playButton_clicked( );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_nextButton_clicked( )
{
    if ( ++m_current_track == m_tracks_count ) m_current_track = 0;
    on_playButton_clicked( );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_horizontalSlider_valueChanged( int value )
{
    send_command( EDC_MPLAYER_VOLCAHNGED, StrUtils::IntToStr( value ) );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_plusButton_clicked( )
{
    ui->horizontalSlider->triggerAction( QAbstractSlider::SliderSingleStepAdd );
}

//--------------------------------------------------------------------------------------------------

void MusicPlayer::on_minusButton_clicked( )
{
    ui->horizontalSlider->triggerAction( QAbstractSlider::SliderSingleStepSub );
}

//--------------------------------------------------------------------------------------------------
