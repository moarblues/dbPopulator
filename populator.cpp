#include "populator.h"

populator::populator(QObject *parent) : QObject(parent)
{

}

populator::~populator()
{

}

void populator::populate(const QString &sInFileName, const QString &sConnParamsFileName)
{
    QPointer<files> fw = new files;

    QStringList slConnParams = fw->readConnectionParameters(sConnParamsFileName);

    if(slConnParams.size()!=6)
    {this->logger("Invalid connection parameters");return;}

    this->logger("Preparing database");
    this->prepareDb(slConnParams);
    this->logger("Preparing file");
    if (this->prepareFile(sInFileName)!=0)
    {this->logger("Unable to prepare registry file");return;}

    QFile fIn;
    QList<QVector<QStringList> > *lvslOut = new QList<QVector<QStringList> >;

    fIn.setFileName(sInFileName);
    if(!fIn.exists())
    {this->logger("File does not exist"); return;}
    this->logger("File found");
    this->logger("Starting extraction");

    QFileInfo fiReg(fIn);

    QDate dReg = QDate::fromString(fiReg.completeBaseName(),"dd.MM.yyyy");
    if(!dReg.isValid())
    {this->logger("Date in file name is invalid");return;}
//    qDebug() << regDate.toString("yyyy-MM-dd");

    if (QString::compare(fiReg.suffix(),"xls")==0)
    {
        this->logger("Extracting .xls file");
        *lvslOut = fw->getXlsData(sInFileName);
    }
    else if (QString::compare(fiReg.suffix(),"xlsx")==0)
    {
        this->logger("Extracting .xlsx file");
        *lvslOut = fw->getXlsxData(sInFileName);
    }
    this->logger(QString("Extraction finished. Vector size ").append(QString::number(lvslOut->size())));
//    qDebug() << lvslOut->at(1);

    QVector<QStringList> vslMps;
    QStringList slMpHeaders = QStringList() << "registry.medicinal_products" << "reg_number, product_type_ref, trade_name_ref, "
                    "registration_type_ref, reg_date, reg_term, reg_exp_date, manufactorer_ref, country_ref, "
                    "mp_classification_ref, inn_ref, atc_classification_ref, dosage_form_ref, dosage, packing, "
                    "shelf_life, gmp_bool_ref, generic_bool_ref, prescription_bool_ref, ozhvls_bool_ref, npp_bool_ref, "
                    "trademark_bool_ref, patent_bool_ref, nd_type_ref, nd_number, bulk_bool_ref, block_bool_ref, "
                    "perpetuity_sign_bool_ref, registry_date_ref" << dReg.toString("yyyy-MM-dd");

    QVector<QStringList> vslMsahcs;
    QStringList slMsahcHeaders = QStringList() << "registry.msahc_products" << "reg_number, product_type_ref, trade_name_ref, "
                       "registration_type_ref, reg_date, reg_term, reg_exp_date, manufactorer_ref, country_ref, "
                       "mp_classification_ref, packing, shelf_life, risk_class_ref, gmp_bool_ref, trademark_bool_ref, "
                       "patent_bool_ref, mt_bool_ref, measurement_means_bool_ref, sterile_bool_ref, kit, bulk_bool_ref, "
                       "block_bool_ref, perpetuity_sign_bool_ref, registry_date_ref" << dReg.toString("yyyy-MM-dd");

//    qDebug() << vslOut->at(1);

//    qDebug() << mpVector.at(0);
//    qDebug() << vslMsahc.at(0);

    this->logger("Preparing referencies");
    QStringList slAtcclfs;
    QStringList slCountries;
    QStringList slDosageforms;
    QStringList slInns;
    QStringList slManufs;
    QStringList slMpclfs;
    QStringList slNdtypes;
    QStringList slProdtypes;
    QStringList slRegtypes;
    QStringList slRiskclasses;
    QStringList slTradenames;
    QStringList slRegdates;
//    QStringList packing;
//    QList<QStringList> units;

//    QList<QStringList> mps;
//    QList<QStringList> msahcs;

    slAtcclfs.append("registry.ref_atc_classifications");
    slAtcclfs.append("classification_name");
    slCountries.append("registry.ref_countries");
    slCountries.append("country_name");
    slDosageforms.append("registry.ref_dosage_forms");
    slDosageforms.append("form_name");
    slInns.append("registry.ref_inns");
    slInns.append("inn");
    slManufs.append("registry.ref_manufactorers");
    slManufs.append("manufactorer_name");
    slMpclfs.append("registry.ref_mp_classifications");
    slMpclfs.append("classification_name");
    slNdtypes.append("registry.ref_nd_types");
    slNdtypes.append("type_name");
    slProdtypes.append("registry.ref_product_types");
    slProdtypes.append("type_name");
    slRegtypes.append("registry.ref_registration_types");
    slRegtypes.append("type_name");
    slRiskclasses.append("registry.ref_risk_classes");
    slRiskclasses.append("class_name");
    slTradenames.append("registry.ref_trade_names");
    slTradenames.append("trade_name");
    slRegdates.append("registry.ref_registry_dates");
    slRegdates.append("registry_date");
    slRegdates.append(dReg.toString("yyyy-MM-dd"));


//    qDebug() << mpVector.at(0).size() << mpVector.at(0);
//    qDebug() << msahcVector.at(0).size() << msahcVector.at(0);
//    qDebug() << mpVector.at(0).at(25) << mpVector.at(0).at(25).isEmpty();

    for (int ireg=0; ireg<lvslOut->size(); ++ireg)
    {
        if (QString::compare(lvslOut->at(ireg).at(0).at(0).simplified(),"ЛС")==0)
        {
            vslMps.append(slMpHeaders);
            for (int irow = 1; irow < lvslOut->at(ireg).size(); ++irow)
            {
                if (lvslOut->at(ireg).at(irow).size()!=29)
                {this->logger(QString("Row size fauyl in ЛС at ").append(QString::number(irow)));return;}

                vslMps.append(lvslOut->at(ireg).at(irow));

                slProdtypes.append(lvslOut->at(ireg).at(irow).at(1));
                slTradenames.append(lvslOut->at(ireg).at(irow).at(2));
                slRegtypes.append(lvslOut->at(ireg).at(irow).at(3));
                slManufs.append(lvslOut->at(ireg).at(irow).at(7));
                slCountries.append(lvslOut->at(ireg).at(irow).at(8));
                slMpclfs.append(lvslOut->at(ireg).at(irow).at(9));
                slInns.append(lvslOut->at(ireg).at(irow).at(10));
                slAtcclfs.append(lvslOut->at(ireg).at(irow).at(11));
                slDosageforms.append(lvslOut->at(ireg).at(irow).at(12));
                slNdtypes.append(lvslOut->at(ireg).at(irow).at(23));
        //        packing.append(mpVector.at(irow).at(14));
            }
        }
        else if (QString::compare(lvslOut->at(ireg).at(0).at(0).simplified(),"ИМН")==0)
        {
            vslMsahcs.append(slMsahcHeaders);
            for (int irow = 1; irow < lvslOut->at(ireg).size(); ++irow)
            {
                if (lvslOut->at(ireg).at(irow).size()!=24)
                {this->logger(QString("Row size fauyl in ИМН at ").append(QString::number(irow)));return;}

                slProdtypes.append(lvslOut->at(ireg).at(irow).at(1));
                slTradenames.append(lvslOut->at(ireg).at(irow).at(2));
                slRegtypes.append(lvslOut->at(ireg).at(irow).at(3));
                slManufs.append(lvslOut->at(ireg).at(irow).at(7));
                slCountries.append(lvslOut->at(ireg).at(irow).at(8));
                slMpclfs.append(lvslOut->at(ireg).at(irow).at(9));
                slRiskclasses.append(lvslOut->at(ireg).at(irow).at(12));
        //        packing.append(msahcVector.at(irow).at(10));
            }
        }
        else
        {this->logger(QString("Unknown registry:").append(lvslOut->at(ireg).at(0).at(0).simplified()));continue;}
    }

    delete lvslOut;

//    qDebug() << prodtypes.at(0) << tradenames.at(0) << regtypes.at(0) << manufs.at(0) << countries.at(0)
//             << mpclfs.at(0) << inns.at(0) << atcclfs.at(0) << dosageforms.at(0) << ndtypes.at(0);

//    int imax=0;
//    for (int iitm=0;iitm<mpVector.size();++iitm)
//    {
//        if (mpVector.at(iitm).at(14).size()>imax)
//        {imax=mpVector.at(iitm).at(14).size();}
//    }
//    qDebug() << imax;

//    for (int iitm=0;iitm<110;++iitm)
//    {qDebug() << iitm%100 << (iitm%100==0);}

    slProdtypes.removeDuplicates();
    slTradenames.removeDuplicates();
    slRegtypes.removeDuplicates();
    slManufs.removeDuplicates();
    slCountries.removeDuplicates();
    slMpclfs.removeDuplicates();
    slInns.removeDuplicates();
    slAtcclfs.removeDuplicates();
    slDosageforms.removeDuplicates();
    slNdtypes.removeDuplicates();
    slRegdates.removeDuplicates();
    slRiskclasses.removeDuplicates();
//    packing.removeDuplicates();

//    qDebug() << slCountries;
//    qDebug() << prodtypes.contains("") << tradenames.contains("") << regtypes.contains("") << manufs.contains("")
//             << countries.contains("") << mpclfs.contains("") << inns.contains("") << atcclfs.contains("")
//             << dosageforms.contains("") << ndtypes.contains("") << regdates.contains("");

    this->logger("Populating referencies");

    this->populateReference(slProdtypes);
    this->populateReference(slTradenames);
    this->populateReference(slRegtypes);
    this->populateReference(slManufs);
    this->populateReference(slCountries);
    this->populateReference(slMpclfs);
    this->populateReference(slInns);
    this->populateReference(slAtcclfs);
    this->populateReference(slDosageforms);
    this->populateReference(slNdtypes);
    this->populateReference(slRegdates);
    this->populateReference(slRiskclasses);

    this->logger("Populating registry");
    this->logger("Populating mp");
    this->populateRegistry(slMpHeaders, vslMps);
    this->logger("Populating msahcp");
    this->populateRegistry(slMsahcHeaders, vslMsahcs);

    this->logger("Populating retail");
    this->populateRetail();

    db->close();
    if (!db->isOpen())
    {this->logger("Database closed");delete db;}
    fw->deleteLater();

//    this->logger("Starting write");

//    QString istr;
//    for(int iitm=0;iitm<packing.size();++iitm)
//    {istr.append(packing.at(iitm)).append("\n");}
//    QPointer<fileWorkout> fw = new fileWorkout;
//    fw->writeFile(istr,"packing");
//    this->logger(QString("Writing finished"));
//    fw->deleteLater();
}

