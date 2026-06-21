void test_innestato_indipendente(int *restrict A, int *restrict B, int N, int M) {
    for (int k = 0; k < M; k++) { // Loop Padre condiviso
        
        // Loop 0: Lavora solo su A
        for (int i = 0; i < N; i++) {
            A[i] = k + i; 
        }
        
        // Loop 1: Lavora solo su B
        for (int j = 0; j < N; j++) {
            B[j] = k * 2; 
        }
    }
}