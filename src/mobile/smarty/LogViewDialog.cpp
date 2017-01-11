#include "LogViewDialog.h"
#include "ui_LogViewDialog.h"

#include "MainWindow.h"

#include <QFile>

#include <common/StdAfx.h>


extern char g_szButtonGrayStyle[];

//--------------------------------------------------------------------------------------------------

LogViewDialog::LogViewDialog( QWidget *parent ) :
QDialog( parent ),
ui( new Ui::ViewLogDialog )
{
    ui->setupUi( this );

    QString styleSheet = "background-color:#101010;";
    ui->plainTextEdit->setStyleSheet( styleSheet );

    ui->closeButton->setStyleSheet( g_szButtonGrayStyle );
    ui->closeButton->setMinimumHeight( 120 );
    ui->refreshButton->setStyleSheet( g_szButtonGrayStyle );
    ui->refreshButton->setMinimumHeight( 120 );

    setWindowState( windowState( ) | Qt::WindowMaximized );
}

//--------------------------------------------------------------------------------------------------

LogViewDialog::~LogViewDialog( )
{
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void LogViewDialog::load_logfile( )
{
    size_t lines = 13;
    QFile logFile( "smarty.log" );
    QString logContent;

    if ( logFile.open( QIODevice::ReadOnly ) )
    {
        logFile.seek( logFile.size( ) - 1 );

        int count = 0;
        while ( ( count <= lines ) && ( logFile.pos( ) > 0 ) )
        {
            QByteArray data = logFile.read( 1 );
            logFile.seek( logFile.pos( ) - 2 );

            if ( data[ 0 ] == '\n' )
            {
                ++count;
            }
        }

        if ( count > 0 )
        {
            logFile.read( 2 );
            logContent = logFile.readAll( );
        }
    }
    else
    {
        logContent = logFile.errorString( );
    }

    set_text( logContent );
}

//--------------------------------------------------------------------------------------------------

void LogViewDialog::set_text( const QString& content )
{
    ui->plainTextEdit->setPlainText( content );
}

//--------------------------------------------------------------------------------------------------

void LogViewDialog::on_closeButton_clicked( )
{
    close( );
}

//--------------------------------------------------------------------------------------------------

void LogViewDialog::on_refreshButton_clicked( )
{
    load_logfile( );
}

//--------------------------------------------------------------------------------------------------
