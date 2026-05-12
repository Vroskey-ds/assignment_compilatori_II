// test/LICM/test2/LICM-test2.c

int test2_licm(int x, int y) {
    int sum = 0;
    int i = 0;

    // Controllo preventivo per assicurarci che i do-while abbiano senso
    if (x <= 0 || y <= 0) return 0; 

    // --- OUTER LOOP ---
    do {
        // Questa istruzione è invariante per TUTTO il programma (dipende dagli argomenti)
        // Dovrebbe finire nel preheader dell'Outer Loop.
        int outer_inv = x * 2;           

        // Questa istruzione varia nell'Outer Loop (dipende da 'i'), 
        // ma sarà INVARIANTE per l'Inner Loop!
        int inner_limit = i + outer_inv; 

        int j = 0;
        
        // --- INNER LOOP ---
        do {
            // Questa istruzione dipende da inner_limit, che nel contesto 
            // di questo ciclo interno è invariante. 
            // Dovrebbe finire nel preheader dell'Inner Loop.
            int inner_inv = inner_limit * 3; 
            
            sum += inner_inv + j;

            // EXIT POINT 1: L'uscita anticipata (Break)
            // L'istruzione 'inner_inv' si trova PRIMA di questo break, quindi lo domina!
            if (sum > 1000) {
                break; 
            }

            j++;
        // EXIT POINT 2: L'uscita naturale del ciclo
        } while (j < y); 

        i++;
    } while (i < x);

    return sum;
}