#ifndef HORIZONTALCONSTRAINT_H
#define HORIZONTALCONSTRAINT_H

#include "Constraint.h"
#include "../GeometryEngine/Point.h"
#include <memory>

class HorizontalConstraint : public Constraint {
private:
    std::shared_ptr<Point> m_p1;
    std::shared_ptr<Point> m_p2;

public:
    HorizontalConstraint(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2)
        : m_p1(p1), m_p2(p2) {}

    double evaluate() const override {
        return m_p1->y() - m_p2->y(); // Y coordinates should be equal
    }

    std::vector<double> getGradient(const std::vector<double*>& allParams) const override {
        std::vector<double> grad(allParams.size(), 0.0);
        
        auto p1Params = m_p1->getParameters();
        auto p2Params = m_p2->getParameters();

        for (size_t i = 0; i < allParams.size(); ++i) {
            if (allParams[i] == p1Params[1]) grad[i] = 1.0;
            else if (allParams[i] == p2Params[1]) grad[i] = -1.0;
        }
        return grad;
    }

    std::string getType() const override { return "Horizontal"; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Horizontal";
        return json;
    }
};

#endif
