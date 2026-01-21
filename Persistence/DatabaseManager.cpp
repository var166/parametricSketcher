#include "DatabaseManager.h"
#include <QVariant>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::init(const QString& dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed";
        return false;
    }

    QSqlQuery query;
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS sketches ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "name TEXT UNIQUE, "
                         "category TEXT)");
    if (!ok) return false;

    ok = query.exec("CREATE TABLE IF NOT EXISTS entities ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "sketch_id INTEGER, "
                    "type TEXT, "
                    "x1 REAL, y1 REAL, "
                    "x2 REAL, y2 REAL, "
                    "radius REAL, "
                    "FOREIGN KEY(sketch_id) REFERENCES sketches(id))");
    if (!ok) return false;

    ok = query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT UNIQUE, "
                    "password TEXT, "
                    "role TEXT)");
    if (!ok) return false;

    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
        
        QStringList roles = {"Student", "Administrator", "Library Manager", "CAD Designer"};
        for (const QString& role : roles) {
            query.addBindValue(role.toLower().replace(" ", ""));
            query.addBindValue("password"); // default password
            query.addBindValue(role);
            query.exec();
        }
    }

    return ok;
}

bool DatabaseManager::saveSketch(const std::shared_ptr<Sketch>& sketch, const QString& name, const QString& category) {
    QSqlQuery query;
    m_db.transaction();

    query.prepare("INSERT OR REPLACE INTO sketches (name, category) VALUES (:name, :category)");
    query.bindValue(":name", name);
    query.bindValue(":category", category);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    int sketchId = query.lastInsertId().toInt();
    if (sketchId == 0) {
        query.prepare("SELECT id FROM sketches WHERE name = :name");
        query.bindValue(":name", name);
        if (query.exec() && query.next()) {
            sketchId = query.value(0).toInt();
        }
    }

    // Delete existing entities for this sketch if it was a replace
    query.prepare("DELETE FROM entities WHERE sketch_id = :sketch_id");
    query.bindValue(":sketch_id", sketchId);
    query.exec();

    for (const auto& entity : sketch->getEntities()) {
        query.prepare("INSERT INTO entities (sketch_id, type, x1, y1, x2, y2, radius) "
                      "VALUES (:sketch_id, :type, :x1, :y1, :x2, :y2, :radius)");
        query.bindValue(":sketch_id", sketchId);
        
        if (entity->getType() == EntityType::Point) {
            auto p = std::dynamic_pointer_cast<Point>(entity);
            query.bindValue(":type", "POINT");
            query.bindValue(":x1", p->x());
            query.bindValue(":y1", p->y());
            query.bindValue(":x2", QVariant());
            query.bindValue(":y2", QVariant());
            query.bindValue(":radius", QVariant());
        } else if (entity->getType() == EntityType::Line) {
            auto l = std::dynamic_pointer_cast<Line>(entity);
            query.bindValue(":type", "LINE");
            query.bindValue(":x1", l->start()->x());
            query.bindValue(":y1", l->start()->y());
            query.bindValue(":x2", l->end()->x());
            query.bindValue(":y2", l->end()->y());
            query.bindValue(":radius", QVariant());
        } else if (entity->getType() == EntityType::Circle) {
            auto c = std::dynamic_pointer_cast<Circle>(entity);
            query.bindValue(":type", "CIRCLE");
            query.bindValue(":x1", c->center()->x());
            query.bindValue(":y1", c->center()->y());
            query.bindValue(":x2", QVariant());
            query.bindValue(":y2", QVariant());
            query.bindValue(":radius", c->radius());
        }
        
        if (!query.exec()) {
            m_db.rollback();
            return false;
        }
    }

    return m_db.commit();
}

std::shared_ptr<Sketch> DatabaseManager::loadSketch(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT id FROM sketches WHERE name = :name");
    query.bindValue(":name", name);
    if (!query.exec() || !query.next()) return nullptr;

    int sketchId = query.value(0).toInt();
    auto sketch = std::make_shared<Sketch>();

    query.prepare("SELECT type, x1, y1, x2, y2, radius FROM entities WHERE sketch_id = :sketch_id");
    query.bindValue(":sketch_id", sketchId);
    if (!query.exec()) return nullptr;

    while (query.next()) {
        QString type = query.value(0).toString();
        double x1 = query.value(1).toDouble();
        double y1 = query.value(2).toDouble();
        
        if (type == "POINT") {
            sketch->addEntity(std::make_shared<Point>(x1, y1));
        } else if (type == "LINE") {
            double x2 = query.value(3).toDouble();
            double y2 = query.value(4).toDouble();
            auto start = std::make_shared<Point>(x1, y1);
            auto end = std::make_shared<Point>(x2, y2);
            sketch->addEntity(std::make_shared<Line>(start, end));
        } else if (type == "CIRCLE") {
            double radius = query.value(5).toDouble();
            auto center = std::make_shared<Point>(x1, y1);
            sketch->addEntity(std::make_shared<Circle>(center, radius));
        }
    }

    return sketch;
}

QStringList DatabaseManager::listSketches(const QString& category) {
    QStringList list;
    QSqlQuery query;
    if (category.isEmpty()) {
        query.prepare("SELECT name FROM sketches");
    } else {
        query.prepare("SELECT name FROM sketches WHERE category = :category");
        query.bindValue(":category", category);
    }
    
    if (query.exec()) {
        while (query.next()) {
            list << query.value(0).toString();
        }
    }
    return list;
}

QStringList DatabaseManager::listCategories() {
    QStringList list;
    QSqlQuery query("SELECT DISTINCT category FROM sketches WHERE category IS NOT NULL AND category != ''");
    while (query.next()) {
        list << query.value(0).toString();
    }
    return list;
}

bool DatabaseManager::checkLogin(const QString& username, const QString& password, QString& role) {
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    
    if (query.exec() && query.next()) {
        role = query.value(0).toString();
        return true;
    }
    return false;
}
