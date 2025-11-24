# компилируем библиотеки
gcc -shared -fPIC -o libimpl1.so libimpl1.c -lm
gcc -shared -fPIC -o libimpl2.so libimpl2.c -lm

# компилируем программы
gcc -o program1 program1.c -L. -limpl1 -lm
gcc -o program2 program2.c -ldl

# устанавливаем путь к библиотекам
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# запускаем
./program1
# или
./program2


