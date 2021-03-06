#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QKeyEvent>
#include <QMessageBox>

#include "src/other/knownprocess.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    int processCount;
    bool inScanning;
    QList<KnownProcess *> postProcessList;
    QStringList importantProjects;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void scan(QString aFolder);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    inline void saveState();
    inline void loadState();

private slots:
    void startScanning();
    void processCompleted(KnownProcess *aProcess);
};

#endif // MAINWINDOW_H
