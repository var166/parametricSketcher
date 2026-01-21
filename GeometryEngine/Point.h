#ifndef POINT_H
#define POINT_H

#include "GeometricEntity.h"
#include <QPainter>

class Point : public GeometricEntity {
private:
    double m_x, m_y;

public:
    Point(double x, double y) : m_x(x), m_y(y) {}
    double x() const { return m_x; }
    double y() const { return m_y; }

    
    void draw(QPainter& painter) const override {
        QColor drawColor = m_selected ? Qt::cyan : m_color;
        painter.setPen(drawColor);
        painter.setBrush(drawColor);
        double size = 3.0 * m_thickness;
        painter.drawEllipse(QPointF(m_x, m_y), size, size);
    }

    bool contains(const QPointF& point, double tolerance) const override {
        double dx = m_x - point.x();
        double dy = m_y - point.y();
        double size = 3.0 * m_thickness;
        return std::sqrt(dx*dx + dy*dy) <= (tolerance + size);
    }

    std::vector<double*> getParameters() override {
        return { &m_x, &m_y };
    }
    
    EntityType getType() const override { return EntityType::Point; }
    
    Point_2 toCgalPoint() const { return Point_2(m_x, m_y); }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Point";
        json["x"] = m_x;
        json["y"] = m_y;
        json["color"] = m_color.name();
        json["thickness"] = m_thickness;
        return json;
    }

    void fromJson(const QJsonObject& json) override {
        m_x = json["x"].toDouble();
        m_y = json["y"].toDouble();
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif