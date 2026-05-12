int test1_licm(int b, int c) {
    int res = 0;
    
    for (int i = 0; i < 100; i++) {
        // Queste istruzioni sono Loop-Invariant
        int a = b + c;      // Dipende solo dagli argomenti (fuori dal loop)
        int e = 3;          // Costante
        int f = a + 2;      // Dipende da 'a' (già invariante) e da 2 (costante)
        
        // Questa NON è Loop-Invariant
        res = res + f + e + i; // Dipende da 'i' e da 'res' (che cambiano a ogni iterazione)
    }
    
    return res;
}