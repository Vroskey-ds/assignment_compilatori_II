// come possiamo notare passo 3 array alla fun senza 'restrict' che sarebbe 'noalis' in IR
void test_confused(int *A, int *B, int *C, int N) {
    // Loop 0: Produce scrivendo sull'array A
    for (int i = 0; i < N; i++) {
        A[i] = i * 2; // scrivo in A
    }
    
    // Loop 1: Consuma leggendo dall'array B
    for (int j = 0; j < N; j++) {
        // scriva in A e leggo da B, qui il compilatore, dal momento che non ho usato restrict non ha la sicurezza che 
        // siano tutti array diversi quelli passati, quindi solleva il flag isConfused()
        C[j] = B[j] + 5; 
    }
}