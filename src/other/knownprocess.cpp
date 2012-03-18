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

    start("C:\\Program Files\\Git\\bin\\git", arguments);

    connect(this, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
}

void KnownProcess::processFinished(int code)
{
    result.append(QString::fromUtf8(readAll()));

    emit completed(this);
}
