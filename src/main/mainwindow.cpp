#include "src/other/global.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());

#ifdef REPOSYNC
    setWindowTitle("RepoSync");
#endif
#ifdef REPOSTATUS
    setWindowTitle("RepoStatus");
#endif

    loadState();

    ui->logTextEdit->append("<span style=\" color:#00ffff;\"><b>Scanning directory:</b></span> "+QDir::toNativeSeparators(selectedDir));
    ui->logTextEdit->append("");

    QTimer::singleShot(0, this, SLOT(startScanning()));
}

MainWindow::~MainWindow()
{
    saveState();

    delete ui;
}

void MainWindow::startScanning()
{
    processCount=0;

    inScanning=true;
    scan(selectedDir);
    inScanning=false;

    ui->logTextEdit->append("");

    if (ui->logTextEdit->toPlainText().count("\n")>1)
    {
        ui->logTextEdit->append("<span style=\" color:#00ff00;\">Scanning completed</span>");
        ui->logTextEdit->append("");
    }
    else
    {
        ui->logTextEdit->append("<span style=\" color:#ff0000;\">Nothing found. Try to modify file \""+QDir::toNativeSeparators(dir)+"data\\config.ini\"</span>");
    }

    for (int i=0; i<postProcessList.length(); i++)
    {
        processCompleted(postProcessList.at(i));
    }
}

void MainWindow::scan(QString aFolder)
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    if (!aFolder.endsWith("/"))
    {
        aFolder.append("/");
    }

    QDir aDir(aFolder);
    QFileInfoList aFiles=aDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);

    for (int i=0; i<aFiles.length(); i++)
    {
        if (
            aFiles.at(i).isDir()
           )
        {
            if (aFiles.at(i).fileName()==".git")
            {
                ui->logTextEdit->append("<span style=\" color:#ffff00;\"><b>Repository found:</b></span> "+QDir::toNativeSeparators(aFolder));

                KnownProcess* aProcess=new KnownProcess(aFolder, this);
                connect(aProcess, SIGNAL(completed(KnownProcess*)), this, SLOT(processCompleted(KnownProcess*)));

                processCount++;

                return;
            }

            scan(aFolder+aFiles.at(i).fileName());
        }
    }
}

void MainWindow::processCompleted(KnownProcess *aProcess)
{
    if (inScanning)
    {
        postProcessList.append(aProcess);
        return;
    }

    processCount--;

    ui->logTextEdit->append("-----------------------------------------------------------------------------------------------");
    ui->logTextEdit->append("<span style=\" color:#ff00ff;\"><b>Result for:</b></span> "+QDir::toNativeSeparators(aProcess->workingDirectory()));
    ui->logTextEdit->append("-----------------------------------------------------------------------------------------------");
    ui->logTextEdit->append("");

    ui->logTextEdit->append(aProcess->result);

    if (processCount==0)
    {
        ui->logTextEdit->append("<span style=\" color:#00ff00;\">That's all. Good luck</span>");
    }
}

void MainWindow::saveState()
{
    QDir(dir).mkpath(dir+"data");
    QFile::remove(dir+"data/config.ini");
    QSettings aSettings(dir+"data/config.ini",QSettings::IniFormat);

    aSettings.beginGroup("Config");

    aSettings.setValue("Selected_Directory",selectedDir);

    aSettings.endGroup();
}

void MainWindow::loadState()
{
    QSettings aSettings(dir+"data/config.ini",QSettings::IniFormat);

    aSettings.beginGroup("Config");

    selectedDir=aSettings.value("Selected_Directory", dir).toString();

    aSettings.endGroup();
}
