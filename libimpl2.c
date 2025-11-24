#include <math.h>

float sin_integral(float a, float b, float e) {
    // Метод трапеций
    float integral = 0.0;
    int n = (int)((b - a) / e);
    for (int i = 0; i < n; i++) {
        float x1 = a + i * e;
        float x2 = a + (i + 1) * e;
        integral += (sinf(x1) + sinf(x2)) * e / 2.0;
    }
    return integral;
}

float cos_derivative(float a, float dx) {
    // (f(a + dx) - f(a - dx)) / (2dx)
    return (cosf(a + dx) - cosf(a - dx)) / (2 * dx);
}