#include "eventhandler.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDebug>

#define SERVICE_NAME "com.piggz.taskswitcher"

EventHandler::EventHandler() {
    qDebug() << "Eventhandler";
    m_worker = new Worker;
    m_worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(this, &EventHandler::start, m_worker, &Worker::readKeyboard);
    connect(m_worker, &Worker::altTabPressed, this, &EventHandler::altTabPressed);
    connect(m_worker, &Worker::altReleased, this, &EventHandler::altReleased);

    m_workerThread.start();
}

EventHandler::~EventHandler() {
    m_workerThread.quit();
    m_workerThread.wait();
}

void EventHandler::altTabPressed()
{
    qDebug() << "Eventhandler::altTabPressed";

    QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::sessionBus());

    if (!m_taskSwitcherVisible)
    {
        /* show taskswitcher and advance one app */
        m_taskSwitcherVisible = true;
        iface.call("nextAppTaskSwitcher");
        iface.call("showTaskSwitcher");
    }
    else
    {
        /* Toggle to next app */
        iface.call("nextAppTaskSwitcher");
    }
}


void EventHandler::altReleased()
{
    QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::sessionBus());

    if (m_taskSwitcherVisible)
    {
        m_taskSwitcherVisible = false;
        iface.call("hideTaskSwitcher");
    }
    
}