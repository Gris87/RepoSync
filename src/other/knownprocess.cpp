#include "src/other/global.h"

#include <QSettings>
#include <QTimer>



QString pathToGit;



KnownProcess::KnownProcess(QString aWorkDirectory, QObject *parent) :
    QProcess(parent)
{
    if (pathToGit == "")
    {
        QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);

        QStringList groups = settings.childGroups();

        for (int i = 0; i < groups.length(); ++i)
        {
            QString displayName = settings.value(groups.at(i) + "/DisplayName").toString();

            if (displayName.contains("Git ver"))
            {
                QString installLocation = settings.value(groups.at(i) + "/InstallLocation").toString();

                if (installLocation != "")
                {
                    pathToGit = QDir::fromNativeSeparators(installLocation + "/bin/git").replace("//", "/");
                }
            }
        }

        if (pathToGit == "")
        {
            pathToGit = "C:/Program Files/Git/cmd/git";
        }
    }

    connect(this, SIGNAL(finished(int)), this, SLOT(processFinished(int)));

    setWorkingDirectory(aWorkDirectory);

    QStringList arguments;

#ifdef REPOSYNC
    step=0;
    arguments.append("reset");
    arguments.append("--hard");
#endif
#ifdef REPOSTATUS
    arguments.append("status");
#endif

    start(pathToGit, arguments);
}

void KnownProcess::processFinished(int /*code*/)
{
    result.append(QString::fromUtf8(readAll()));

#ifdef REPOSYNC
    QTimer::singleShot(100, this, SLOT(delayedStart()));
#endif
#ifdef REPOSTATUS
    emit completed(this);
#endif
}

#ifdef REPOSYNC
void KnownProcess::delayedStart()
{
    ++step;

    switch (step)
    {
        case 1:
        {
            QStringList arguments;

            arguments.append("clean");
            arguments.append("-df");

            start(pathToGit, arguments);
        }
        break;

        case 2:
        {
            QStringList arguments;

            arguments.append("pull");

            start(pathToGit, arguments);
        }
        break;

        default:
        {
            emit completed(this);
        }
        break;
    }
}
#endif
