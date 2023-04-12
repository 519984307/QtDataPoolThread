#include "SqliteThread.h"

SqliteThread::SqliteThread(QObject* parent)
{
}

SqliteThread::~SqliteThread()
{
	m_stoped = true;
	m_waitCondition.wakeOne();

	quit();
	wait();
}

void SqliteThread::setDBInfo(QString hostName, QString databaseName, QString user, QString pwd)
{
	m_hostName = hostName;
	m_databaseName = databaseName;
	m_user = user;
	m_pwd = pwd;
}

void SqliteThread::addQuery(QString cmd)
{
	QMutexLocker lock(&m_dataMutex);

	m_taskList.append(cmd);
	m_waitCondition.wakeOne();
}

void SqliteThread::stopRun()
{
	m_stoped = true;
	m_waitCondition.wakeOne();

	quit();
	wait();
}

void SqliteThread::run()
{
	auto db = QSqlDatabase::addDatabase("QSQLITE"/*如果需要添加多个数据库连接需要设置连接的名字,"connectName"*/);
	db.setHostName(m_hostName);
	db.setDatabaseName(m_databaseName);
	db.setUserName(m_user);
	db.setPassword(m_pwd);
	if (!db.open())
	{
		qDebug() << "DB OPEN ERROR :" << db.lastError();
	}

	QSqlQuery  query(db);
	m_taskList.clear();
	m_stoped = false;
	
	emit signDBConnected();

	while (true)
	{
		QMutexLocker lock(&m_dataMutex);
		if (m_taskList.isEmpty())
		{
			m_waitCondition.wait(&m_dataMutex);
		}
		else
		{
			auto data = m_taskList.takeFirst();
			lock.unlock();

			if (!query.exec(data.toUtf8().data()))
			{
				qDebug() << QStringLiteral("INSERT CMD ERROR [%1]").arg(data) ;
			}
		}

		if (m_stoped)
		{
			break;
		}
	}
}


