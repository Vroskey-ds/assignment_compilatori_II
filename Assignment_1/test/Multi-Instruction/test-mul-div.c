int test_mul_div(int x, int y) {
    
    // --- GRUPPO 1: CASI VALIDI ---
    int m1 = x * 42;
    int res1 = m1 / 42;

    int m2 = 100 * x;
    int res2 = m2 / 100;

    // --- GRUPPO 2: TRAPPOLA TRONCAMENTO/PERDITA INFORMAZIONE (CONTROLLO EXACT) ---
    int d1 = x / 5;
    int res3 = d1 * 5;      
    int res4 = 5 * d1;      

    // --- GRUPPO 3: TRAPPOLE ---
    int t1 = x * 10;
    int trap1 = t1 / 2;

    int t2 = x * y;
    int trap2 = t2 / y;

    return res1 + res2 + res3 + res4 + trap1 + trap2;
}