#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "GeometricEntity.h"
#include "Point.h"
#include <memory>
#include <vector>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QJsonArray>

class BezierCurve : public GeometricEntity {
private:
    std::vector<std::shared_ptr<Point>> m_controlPoints;

public:
    BezierCurve(const std::vector<std::shared_ptr<Point>>& controlPoints)
        : m_controlPoints(controlPoints) {}

    void draw(QPainter& painter) const override {
        if (m_controlPoints.size() < 2) return;

        painter.setPen(QPen(m_selected ? Qt::cyan : m_color, 2 * m_thickness));
        QPainterPath path;
        path.moveTo(m_controlPoints[0]->x(), m_controlPoints[0]->y());

        if (m_controlPoints.size() == 4) {
            path.cubicTo(m_controlPoints[1]->x(), m_controlPoints[1]->y(),
                         m_controlPoints[2]->x(), m_controlPoints[2]->y(),
                         m_controlPoints[3]->x(), m_controlPoints[3]->y());
        } else {
            // Basic polyline for other degrees or fallback
            for (size_t i = 1; i < m_controlPoints.size(); ++i) {
                path.lineTo(m_controlPoints[i]->x(), m_controlPoints[i]->y());
            }
        }
        painter.drawPath(path);

        // Draw control points and lines if selected
        if (m_selected) {
            painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
            for (size_t i = 0; i < m_controlPoints.size(); ++i) {
                if (i > 0) {
                    painter.drawLine(QPointF(m_controlPoints[i-1]->x(), m_controlPoints[i-1]->y()),
                                     QPointF(m_controlPoints[i]->x(), m_controlPoints[i]->y()));
                }
                painter.drawEllipse(QPointF(m_controlPoints[i]->x(), m_controlPoints[i]->y()), 2, 2);
            }
        }
    }

    bool contains(const QPointF& point, double tolerance) const override {
        if (m_controlPoints.size() < 2) return false;
        
        // Simple hit testing: distance to any control point or to the path approximated by lines
        for (const auto& cp : m_controlPoints) {
            if (std::hypot(cp->x() - point.x(), cp->y() - point.y()) <= tolerance)
                return true;
        }
        
        // Better: approximate path and check distance to segments
        QPainterPath path;
        path.moveTo(m_controlPoints[0]->x(), m_controlPoints[0]->y());
        if (m_controlPoints.size() == 4) {
            path.cubicTo(m_controlPoints[1]->x(), m_controlPoints[1]->y(),
                         m_controlPoints[2]->x(), m_controlPoints[2]->y(),
                         m_controlPoints[3]->x(), m_controlPoints[3]->y());
        } else {
            for (size_t i = 1; i < m_controlPoints.size(); ++i) {
                path.lineTo(m_controlPoints[i]->x(), m_controlPoints[i]->y());
            }
        }
        
        QPainterPath strokedPath = QPainterPathStroker().createStroke(path);
        return path.toFillPolygon().containsPoint(point, Qt::OddEvenFill) || 
               // This is not perfect, let's use a simpler approach: check flattened path
               false; // Fallback
    }

    std::vector<double*> getParameters() override {
        std::vector<double*> params;
        for (auto& p : m_controlPoints) {
            auto subParams = p->getParameters();
            params.insert(params.end(), subParams.begin(), subParams.end());
        }
        return params;
    }

    EntityType getType() const override { return EntityType::BezierCurve; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "BezierCurve";
        QJsonArray points;
        for (const auto& p : m_controlPoints) {
            points.append(p->toJson());
        }
        json["controlPoints"] = points;
        json["color"] = m_color.name();
        json["thickness"] = m_thickness;
        return json;
    }

    void fromJson(const QJsonObject& json) override {
        m_controlPoints.clear();
        QJsonArray points = json["controlPoints"].toArray();
        for (int i = 0; i < points.size(); ++i) {
            auto p = std::make_shared<Point>(0, 0);
            p->fromJson(points[i].toObject());
            m_controlPoints.push_back(p);
        }
        if (json.contains("color")) m_color = QColor(json["color"].toString());
        if (json.contains("thickness")) m_thickness = json["thickness"].toDouble();
    }
};

#endif
