#include "GeometricEntityFactory.h"
#include "Point.h"
#include "Line.h"
#include "Circle.h"
#include "Ellipse.h"
#include "RegularPolygon.h"
#include "BezierCurve.h"

std::shared_ptr<GeometricEntity> GeometricEntityFactory::createEntity(const std::string& type) {
    if (type == "Point") return std::make_shared<Point>(0, 0);
    if (type == "Line") return std::make_shared<Line>(nullptr, nullptr);
    if (type == "Circle") return std::make_shared<Circle>(nullptr, 0);
    if (type == "Ellipse") return std::make_shared<Ellipse>(nullptr, 0, 0);
    if (type == "RegularPolygon") return std::make_shared<RegularPolygon>(nullptr, 0, 3);
    if (type == "BezierCurve") return std::make_shared<BezierCurve>(std::vector<std::shared_ptr<Point>>{});
    return nullptr;
}
