// in questo esempio la fusione viene abortita in modo conservativo dato che ho due loop top level.
// questo perchè la funzione GetDirection chiamata al livello 0 (top-level) causa segmetation fault, dato che
// è pensata per essere usata almeno dal livello 1.
// Dunque concludiamo che nel caso di loop top-level dipendendenti, abortiamo conservativamente la fusione dato che
// non siamo sicuri, a causa del comportamento di getDirection, della direzione della dipendenza (fonderemmo una dipendenza backward).

int test_confused(int *A, int N) {
    // Loop 0: Scrittura (Store)
    for (int i = 0; i < N; i++) {
        A[i] = i * 10;
    }

    int sum = 0;
    // Loop 1: Lettura (Load)
    for (int j = 0; j < N; j++) {
        sum += A[j];
    }

   
    return sum; 
}