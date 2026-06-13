#include "transpose_matrix.h"

Matrix transpose(const Matrix& matrix)
{
    if (matrix.empty() || matrix.front().empty()) {
        return {};
    }

    Matrix result(matrix.front().size(), Row(matrix.size()));
    for (size_t row = 0; row < matrix.size(); ++row) {
        for (size_t column = 0; column < matrix[row].size(); ++column) {
            result[column][row] = matrix[row][column];
        }
    }
    return result;
}
