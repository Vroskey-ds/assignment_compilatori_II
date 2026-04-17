int algebric_identity(int x, int y) {
    
    // --- GRUPPO 1: ADDIZIONE ---
    int a1 = x + 0;
    int a2 = 0 + y;

    // --- GRUPPO 2: MOLTIPLICAZIONE ---
    int m1 = a1 * 1;
    int m2 = 1 * a2;

    // --- GRUPPO 3: SOTTRAZIONE ---
    int s1 = m1 - 0;
    
    // --- TRAPPOLA SOTTRAZIONE ---
    int trap_sub = 0 - x;

    // --- GRUPPO 4: DIVISIONE ---
    int d1 = m2 / 1;
    
    // --- TRAPPOLA DIVISIONE ---
    int trap_div = 1 / y;

    return s1 + d1 + trap_sub + trap_div;
}