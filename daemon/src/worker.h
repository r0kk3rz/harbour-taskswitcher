#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void readKeyboard(const QString &device);

signals:
    void altTabPressed();
    void altReleased();
};

#endif
