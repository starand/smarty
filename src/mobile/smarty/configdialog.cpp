#include "ConfigDialog.h"
#include "ui_configdialog.h"

#include <logger.h>
#include <ConfigFile.h>

#include <QFile>
#include <QTextStream>

#ifdef WINDOWS
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif


//--------------------------------------------------------------------------------------------------

static const char g_szSmartyConfigFileName[] = "smarty.cfg";

static const char g_szServerSection[] = "server";
static const char g_szServerAddress[] = "Address";
static const char g_szServerPort[] = "Port";
static const char g_szServerPassword[] = "Password";

//--------------------------------------------------------------------------------------------------

ConfigDialog::ConfigDialog( QWidget *parent ) :
QDialog( parent ),
ui( new Ui::ConfigDialog ),
m_dialogResult( ER_NONE )
{
    ui->setupUi( this );
    this->setStyleSheet( "background-color:#000000;" );
    setWindowTitle( "Edit Config" );

    QString elemStyle = "background-color:#101010;";
    ui->cancelButton->setStyleSheet( elemStyle );
    ui->saveButton->setStyleSheet( elemStyle );
    ui->serverEdit->setStyleSheet( elemStyle );
    ui->portEdit->setStyleSheet( elemStyle );
    ui->passwordEdit->setStyleSheet( elemStyle );

#ifdef Q_OS_ANDROID
    setWindowState( windowState( ) | Qt::WindowMaximized );
#endif
}

//--------------------------------------------------------------------------------------------------

ConfigDialog::~ConfigDialog( )
{
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void ConfigDialog::on_saveButton_clicked( )
{
    save_config( );
    m_dialogResult = ER_SAVED;
    close( );
}

//--------------------------------------------------------------------------------------------------

bool ConfigDialog::save_config( )
{
    QString content = "[" + QString( g_szServerSection ) + "]"
        + "\n" + g_szServerAddress + " = " + ui->serverEdit->text( )
        + "\n" + g_szServerPort + " = " + ui->portEdit->text( )
        + "\n" + g_szServerPassword + " = " + ui->passwordEdit->text( );

    QFile configFile( g_szSmartyConfigFileName );
    if ( !configFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        LOG_ERROR( "Could not write config file %s", g_szSmartyConfigFileName );
        return false;
    }

    QTextStream stream( &configFile );
    stream << content;
    configFile.close( );

    m_address = ui->serverEdit->text( ).toStdString( );
    m_port = ui->portEdit->text( ).toStdString( );
    m_password = ui->passwordEdit->text( ).toStdString( );

    LOG_INFO( "Config file %s was updated", g_szSmartyConfigFileName );
}

//--------------------------------------------------------------------------------------------------

bool ConfigDialog::load_config( )
{
    if ( !config_exists( ) )
    {
        return false;
    }

    CConfigFile config;
    if ( !config.LoadFromFile( g_szSmartyConfigFileName ) )
    {
        LOG_ERROR( "Could not load config from file %s", g_szSmartyConfigFileName );
        return false;
    }

    config.GetOptionValue( g_szServerSection, g_szServerAddress, m_address );
    ui->serverEdit->setText( m_address.c_str( ) );
    config.GetOptionValue( g_szServerSection, g_szServerPort, m_port );
    ui->portEdit->setText( m_port.c_str( ) );
    config.GetOptionValue( g_szServerSection, g_szServerPassword, m_password );
    ui->passwordEdit->setText( m_password.c_str( ) );

    return !( m_address.empty( ) || m_port.empty( ) || m_password.empty( ) );
}

//--------------------------------------------------------------------------------------------------

/*static */
bool ConfigDialog::config_exists( )
{
    //return QFile(g_szSmartyConfigFileName).exists();
    return access( g_szSmartyConfigFileName, 0 ) == 0;
}

//--------------------------------------------------------------------------------------------------

void ConfigDialog::on_cancelButton_clicked( )
{
    m_dialogResult = ER_CANCELLED;
    close( );
}

//--------------------------------------------------------------------------------------------------

ERESULT ConfigDialog::showModal( )
{
    exec( );
    return m_dialogResult;
}

//--------------------------------------------------------------------------------------------------

const char *ConfigDialog::get_server_address( )
{
    static std::string address = ui->serverEdit->text( ).toStdString( );
    return address.c_str( );
}

//--------------------------------------------------------------------------------------------------

const char *ConfigDialog::get_server_password( )
{
    static std::string address = ui->passwordEdit->text( ).toStdString( );
    return address.c_str( );
}

//--------------------------------------------------------------------------------------------------

ushort ConfigDialog::get_server_port( ) const
{
    return (ushort)atoi( m_port.c_str( ) );
}

//--------------------------------------------------------------------------------------------------
