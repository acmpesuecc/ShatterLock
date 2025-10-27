
/*******************************************************************************
 * HILL CIPHER FUNCTIONS
 ******************************************************************************/

/**
 * Multiplies message vectors by Hill cipher key matrix
 * @param R1 - Number of rows in message matrix
 * @param m1 - Message matrix (R1 x 26)
 * @param key - Hill cipher key (26 x 26)
 * @param m_out - Output matrix (R1 x 26) as lowercase letters
 * Note: Used internally by hill_encrypt
 */
void multiply_matrices(int R1, int m1[R1][26], int key[26][26], char m_out[R1][26]) { //copilot helped with this, pls forgive.
    char letters[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < 26; j++) {
            int sum = 0;
            for (int k = 0; k < 26; k++) {
                sum = (sum + m1[i][k] * key[k][j]) % 26;
            }
            m_out[i][j] = letters[(sum + 26) % 26];
        }
    }
}

// Encrypts numpacks packets using the Hill cipher key (packets_in × hillkey) //copilot helped with this, pls forgive.
void hill_encrypt(int numpacks, char packets_in[numpacks][26], int hillkey[26][26], char packets_out[numpacks][26]) {
    int packets_num[numpacks][26];
    for (int i = 0; i < numpacks; i++) {
        for (int j = 0; j < 26; j++) {
            packets_num[i][j] = ((int)packets_in[i][j] + 26 - 97) % 26;
        }
    }
    multiply_matrices(numpacks, packets_num, hillkey, packets_out);
}

/**
 * Computes modular multiplicative inverse of a number modulo 26
 * @param a - Number to find inverse of
 * @return Modular inverse, or 0 if not invertible
 * Note: Used for Hill cipher key matrix inversion
 */
int modinv26(int a) { //github copilot mad ethis. pls forgive.
    // Returns the modular inverse of a mod 26, or 0 if not invertible
    a = a % 26;
    for (int x = 1; x < 26; x++) {
        if ((a * x) % 26 == 1) return x;
    }
    return 0;
}

int invertMatrixMod26(int input[26][26], int output[26][26]) { //github copilot made this. pls forgive.
    int n = 26;
    int aug[26][52];
    // Set up augmented matrix [input | I]
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i][j] = input[i][j] % 26;
            if (aug[i][j] < 0) aug[i][j] += 26;
            aug[i][j + n] = (i == j) ? 1 : 0;
        }
    }
    // Gauss-Jordan elimination
    for (int col = 0; col < n; col++) {
        // Find pivot
        int pivot = -1;
        for (int row = col; row < n; row++) {
            if (aug[row][col] != 0 && modinv26(aug[row][col]) != 0) {
                pivot = row;
                break;
            }
        }
        if (pivot == -1) return 0; // Not invertible
        // Swap rows if needed
        if (pivot != col) {
            for (int k = 0; k < 2 * n; k++) {
                int tmp = aug[col][k];
                aug[col][k] = aug[pivot][k];
                aug[pivot][k] = tmp;
            }
        }
        // Scale pivot row
        int inv = modinv26(aug[col][col]);
        for (int k = 0; k < 2 * n; k++) {
            aug[col][k] = (aug[col][k] * inv) % 26;
            if (aug[col][k] < 0) aug[col][k] += 26;
        }
        // Eliminate other rows
        for (int row = 0; row < n; row++) {
            if (row == col) continue;
            int factor = aug[row][col];
            for (int k = 0; k < 2 * n; k++) {
                aug[row][k] = (aug[row][k] - factor * aug[col][k]) % 26;
                if (aug[row][k] < 0) aug[row][k] += 26;
            }
        }
    }
    // Extract inverse
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            output[i][j] = aug[i][j + n];
        }
    }
    return 1;
}

// Decrypts numpacks packets using the Hill cipher key (by inverting the key) //copilot made this, pls forgive.
void hill_decrypt(int numpacks, char packets_in[numpacks][26], int hillkey[26][26], char packets_out[numpacks][26]) {
    int invkey[26][26];
    if (!invertMatrixMod26(hillkey, invkey)) {
        printf("Error: Hill key is not invertible!\n");
        return;
    }
    int packets_num[numpacks][26];
    for (int i = 0; i < numpacks; i++) {
        for (int j = 0; j < 26; j++) {
            packets_num[i][j] = ((int)packets_in[i][j] + 26 - 97) % 26;
        }
    }
    multiply_matrices(numpacks, packets_num, invkey, packets_out);
}
