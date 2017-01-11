#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QDialog>

#include <common/client_protocol.h>


namespace Ui
{
    class MusicPlayer;
}


class client_lib_t;

class MusicPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit MusicPlayer( QWidget *parent = 0 );
    ~MusicPlayer( );

public:
    void loadPlayList( uint desktop_index, client_lib_t &clientlib );

    void on_notification( mobile_notification_t *notification );
    void refresh_playlist( const std::string &playlist );

    private slots:
    void on_playList_clicked( const QModelIndex &index );
    void on_playButton_clicked( );
    void on_pauseButton_clicked( );
    void on_prevButton_clicked( );
    void on_nextButton_clicked( );

    void on_horizontalSlider_valueChanged( int value );

    void on_plusButton_clicked( );

    void on_minusButton_clicked( );

private:
    void send_command( EDESKTOPCOMMAND cmd, const std::string &params = "" );
    void set_selection( );

private:
    Ui::MusicPlayer *ui;

    client_lib_t *m_client_lib;
    uint m_desktop_index;

    int m_tracks_count;
    int m_current_track;
};

#endif // MUSICPLAYER_H
