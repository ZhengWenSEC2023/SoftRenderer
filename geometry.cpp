#include "geometry.h"

template <> template <> Vec3<int>::Vec3(const Vec3<float>& v) : x(int(v.x + .5)), y(int(v.y + .5)), z(int(v.z + .5)) {}

template <> template <> Vec3<float>::Vec3(const Vec3<int>& v) : x(v.x), y(v.y), z(v.z) {}

////////////////////////////////////////////////

Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))), rows(r), cols(c) {}
int Matrix::nrows() {
    return rows;
}

int Matrix::ncols() {
    return cols;
}
Matrix Matrix::identity(int dimensions) {
    Matrix E(dimensions, dimensions);
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            E[i][j] = (i == j ? 1.f : 0.f);
        }
    }
    return E;
}
std::vector<float>& Matrix::operator[](const int i) {
    assert(i >= 0 && i < rows);
    return m[i];
}
Matrix Matrix::operator*(const Matrix& a) {
    assert(cols == a.rows);
    Matrix res(rows, a.cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            res.m[i][j] = 0.f;
            for (int k = 0; k < cols; k++) {
                res.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
    }
    return res;
}
Matrix Matrix::transpose() {
    Matrix res(cols, rows);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            res[j][i] = m[i][j];
    return res;
}
Matrix Matrix::inverse() {
    // Gaussian method
    assert(rows == cols);
    Matrix res(rows, cols * 2);

    // init
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            res[i][j] = m[i][j];
    for (int i = 0; i < rows; i++)
        res[i][i + cols] = 1;

    // eliminate to be upper triangle matrix
    for (int i = 0; i < rows - 1; i++) {
        for (int j = res.cols - 1; j >= 0; j--)
            res[i][j] /= res[i][i];
        for (int k = i + 1; k < rows; k++) {
            float coeff = res[k][i];
            for (int j = 0; j < res.cols; j++) {
                res[k][j] -= res[i][j] * coeff;
            }
        }
    }
    for (int j = res.cols - 1; j >= rows - 1; j--)
        res[rows - 1][j] /= res[rows - 1][rows - 1];

    // eliminate to be identicle matrix
    for (int i = rows - 1; i > 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            float coeff = res[k][i];
            for (int j = 0; j < res.cols; j++) {
                res[k][j] -= res[i][j] * coeff;
            }
        }
    }

    // return the right part
    Matrix truncate(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            truncate[i][j] = res[i][j + cols];
    return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i = 0; i < m.nrows(); i++) {
        for (int j = 0; j < m.ncols(); j++) {
            s << m[i][j];
            if (j < m.ncols() - 1) s << "\t";
        }
        s << "\n";
    }
    return s;
}