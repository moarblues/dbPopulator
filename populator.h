#ifndef POPULATOR_H
#define POPULATOR_H

#include <QObject>
#include <QPointer>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "files.h"

class populator : public QObject
{
    Q_OBJECT

    QSqlDatabase *db;

    int prepareFile(const QString &sInFileName);
    int prepareDb(const QStringList &slConnParamsFileName);
    void populateReference(const QStringList &slRef);
    void populateRegistry(const QStringList &slRegDb, const QVector<QStringList> &vslReg);
    void populateRetail();
    QList<QStringList> executeQuery(const QString &sQry, bool bPrepare);

public:
    explicit populator(QObject *parent = 0);
    virtual ~populator();

signals:

public slots:
    void createDatabase(const QString &sQueryDirPath, const QString &sConnParamsFileName);
    void populate(const QString &sInFileName, const QString &sConnParamsFileName);
    void logger(const QString &sLog);
};

#endif // POPULATOR_H
