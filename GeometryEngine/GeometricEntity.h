#ifndef GEOMETRICENTITY_H
#define GEOMETRICENTITY_H

#include <CGAL/Simple_cartesian.h>
#include <vector>
#include <QPainter> 
#include <QJsonObject>
#include <QColor>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2 Point_2;

enum class EntityType { 
    Point, 
    Line, 
    Circle, 
    Ellipse, 
    RegularPolygon, 
    BezierCurve 
};

class GeometricEntity {
protected:
    bool m_selected = false;
    QColor m_color = Qt::black;
    double m_thickness = 1.0;

public:
    virtual ~GeometricEntity() = default;

    virtual void draw(QPainter& painter) const = 0;

    // For the solver: returns pointers to mutable parameters
    virtual std::vector<double*> getParameters() = 0;

    virtual EntityType getType() const = 0;

    // JSON Serialization
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& json) = 0;

    virtual bool contains(const QPointF& point, double tolerance) const = 0;
    
    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const { return m_selected; }

    void setColor(QColor color) { m_color = color; }
    QColor color() const { return m_color; }

    void setThickness(double thickness) { m_thickness = thickness; }
    double thickness() const { return m_thickness; }
};

#endif