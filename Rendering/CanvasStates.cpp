#include "CanvasStates.h"
#include "Canvas.h"
#include "../GeometryEngine/Point.h"
#include "../GeometryEngine/Line.h"
#include "../GeometryEngine/Circle.h"
#include "../GeometryEngine/Ellipse.h"
#include "../GeometryEngine/BezierCurve.h"
#include <cmath>

// SelectState
void SelectState::handleMousePress(QMouseEvent* event) {
    if (!m_canvas->sketch()) return;

    QPointF worldPos = m_canvas->mapToWorld(event->pos());
    bool found = false;
    double tolerance = 5.0 / m_canvas->scale();
    const auto& entities = m_canvas->sketch()->getEntities();
    
    for (auto it = entities.rbegin(); it != entities.rend(); ++it) {
        if ((*it)->contains(worldPos, tolerance)) {
            bool currentState = (*it)->isSelected();
            for (auto& e : entities) e->setSelected(false);
            (*it)->setSelected(!currentState);
            found = true;
            break;
        }
    }
    if (!found) {
        for (auto& e : entities) e->setSelected(false);
    }
    emit m_canvas->selectionChanged();
    m_canvas->update();
}

// PointState
void PointState::handleMousePress(QMouseEvent* event) {
    QPointF worldPos = m_canvas->mapToWorld(event->pos());
    m_canvas->addPoint(worldPos);
}

// LineState
void LineState::handleMousePress(QMouseEvent* event) {
    m_start = m_canvas->mapToWorld(event->pos());
    m_end = m_start;
    m_active = true;
}

void LineState::handleMouseMove(QMouseEvent* event) {
    if (m_active) {
        m_end = m_canvas->mapToWorld(event->pos());
        m_canvas->update();
    }
}

void LineState::handleMouseRelease(QMouseEvent* event) {
    if (m_active) {
        m_end = m_canvas->mapToWorld(event->pos());
        auto p1 = std::make_shared<Point>(m_start.x(), m_start.y());
        auto p2 = std::make_shared<Point>(m_end.x(), m_end.y());
        auto line = std::make_shared<Line>(p1, p2);
        m_canvas->sketch()->addEntity(line);
        m_active = false;
        m_canvas->update();
    }
}

void LineState::draw(QPainter& painter) {
    if (m_active) {
        painter.setPen(QPen(Qt::red, 1 / m_canvas->scale(), Qt::DashLine));
        painter.drawLine(m_start, m_end);
    }
}

// CircleState
void CircleState::handleMousePress(QMouseEvent* event) {
    m_center = m_canvas->mapToWorld(event->pos());
    m_radius = 0;
    m_active = true;
}

void CircleState::handleMouseMove(QMouseEvent* event) {
    if (m_active) {
        QPointF worldPos = m_canvas->mapToWorld(event->pos());
        m_radius = std::hypot(worldPos.x() - m_center.x(), worldPos.y() - m_center.y());
        m_canvas->update();
    }
}

void CircleState::handleMouseRelease(QMouseEvent* event) {
    if (m_active) {
        QPointF worldPos = m_canvas->mapToWorld(event->pos());
        double radius = std::hypot(worldPos.x() - m_center.x(), worldPos.y() - m_center.y());
        auto center = std::make_shared<Point>(m_center.x(), m_center.y());
        auto circle = std::make_shared<Circle>(center, radius);
        m_canvas->sketch()->addEntity(circle);
        m_active = false;
        m_canvas->update();
    }
}

void CircleState::draw(QPainter& painter) {
    if (m_active) {
        painter.setPen(QPen(Qt::blue, 1 / m_canvas->scale(), Qt::DashLine));
        painter.drawEllipse(m_center, m_radius, m_radius);
    }
}

// EllipseState
void EllipseState::handleMousePress(QMouseEvent* event) {
    m_center = m_canvas->mapToWorld(event->pos());
    m_rx = 0;
    m_ry = 0;
    m_active = true;
}

void EllipseState::handleMouseMove(QMouseEvent* event) {
    if (m_active) {
        QPointF worldPos = m_canvas->mapToWorld(event->pos());
        m_rx = std::abs(worldPos.x() - m_center.x());
        m_ry = std::abs(worldPos.y() - m_center.y());
        m_canvas->update();
    }
}

void EllipseState::handleMouseRelease(QMouseEvent* event) {
    if (m_active) {
        QPointF worldPos = m_canvas->mapToWorld(event->pos());
        double rx = std::abs(worldPos.x() - m_center.x());
        double ry = std::abs(worldPos.y() - m_center.y());
        auto center = std::make_shared<Point>(m_center.x(), m_center.y());
        auto ellipse = std::make_shared<Ellipse>(center, rx, ry);
        m_canvas->sketch()->addEntity(ellipse);
        m_active = false;
        m_canvas->update();
    }
}

void EllipseState::draw(QPainter& painter) {
    if (m_active) {
        painter.setPen(QPen(Qt::blue, 1 / m_canvas->scale(), Qt::DashLine));
        painter.drawEllipse(m_center, m_rx, m_ry);
    }
}

// BezierState
void BezierState::handleMousePress(QMouseEvent* event) {
    m_points.push_back(m_canvas->mapToWorld(event->pos()));
    if (m_points.size() == 4) {
        std::vector<std::shared_ptr<Point>> pts;
        for (const auto& p : m_points) {
            pts.push_back(std::make_shared<Point>(p.x(), p.y()));
        }
        auto bezier = std::make_shared<BezierCurve>(pts);
        m_canvas->sketch()->addEntity(bezier);
        m_points.clear();
    }
    m_canvas->update();
}

void BezierState::handleMouseMove(QMouseEvent* event) {
    if (!m_points.empty()) {
        m_canvas->update();
    }
}

void BezierState::draw(QPainter& painter) {
    if (!m_points.empty()) {
        painter.setPen(QPen(Qt::darkCyan, 1 / m_canvas->scale(), Qt::DashLine));
        for (size_t i = 0; i < m_points.size(); ++i) {
            painter.drawEllipse(m_points[i], 2 / m_canvas->scale(), 2 / m_canvas->scale());
            if (i > 0) {
                painter.drawLine(m_points[i - 1], m_points[i]);
            }
        }
        // Tracking to current mouse position
        QPointF mousePos = m_canvas->mapToWorld(m_canvas->mapFromGlobal(QCursor::pos()));
        painter.drawLine(m_points.back(), mousePos);
    }
}
