#include "eventhandler.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDebug>

#define SERVICE_NAME "com.piggz.taskswitcher"

EventHandler::EventHandler() {
    m_worker = new Worker;
    m_worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(this, &EventHandler::start, m_worker, &Worker::readKeyboard);
    connect(m_worker, &Worker::altTabPressed, this, &EventHandler::altTabPressed);
    connect(m_worker, &Worker::altReleased, this, &EventHandler::altReleased);
    connect(m_worker, &Worker::finished, this, &EventHandler::workerFinished);
    m_workerThread.start();

    //Start a timer to check for BT keyboard
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &EventHandler::checkForDevice);
    m_timer->start(10000);
}

EventHandler::~EventHandler() {
    m_workerThread.quit();
    m_workerThread.wait();
}

void EventHandler::startWorker(const QString &device)
{
    start(device);
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

void EventHandler::workerFinished()
{
    qDebug() << "Worker has finished";
}

void EventHandler::checkForDevice()
{

}

QString EventHandler::getDeviceFile(const QString &name)
{
    bool name_found = false;
    QString eventstring;
    QString devicepath;

    QFile file("/proc/bus/input/devices");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (!name_found)
        {
            if (line.contains(name)) {
                name_found = true;
            }
        }
        else
        {
            if (line.contains("event"))
            {
                eventstring = line.mid(line.indexOf("event"));
                eventstring = eventstring.mid(0, eventstring.indexOf(' '));
                break;
            }
        }
    }

    devicepath = "/dev/input/" + eventstring;
    qDebug() <<  "devicepath:" << devicepath;

    return devicepath;

}
