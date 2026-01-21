#include "Canvas.h"
#include "CanvasStates.h"
#include "../GeometryEngine/Point.h"
#include <cmath>

Canvas::Canvas(QWidget* parent)
    : QOpenGLWidget(parent),
      m_mode(Mode::None),
      m_sketch(nullptr),
      m_state(nullptr),
      m_scale(1.0),
      m_offset(0, 0),
      m_showGrid(true) 
{
    setMouseTracking(true);
}

Canvas::~Canvas() {}

void Canvas::setSketch(std::shared_ptr<Sketch> sketch) {
    m_sketch = sketch;
}

void Canvas::setDrawingMode(Mode mode) {
    if (m_mode == mode) return;
    
    m_mode = mode;
    
    switch (m_mode) {
        case Mode::Select:  m_state = std::make_unique<SelectState>(this); break;
        case Mode::Point:   m_state = std::make_unique<PointState>(this); break;
        case Mode::Line:    m_state = std::make_unique<LineState>(this); break;
        case Mode::Circle:  m_state = std::make_unique<CircleState>(this); break;
        case Mode::Ellipse: m_state = std::make_unique<EllipseState>(this); break;
        case Mode::Bezier:  m_state = std::make_unique<BezierState>(this); break;
        default:            m_state = nullptr; break;
    }

    // Deselect all when changing mode
    if (m_sketch) {
        for (auto& entity : m_sketch->getEntities()) {
            entity->setSelected(false);
        }
    }
    update();
}

void Canvas::toggleGrid() {
    m_showGrid = !m_showGrid;
    update();
}

QPointF Canvas::mapToWorld(const QPointF& screenPos) const {
    return (screenPos - m_offset) / m_scale;
}

void Canvas::addPoint(const QPointF& pos) {
    if (!m_sketch) return;
    auto point = std::make_shared<Point>(pos.x(), pos.y());
    m_sketch->addEntity(point);
    update();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_lastMousePos = event->pos();
        return;
    }

    if (m_state) {
        m_state->handleMousePress(event);
    } else {
        QOpenGLWidget::mousePressEvent(event);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::MiddleButton) {
        m_offset += (event->pos() - m_lastMousePos);
        m_lastMousePos = event->pos();
        update();
        return;
    }

    if (m_state) {
        m_state->handleMouseMove(event);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (m_state) {
        m_state->handleMouseRelease(event);
    }
}

void Canvas::wheelEvent(QWheelEvent* event) {
    double factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    QPointF mousePos = event->position();
    QPointF beforeScale = mapToWorld(mousePos);
    
    m_scale *= factor;
    
    QPointF afterScale = mapToWorld(mousePos);
    m_offset += (afterScale - beforeScale) * m_scale;
    
    update();
}

void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    painter.translate(m_offset);
    painter.scale(m_scale, m_scale);

    if (m_showGrid) {
        drawGrid(painter);
    }

    if (m_sketch) {
        m_sketch->draw(painter);
    }

    if (m_state) {
        m_state->draw(painter);
    }
}

void Canvas::drawGrid(QPainter& painter) {
    double step = 50.0;
    while (step * m_scale < 20) step *= 2;
    while (step * m_scale > 100) step /= 2;

    QPointF topLeft = mapToWorld(QPointF(0, 0));
    QPointF bottomRight = mapToWorld(QPointF(width(), height()));
    
    double left = std::floor(topLeft.x() / step) * step;
    double top = std::floor(topLeft.y() / step) * step;
    double right = bottomRight.x();
    double bottom = bottomRight.y();

    painter.setPen(QPen(QColor(230, 230, 230), 0));
    
    for (double x = left; x <= right; x += step) {
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));
    }
    for (double y = top; y <= bottom; y += step) {
        painter.drawLine(QPointF(left, y), QPointF(right, y));
    }
    
    painter.setPen(QPen(QColor(200, 200, 200), 1 / m_scale));
    painter.drawLine(QPointF(0, top), QPointF(0, bottom));
    painter.drawLine(QPointF(left, 0), QPointF(right, 0));
}
