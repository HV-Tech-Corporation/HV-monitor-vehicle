#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class DataStorage : public QObject
{
    Q_OBJECT
public:
    explicit DataStorage(QObject *parent = nullptr);

    bool initDatabase(const QString &dbPath);
    bool saveRecord(const QString &timestamp, const QString &imageName, int costs, const QString &riskLevel);

private:
    QSqlDatabase db;
};

#endif // DATASTORAGE_H
