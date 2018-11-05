#include "eventhandler.cpp"


 EventHandler:EventHandler() {
        Worker *worker = new Worker;
        worker->moveToThread(&m_workerThread);
        connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &EventHandler::operate, m_worker, &Worker::doWork);
        connect(m_worker, &Worker::altTabPressed, this, &EvemtHandler::altTabPressed;
        m_workerThread.start();
    }
    ~EventHandler::EventHandler() {
        m_workerThread.quit();
        m_workerThread.wait();
}
