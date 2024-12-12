#include "datastorage.h"

DataStorage::DataStorage(QObject *parent) : QObject(parent)
{
}

bool DataStorage::initDatabase(const QString &dbPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            imageName TEXT NOT NULL,
            costs REAL NOT NULL,
            riskLevel TEXT NOT NULL
        )
    )";

    if (!query.exec(createTableSQL)) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database initialized successfully at" << dbPath;
    return true;
}

bool DataStorage::saveRecord(const QString &timestamp, const QString &imageName, int costs, const QString &riskLevel)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Records (timestamp, imageName, costs, riskLevel) "
                  "VALUES (:timestamp, :imageName, :costs, :riskLevel)");
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":imageName", imageName);
    query.bindValue(":costs", costs);
    query.bindValue(":riskLevel", riskLevel);

    if (!query.exec()) {
        qWarning() << "Failed to insert record:" << query.lastError().text();
        return false;
    }

    qDebug() << "Record saved: Timestamp =" << timestamp
             << ", Image =" << imageName
             << ", Costs =" << costs
             << ", Risk Level =" << riskLevel;

    return true;
}
