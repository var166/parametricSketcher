#ifndef REGULARPOLYGON_H
#define REGULARPOLYGON_H

#include "GeometricEntity.h"
#include "Point.h"
#include <memory>
#include <QPainter>
#include <cmath>

class RegularPolygon : public GeometricEntity {
private:
    std::shared_ptr<Point> m_center;
    double m_radius;
    int m_sides;
    double m_rotation; // in radians

public:
    RegularPolygon(std::shared_ptr<Point> center, double radius, int sides, double rotation = 0)
        : m_center(center), m_radius(radius), m_sides(sides), m_rotation(rotation) {}

    void draw(QPainter& painter) const override {
        if (!m_center || m_sides < 3) return;

        painter.setPen(QPen(m_selected ? Qt::cyan : m_color, 2 * m_thickness));
        QPolygonF polygon;
        for (int i = 0; i < m_sides; ++i) {
            double angle = m_rotation + 2.0 * M_PI * i / m_sides;
            polygon << QPointF(m_center->x() + m_radius * std::cos(angle),
                               m_center->y() + m_radius * std::sin(angle));
        }
        painter.drawPolygon(polygon);
    }

    bool contains(const QPointF& point, double tolerance) const override {
        if (!m_center || m_sides < 3) return false;
        
        // Check distance to edges
        QPolygonF polygon;
        for (int i = 0; i < m_sides; ++i) {
            double angle = m_rotation + 2.0 * M_PI * i / m_sides;
            polygon << QPointF(m_center->x() + m_radius * std::cos(angle),
                               m_center->y() + m_radius * std::sin(angle));
        }
        
        for (int i = 0; i < polygon.size(); ++i) {
            QPointF p1 = polygon[i];
            QPointF p2 = polygon[(i + 1) % polygon.size()];
            
            double l2 = QPointF::dotProduct(p2 - p1, p2 - p1);
            double t = std::max(0.0, std::min(1.0, QPointF::dotProduct(point - p1, p2 - p1) / l2));
            QPointF projection = p1 + t * (p2 - p1);
            if (std::sqrt(QPointF::dotProduct(point - projection, point - projection)) <= tolerance)
                return true;
        }
        return false;
    }

    std::vector<double*> getParameters() override {
        std::vector<double*> params;
        if (m_center) {
            auto p = m_center->getParameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        params.push_back(&m_radius);
        params.push_back(&m_rotation);
        return params;
    }

    EntityType getType() const override { return EntityType::RegularPolygon; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "RegularPolygon";
        if (m_center) json["center"] = m_center->toJson();
        json["radius"] = m_radius;
        json["sides"] = m_sides;
        json["rotation"] = m_rotation;
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
        m_sides = json["sides"].toInt();
        m_rotation = json["rotation"].toDouble();
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif
