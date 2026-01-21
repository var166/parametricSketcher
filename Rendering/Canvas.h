#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QCursor>
#include <memory>
#include "../GeometryEngine/Sketch.h"

class CanvasState;

class Canvas : public QOpenGLWidget {
    Q_OBJECT

public:
    enum class Mode {
        None,
        Select,
        Point,
        Line,
        Circle,
        Ellipse,
        Bezier
    };

    Canvas(QWidget* parent = nullptr);
    ~Canvas() override;

    void setSketch(std::shared_ptr<Sketch> sketch);
    std::shared_ptr<Sketch> sketch() const { return m_sketch; }
    
    void setDrawingMode(Mode mode);
    Mode drawingMode() const { return m_mode; }

    void toggleGrid();
    double scale() const { return m_scale; }
    
    QPointF mapToWorld(const QPointF& screenPos) const;
    void addPoint(const QPointF& pos);

signals:
    void selectionChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void drawGrid(QPainter& painter);

private:
    Mode m_mode;
    std::shared_ptr<Sketch> m_sketch;
    std::unique_ptr<CanvasState> m_state;

    double m_scale;
    QPointF m_offset;
    QPoint m_lastMousePos;
    bool m_showGrid;
};

#endif // CANVAS_H