int populator::prepareFile(const QString &sInFileName)
{
    if(sInFileName.isEmpty())
    {this->logger("Empty file name"); return 1;}

    return 0;
}

int populator::prepareDb(const QStringList &slConnParamsFileName)
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase(slConnParamsFileName.at(0)));
    db->setConnectOptions();
    db->setHostName(slConnParamsFileName.at(1));
    db->setDatabaseName(slConnParamsFileName.at(2));
    db->setUserName(slConnParamsFileName.at(3));
    db->setPassword(slConnParamsFileName.at(4));
    db->setPort(slConnParamsFileName.at(5).toInt());
    if (!db->open())
    {this->logger(db->lastError().text());return 1;}
    else
    {this->logger("Database ready");return 0;}
    return 0;
}

void populator::createDatabase(const QString &sQueryDirPath, const QString &sConnParamsFileName)
{
    QPointer<files> fw = new files;

    QStringList slConnParams = fw->readConnectionParameters(sConnParamsFileName);

    if(slConnParams.size()!=6)
    {this->logger("Invalid connection parameters");return;}

    this->logger("Preparing database");
    this->prepareDb(slConnParams);

    QDir queryFilesDir(sQueryDirPath);
    QStringList slItems = queryFilesDir.entryList(QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);

    QStringList slReferences;
    QStringList slTriggers;
    QStringList slMainTables;
    QStringList slViews;

    this->logger(QString("Directory effective entries count ").append(QString::number(slItems.size())));
    this->logger("Preparing query files");

    for (int i=0;i<slItems.size();++i)
    {
        QStringList slParts = slItems.at(i).split("-");
        QStringList slPurpose = slParts.at(0).split("_");

        if (slParts.size()<2)
        {continue;}
        else if (QString::compare(slPurpose.at(0),"createtable")==0)
        {
            if (slPurpose.size()<2)
            {continue;}
            else if(QString::compare(slPurpose.at(1),"ref")==0)
            {slReferences.append(slItems.at(i));}
            else if(QString::compare(slPurpose.at(1),"main")==0)
            {slMainTables.append(slItems.at(i));}
        }
        else if (QString::compare(slPurpose.at(0),"createfunction")==0)
        {
            if (slPurpose.size()<2)
            {continue;}
            else if (QString::compare(slPurpose.at(1),"trigger")==0)
            {slTriggers.append(slItems.at(i));}
        }
        else if (QString::compare(slPurpose.at(0),"createview")==0)
        {slViews.append(slItems.at(i));}
    }

    this->logger(QString("Preparation complete\n").append("References count ").append(QString::number(slReferences.size()))
                 .append("\n").append("Functions count ").append(QString::number(slTriggers.size())).append("\n")
                 .append("Main tables count ").append(QString::number(slMainTables.size())).append("\n")
                 .append("Views count ").append(QString::number(slViews.size())).append("\n"));

//    qDebug() << fw->readTextQuery(QString(sQueryDirPath).append("/").append(slReferences.at(0)));
//    qDebug() << fw->readTextQuery(QString(sQueryDirPath).append("/").append(slTriggers.at(0)));

    this->logger("Creating functions");

    for (int i=0;i<slTriggers.size();++i)
    {this->executeQuery(fw->readTextQuery(QString(sQueryDirPath).append("/").append(slTriggers.at(i))),false);}

    this->logger("Creating reference tables");

    for (int i=0;i<slReferences.size();++i)
    {this->executeQuery(fw->readTextQuery(QString(sQueryDirPath).append("/").append(slReferences.at(i))),false);}


    this->logger("Creating main tables");

    for (int i=0;i<slMainTables.size();++i)
    {this->executeQuery(fw->readTextQuery(QString(sQueryDirPath).append("/").append(slMainTables.at(i))),false);}

    this->logger("Creating views");

    for (int i=0;i<slViews.size();++i)
    {this->executeQuery(fw->readTextQuery(QString(sQueryDirPath).append("/").append(slViews.at(i))),false);}

    this->logger("Creation complete");

    db->close();
    if (!db->isOpen())
    {this->logger("Database closed");delete db;}

    fw->deleteLater();
}

