#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update_output();
    void edge_closed();
    void on_connect_clicked();
    void on_disconnect_clicked();
    void on_browse_for_edge_clicked();

    void on_actionSave_As_triggered();

    void on_actionQuit_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;
    QProcess* edge;
    QSettings* settings;
    QString activeFile;

    void load_save(QString fileName);
    void save_file(QString fileName);
    void setActiveFile(QString fileName);
};

#endif // MAINWINDOW_H
