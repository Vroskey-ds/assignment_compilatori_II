int strength_reduction(int x, int y) {

    // --- GRUPPO 1: MOLTIPLICAZIONE (Potenze del 2 esatte) ---
    int m1 = x * 16;
    int m2 = 32 * x;

    // --- GRUPPO 2: MOLTIPLICAZIONE (Potenza del 2 - 1) ---
    int m3 = x * 15;
    int m4 = 31 * x;

    // --- GRUPPO 3: MOLTIPLICAZIONE (Potenza del 2 + 1) ---
    int m5 = x * 17;
    int m6 = 33 * x;

    // --- GRUPPO 4: DIVISIONE (Potenze del 2 esatte) ---
    int d1 = x / 8;

    // --- GRUPPO 5: TRAPPOLE ---
    int trap1 = x * 27;
    int trap2 = x / 7;
    int trap3 = 16 / x;
    int trap4 = x * y;

    return m1 + m2 + m3 + m4 + m5 + m6 + d1 + trap1 + trap2 + trap3 + trap4;
}