#include "eventhandler.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDebug>

#define SERVICE_NAME "com.piggz.taskswitcher"

EventHandler::EventHandler() {
    qDebug() << "Eventhandler";
    Worker *worker = new Worker;
    worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &EventHandler::start, m_worker, &Worker::readKeyboard);
    connect(m_worker, &Worker::altTabPressed, this, &EventHandler::altTabPressed);

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
        iface.call("showTaskSwitcher");
    }
}
