#include <QCoreApplication>

#include "populator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPointer <populator> pp = new populator;
    QString sInFileName;
    QString sConnParams;
    QString sQueryDirPath;
    bool bCreate=false;

    for (int i=0; i<argc; ++i)
    {
        QStringList slArgv = QString(argv[i]).split("=");

        if (QString::compare(slArgv.at(0),"-createdb")==0)
        {
            if (slArgv.at(1).isEmpty())
            {pp->logger("Empty file path"); continue;}
            bCreate=true;
            sQueryDirPath = slArgv.at(1);
        }

        if (QString::compare(slArgv.at(0),"-filename")==0)
        {
            if (slArgv.at(1).isEmpty())
            {pp->logger("Empty file name"); continue;}

            sInFileName = slArgv.at(1);
        }

        if (QString::compare(slArgv.at(0),"-connparams")==0)
        {
            if (slArgv.at(1).isEmpty())
            {pp->logger("Empty connection parameters file name"); continue;}
            sConnParams = slArgv.at(1);
        }
    }

    sInFileName = "/home/ap/dev/qt/pharmacy/dbPopulator/build-dbPopulator-Desktop_Qt_5_6_0_GCC_64bit-Debug/bin/28.04.2016.xlsx";
    sConnParams = "/home/ap/dev/qt/pharmacy/dbPopulator/build-dbPopulator-Desktop_Qt_5_6_0_GCC_64bit-Debug/bin/connparams";
    sQueryDirPath = "/home/ap/dev/qt/pharmacy/db/dbqueries/create/registry";
    bCreate = true;

    if(bCreate){pp->createDatabase(sQueryDirPath,sConnParams);}
    else {pp->populate(sInFileName,sConnParams);};
    pp->deleteLater();

    return a.exec();
}
