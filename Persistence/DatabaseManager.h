#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <memory>
#include <vector>
#include "../GeometryEngine/Sketch.h"
#include "../GeometryEngine/GeometricEntity.h"
#include "../GeometryEngine/Point.h"
#include "../GeometryEngine/Line.h"
#include "../GeometryEngine/Circle.h"

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool init(const QString& dbName = "sketcher.db");
    
    bool saveSketch(const std::shared_ptr<Sketch>& sketch, const QString& name, const QString& category);
    std::shared_ptr<Sketch> loadSketch(const QString& name);
    QStringList listSketches(const QString& category = QString());
    QStringList listCategories();

    bool checkLogin(const QString& username, const QString& password, QString& role);

private:
    DatabaseManager() = default;
    QSqlDatabase m_db;
};

#endif
