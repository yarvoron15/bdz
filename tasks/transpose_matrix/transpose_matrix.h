#ifndef TRANSPOSE_MATRIX_H_
#define TRANSPOSE_MATRIX_H_


#include <vector>

using Row = std::vector<int>;
using Matrix = std::vector<Row>;

Matrix transpose(const Matrix& matrix);


#endif // TRANSPOSE_MATRIX_H_
