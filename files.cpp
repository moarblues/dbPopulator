#include "files.h"

files::files(QObject *parent) : QObject(parent)
{
    settingsFileName = QDir::currentPath().append("/settings.conf");
}

files::~files()
{

}

QStringList files::readSettings()
{
    QStringList outList;
    QFile fSettings(settingsFileName);
    if (fSettings.open(QFile::ReadOnly))
    {
//        const QString &errWS("Путь каталога или значение не должны содержать символа ");
        QString fline;

        QTextStream in(&fSettings);
        in.setCodec("UTF-8");
        do
        {
            fline = in.readLine();

            if (fline.isEmpty() || QString::compare(fline.at(0),"#") == 0)
            {continue;}

            outList.append(fline);
            continue;
        } while (!fline.isNull());
    }
    return outList;
}

QStringList files::readConnectionParameters(const QString &sCpfn)
{
    QStringList outList;
    QFile fcp(sCpfn);
    if (fcp.open(QFile::ReadOnly))
    {
        QTextStream in(&fcp);
        in.setCodec("UTF-8");

        outList = in.readLine().split(",");
    }
    return outList;
}

QString files::readTextQuery(const QString &sPath)
{
    QString sOutput;
    QFile fcp(sPath);
    if (fcp.open(QFile::ReadOnly))
    {
        QTextStream in(&fcp);
        in.setCodec("UTF-8");

        QString sLine;

        do
        {
            sLine = in.readLine();
            QStringList slTmp = sLine.split(" ");

            for (int i=0;i<slTmp.size();++i)
            {
                if (QString::compare(slTmp.at(i),"--")==0)
                {sOutput.append(" "); break;}
                sOutput.append(slTmp.at(i)).append(" ");
            }
        }
        while (!sLine.isNull());
    }
    return sOutput;
}

void files::writeFile(const QString &sOut, const QString &sFname)
{
    QFile *outfile = new QFile(this);
    QTextStream out(outfile);

    outfile->setFileName(sFname);
    out.setCodec("UTF-8");
    if (outfile->open(QFile::WriteOnly))
    {out << sOut;}
    outfile->flush();
    outfile->close();
    delete outfile;
}

QList<QVector<QStringList> > files::getXlsData(const QString &sFpath)
{
    unsigned int worksheet_index;
    unsigned int max_worksheet;
    unsigned int info;
    int ret;
    const void *handle;
    unsigned int rows;
    unsigned short columns;
    unsigned int row;
    unsigned short col;

    QList<QVector<QStringList> > lvslOutput;

    // opening the .XLS file [Workbook]
    ret = freexl_open (sFpath.toStdString().c_str(), &handle);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(QString("Ошибка открытия: ").append(QString::number(ret)));
        return lvslOutput;
    }

    // checking for Password (obfuscated/encrypted)
    ret = freexl_get_info (handle, FREEXL_BIFF_PASSWORD, &info);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(QString("Ошибка получения информации (необходим пароль) ")
                .append(QString::number(FREEXL_BIFF_PASSWORD)).append(": ").append(QString::number(ret)));
        return lvslOutput;
    }
    switch (info)
    {
        case FREEXL_BIFF_PLAIN:
            break;
        case FREEXL_BIFF_OBFUSCATED:
        default:
            emit this->setStatus(QString("Защищено паролем, нет доступа"));
            return lvslOutput;
    };

    // querying BIFF Worksheet entries
    ret = freexl_get_info (handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(QString("Ошибка получения информации (ошибка чтения листов)")
               .append(FREEXL_BIFF_SHEET_COUNT).append(": ").append(QString::number(ret)));
        return lvslOutput;
    }

    for (worksheet_index = 0; worksheet_index < max_worksheet; ++worksheet_index)
    {
        QVector<QStringList> vslOut;

        const char *utf8_worsheet_name;
        ret = freexl_get_worksheet_name (handle, worksheet_index, &utf8_worsheet_name);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(QString("Ошибка чтения листов: ")
                                 .append(QString::number(ret)));
            return lvslOutput;
         }
    // selecting the active Worksheet
        ret = freexl_select_active_worksheet (handle, worksheet_index);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(QString("Ошибка выбора текущего листа: ").append(QString::number(ret)));
            return lvslOutput;
        }
    // dimensions
        ret = freexl_worksheet_dimensions (handle, &rows, &columns);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(QString("Ошибка структуры листов: ")
                                 .append(QString::number(ret)));
            return lvslOutput;
        }

        vslOut.append(QStringList() << QString(utf8_worsheet_name));

        for (row = 0; row < rows; row++)
        {
            QStringList slRecord;
            FreeXL_CellValue cell;
            for (col = 0; col < columns; ++col)
            {
                QString sCell;
                ret = freexl_get_cell_value (handle, row, col, &cell);
                if (ret != FREEXL_OK)
                {
                    emit this->setStatus(QString("Ошибка значения ячейки (строка=")
                                         .append(row).append(" столбец=").append(col).append("): ")
                                         .append(QString::number(ret)));
                    return lvslOutput;
                }
                switch (cell.type)
                {
                case FREEXL_CELL_INT:
                    sCell = QString::number(cell.value.int_value);
                    break;
                case FREEXL_CELL_DOUBLE:
                    sCell = QString::number(cell.value.double_value);
                    break;
                case FREEXL_CELL_TEXT:
                case FREEXL_CELL_SST_TEXT:
                    sCell = cell.value.text_value;
                    break;
                case FREEXL_CELL_DATE:
                case FREEXL_CELL_DATETIME:
                case FREEXL_CELL_TIME:
                    sCell = cell.value.text_value;
                    break;
                case FREEXL_CELL_NULL:
                    sCell = "";
                default:
                    break;
                };
                slRecord.append(sCell.simplified());
            }
//            if (recordRow.at(0).isEmpty()) {continue;}
            vslOut.append(slRecord);
        }
        lvslOutput.append(vslOut);
    }
    ret = freexl_close(handle);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(QString("Ошибка закрытия: ").append(QString::number(ret)));
        return lvslOutput;
    }
    return lvslOutput;
}

QList<QVector<QStringList> > files::getXlsxData(const QString &sFpath)
{
    QList<QVector<QStringList> > lvslOutput;

    QXlsx::Document xlsxentry(sFpath);
    QStringList snList= xlsxentry.sheetNames();

//    QString errStr;

    for (int listIdx=0; listIdx<snList.size(); ++listIdx)
    {
        QVector<QStringList> vslOut;
        vslOut.append(QStringList() << snList.at(listIdx));
        xlsxentry.selectSheet(snList.at(listIdx));
        qDebug() << snList.at(listIdx) << xlsxentry.dimension().rowCount() << xlsxentry.dimension().columnCount();
        qDebug() << xlsxentry.read(1,1).toString().simplified();
        for (int irow=0; irow<=xlsxentry.dimension().rowCount(); ++irow)
        {
            QStringList slRecord;

            for(int jcol=0; jcol<=xlsxentry.dimension().columnCount(); ++jcol)
            {slRecord.append(xlsxentry.read(irow,jcol).toString().simplified());}

    //        if (recordRow.at(0).isEmpty()) {continue;}
            if (irow==3) {qDebug() << slRecord << slRecord.size();}
            vslOut.append(slRecord);
            continue;
        }
        lvslOutput.append(vslOut);
    }
    return lvslOutput;
}
