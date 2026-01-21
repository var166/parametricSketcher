#include "Solver.h"
#include <iostream>

Solver::Status Solver::solve() {
    if (m_constraints.empty()) return Status::Solved;
    if (m_parameters.empty()) return Status::UnderConstrained;

    const int maxIterations = 50;
    const double epsilon = 1e-6;

    for (int iter = 0; iter < maxIterations; ++iter) {
        Eigen::VectorXd r(m_constraints.size());
        for (size_t i = 0; i < m_constraints.size(); ++i) {
            r(i) = m_constraints[i]->evaluate();
        }

        if (r.norm() < epsilon) {
            // Check degrees of freedom
            if (m_constraints.size() < m_parameters.size()) return Status::UnderConstrained;
            if (m_constraints.size() > m_parameters.size()) return Status::OverConstrained;
            return Status::Solved;
        }

        Eigen::MatrixXd J(m_constraints.size(), m_parameters.size());
        for (size_t i = 0; i < m_constraints.size(); ++i) {
            std::vector<double> grad = m_constraints[i]->getGradient(m_parameters);
            for (size_t j = 0; j < m_parameters.size(); ++j) {
                J(i, j) = grad[j];
            }
        }

        // Newton step: Delta = -J^+ * r
        // Using pseudo-inverse for non-square systems
        Eigen::VectorXd delta = J.completeOrthogonalDecomposition().solve(-r);

        for (size_t j = 0; j < m_parameters.size(); ++j) {
            *m_parameters[j] += delta(j);
        }
    }

    return Status::Failed;
}