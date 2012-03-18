#ifndef KNOWNPROCESS_H
#define KNOWNPROCESS_H

#include <QProcess>

class KnownProcess : public QProcess
{
    Q_OBJECT
public:
#ifdef REPOSYNC
    int step;
#endif

    QString result;

    explicit KnownProcess(QString aWorkDirectory, QObject *parent = 0);

signals:
    void completed(KnownProcess *aProcess);

public slots:
    void processFinished(int code);
};

#endif // KNOWNPROCESS_H
