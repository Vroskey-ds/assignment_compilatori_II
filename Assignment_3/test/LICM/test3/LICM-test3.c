int test3_licm(int x, int y, int z) {
    int sum = 0;
    int i = 0;

    if (x <= 0 || y <= 0) return 0; 

    // --- OUTER LOOP ---
    do {
        int j = 0;
        
        // --- INNER LOOP ---
        do {
            // questa deve essere spostata nel pre header prima dell'inner loop e poi dell outer, quindi alla
            // fine dovremmo ritrovarla nel pre header del outer
            int super_inv = x * z; 
            
            sum += super_inv + j;
            j++;

        } while (j < y); 

        i++;
    } while (i < x);

    return sum;
}