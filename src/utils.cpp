#include "utils.h"

#include <QFile>
#include <QDebug>

QByteArray readFile(const QString &path)
{
    QByteArray ret;

    QFile fi(path);
    if (!fi.open(QIODevice::ReadOnly)) {
        qDebug() << "read file failed. error=" << fi.errorString();
        return ret;
    }

    ret = fi.readAll();
    fi.close();

    return ret;
}
