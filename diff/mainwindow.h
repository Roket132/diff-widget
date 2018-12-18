#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void add_Items_Main(QStringList name);

private slots:
    void onActiontestTriggered();

    void on_Reload_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
