#include "MainWindow.h"
#include "notificationclient.h"

#include <QApplication>
#include <QFile>

#include <logger.h>


//--------------------------------------------------------------------------------------------------

void write_logger_config( );

//--------------------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
    write_logger_config( );
    logger::initialize( "smarty.mobile", "logger.cfg" );
    QApplication qApplication( argc, argv );

    MainWindow mainWindow;
    if ( !mainWindow.initialize( ) )
    {
        return -1;
    }

    mainWindow.show( );

    return qApplication.exec( );
}

//--------------------------------------------------------------------------------------------------

void write_logger_config( )
{
    FILE *fp = fopen( "logger.cfg", "w" );
    if ( fp )
    {
        fputs( "[file]\n", fp );
        fputs( "log_file = ~/self.log\n", fp );
        fputs( "severity = 6\n", fp );
        fclose( fp );
    }
}

//--------------------------------------------------------------------------------------------------
