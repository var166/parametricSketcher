#include "PersistenceManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "../GeometryEngine/GeometricEntityFactory.h"

bool PersistenceManager::saveSketch(const std::shared_ptr<Sketch>& sketch, const QString& filePath) {
    QJsonObject root;
    QJsonArray entities;
    for (const auto& entity : sketch->getEntities()) {
        entities.append(entity->toJson());
    }
    root["entities"] = entities;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return false;

    file.write(QJsonDocument(root).toJson());
    return true;
}

std::shared_ptr<Sketch> PersistenceManager::loadSketch(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();
    QJsonArray entities = root["entities"].toArray();

    auto sketch = std::make_shared<Sketch>();
    for (int i = 0; i < entities.size(); ++i) {
        QJsonObject obj = entities[i].toObject();
        QString type = obj["type"].toString();
        
        std::shared_ptr<GeometricEntity> entity = GeometricEntityFactory::createEntity(type.toStdString());

        if (entity) {
            entity->fromJson(obj);
            sketch->addEntity(entity);
        }
    }
    return sketch;
}
