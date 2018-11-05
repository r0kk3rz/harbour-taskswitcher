#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

class EventHandler : public QObject
{
    Q_OBJECT
    
    public:
    EventHandler();
    
    Q_SLOT void altTabPressed();
    Q_SIGNAL start()
    
    private:
    
    QThread m_workerThread;
    Worker *m_worker;
    
};

#endif
