#ifndef COINCIDENTCONSTRAINT_H
#define COINCIDENTCONSTRAINT_H

#include "Constraint.h"
#include "../GeometryEngine/Point.h"
#include <memory>

class CoincidentConstraint : public Constraint {
private:
    std::shared_ptr<Point> m_p1;
    std::shared_ptr<Point> m_p2;

public:
    CoincidentConstraint(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2)
        : m_p1(p1), m_p2(p2) {}

    double evaluate() const override {
        double dx = m_p1->x() - m_p2->x();
        double dy = m_p1->y() - m_p2->y();
        return dx * dx + dy * dy; // Distance squared should be 0
    }

    std::vector<double> getGradient(const std::vector<double*>& allParams) const override {
        std::vector<double> grad(allParams.size(), 0.0);
        double dx = 2.0 * (m_p1->x() - m_p2->x());
        double dy = 2.0 * (m_p1->y() - m_p2->y());

        auto p1Params = m_p1->getParameters();
        auto p2Params = m_p2->getParameters();

        for (size_t i = 0; i < allParams.size(); ++i) {
            if (allParams[i] == p1Params[0]) grad[i] = dx;
            else if (allParams[i] == p1Params[1]) grad[i] = dy;
            else if (allParams[i] == p2Params[0]) grad[i] = -dx;
            else if (allParams[i] == p2Params[1]) grad[i] = -dy;
        }
        return grad;
    }

    std::string getType() const override { return "Coincident"; }

    QJsonObject toJson() const override {
        QJsonObject json;
        json["type"] = "Coincident";
        // In a real system we would use IDs here
        return json;
    }
};

#endif
