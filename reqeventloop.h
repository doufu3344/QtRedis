#ifndef REQUESTEVENTLOOP_H
#define REQUESTEVENTLOOP_H

#include <QEventLoop>
#include <QTimer>

#define TIME_OUT 5  //5s
class ReqEventLoop : public QEventLoop
{
    Q_OBJECT

public:
    explicit ReqEventLoop(int timeout = TIME_OUT, QObject *parent = 0);
    ~ReqEventLoop();
	int exec(ProcessEventsFlags flags = AllEvents);
	inline bool isTimeout(){ return m_bTimeout; }

public slots:
    void quit();

private slots:
	void timeout();

private:
	QTimer m_Timer;
	bool m_bTimeout;
};

#endif // REQUESTEVENTLOOP_H
