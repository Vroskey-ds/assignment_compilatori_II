// nested.c
int nested_loops(int n, int m) {
    int sum = 0;
    
    // Loop Esterno
    for (int i = 0; i < n; i++) {
        
        int j = 0;
        // Loop Interno (Sotto-loop)
        while (j < m) {
            if (i % 2 == 0) {
                sum += i * j;
            } else {
                sum -= j;
            }
            j++;
        }
        
    }
    
    return sum;
}