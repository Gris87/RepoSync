#include "src/other/global.h"

KnownProcess::KnownProcess(QString aWorkDirectory, QObject *parent) :
    QProcess(parent)
{
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

    start("C:\\Program Files\\Git\\cmd\\git.cmd", arguments);

    connect(this, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
}

void KnownProcess::processFinished(int /*code*/)
{
    result.append(QString::fromUtf8(readAll()));

#ifdef REPOSYNC
    step++;

    if (step==1)
    {
        QStringList arguments;

        arguments.append("clean");
        arguments.append("-df");

        start("C:\\Program Files\\Git\\cmd\\git.cmd", arguments);
    }
    else
    if (step==2)
    {
        QStringList arguments;

        arguments.append("pull");

        start("C:\\Program Files\\Git\\cmd\\git.cmd", arguments);
    }
    else
    if (step==3)
    {
        emit completed(this);
    }
#endif
#ifdef REPOSTATUS
    emit completed(this);
#endif
}