void populator::populateReference(const QStringList &slRef)
{
    this->logger(slRef.at(0));

    bool newinsert=true;
    QString sQry;

    for (int irow=2; irow<slRef.size(); ++irow)
    {
        if (newinsert)
        {
            sQry = QString("INSERT INTO ").append(slRef.at(0).simplified()).append(" (").append(slRef.at(1).simplified())
                    .append(") VALUES ");
            newinsert=false;
        }
//        if (regList.at(irow).isEmpty())
//        {continue;}
        sQry.append("(");

        sQry.append("'").append(QString(slRef.at(irow)).replace("'","''").simplified()).append("'");

        sQry.append(")");
        if (irow%100==0)
        {
            newinsert=true;
            this->logger(QString("populating ").append(QString::number(irow)).append(" of ")
                         .append(QString::number(slRef.size())));
            this->executeQuery(sQry,true);
            continue;
        }

        if(irow<slRef.size()-1)
        {
            sQry.append(", ");
            continue;
        }
        this->executeQuery(sQry,true);
    }
//    strqry.append(")");
//    qDebug() << strqry;
//    this->executeQuery(sQry);
    this->logger(QString(slRef.at(0)).append(" populated"));
}

void populator::populateRegistry(const QStringList &slRegDb, const QVector<QStringList> &vslReg)
{
    this->logger(slRegDb.at(0));
    QString sQry;
    int atcclf_refcol=-1;
    int countries_refcol=-1;
    int dosageforms_refcol=-1;
    int inns_refcol=-1;
    int manufs_refcol=-1;
    int mpclfs_refcol=-1;
    int ndtypes_refcol=-1;
    int prodtypes_refcol=-1;
    int regtypes_refcol=-1;
    int riskclasses_refcol=-1;
    int tradenames_refcol=-1;
    int regdates_refcol=-1;
    int regdate_col=-1;
    int regexpdate_col=-1;
    int gmpbool_col =-1;
    int genericbool_col =-1;
    int prescriptionbool_col =-1;
    int ozhvlsbool_col =-1;
    int nppbool_col =-1;
    int trademarkbool_col =-1;
    int patentbool_col =-1;
    int bulkbool_col =-1;
    int blockbool_col =-1;
    int perpetuitysign_col =-1;
    int mtbool_col =-1;
    int measurement_meansbool_col =-1;
    int sterilebool_col =-1;
    bool newinsert=true;

    QStringList slCols = slRegDb.at(1).simplified().split(",");
//    qDebug() << colList;
    for(int iitm=0; iitm<slCols.size(); ++iitm)
    {
        if(QString::compare("product_type_ref",slCols.at(iitm).simplified())==0)
        {prodtypes_refcol = iitm;}
        else if(QString::compare("trade_name_ref",slCols.at(iitm).simplified())==0)
        {tradenames_refcol = iitm;}
        else if(QString::compare("registration_type_ref",slCols.at(iitm).simplified())==0)
        {regtypes_refcol = iitm;}
        else if(QString::compare("manufactorer_ref",slCols.at(iitm).simplified())==0)
        {manufs_refcol = iitm;}
        else if(QString::compare("country_ref",slCols.at(iitm).simplified())==0)
        {countries_refcol = iitm;}
        else if(QString::compare("mp_classification_ref",slCols.at(iitm).simplified())==0)
        {mpclfs_refcol = iitm;}
        else if(QString::compare("inn_ref",slCols.at(iitm).simplified())==0)
        {inns_refcol = iitm;}
        else if(QString::compare("atc_classification_ref",slCols.at(iitm).simplified())==0)
        {atcclf_refcol = iitm;}
        else if(QString::compare("dosage_form_ref",slCols.at(iitm).simplified())==0)
        {dosageforms_refcol = iitm;}
        else if(QString::compare("nd_type_ref",slCols.at(iitm).simplified())==0)
        {ndtypes_refcol = iitm;}
        else if(QString::compare("registry_date_ref",slCols.at(iitm).simplified())==0)
        {regdates_refcol = iitm;}
        else if(QString::compare("risk_class_ref",slCols.at(iitm).simplified())==0)
        {riskclasses_refcol = iitm;}
        else if(QString::compare("reg_date",slCols.at(iitm).simplified())==0)
        {regdate_col = iitm;}
        else if(QString::compare("reg_exp_date",slCols.at(iitm).simplified())==0)
        {regexpdate_col = iitm;}
        else if(QString::compare("gmp_bool_ref",slCols.at(iitm).simplified())==0)
        {gmpbool_col = iitm;}
        else if(QString::compare("generic_bool_ref",slCols.at(iitm).simplified())==0)
        {genericbool_col = iitm;}
        else if(QString::compare("prescription_bool_ref",slCols.at(iitm).simplified())==0)
        {prescriptionbool_col = iitm;}
        else if(QString::compare("ozhvls_bool_ref",slCols.at(iitm).simplified())==0)
        {ozhvlsbool_col = iitm;}
        else if(QString::compare("npp_bool_ref",slCols.at(iitm).simplified())==0)
        {nppbool_col = iitm;}
        else if(QString::compare("trademark_bool_ref",slCols.at(iitm).simplified())==0)
        {trademarkbool_col = iitm;}
        else if(QString::compare("patent_bool_ref",slCols.at(iitm).simplified())==0)
        {patentbool_col = iitm;}
        else if(QString::compare("bulk_bool_ref",slCols.at(iitm).simplified())==0)
        {bulkbool_col = iitm;}
        else if(QString::compare("block_bool_ref",slCols.at(iitm).simplified())==0)
        {blockbool_col = iitm;}
        else if(QString::compare("perpetuity_sign_bool_ref",slCols.at(iitm).simplified())==0)
        {perpetuitysign_col = iitm;}
        else if(QString::compare("mt_bool_ref",slCols.at(iitm).simplified())==0)
        {mtbool_col = iitm;}
        else if(QString::compare("measurement_means_bool_ref",slCols.at(iitm).simplified())==0)
        {measurement_meansbool_col = iitm;}
        else if(QString::compare("sterile_bool_ref",slCols.at(iitm).simplified())==0)
        {sterilebool_col = iitm;}
    }

    for (int irow=0; irow<vslReg.size(); ++irow)
    {
        if (newinsert)
        {
            sQry = QString("INSERT INTO ").append(slRegDb.at(0).simplified()).append(" (").append(slRegDb.at(1).simplified())
                    .append(") VALUES ");
            newinsert=false;
        }

        if (vslReg.at(irow).isEmpty())
        {continue;}

        sQry.append("(");

        for (int jcol=0; jcol<vslReg.at(irow).size(); ++jcol)
        {
            QString sTmp = vslReg.at(irow).at(jcol);
            sTmp.replace("'","''");
            sTmp=sTmp.simplified();
//            if (regVector.at(irow).at(jcol).simplified().isEmpty()) {tmpStr="DEFAULT";}

//            if(QString::compare(regDbList.at(0).simplified(),"registry.medicinal_products")==0)
//            {
//                if()
//                {}
//                if (jcol == 25 && tmpStr.isEmpty())
//                {boolcol = jcol; tmpStr = "FALSE";}
//            }
//            else if (QString::compare(regDbList.at(0).simplified(),"registry.msahc_products")==0)
//            {

//            }

            if (jcol == prodtypes_refcol)
            {
                sQry.append("(SELECT registry.ref_product_types.type_id FROM registry.ref_product_types "
                                      "WHERE registry.ref_product_types.type_alias = '").append(sTmp).append("')");
            }
            else if (jcol == tradenames_refcol)
            {
                sQry.append("(SELECT registry.ref_trade_names.name_id FROM registry.ref_trade_names "
                              "WHERE registry.ref_trade_names.trade_name = '").append(sTmp).append("')");
            }
            else if (jcol == regtypes_refcol)
            {
                sQry.append("(SELECT registry.ref_registration_types.type_id FROM registry.ref_registration_types "
                              "WHERE registry.ref_registration_types.type_name = '").append(sTmp).append("')");
            }
            else if (jcol == manufs_refcol)
            {
                sQry.append("(SELECT registry.ref_manufactorers.manufactorer_id FROM registry.ref_manufactorers "
                              "WHERE registry.ref_manufactorers.manufactorer_name = '").append(sTmp).append("')");
            }
            else if (jcol == countries_refcol)
            {
                sQry.append("(SELECT registry.ref_countries.country_id FROM registry.ref_countries "
                              "WHERE registry.ref_countries.country_name = '").append(sTmp).append("')");
            }
            else if (jcol == mpclfs_refcol)
            {
                sQry.append("(SELECT registry.ref_mp_classifications.classification_id FROM registry.ref_mp_classifications "
                              "WHERE registry.ref_mp_classifications.classification_name = '").append(sTmp).append("')");
            }
            else if (jcol == inns_refcol)
            {
                sQry.append("(SELECT registry.ref_inns.inn_id FROM registry.ref_inns "
                              "WHERE registry.ref_inns.inn = '").append(sTmp).append("')");
            }
            else if (jcol == atcclf_refcol)
            {
                sQry.append("(SELECT registry.ref_atc_classifications.classification_id FROM registry.ref_atc_classifications "
                              "WHERE registry.ref_atc_classifications.classification_name = '").append(sTmp).append("')");
            }
            else if (jcol == dosageforms_refcol)
            {
                sQry.append("(SELECT registry.ref_dosage_forms.form_id FROM registry.ref_dosage_forms "
                              "WHERE registry.ref_dosage_forms.form_name = '").append(sTmp).append("')");
            }
            else if (jcol == ndtypes_refcol)
            {
                sQry.append("(SELECT registry.ref_nd_types.type_id FROM registry.ref_nd_types "
                              "WHERE registry.ref_nd_types.type_name = '").append(sTmp).append("')");
            }
            else if (jcol == regdates_refcol)
            {
                sQry.append("(SELECT registry.ref_registry_dates.date_id FROM registry.ref_registry_dates "
                              "WHERE registry.ref_registry_dates.registry_date = '").append(sTmp).append("')");
            }
            else if (jcol == riskclasses_refcol)
            {
                sQry.append("(SELECT registry.ref_risk_classes.class_id FROM registry.ref_risk_classes "
                              "WHERE registry.ref_risk_classes.class_name = '").append(sTmp).append("')");
            }
            else if (jcol == gmpbool_col || jcol == genericbool_col || jcol == prescriptionbool_col || jcol == ozhvlsbool_col ||
                     jcol == nppbool_col || jcol == trademarkbool_col || jcol == patentbool_col || jcol == bulkbool_col ||
                     jcol == blockbool_col || jcol == mtbool_col || jcol == measurement_meansbool_col || jcol == sterilebool_col ||
                     jcol == perpetuitysign_col)
            {
                if (QString::compare(sTmp,"TRUE",Qt::CaseInsensitive)==0 ||
                        QString::compare(sTmp,"FALSE",Qt::CaseInsensitive)==0)
                {
                    sQry.append("(SELECT registry.ref_bools.bool_id FROM registry.ref_bools "
                              "WHERE registry.ref_bools.bool_value = '").append(sTmp.toUpper()).append("')");
                }
                else
                {sQry.append("DEFAULT");}
            }
            else if (jcol == regdate_col || jcol == regexpdate_col)
            {
                QDate regdate = QDate::fromString(sTmp,"dd.MM.yyyy");
                if (!regdate.isValid())
                {regdate = QDate::fromString(sTmp,"yyyy-MM-dd");}

                if(regdate.isValid())
                {sQry.append("'").append(regdate.toString("yyyy-MM-dd")).append("'");}
                else
                {sQry.append("DEFAULT");}

            }
            else
            {
                if(QString::compare(sTmp,"DEFAULT")!=0)
                {sQry.append("'").append(sTmp).append("'");}
                else
                {sQry.append(sTmp);}
            }

            sQry.append(", ");

            if(jcol==vslReg.at(irow).size()-1)
            {
                sQry.append("(SELECT registry.ref_registry_dates.date_id FROM registry.ref_registry_dates "
                           "WHERE registry.ref_registry_dates.registry_date = '").append(slRegDb.at(2).simplified()).append("')");
                continue;
            }
        }

        sQry.append(")");

        if (irow%100==0)
        {
            newinsert=true;
            this->logger(QString("populating ").append(QString::number(irow)).append(" of ")
                         .append(QString::number(vslReg.size())));
            this->executeQuery(sQry,true);
            continue;
        }

        if(irow<vslReg.size()-1)
        {
            sQry.append(", ");
            continue;
        }
        this->executeQuery(sQry,true);
    }
//    strqry.append(")");
//    qDebug() << strqry;
//    this->executeQuery(sQry);
    this->logger(QString(slRegDb.at(0)).append(" populated"));

//    QPointer<fileWorkout> fw = new fileWorkout;
//    fw->writeFile(strqry,"msahc");
    //    this->logger(QString("Writing finished. File name ").append("msahc"));
}

