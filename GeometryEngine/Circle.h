#ifndef CIRCLE_H
#define CIRCLE_H

#include "GeometricEntity.h"
#include "Point.h"
#include <memory>
#include <QPainter>
#include <cmath>

class Circle : public GeometricEntity {
private:
    std::shared_ptr<Point> m_center;
    double m_radius;

public:
    Circle(std::shared_ptr<Point> center, double radius)
        : m_center(center), m_radius(radius) {}

    void draw(QPainter& painter) const override {
        if (m_center) {
            painter.setPen(QPen(m_selected ? Qt::cyan : m_color, 2 * m_thickness));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(QPointF(m_center->x(), m_center->y()), m_radius, m_radius);
        }
    }

    bool contains(const QPointF& point, double tolerance) const override {
        if (!m_center) return false;
        double dx = m_center->x() - point.x();
        double dy = m_center->y() - point.y();
        double dist = std::sqrt(dx*dx + dy*dy);
        return std::abs(dist - m_radius) <= tolerance;
    }

    std::vector<double*> getParameters() override {
        std::vector<double*> params;
        if (m_center) {
            auto p = m_center->getParameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        params.push_back(&m_radius);
        return params;
    }

    EntityType getType() const override { return EntityType::Circle; }

    std::shared_ptr<Point> center() const { return m_center; }
    double radius() const { return m_radius; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Circle";
        if (m_center) json["center"] = m_center->toJson();
        json["radius"] = m_radius;
        json["color"] = m_color.name();
        json["thickness"] = m_thickness;
        return json;
    }

    void fromJson(const QJsonObject& json) override {
        if (json.contains("center")) {
            m_center = std::make_shared<Point>(0, 0);
            m_center->fromJson(json["center"].toObject());
        }
        m_radius = json["radius"].toDouble();
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif // CIRCLE_H