#ifndef FILES_H
#define FILES_H

#include <QObject>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "lib/incl/freexl/freexl.h"
#include "xlsxdocument.h"

class files : public QObject
{
    Q_OBJECT

    QString settingsFileName;

public:
    explicit files(QObject *parent = 0);
    virtual ~files();

signals:
    void setStatus(const QString &sLine);

public slots:
    QStringList readSettings();
    QStringList readConnectionParameters(const QString &sCpfn);
    QString readTextQuery(const QString &sPath);
    void writeFile(const QString &sOut, const QString &sFname);
    QList<QVector<QStringList> > getXlsData(const QString &sFpath);
    QList<QVector<QStringList> > getXlsxData(const QString &sFpath);

};

#endif // FILES_H
