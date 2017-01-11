#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>


namespace Ui
{
    class ConfigDialog;
}

enum ERESULT
{
    ER_NONE,

    ER_SAVED,
    ER_CANCELLED,
};

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog( QWidget *parent = 0 );
    ~ConfigDialog( );

    private slots:
    void on_saveButton_clicked( );
    void on_cancelButton_clicked( );

private:
    bool save_config( );

public:
    bool load_config( );

    static bool config_exists( );
    ERESULT result( ) const
    {
        return m_dialogResult;
    }

    ERESULT showModal( );

public:
    const char *get_server_address( );
    const char *get_server_password( );
    ushort get_server_port( ) const;

private:
    Ui::ConfigDialog *ui;

    ERESULT m_dialogResult;

    std::string m_address, m_port, m_password;
};

#endif // CONFIGDIALOG_H
