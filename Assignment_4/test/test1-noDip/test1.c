void test_indipendente(int *restrict A, int *restrict B, int N) {
    // Loop 0
    for (int i = 0; i < N; i++) {
        A[i] = i * 2;
    }
    
    // Loop 1
    for (int j = 0; j < N; j++) {
        B[j] = j + 5;
    }
}