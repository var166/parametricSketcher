#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <memory>
#include "Constraint.h"
#include <Eigen/Dense>

class Solver {
public:
    enum class Status { Solved, UnderConstrained, OverConstrained, Failed };

    Solver() = default;

    void addConstraint(std::shared_ptr<Constraint> constraint) {
        m_constraints.push_back(constraint);
    }

    void addParameter(double* param) {
        m_parameters.push_back(param);
    }

    Status solve();

private:
    std::vector<std::shared_ptr<Constraint>> m_constraints;
    std::vector<double*> m_parameters;
};

#endif
