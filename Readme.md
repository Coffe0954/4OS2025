# компил библиотек
gcc -shared -fPIC -o libimpl1.so libimpl1.c -lm
gcc -shared -fPIC -o libimpl2.so libimpl2.c -lm

# компил прог
gcc -o program1 program1.c -L. -limpl1 -lm
gcc -o program2 program2.c -ldl

# путь к библиотекам
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# запуск
./program1
./program2

<img width="789" height="1104" alt="image" src="https://github.com/user-attachments/assets/ca8c0c1f-706c-49d2-a6e7-b95821d60e60" />
<img width="732" height="364" alt="image" src="https://github.com/user-attachments/assets/d1dd34bd-aade-4a28-a318-05c8102e8d99" />


