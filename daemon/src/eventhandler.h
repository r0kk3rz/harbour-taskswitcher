#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QThread>

#include "worker.h"

class EventHandler : public QObject
{
    Q_OBJECT
    
    public:
    EventHandler();
    ~EventHandler();

    Q_SLOT void altTabPressed();
    Q_SLOT void altReleased();
    Q_SIGNAL void start(const QString &device);
    
    private:
    
    QThread m_workerThread;
    Worker *m_worker;
    bool m_taskSwitcherVisible  = false;

};

#endif
