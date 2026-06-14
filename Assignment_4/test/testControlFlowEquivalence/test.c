void test_dominanza(int *restrict A, int *restrict B, int N, int flag) {
    
    // Loop 0: Viene eseguito SEMPRE
    for (int i = 0; i < N; i++) {
        A[i] = i * 2;
    }

    // Loop 1: Viene eseguito SOLO SE il flag è vero
    if (flag) {
        for (int j = 0; j < N; j++) {
            B[j] = A[j] + 5;
        }
    }
}