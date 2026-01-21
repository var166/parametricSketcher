#include "Sketch.h"

Sketch::Sketch() {}
Sketch::~Sketch() = default;

void Sketch::addEntity(std::shared_ptr<GeometricEntity> entity) {
    if (entity) {
        m_entities.push_back(entity);
    }
}

// Implementation with QPainter&
void Sketch::draw(QPainter& painter) const {
    for (const auto& entity : m_entities) {
        if (entity) {
            entity->draw(painter);
        }
    }
}

const std::vector<std::shared_ptr<GeometricEntity>>& Sketch::getEntities() const {
    return m_entities;
}

void Sketch::update() {
    // Solver logic to be added later
}