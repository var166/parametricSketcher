#ifndef SKETCH_H
#define SKETCH_H

#include <vector>
#include <memory>
#include <QPainter> 
#include "GeometricEntity.h"

class Sketch {
private:
    std::vector<std::shared_ptr<GeometricEntity>> m_entities;

public:
    Sketch();
    ~Sketch();

    void addEntity(std::shared_ptr<GeometricEntity> entity);
    
    void draw(QPainter& painter) const;

    const std::vector<std::shared_ptr<GeometricEntity>>& getEntities() const;
    void update();
};

#endif