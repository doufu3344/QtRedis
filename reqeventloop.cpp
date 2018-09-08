#include "reqeventloop.h"

ReqEventLoop::ReqEventLoop(int timeout, QObject *parent)
	:QEventLoop(parent)
{
	m_bTimeout = false;
	m_Timer.setSingleShot(true);
	m_Timer.setInterval(timeout * 1000);
	connect(&m_Timer, &QTimer::timeout, this, &ReqEventLoop::timeout);
}

ReqEventLoop::~ReqEventLoop()
{
	if (m_Timer.isActive())
		m_Timer.stop();
}

int ReqEventLoop::exec(ProcessEventsFlags flags)
{
	m_Timer.start();
	return QEventLoop::exec(flags);
}

void ReqEventLoop::timeout()
{
	m_bTimeout = true;
	QEventLoop::quit();
}

void ReqEventLoop::quit()
{
	if (m_Timer.isActive())
		m_Timer.stop();

	m_bTimeout = false;//not timer triggerd
	QEventLoop::quit();
}
