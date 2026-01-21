#ifndef CANVASSTATES_H
#define CANVASSTATES_H

#include "CanvasState.h"
#include <vector>
#include <QPointF>

class SelectState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override {}
    void handleMouseRelease(QMouseEvent* event) override {}
    void draw(QPainter& painter) override {}
};

class PointState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override {}
    void handleMouseRelease(QMouseEvent* event) override {}
    void draw(QPainter& painter) override {}
};

class LineState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override;
    void handleMouseRelease(QMouseEvent* event) override;
    void draw(QPainter& painter) override;
private:
    bool m_active = false;
    QPointF m_start;
    QPointF m_end;
};

class CircleState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override;
    void handleMouseRelease(QMouseEvent* event) override;
    void draw(QPainter& painter) override;
private:
    bool m_active = false;
    QPointF m_center;
    double m_radius = 0.0;
};

class EllipseState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override;
    void handleMouseRelease(QMouseEvent* event) override;
    void draw(QPainter& painter) override;
private:
    bool m_active = false;
    QPointF m_center;
    double m_rx = 0.0;
    double m_ry = 0.0;
};

class BezierState : public CanvasState {
public:
    using CanvasState::CanvasState;
    void handleMousePress(QMouseEvent* event) override;
    void handleMouseMove(QMouseEvent* event) override;
    void handleMouseRelease(QMouseEvent* event) override {}
    void draw(QPainter& painter) override;
private:
    std::vector<QPointF> m_points;
};

#endif