void populator::populateRetail()
{
    QList<QStringList> lslSqlResult;
    QString sQry;
    sQry = QString("SELECT registry.view_mp.product_id,registry.view_mp.trade_name,registry.view_mp.dosage_form_name,"
                   "registry.view_mp.dosage,registry.view_mp.packing,registry.view_mp.manufactorer_name,"
                   "registry.view_mp.country_name FROM registry.view_mp");
    this->logger("Selecting mps");
    lslSqlResult.append(this->executeQuery(sQry,true));

    sQry = QString("SELECT registry.view_msahcp.product_id,registry.view_msahcp.trade_name,registry.view_msahcp.packing,"
                   "registry.view_msahcp.manufactorer_name,registry.view_msahcp.country_name FROM registry.view_msahcp");
    this->logger("Selecting msahcps");
    lslSqlResult.append(this->executeQuery(sQry,true));

    QString sProductType;
    QList<QStringList> lslRetailInsert;

    for(int irow=0;irow<lslSqlResult.size();++irow)
    {
        for (int iirow=0;iirow<lslSqlResult.at(irow).size();++iirow)
        {lslSqlResult[irow][iirow].replace("'","''");}
//        qDebug() << irow;
        if(lslSqlResult.at(irow).size()==7)
        {
            sProductType = "ЛС";

            QString sDosageName;

            if (lslSqlResult.at(irow).at(2).contains("таблетки", Qt::CaseInsensitive)) //Таблетки
            {sDosageName = "таб.";}
            else if (lslSqlResult.at(irow).at(2).contains("мазь", Qt::CaseInsensitive)) //Мазь
            {sDosageName = "мазь";}
            else if (lslSqlResult.at(irow).at(2).contains("гель", Qt::CaseInsensitive)) //Гель
            {sDosageName = "гель";}
            else if (lslSqlResult.at(irow).at(2).contains("порошок", Qt::CaseInsensitive)) //Порошок
            {sDosageName = "пор.";}
            else if (lslSqlResult.at(irow).at(2).contains("раствор", Qt::CaseInsensitive) &&
                     !lslSqlResult.at(irow).at(2).contains("лиофилизат", Qt::CaseInsensitive) &&
                     !lslSqlResult.at(irow).at(2).contains("микросферы", Qt::CaseInsensitive)) //Раствор
            {sDosageName = "р-р";}
            else if (lslSqlResult.at(irow).at(2).contains("настойка", Qt::CaseInsensitive)) //Настойка
            {sDosageName = "н-ка";}
            else if (lslSqlResult.at(irow).at(2).contains("капли", Qt::CaseInsensitive)) //Капли
            {sDosageName = "кап.";}
            else if (lslSqlResult.at(irow).at(2).contains("сироп", Qt::CaseInsensitive)) //Сироп
            {sDosageName = "сироп";}
            else if (lslSqlResult.at(irow).at(2).contains("капсулы", Qt::CaseInsensitive)) //Капсулы
            {sDosageName = "капс.";}
            else if (lslSqlResult.at(irow).at(2).contains("суспензия", Qt::CaseInsensitive)) //Суспензия
            {sDosageName = "сусп.";}
            else if (lslSqlResult.at(irow).at(2).contains("спрей", Qt::CaseInsensitive)) //Спрей
            {sDosageName = "спрей";}
            else if (lslSqlResult.at(irow).at(2).contains("лиофилизат", Qt::CaseInsensitive)) //Лиофилизат
            {sDosageName = "лиофилизат";}
            else if (lslSqlResult.at(irow).at(2).contains("концентрат", Qt::CaseInsensitive)) //Концентрат
            {sDosageName = "концентрат";}
            else if (lslSqlResult.at(irow).at(2).contains("суппозитории", Qt::CaseInsensitive)) //Суппозитории
            {sDosageName = "супп.";}
            else if (lslSqlResult.at(irow).at(2).contains("пастилки", Qt::CaseInsensitive)) //Пастилки
            {sDosageName = "паст.";}
            else if (lslSqlResult.at(irow).at(2).contains("крем", Qt::CaseInsensitive)) //Крем
            {sDosageName = "крем";}
            else if (lslSqlResult.at(irow).at(2).contains("гранулы", Qt::CaseInsensitive)) //Гранулы
            {sDosageName = "гранулы";}
            else if (lslSqlResult.at(irow).at(2).contains("линимент", Qt::CaseInsensitive)) //Линимент
            {sDosageName = "линимент";}
            else if (lslSqlResult.at(irow).at(2).contains("шампунь", Qt::CaseInsensitive)) //Шампунь
            {sDosageName = "шампунь";}
            else if (lslSqlResult.at(irow).at(2).contains("жидкость", Qt::CaseInsensitive)) //Жидкость
            {sDosageName = "жидкость";}
            else if (lslSqlResult.at(irow).at(2).contains("масло", Qt::CaseInsensitive)) //Масло
            {sDosageName = "масло";}
            else if (lslSqlResult.at(irow).at(2).contains("экстракт", Qt::CaseInsensitive)) //Экстракт
            {sDosageName = "экстракт";}
            else if (lslSqlResult.at(irow).at(2).contains("аэрозоль", Qt::CaseInsensitive)) //Аэрозоль
            {sDosageName = "аэрозоль";}
            else if (lslSqlResult.at(irow).at(2).contains("фито-чай", Qt::CaseInsensitive)) //Фито-чай
            {sDosageName = "фито-чай";}
            else if (lslSqlResult.at(irow).at(2).contains("трава", Qt::CaseInsensitive)) //Трава
            {sDosageName = "трава";}
            else if (lslSqlResult.at(irow).at(2).contains("драже", Qt::CaseInsensitive)) //Драже
            {sDosageName = "драже";}
            else if (lslSqlResult.at(irow).at(2).contains("лак", Qt::CaseInsensitive)) //Лак
            {sDosageName = "лак";}
            else if (lslSqlResult.at(irow).at(2).contains("эмульсия", Qt::CaseInsensitive)) //Эмульсия
            {sDosageName = "эмульсия";}
            else if (lslSqlResult.at(irow).at(2).contains("леденцы", Qt::CaseInsensitive)) //Леденцы
            {sDosageName = "леденцы";}
            else if (lslSqlResult.at(irow).at(2).contains("цветки", Qt::CaseInsensitive)) //Цветки
            {sDosageName = "цветки";}
            else if (lslSqlResult.at(irow).at(2).contains("плоды", Qt::CaseInsensitive)) //Плоды
            {sDosageName = "плоды";}
            else if (lslSqlResult.at(irow).at(2).contains("пластырь", Qt::CaseInsensitive)) //Пластырь
            {sDosageName = "пласт.";}
            else if (lslSqlResult.at(irow).at(2).contains("внутриматочная система", Qt::CaseInsensitive)) //Внутриматочная система
            {sDosageName = "внутримат. сист.";}
            else if (lslSqlResult.at(irow).at(2).contains("эликсир", Qt::CaseInsensitive)) //Эликсир
            {sDosageName = "эликсир";}
            else if (lslSqlResult.at(irow).at(2).contains("микропеллеты", Qt::CaseInsensitive)) //Микропеллеты
            {sDosageName = "микропелл.";}
            else if (lslSqlResult.at(irow).at(2).contains("пессарии вагинальные", Qt::CaseInsensitive)) //Пессарии вагинальные
            {sDosageName = "песс. ваг.";}
            else if (lslSqlResult.at(irow).at(2).contains("трансдермальная", Qt::CaseInsensitive)) //Трансдермальная терапевтическая система (ТТС)
            {sDosageName = "ттс";}
            else if (lslSqlResult.at(irow).at(2).contains("паста", Qt::CaseInsensitive)) //Паста
            {sDosageName = "паста";}
            else if (lslSqlResult.at(irow).at(2).contains("имплантат", Qt::CaseInsensitive) ||
                     lslSqlResult.at(irow).at(2).contains("имплантант", Qt::CaseInsensitive)) //Имплантат
            {sDosageName = "имплантат";}
            else if (lslSqlResult.at(irow).at(2).contains("масса мазеобразная", Qt::CaseInsensitive)) //Масса мазеобразная
            {sDosageName = "масса мазеобр.";}
            else if (lslSqlResult.at(irow).at(2).contains("гранулят", Qt::CaseInsensitive)) //Гранулят
            {sDosageName = "гранулят";}
            else if (lslSqlResult.at(irow).at(2).contains("набор", Qt::CaseInsensitive)) //Набор
            {sDosageName = "набор";}
            else if (lslSqlResult.at(irow).at(2).contains("растительное сырье", Qt::CaseInsensitive)) //растительное сырье
            {sDosageName = "раст. сырье";}
            else if (lslSqlResult.at(irow).at(2).contains("нет данных", Qt::CaseInsensitive)) //Нет данных
            {sDosageName = "";}
            else if (lslSqlResult.at(irow).at(2).contains("желе", Qt::CaseInsensitive)) //Желе
            {sDosageName = "желе";}
            else if (lslSqlResult.at(irow).at(2).contains("бальзам", Qt::CaseInsensitive)) //Бальзам
            {sDosageName = "бальзам";}
            else if (lslSqlResult.at(irow).at(2).contains("резинки", Qt::CaseInsensitive)) //резинки
            {sDosageName = "жев. рез.";}
            else if (lslSqlResult.at(irow).at(2).contains("губка", Qt::CaseInsensitive)) //губка
            {sDosageName = "губка";}
            else if (lslSqlResult.at(irow).at(2).contains("присыпка", Qt::CaseInsensitive)) //Присыпка
            {sDosageName = "присыпка";}
            else if (lslSqlResult.at(irow).at(2).contains("лосьон", Qt::CaseInsensitive)) //Лосьон
            {sDosageName = "лосьон";}
            else if (lslSqlResult.at(irow).at(2).contains("листья", Qt::CaseInsensitive)) //Листья
            {sDosageName = "листья";}
            else if (lslSqlResult.at(irow).at(2).contains("сжатый", Qt::CaseInsensitive)) //Газ сжатый
            {sDosageName = "газ";}
            else if (lslSqlResult.at(irow).at(2).contains("кольца вагинальные", Qt::CaseInsensitive)) //Кольца вагинальные
            {sDosageName = "кольца ваг.";}
            else if (lslSqlResult.at(irow).at(2).contains("столбики", Qt::CaseInsensitive)) //Столбики
            {sDosageName = "столбики";}
            else if (lslSqlResult.at(irow).at(2).contains("корневища", Qt::CaseInsensitive)) //Корневища
            {sDosageName = "корневища";}
            else if (lslSqlResult.at(irow).at(2).contains("почки", Qt::CaseInsensitive)) //Почки
            {sDosageName = "почки";}
            else if (lslSqlResult.at(irow).at(2).contains("микросферы", Qt::CaseInsensitive)) //Микросферы
            {sDosageName = "микросферы";}

            QList<QStringList> lslDosages;
            QList<int> liPackings;

            if (!lslSqlResult.at(irow).at(4).isEmpty())
            {
//              QList<QStringList> lslRow;

                QStringList slRow = lslSqlResult.at(irow).at(4).split(",");

                for (int iitm=0;iitm<slRow.size();++iitm)
                {
                    bool bpacking = false;
                    bool bampule = false;
                    bool bvial = false;
                    QString sTemp = slRow.at(iitm).simplified().remove("'");

                    if (sTemp.contains("картон", Qt::CaseInsensitive) || sTemp.contains("поддон", Qt::CaseInsensitive) ||
                        sTemp.contains("коробка", Qt::CaseInsensitive) || sTemp.contains("пачка", Qt::CaseInsensitive) ||
                        sTemp.contains("ящик", Qt::CaseInsensitive) ||
                                       (sTemp.contains("упаковка", Qt::CaseInsensitive) &&
                                        !(sTemp.contains("контурная", Qt::CaseInsensitive) ||
                                        sTemp.contains("ячейковая", Qt::CaseInsensitive)))) //Картон, поддон, коробка, пачка, ящик
                    {bpacking=true;}
//                    else if (sTemp.contains("полимерным материалом"))
//                    {continue;}
                    else if (sTemp.contains("контурная", Qt::CaseInsensitive) ||
                             sTemp.contains("ячейковая", Qt::CaseInsensitive))
                    {bpacking=true;}
                    else if (sTemp.contains("флакон", Qt::CaseInsensitive)) //Флакон
                    {bvial=true;}
                    else if (sTemp.contains("ампула", Qt::CaseInsensitive)) //Ампула
                    {bampule=true;}
//                    else if (sTemp.contains("туба", Qt::CaseInsensitive)) //Туба
//                    {bunithit=true;}
//                    else if (sTemp.contains("контурная", Qt::CaseInsensitive) ||
//                             sTemp.contains("ячейковая", Qt::CaseInsensitive)) //контурная
//                    {bunithit=true;}
//                    else if (sTemp.contains("карпула", Qt::CaseInsensitive)) //Карпула
//                    {bunithit=true;}
//                    else if (sTemp.contains("контейнер", Qt::CaseInsensitive)) //Контейнер
//                    {bunithit=true;}
//                    else if (sTemp.contains("банка", Qt::CaseInsensitive)) //Банка
//                    {bunithit=true;}
//                    else if (sTemp.contains("бутыл", Qt::CaseInsensitive)) //Бутылка, бутыль
//                    {bunithit=true;}
//                    else if (sTemp.contains("мешок", Qt::CaseInsensitive)) //Мешок
//                    {bunithit=true;}
//                    else if (sTemp.contains("картридж", Qt::CaseInsensitive)) //Картридж
//                    {bunithit=true;}
//                    else if (sTemp.contains("пакет", Qt::CaseInsensitive) &&
//                             !sTemp.contains("фильтр", Qt::CaseInsensitive)) //Пакет
//                    {bunithit=true;}
//                    else if (sTemp.contains("саше", Qt::CaseInsensitive)) //Саше
//                    {bunithit=true;}
//                    else if (sTemp.contains("пенал", Qt::CaseInsensitive)) //Пенал
//                    {bunithit=true;}
//                    else if (sTemp.contains("баллон", Qt::CaseInsensitive)) //Баллон
//                    {bunithit=true;}
//                    else if (sTemp.contains("бочка", Qt::CaseInsensitive)) //Бочка
//                    {bunithit=true;}
//                    else if (sTemp.contains("шприц", Qt::CaseInsensitive)) //Шприц
//                    {bunithit=true;}
//                    else if (sTemp.contains("тюбик", Qt::CaseInsensitive) &&
//                             !sTemp.contains("шприц", Qt::CaseInsensitive)) //Тюбик
//                    {bunithit=true;}
//                    else if (sTemp.contains("фильтр", Qt::CaseInsensitive)) //Фильтр-пакет
//                    {bunithit=true;}
//                    else if (sTemp.contains("фольга", Qt::CaseInsensitive)) //Фольга ламинированная
//                    {bunithit=true;}
//                    else if (sTemp.contains("бумага", Qt::CaseInsensitive)) //Бумага парафинированная
//                    {bunithit=true;}
//                    else if (sTemp.contains("ингалятор", Qt::CaseInsensitive)) //Ингалятор
//                    {bunithit=true;}
//                    else if (sTemp.contains("аппликатор", Qt::CaseInsensitive)) //Аппликатор
//                    {bunithit=true;}
//                    else if (sTemp.contains("стрип", Qt::CaseInsensitive)) //Стрип
//                    {bunithit=true;}
//                    else if (sTemp.contains("стаканчик", Qt::CaseInsensitive)) //Стаканчик полимерный
//                    {bunithit=true;}
//                    else if (sTemp.contains("конверт", Qt::CaseInsensitive)) //Конверт
//                    {bunithit=true;}
//                    else if (sTemp.contains("канистра", Qt::CaseInsensitive)) //Канистра
//                    {bunithit=true;}
//                    else if (sTemp.contains("пластырь", Qt::CaseInsensitive)) //Пластырь
//                    {bunithit=true;}
//                    else if (sTemp.contains("наклейка", Qt::CaseInsensitive)) //Наклейка
//                    {bunithit=true;}
//                    else if (sTemp.contains("стик", Qt::CaseInsensitive)) //Стик
//                    {bunithit=true;}
//                    else if (sTemp.contains("пленка", Qt::CaseInsensitive)) //Плёнка
//                    {bunithit=true;}
//                    else if (sTemp.contains("оберток", Qt::CaseInsensitive)) //Оберток
//                    {bunithit=true;}
//                    else if (sTemp.contains("игла", Qt::CaseInsensitive)) //Игла
//                    {bunithit=true;}
//                    else if (sTemp.contains("этикетка", Qt::CaseInsensitive)) //Этикетка
//                    {bunithit=true;}
//                    else if (sTemp.contains("автоинжектор", Qt::CaseInsensitive)) //Автоинжектор
//                    {bunithit=true;}
//                    else if (sTemp.contains("раскладушка", Qt::CaseInsensitive)) //Книжка-раскладушка
//                    {bunithit=true;}
//                    else if (sTemp.contains("барабан", Qt::CaseInsensitive)) //Барабан картонный
//                    {bunithit=true;}
//                    else if (sTemp.contains("тара", Qt::CaseInsensitive)) //Тара
//                    {bunithit=true;}
//                    else if (sTemp.contains("фляга", Qt::CaseInsensitive)) //Фляга
//                    {bunithit=true;}

                    if (bpacking && !sTemp.contains("№")) {sTemp.append(" №1");}
                    QStringList slDos;
                    int numb=1;

                    QStringList slTmp = sTemp.split(" ");
                    QString sDosCurr;
                    for (int ititm=0;ititm<slTmp.size();++ititm)
                    {
                        if (slTmp.at(ititm).isEmpty())
                        {continue;}

                        if (slTmp.at(ititm).at(0).isDigit())
                        {sDosCurr.append(slTmp.at(ititm));continue;}

                        if (slTmp.at(ititm).contains("/"))
                        {sDosCurr.append(" ").append(slTmp.at(ititm));continue;}

                        if (slTmp.at(ititm).contains("мл") || slTmp.at(ititm).contains("мг") ||
                            slTmp.at(ititm).contains("кг") || QString::compare(slTmp.at(ititm),"г")==0 ||
                            QString::compare(slTmp.at(ititm),"%")==0)
                        {sDosCurr.append(slTmp.at(ititm));}

                        if (QString::compare(slTmp.at(ititm).at(0),"№")==0)
                        {
                            QString sCurr = slTmp.at(ititm);
                            sCurr.remove(",");

                            if(bpacking)
                            {
                                sCurr.remove("№");
                                numb = sCurr.toInt();
                                liPackings.append(numb);
                            }
                            else
                            {sDosCurr.append(" ").append(sCurr);}

                            if (numb>1){bpacking=true;bampule=false;bvial=false;}
                        }
                    }

                    QString sUnitCurr;

                    if (bpacking)
                    {sUnitCurr = "уп";}
                    else if (bampule)
                    {sUnitCurr = "амп";}
                    else if (bvial)
                    {sUnitCurr = "фл";}
                    else
                    {sUnitCurr = "шт";}

                    if (!sDosCurr.isEmpty() && !sUnitCurr.isEmpty())
                    {slDos.append(sDosCurr.simplified());slDos.append(sUnitCurr);lslDosages.append(slDos);}
                }
            }
            else
            {lslDosages.append(QStringList() << "" << "н/д");}

            QList<QStringList> lslCurrent;
            QStringList slCurrent;
            int iirow=0;

            QString sTradeName;
            sTradeName.append(QString(lslSqlResult.at(irow).at(1)).remove("®")).append(" ")
                              .append(lslSqlResult.at(irow).at(3));

            for(;iirow<lslDosages.size();++iirow)
            {
                int iinumb = 1;
//                qDebug() << iirow << lslDosages.size() << lslDosages.at(iirow);

                QString sn = lslDosages.at(iirow).at(0);
                QString su = lslDosages.at(iirow).at(1);

                if (sn.contains("№"))
                {
                    if (QString::compare(sn.at(0),"№")==0)
                    {
                        if (iirow<liPackings.size())
                        {
                            sn.remove("№");
                            iinumb=liPackings.at(iirow)*sn.toInt();
                            sn.clear();
                            sn.append(" №").append(QString::number(iinumb));
                        }
                    }
                    else
                    {lslDosages.removeAt(iirow); --iirow;}
                }
                else
                {
                    if (iirow<liPackings.size())
                    {
                        iinumb=liPackings.at(iirow);
                        sn.append(" №").append(QString::number(iinumb));
                    }
                }

//                qDebug() << "here" << lslDosages << irow << lslDosages.size();
                slCurrent.clear();
                slCurrent.append(lslSqlResult.at(irow).at(0));
                slCurrent.append(sProductType.replace("'","''"));
                slCurrent.append(QString(sTradeName).simplified().append(" ").append(sn).replace("'","''"));
                slCurrent.append(lslSqlResult.at(irow).at(5));
                slCurrent.append(lslSqlResult.at(irow).at(6));
                slCurrent.append(su.replace("'","''"));
                lslCurrent.append(slCurrent);
            }

//            qDebug() << irow << iirow << lslDosages.size() << liPackings.size();

            if (iirow==lslDosages.size()-1 || lslDosages.size()==0)
            {
                for (;iirow<liPackings.size();++iirow)
                {
                    slCurrent.clear();
                    slCurrent.append(lslSqlResult.at(irow).at(0));
                    slCurrent.append(sProductType.replace("'","''"));
                    slCurrent.append(QString(sTradeName).simplified().append(" №").append(QString::number(liPackings.at(iirow))).replace("'","''"));
                    slCurrent.append(lslSqlResult.at(irow).at(5));
                    slCurrent.append(lslSqlResult.at(irow).at(6));
                    slCurrent.append("уп");
                    lslCurrent.append(slCurrent);
                }
            }

            lslRetailInsert.append(lslCurrent);

//            if(irow==25) {qDebug() << lslRetailInsert; return;}
        }

        else if(lslSqlResult.at(irow).size()==5)
        {
            sProductType = "ИМН";

//            qDebug() << lslSqlResult.at(irow);

            QList<QStringList> lslDosages;
            QList<int> liPackings;

            if (!lslSqlResult.at(irow).at(2).isEmpty())
            {
                QStringList slRow = lslSqlResult.at(irow).at(2).split(",");
                bool bpacking = false;
                bool bnd = false;

                for (int iitm=0;iitm<slRow.size();++iitm)
                {
                    QString sTemp = slRow.at(iitm).simplified().remove("'");

                    if (sTemp.contains("картон", Qt::CaseInsensitive) || sTemp.contains("поддон", Qt::CaseInsensitive) ||
                        sTemp.contains("коробка", Qt::CaseInsensitive) || sTemp.contains("пачка", Qt::CaseInsensitive) ||
                        sTemp.contains("ящик", Qt::CaseInsensitive) ||
                                       (sTemp.contains("упаковка", Qt::CaseInsensitive) &&
                                        !(sTemp.contains("контурная", Qt::CaseInsensitive) ||
                                        sTemp.contains("ячейковая", Qt::CaseInsensitive)))) //Картон, поддон, коробка, пачка, ящик
                    {bpacking=true;}
                    else if (sTemp.contains("нет данных", Qt::CaseInsensitive)) //Нет данных
                    {bnd=true;}

                    if (bpacking && !sTemp.contains("№")) {sTemp.append(" №1");}
                    QStringList slDos;
                    int numb=1;

                    QStringList slTmp = sTemp.split(" ");
                    QString sDosCurr;
                    for (int ititm=0;ititm<slTmp.size();++ititm)
                    {
                        if (slTmp.at(ititm).isEmpty())
                        {continue;}

                        if (slTmp.at(ititm).at(0).isDigit())
                        {sDosCurr.append(slTmp.at(ititm));continue;}

                        if (slTmp.at(ititm).contains("/"))
                        {sDosCurr.append(slTmp.at(ititm));continue;}

                        if (slTmp.at(ititm).contains("шт") || slTmp.at(ititm).contains("уп") || slTmp.at(ititm).contains("мл") ||
                            slTmp.at(ititm).contains("фл") || slTmp.at(ititm).contains("мг") ||
                            slTmp.at(ititm).contains("кг") || QString::compare(slTmp.at(ititm),"г")==0 ||
                            QString::compare(slTmp.at(ititm),"%")==0)
                        {sDosCurr.append(slTmp.at(ititm));}

                        if (QString::compare(slTmp.at(ititm).at(0),"№")==0)
                        {
                            QString sCurr = slTmp.at(ititm);
                            sCurr.remove(",");

                            if(bpacking)
                            {
                                sCurr.remove("№");
                                numb = sCurr.toInt();
                                liPackings.append(numb);
                            }
                            else
                            {sDosCurr.append(" ").append(sCurr);}

                            if (numb>1){bpacking=true;bnd=false;}
                        }
                    }

                    QString sUnitCurr;

                    if (bpacking)
                    {sUnitCurr = "уп";}
                    else if (bnd)
                    {sUnitCurr = "н/д";}
                    else
                    {sUnitCurr = "шт";}

                    if (!slDos.isEmpty() && !sUnitCurr.isEmpty())
                    {slDos.append(sDosCurr.simplified());slDos.append(sUnitCurr);lslDosages.append(slDos);}
                }
            }
            else
            {lslDosages.append(QStringList() << "" << "н/д");}

//            qDebug() << lslDosages << liPackings;

            QList<QStringList> lslCurrent;
            QStringList slCurrent;
            int iirow=0;

            QString sTradeName;
            sTradeName.append(QString(lslSqlResult.at(irow).at(1)).remove("®")).append(" ");

            for(;iirow<lslDosages.size();++iirow)
            {
                int iinumb = 1;
//                qDebug() << iirow << lslDosages.size() << lslDosages.at(iirow);

                QString sn = lslDosages.at(iirow).at(0);
                QString su = lslDosages.at(iirow).at(1);

                if (sn.contains("№"))
                {
                    if (QString::compare(sn.at(0),"№")==0)
                    {
                        if (iirow<liPackings.size())
                        {
                            sn.remove("№");
                            iinumb=liPackings.at(iirow)*sn.toInt();
                            sn.clear();
                            sn.append(" №").append(QString::number(iinumb));
                        }
                    }
                    else
                    {lslDosages.removeAt(iirow); --iirow;}
                }
                else
                {
                    if (iirow<liPackings.size())
                    {
                        iinumb=liPackings.at(iirow);
                        sn.append(" №").append(QString::number(iinumb));
                    }
                }

//                qDebug() << "here" << lslDosages << irow << lslDosages.size();
                slCurrent.clear();
                slCurrent.append(lslSqlResult.at(irow).at(0));
                slCurrent.append(sProductType.replace("'","''"));
                slCurrent.append(QString(sTradeName).simplified().append(" ").append(sn).replace("'","''"));
                slCurrent.append(lslSqlResult.at(irow).at(3));
                slCurrent.append(lslSqlResult.at(irow).at(4));
                slCurrent.append(su);
                lslCurrent.append(slCurrent);
            }

//            qDebug() << irow << iirow << lslDosages.size() << liPackings.size();

            if (iirow==lslDosages.size()-1 || lslDosages.size()==0)
            {
                for (;iirow<liPackings.size();++iirow)
                {
                    slCurrent.clear();
                    slCurrent.append(lslSqlResult.at(irow).at(0));
                    slCurrent.append(sProductType.replace("'","''"));
                    slCurrent.append(QString(sTradeName).simplified().append(" №").append(QString::number(liPackings.at(iirow))).replace("'","''"));
                    slCurrent.append(lslSqlResult.at(irow).at(3));
                    slCurrent.append(lslSqlResult.at(irow).at(4));
                    slCurrent.append("уп");
                    lslCurrent.append(slCurrent);
                }
            }

            lslRetailInsert.append(lslCurrent);
        }
        else
        {this->logger(QString("query size fayul at").append(QString::number(irow)));return;}
    }

    bool newinsert=true;
//    if (newinsert)
//    {
//        qDebug() << lslRetailInsert.size();
//        qDebug() << lslRetailInsert.at(20794);
//        for (int jrow=0;jrow<lslRetailInsert.size();++jrow)
//        {
//            if (QString::compare(lslRetailInsert.at(jrow).at(0),"20794")==0)
//            {qDebug() << lslRetailInsert.at(jrow);return;}
//        }
//        return;
//    }

    for (int irow=0; irow<lslRetailInsert.size(); ++irow)
    {
        if (newinsert)
        {
            sQry = QString("INSERT INTO retail.products (registry_ref, product_type_ref, product_name, "
                           "manufactorer_name_ref, country_ref, unit_ref)  VALUES ");
            newinsert=false;
        }

        if (lslRetailInsert.at(irow).isEmpty())
        {continue;}
        if (lslRetailInsert.at(irow).size() != 6)
        {this->logger(QString("insert size fayul at").append(QString::number(irow)));return;}

        sQry.append("('").append(lslRetailInsert.at(irow).at(0)).append("',")
                         .append("(SELECT registry.ref_product_types.type_id FROM registry.ref_product_types "
                                 "WHERE registry.ref_product_types.type_alias = '").append(lslRetailInsert.at(irow).at(1))
                         .append("'),'").append(lslRetailInsert.at(irow).at(2)).append("',")
                         .append("(SELECT registry.ref_manufactorers.manufactorer_id FROM registry.ref_manufactorers "
                                 "WHERE registry.ref_manufactorers.manufactorer_name = '").append(lslRetailInsert.at(irow).at(3))
                         .append("'),").append("(SELECT registry.ref_countries.country_id FROM registry.ref_countries "
                                 "WHERE registry.ref_countries.country_name = '").append(lslRetailInsert.at(irow).at(4))
                         .append("'),").append("(SELECT retail.units.unit_id FROM retail.units "
                                 "WHERE retail.units.unit_name = '").append(lslRetailInsert.at(irow).at(5)).append("')");

        sQry.append(")");

        if (irow%100==0)
        {
            newinsert=true;
            this->logger(QString("populating ").append(QString::number(irow)).append(" of ")
                         .append(QString::number(lslRetailInsert.size())));
            this->executeQuery(sQry,true);
            continue;
        }

        if(irow<lslRetailInsert.size()-1)
        {
            sQry.append(", ");
            continue;
        }
        if (this->executeQuery(sQry,true).isEmpty()) {return;}
    }

//    qDebug() << "Остальные упаковки\n" << slRest << "\n" << "Остальные формы дозировок\n" << slUnitsRest;
}

QList<QStringList> populator::executeQuery(const QString &sQry, bool bPrepare)
{
//    this->logger("Starting write");

//    QPointer<fileWorkout> fw = new fileWorkout;
//    fw->writeFile(strqry,"qryfile");
//    this->logger(QString("Writing finished"));
//    fw->deleteLater();
//    if (!strqry.isEmpty()) {return QByteArray();}

    QList<QStringList> lslResult;
    QSqlQuery *qry = new QSqlQuery(*db);

    if (bPrepare)
    {if(!qry->prepare(sQry)) {this->logger(qry->lastError().text()); return lslResult;}}

    if(!qry->exec(sQry))
    {this->logger(qry->lastError().text()); return lslResult;}

    while (qry->next())
    {
        QStringList slSqlOutput;
        for (int jCol = 0; jCol < qry->record().count(); ++jCol)
        {slSqlOutput.append(qry->value(jCol).toString());}
        lslResult.append(slSqlOutput);
    }

    delete qry;

    this->logger("query done");
    return lslResult;
}

void populator::logger(const QString &sLog)
{
    fprintf(stdout, "%s\n", sLog.toStdString().c_str());
    fflush(stdout);
}
