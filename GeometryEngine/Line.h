#ifndef LINE_H
#define LINE_H

#include "GeometricEntity.h"
#include "Point.h"
#include <memory>
#include <QPainter>

class Line : public GeometricEntity {
private:
    std::shared_ptr<Point> m_start;
    std::shared_ptr<Point> m_end;

public:
    Line(std::shared_ptr<Point> start, std::shared_ptr<Point> end) 
        : m_start(start), m_end(end) {}

    // Updated draw function using m_start and m_end
    void draw(QPainter& painter) const override {
        if (m_start && m_end) {
            painter.setPen(QPen(m_selected ? Qt::cyan : m_color, 2 * m_thickness));
            painter.drawLine(QPointF(m_start->x(), m_start->y()), QPointF(m_end->x(), m_end->y()));
        }
    }

    bool contains(const QPointF& point, double tolerance) const override {
        if (!m_start || m_end == nullptr) return false;
        
        QPointF p1(m_start->x(), m_start->y());
        QPointF p2(m_end->x(), m_end->y());
        
        // Distance from point to line segment
        double l2 = QPointF::dotProduct(p2 - p1, p2 - p1);
        if (l2 == 0.0) return std::sqrt(QPointF::dotProduct(point - p1, point - p1)) <= tolerance;
        
        double t = std::max(0.0, std::min(1.0, QPointF::dotProduct(point - p1, p2 - p1) / l2));
        QPointF projection = p1 + t * (p2 - p1);
        double dist = std::sqrt(QPointF::dotProduct(point - projection, point - projection));
        
        return dist <= tolerance;
    }
    std::vector<double*> getParameters() override {
        std::vector<double*> params;
        if (m_start) {
            auto p = m_start->getParameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        if (m_end) {
            auto p = m_end->getParameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        return params;
    }

    EntityType getType() const override { return EntityType::Line; }

    std::shared_ptr<Point> start() const { return m_start; }
    std::shared_ptr<Point> end() const { return m_end; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Line";
        if (m_start) json["start"] = m_start->toJson();
        if (m_end) json["end"] = m_end->toJson();
        json["color"] = m_color.name();
        json["thickness"] = m_thickness;
        return json;
    }

    void fromJson(const QJsonObject& json) override {
        if (json.contains("start")) {
            m_start = std::make_shared<Point>(0, 0);
            m_start->fromJson(json["start"].toObject());
        }
        if (json.contains("end")) {
            m_end = std::make_shared<Point>(0, 0);
            m_end->fromJson(json["end"].toObject());
        }
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif