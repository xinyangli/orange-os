int main() {
    char s[] = {'Y', 0xF, 'e', 0xF ,'s', 0xF};
    __builtin_memcpy((void *)0xb8000, s, 6);
    while(1) ;
}
