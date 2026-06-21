// in questo esercizio la fusione verrà abortita.
// Questo perchè la DI tipicamente per ottenre la direzione della dipendenza esegue un calcolo matematico (quando si è all'interno di un unico cilo).
// Dal momento che in questo esempio ho due cicli diversi, con due variabili di induzione differenti, l'analisi non è in
// grado di effettuare questo calcolo con due variabili diverse quindi setta la direzione a "*", ovvero "potrebbe essere una qualsiasi direzione".
// Per questo motivo la direzione potrebbe essere anche backward "GT", quindi il passo abortisce la fusione

void test_confuso(int *restrict A, int N, int M) {
    int sum = 0;

    // Loop Padre (Livello 1)
    for (int k = 0; k < M; k++) {
        
        // Loop 0 Figlio (Livello 2): Scrive su A
        for (int i = 0; i < N; i++) {
            A[i] = i * 10;
        }

        // Loop 1 Figlio (Livello 2): Legge da A
        for (int j = 0; j < N; j++) {
            sum += A[j];
        }
    }
}