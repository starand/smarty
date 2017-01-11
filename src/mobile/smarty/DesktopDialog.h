#ifndef DESKTOPCONTROL_H
#define DESKTOPCONTROL_H

#include "DesktopList.h"
#include "MusicPlayer.h"

#include <common/enums.h>
#include <QDialog>


namespace Ui
{
    class DesktopControl;
}

class client_lib_t;
struct mobile_notification_t;

class DesktopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DesktopDialog( client_lib_t *clientlib, QWidget *parent = 0 );
    ~DesktopDialog( );

public:
    void set_desktop_info( const desktop_info_t & desktop_info );

public:
    void on_notification( mobile_notification_t *notification );

private:
    void execute_command( EDESKTOPCOMMAND command, const std::string &params = "" ) const;

    private slots:
    void on_closeButton_clicked( );
    void on_screenOffButton_clicked( );
    void on_shutdownButton_clicked( );
    void on_lockBtn_clicked( );
    void on_soundOffBtn_clicked( );
    void on_musicButton_clicked( );

private:
    Ui::DesktopControl *ui;

    desktop_info_t m_desktopInfo;
    client_lib_t *m_clientlib;

    MusicPlayer m_musicPlayer;
};

#endif // DESKTOPCONTROL_H
