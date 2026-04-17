// test_add_sub.c
int test_add_sub(int b, int x) {
    
    // --- GRUPPO 1: CASI VALIDI ---

    int a1 = b + 15;
    int res1 = a1 - 15;     

    int a2 = 20 + b;
    int res2 = a2 - 20;    

    int a3 = b - 7;
    int res3 = a3 + 7;      

    int a4 = b - 8;
    int res4 = 8 + a4;      

    // --- GRUPPO 2: TRAPPOLE ---

    int t1 = b + 10;
    int trap1 = t1 - 5;

    int t2 = b + x;
    int trap2 = t2 - x; 

    return res1 + res2 + res3 + res4 + trap1 + trap2;
}