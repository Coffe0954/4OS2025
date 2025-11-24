#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

// типы функций
typedef float (*sin_integral_func)(float, float, float);
typedef float (*cos_derivative_func)(float, float);

void* current_lib = NULL;
sin_integral_func sin_integral = NULL;
cos_derivative_func cos_derivative = NULL;
int current_impl = 1;

void print_string(const char* str) {
    write(STDOUT_FILENO, str, strlen(str));
}

void print_float(float value) {
    char buffer[32];
    int len = 0;
    
    if (value < 0) {
        buffer[len++] = '-';
        value = -value;
    }
    
    int integer_part = (int)value;
    float fractional_part = value - integer_part;
    
    char int_buffer[32];
    int int_len = 0;
    
    if (integer_part == 0) {
        int_buffer[int_len++] = '0';
    } else {
        while (integer_part > 0) {
            int_buffer[int_len++] = '0' + (integer_part % 10);
            integer_part /= 10;
        }
        for (int i = 0; i < int_len; i++) {
            buffer[len++] = int_buffer[int_len - 1 - i];
        }
    }
    
    buffer[len++] = '.';
    for (int i = 0; i < 6; i++) {
        fractional_part *= 10;
        int digit = (int)fractional_part;
        buffer[len++] = '0' + digit;
        fractional_part -= digit;
    }
    
    buffer[len++] = '\n';
    write(STDOUT_FILENO, buffer, len);
}

int parse_float(const char* str, float* result) {
    float value = 0.0;
    float sign = 1.0;
    float decimal = 0.1;
    int i = 0;
    int has_decimal = 0;
    
    if (str[0] == '-') {
        sign = -1.0;
        i++;
    }
    
    for (; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            has_decimal = 1;
            continue;
        }
        
        if (str[i] >= '0' && str[i] <= '9') {
            if (!has_decimal) {
                value = value * 10.0 + (str[i] - '0');
            } else {
                value += (str[i] - '0') * decimal;
                decimal *= 0.1;
            }
        } else {
            return -1;
        }
    }
    
    *result = value * sign;
    return 0;
}

int load_library(const char* libname) {
    if (current_lib) {
        dlclose(current_lib);
    }
    
    current_lib = dlopen(libname, RTLD_LAZY);
    if (!current_lib) {
        return -1;
    }
    
    sin_integral = (sin_integral_func)dlsym(current_lib, "sin_integral");
    cos_derivative = (cos_derivative_func)dlsym(current_lib, "cos_derivative");
    
    if (!sin_integral || !cos_derivative) {
        dlclose(current_lib);
        current_lib = NULL;
        return -1;
    }
    
    return 0;
}

const char* get_impl_description(int impl) {
    if (impl == 1) {
        return "rectangle method / simple derivative";
    } else {
        return "trapezoidal method / central difference derivative";
    }
}

void print_menu() {
    print_string("\n==(dynamic version)==\n");
    print_string("available commands:\n");
    print_string("  1 a b e    - compute integral of sin(x) from a to b with step e\n");
    print_string("  2 a dx     - compute derivative of cos(x) at point a with step dx\n");
    print_string("  0          - switch algorithm implementation\n");
    print_string("  help       - show this menu\n");
    print_string("  exit       - exit program\n");
    print_string("examples:\n");
    print_string("  1 0 3.14159 0.01 - integral of sin from 0 to pi\n");
    print_string("  2 0 0.001       - derivative of cos at point 0\n");
    print_string("current implementation: ");
    print_string(get_impl_description(current_impl));
    print_string("\n> ");
}

int main() {
    // загружаем первую библиотеку по умолчанию
    if (load_library("./libimpl1.so") != 0) {
        print_string("error: cannot load libimpl1.so\n");
        print_string("make sure the library files are in current directory\n");
        return 1;
    }
    
    char buffer[256];
    char command[32];
    char arg1[32], arg2[32], arg3[32];
    print_menu();
    
    while (1) {
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;
        
        buffer[bytes_read] = '\0';
        
        // убираем символ новой строки
        if (buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';
        }
        
        int parsed = sscanf(buffer, "%31s %31s %31s %31s", command, arg1, arg2, arg3);
        
        if (parsed >= 1) {
            if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
                print_string("goodbye!\n");
                break;
            }
            else if (strcmp(command, "help") == 0) {
                print_menu();
            }
            else if (strcmp(command, "0") == 0) {
                // переключение реализации
                if (current_impl == 1) {
                    if (load_library("./libimpl2.so") == 0) {
                        current_impl = 2;
                        print_string("switched to implementation 2: ");
                        print_string(get_impl_description(current_impl));
                        print_string("\n");
                    } else {
                        print_string("error: cannot load libimpl2.so\n");
                        print_string("make sure the file exists in current directory\n");
                    }
                } else {
                    if (load_library("./libimpl1.so") == 0) {
                        current_impl = 1;
                        print_string("switched to implementation 1: ");
                        print_string(get_impl_description(current_impl));
                        print_string("\n");
                    } else {
                        print_string("error: cannot load libimpl1.so\n");
                    }
                }
            }
            else if (strcmp(command, "1") == 0) {
                if (parsed == 4) {
                    float a, b, e;
                    if (parse_float(arg1, &a) == 0 && 
                        parse_float(arg2, &b) == 0 && 
                        parse_float(arg3, &e) == 0) {
                        
                        if (e <= 0) {
                            print_string("error: step e must be positive\n");
                        } else if (a >= b) {
                            print_string("error: a must be less than b\n");
                        } else {
                            print_string("computing integral of sin(x)...\n");
                            print_string("  from: ");
                            print_float(a);
                            print_string("  to: ");
                            print_float(b);
                            print_string("  step: ");
                            print_float(e);
                            print_string("  method: ");
                            print_string(current_impl == 1 ? "rectangles" : "trapezoids");
                            print_string("\n");
                            
                            float result = sin_integral(a, b, e);
                            print_string("result: ");
                            print_float(result);
                        }
                    } else {
                        print_string("error: invalid number format. use: 1 a b e\n");
                    }
                } else {
                    print_string("error: command 1 requires 3 arguments: a b e\n");
                    print_string("example: 1 0 3.14159 0.01\n");
                }
            }
            else if (strcmp(command, "2") == 0) {
                if (parsed == 3) {
                    float a, dx;
                    if (parse_float(arg1, &a) == 0 && 
                        parse_float(arg2, &dx) == 0) {
                        
                        if (dx <= 0) {
                            print_string("error: step dx must be positive\n");
                        } else {
                            print_string("computing derivative of cos(x)...\n");
                            print_string("  point: ");
                            print_float(a);
                            print_string("  step: ");
                            print_float(dx);
                            print_string("  method: ");
                            print_string(current_impl == 1 ? "simple difference" : "central difference");
                            print_string("\n");
                            
                            float result = cos_derivative(a, dx);
                            print_string("result: ");
                            print_float(result);
                        }
                    } else {
                        print_string("error: invalid number format. use: 2 a dx\n");
                    }
                } else {
                    print_string("error: command 2 requires 2 arguments: a dx\n");
                    print_string("example: 2 0 0.001\n");
                }
            }
            else {
                print_string("unknown command. type 'help' for help.\n");
            }
        }
        
        print_string("> ");
    }
    
    if (current_lib) {
        dlclose(current_lib);
    }
    
    return 0;
}