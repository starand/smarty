#ifndef DESKTOPDIALOG_H
#define DESKTOPDIALOG_H

#include <QDialog>

#include <threading.h>
#include <map>


namespace Ui
{
    class DesktopDialog;
}


struct mobile_notification_t;
class QPushButton;
class DesktopDialog;
class client_lib_t;

typedef struct
{
    uint index;
    std::string name;
    QPushButton *button;
}
desktop_info_t;

typedef std::map<uint, desktop_info_t> desktop_list_t;

class DesktopList : public QDialog
{
    Q_OBJECT

public:
    explicit DesktopList( client_lib_t *clientlib, QWidget *parent = 0 );
    ~DesktopList( );

private:
    void create_internal_objects( );
    void destroy_internal_objects( );

public:
    void handle_notification( mobile_notification_t *notification );

private:
    void add_desktop( uint index, const string &name );
    void remove_desktop( uint index );
    void do_remove_button( desktop_list_t::iterator iter );
    void clear_desktop_list( );

    QPushButton *create_button( const char *name );
    desktop_list_t::iterator find_by_button( QPushButton *button );

    private slots:
    void on_desktop_selected( );

private:
    Ui::DesktopDialog *ui;

    desktop_list_t m_desktop_list;
    mutex_t m_desktop_list_lock;

    DesktopDialog *m_desktopDialog;
    client_lib_t *m_clientlib;
};

#endif // DESKTOPDIALOG_H
