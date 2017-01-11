#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <common/enums.h>
#include <memory>

struct mobile_notification_t;

///////////////////////////////////////////////////////////////
// gui_updater_t declaration

class gui_updater_t : public QObject
{
    Q_OBJECT

public:
    explicit gui_updater_t( QObject *parent = 0 ) : QObject( parent )
    {
    }

public:
    static void light_changed_callback( uchar state );
    static void notification_callback( mobile_notification_t *notification );

public:
    void on_light_changed( uchar state )
    {
        emit light_changed_signal( state );
    }
    void on_notification( mobile_notification_t *notification );

signals:
    void light_changed_signal( uchar );
    void mobile_notification_signal( mobile_notification_t *notification );
};


///////////////////////////////////////////////////////////////
// class MainWindow declaration

class client_lib_t;
class QPushButton;
class LogViewDialog;
class ConfigDialog;
class QGestureEvent;
class DesktopList;
class config_parser_t;
class QMenu;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow( );

private slots:
    void on_actionView_Log_triggered( );
    void on_actionExit_triggered( );
    void on_actionChange_config_triggered( );
    void on_actionDesktop_s_triggered( );

    void on_light_button_pressed( );
    void on_light_button_released( );

    void on_actionDisbale_Buttons_triggered( );
    void on_actionEnable_Buttons_triggered( );

    void disableButton( );
    void delayedOff10( );
    void delayedOff30( );
    void delayedOff60( );
    void delayedOff300( );
    void delayedOff600( );

    void set_automatic_mode( );
    void set_standard_mode( );

public:
    struct ButtonInfo
    {
        QPushButton *button;
        bool state;
        bool disabled;
        time_t press_time;
        bool automatic_set;
        LampMode mode;

        ButtonInfo( )
            : button( nullptr )
            , state( false )
            , disabled( false )
            , press_time( 0 )
            , automatic_set( false )
            , mode( LampMode::STANDARD )
        {
        }
    };

private:
    void delayedOff( uint timeout );
    void set_mode( LampMode mode );

public:
    bool initialize( );
    bool setDarkStyle( );

    void showNotification( const QString& message );

public:
    bool event( QEvent *event );
    bool gestureEvent( QGestureEvent *event );

public slots:
    void update_light_buttons( uchar state );
    void repaint_buttons( );
    void process_notification( mobile_notification_t *notification );

private:
    size_t get_pin_for_button( size_t btn_idx ) const;

private:
    void create_internal_objects( );
    bool create_buttons_array( );
    bool load_config( );

    void destroy_internal_objects( );
    void destroy_buttons_array( );

    void show_error( const QString& error );
    void set_status( const QString& status );

    void show_context_menu( );

public:
    size_t getMenuHeight( ) const;

private:
    size_t get_button_index( QObject *button );

private:
    Ui::MainWindow *ui;

    client_lib_t *m_clientlib;
    gui_updater_t m_gui_updater;

    ButtonInfo *m_buttons;
    size_t m_buttons_count;

    config_parser_t *m_config;

    LogViewDialog *m_viewLogDialog;
    ConfigDialog *m_configDialog;
    DesktopList *m_desktopDialog;

    bool m_bInitialized;
    std::shared_ptr< QMenu > m_context_menu;
    size_t m_button_idx;
};


#endif // MAINWINDOW_H
