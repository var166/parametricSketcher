#ifndef GEOMETRICENTITYFACTORY_H
#define GEOMETRICENTITYFACTORY_H

#include <memory>
#include <string>
#include "GeometricEntity.h"

class GeometricEntityFactory {
public:
    static std::shared_ptr<GeometricEntity> createEntity(const std::string& type);
};

#endif
