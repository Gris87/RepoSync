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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (
        processCount==0
        &&
        !inScanning
       )
    {
        event->accept();
    }
    else
    {
        if (QMessageBox::question(this, "Are you go away?", "Task still in progress. Do you want to continue?", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape)==QMessageBox::Yes)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (
        processCount==0
        &&
        !inScanning
        &&
        (
         event->key()==Qt::Key_Escape
         ||
         event->key()==Qt::Key_Return
         ||
         event->key()==Qt::Key_Enter
        )
       )
    {
        close();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::startScanning()
{
    processCount=0;

    inScanning=true;
    scan(selectedDir);
    inScanning=false;

    ui->logTextEdit->append("");

    if (ui->logTextEdit->toPlainText().count("\n")>2)
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
            if (aFiles.at(i).fileName()==".git" && !aFolder.endsWith("/webDownloader/"))
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
    ui->logTextEdit->append(QDir::toNativeSeparators(aProcess->workingDirectory()));
    ui->logTextEdit->append("-----------------------------------------------------------------------------------------------");
    ui->logTextEdit->append("");

    int state=-1;

    QStringList aLines=aProcess->result.split("\n");

    for (int i=0; i<aLines.length(); i++)
    {
        QString aOneLine=aLines.at(i);

#ifdef REPOSYNC
        if (aOneLine.contains("Fast-forward"))
        {
            state=0;
        }
        else
        if (
            state==0
            &&
            aOneLine.contains("|")
            &&
            (
             aOneLine.contains("+")
             ||
             aOneLine.contains("-")
            )
           )
        {
            QString pluses="";
            QString minuses="";

            while (aOneLine.endsWith("-"))
            {
                aOneLine.remove(aOneLine.length()-1, 1);
                minuses.append("-");
            }

            while (aOneLine.endsWith("+"))
            {
                aOneLine.remove(aOneLine.length()-1, 1);
                pluses.append("+");
            }

            for (int i=aOneLine.length()-1; i>=0; i--)
            {
                if (aOneLine.at(i)==' ')
                {
                    int end=i;
                    QString aSpaces="";

                    while (i>=0 && aOneLine.at(i)==' ')
                    {
                        i--;
                        aSpaces.append(".");
                    }

                    i++;

                    aSpaces.insert(0, "<span style=\" color:#000000;\">");
                    aSpaces.append("</span>");

                    aOneLine.replace(i, end-i+1, aSpaces);
                }
            }

            aOneLine.append("<span style=\" color:#00ff00;\">"+pluses+"</span>");
            aOneLine.append("<span style=\" color:#ff0000;\">"+minuses+"</span>");
        }
#endif
#ifdef REPOSTATUS
        if (aOneLine.contains("Changes to be committed:"))
        {
            state=0;
        }
        else
        if (aOneLine.contains("Changes not staged for commit:"))
        {
            state=1;
        }
        else
        if (aOneLine.contains("Untracked files:"))
        {
            state=2;
        }
        else
        if (
            aOneLine.startsWith("#")
            &&
            aOneLine.length()>1
            &&
            aOneLine.at(1).unicode()==9
           )
        {
            if (state>=0)
            {
                aOneLine.remove(0, 2);

                if (state==0)
                {
                    aOneLine="<span style=\" color:#00ff00;\">"+aOneLine+"</span>";
                }
                else
                {
                    aOneLine="<span style=\" color:#ff0000;\">"+aOneLine+"</span>";
                }

                aOneLine.insert(0, "#<span style=\" color:#000000;\">.......</span>");
            }
        }
#endif
        ui->logTextEdit->append(aOneLine);
    }

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
