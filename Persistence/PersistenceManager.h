#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include <QString>
#include <memory>
#include "../GeometryEngine/Sketch.h"

class PersistenceManager {
public:
    static bool saveSketch(const std::shared_ptr<Sketch>& sketch, const QString& filePath);
    static std::shared_ptr<Sketch> loadSketch(const QString& filePath);
};

#endif
