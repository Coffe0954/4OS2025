#include <math.h>

float sin_integral(float a, float b, float e) {
    // Метод прямоугольников
    float integral = 0.0;
    int n = (int)((b - a) / e);
    for (int i = 0; i < n; i++) {
        float x = a + i * e;
        integral += sinf(x) * e;
    }
    return integral;
}

float cos_derivative(float a, float dx) {
    // (f(a + dx) - f(a)) / dx
    return (cosf(a + dx) - cosf(a)) / dx;
}