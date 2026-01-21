#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "GeometricEntity.h"
#include "Point.h"
#include <memory>
#include <QPainter>

class Ellipse : public GeometricEntity {
private:
    std::shared_ptr<Point> m_center;
    double m_rx, m_ry;

public:
    Ellipse(std::shared_ptr<Point> center, double rx, double ry)
        : m_center(center), m_rx(rx), m_ry(ry) {}

    void draw(QPainter& painter) const override {
        if (m_center) {
            painter.setPen(QPen(m_selected ? Qt::cyan : m_color, 2 * m_thickness));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(QPointF(m_center->x(), m_center->y()), m_rx, m_ry);
        }
    }

    bool contains(const QPointF& point, double tolerance) const override {
        if (!m_center) return false;
        double dx = point.x() - m_center->x();
        double dy = point.y() - m_center->y();
        
        // Equation of ellipse: (x/rx)^2 + (y/ry)^2 = 1
        // We check if it's close to 1
        double val = (dx*dx) / (m_rx*m_rx) + (dy*dy) / (m_ry*m_ry);
        // This is a bit tricky with tolerance, let's use a simple distance based check for now or similar
        // Better: check if point is within tolerance of the boundary
        // For simplicity, we can just check if the point satisfies the ellipse equation within some epsilon
        return std::abs(std::sqrt(val) - 1.0) < (tolerance / std::min(m_rx, m_ry));
    }

    std::vector<double*> getParameters() override {
        std::vector<double*> params;
        if (m_center) {
            auto p = m_center->getParameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        params.push_back(&m_rx);
        params.push_back(&m_ry);
        return params;
    }

    EntityType getType() const override { return EntityType::Ellipse; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Ellipse";
        if (m_center) json["center"] = m_center->toJson();
        json["rx"] = m_rx;
        json["ry"] = m_ry;
        json["color"] = m_color.name();
        json["thickness"] = m_thickness;
        return json;
    }

    void fromJson(const QJsonObject& json) override {
        if (json.contains("center")) {
            m_center = std::make_shared<Point>(0, 0);
            m_center->fromJson(json["center"].toObject());
        }
        m_rx = json["rx"].toDouble();
        m_ry = json["ry"].toDouble();
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif
