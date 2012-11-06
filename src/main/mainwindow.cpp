#include "src/other/global.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        if (QMessageBox::question(this, "Want to go?", "Task still in progress. Do you want to continue?", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape)==QMessageBox::Yes)
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
#ifdef REPOSYNC
    setWindowTitle("RepoSync - Scanning");
#endif
#ifdef REPOSTATUS
    setWindowTitle("RepoStatus - Scanning");
#endif

    processCount=0;

    inScanning=true;
    scan(selectedDir);
    inScanning=false;

    ui->logTextEdit->append("");

    if (ui->logTextEdit->toPlainText().count("\n")>2)
    {
        ui->logTextEdit->append("<span style=\" color:#00ff00;\">Scanning completed</span>");
        ui->logTextEdit->append("");

#ifdef REPOSYNC
    setWindowTitle("RepoSync - Processing");
#endif
#ifdef REPOSTATUS
    setWindowTitle("RepoStatus - Processing");
#endif
    }
    else
    {
        ui->logTextEdit->append("<span style=\" color:#ff0000;\">Nothing found. Try to modify file \""+QDir::toNativeSeparators(dir)+"data\\config.ini\"</span>");

#ifdef REPOSYNC
    setWindowTitle("RepoSync - Scanning error");
#endif
#ifdef REPOSTATUS
    setWindowTitle("RepoStatus - Scanning error");
#endif
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
        if (aFiles.at(i).fileName()==".git" && !aFolder.endsWith("/webDownloader/") && !aFolder.endsWith("/ColladaToXG_UI/"))
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
    bool isImportant=false;

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
            isImportant=true;

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
        if (aOneLine.contains("Your branch is ahead of"))
        {
            isImportant=true;
            aOneLine="<span style=\" color:#ff0000;\">"+aOneLine+"</span>";
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
                isImportant=true;

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

    if (isImportant)
    {
        importantProjects.append(QDir::toNativeSeparators(aProcess->workingDirectory()));
    }

    if (processCount==0)
    {
        if (importantProjects.length()>0)
        {
#ifdef REPOSYNC
            ui->logTextEdit->append("<span style=\" color:#8080ff;\">Updated projects:</span>");
#endif
#ifdef REPOSTATUS
            ui->logTextEdit->append("<span style=\" color:#8080ff;\">Please verify projects:</span>");
#endif

            for (int i=0; i<importantProjects.length(); ++i)
            {
                ui->logTextEdit->append(importantProjects.at(i));
            }

            ui->logTextEdit->append("");
        }

        ui->logTextEdit->append("<span style=\" color:#00ff00;\">That's all. Good luck</span>");

#ifdef REPOSYNC
        setWindowTitle("RepoSync - Completed");
#endif
#ifdef REPOSTATUS
        setWindowTitle("RepoStatus - Completed");
#endif

        QApplication::alert(this);
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

    aSettings.beginGroup("States");
    aSettings.setValue("Geometry",saveGeometry());
    aSettings.endGroup();
}

void MainWindow::loadState()
{
    QSettings aSettings(dir+"data/config.ini",QSettings::IniFormat);

    aSettings.beginGroup("Config");
    selectedDir=aSettings.value("Selected_Directory", dir).toString();
    aSettings.endGroup();

    aSettings.beginGroup("States");
    restoreGeometry(aSettings.value("Geometry").toByteArray());
    aSettings.endGroup();
}
