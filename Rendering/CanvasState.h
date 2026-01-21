#ifndef CANVASSTATE_H
#define CANVASSTATE_H

#include <QMouseEvent>
#include <QPainter>

class Canvas;

class CanvasState {
public:
    explicit CanvasState(Canvas* canvas) : m_canvas(canvas) {}
    virtual ~CanvasState() = default;

    virtual void handleMousePress(QMouseEvent* event) = 0;
    virtual void handleMouseMove(QMouseEvent* event) = 0;
    virtual void handleMouseRelease(QMouseEvent* event) = 0;
    virtual void draw(QPainter& painter) = 0;

protected:
    Canvas* m_canvas;
};

#endif
