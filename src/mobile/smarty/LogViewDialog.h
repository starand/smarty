#ifndef VIEWLOGDIALOG_H
#define VIEWLOGDIALOG_H

#include <QDialog>

namespace Ui
{
    class ViewLogDialog;
}

class LogViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewDialog( QWidget *parent = 0 );
    ~LogViewDialog( );

public:
    void load_logfile( );
private:
    void set_text( const QString &content );

    private slots:
    void on_closeButton_clicked( );
    void on_refreshButton_clicked( );

private:
    Ui::ViewLogDialog *ui;
};

#endif // VIEWLOGDIALOG_H
