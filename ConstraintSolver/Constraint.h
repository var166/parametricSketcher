#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <vector>
#include <string>
#include <QJsonObject>
#include <Eigen/Dense>

class Constraint {
public:
    virtual ~Constraint() = default;

    // Returns the residual value of the constraint equation (should be 0 when satisfied)
    virtual double evaluate() const = 0;

    // Returns the Jacobian (partial derivatives) with respect to the given parameters
    virtual std::vector<double> getGradient(const std::vector<double*>& allParams) const = 0;

    virtual std::string getType() const = 0;

    virtual QJsonObject toJson() const = 0;
};

#endif
