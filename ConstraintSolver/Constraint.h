#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <vector>
#include <string>
#include <QJsonObject>
#include <Eigen/Dense>

class Constraint {
public:
    virtual ~Constraint() = default;

    virtual double evaluate() const = 0;

    virtual std::vector<double> getGradient(const std::vector<double*>& allParams) const = 0;

    virtual std::string getType() const = 0;

    virtual QJsonObject toJson() const = 0;
};

#endif
