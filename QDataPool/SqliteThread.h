#pragma once
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>

class SqliteThread :
    public QThread
{
    Q_OBJECT
public:
    explicit SqliteThread(QObject* parent = nullptr);

    ~SqliteThread();

    void setDBInfo(QString hostName, QString databaseName, QString user, QString pwd);

    void addQuery(QString cmd);

    void stopRun();

    Q_SIGNAL void signDBConnected();

protected:
    void run();

private:
    QString m_hostName;
    QString m_databaseName;
    QString m_user;
    QString m_pwd;
    QStringList m_taskList;

    QMutex  m_dataMutex;
    QWaitCondition m_waitCondition;

    bool m_stoped = 1;
};


