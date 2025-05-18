#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

void gaussian_elimination(double **A, double *b, double *x, int n) {
    // creates augmented matrix
    for (int i = 0; i < n; i++) {
        A[i][n] = b[i];
    }

    // keeps track if matrix is singular
    bool singular = false;

    // elimination process
    for (int i = 0; i < n - 1; i++) {
        // determines pivot row
        int p = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(A[k][i]) > fabs(A[p][i])) {
                p = k;
            }
        }

        // if all possible pivots in the column are zero (to machine precision), the matrix is singular
        if (fabs(A[p][i]) <= 1e-10) {
            singular = true;
            break;
        }

        // if p does not equal i, switches the ith and pth rows
        if (p != i) {
            double *temp = A[i];
            A[i] = A[p];
            A[p] = temp;
        }

        // elementary row operation
        for (int j = i + 1; j < n; j++) {
            double factor = A[j][i] / A[i][i];
            for (int k = i; k <= n; k++) {
                A[j][k] -= factor * A[i][k];
            }
        }
    }

    // if bottom right element of A is 0 (to machine precision), then its entire row is 0, and A is singular
    if (fabs(A[n - 1][n - 1]) <= 1e-10) {
        singular = true;
    }

    // performs backward substitution to solve Ax = b if matrix nonsingular
    if (!singular) {
        x[n - 1] = A[n - 1][n] / A[n - 1][n - 1];
        for (int i = n - 2; i >= 0; i--) {
            double S = 0;
            for (int j = i + 1; j < n; j++) {
                S += A[i][j] * x[j];
            }
            x[i] = (A[i][n] - S) / A[i][i];
        }
    }

    // prints warning if matrix is singular
    if (singular) {
        printf("Warning: Matrix is singular to working precision.\n");
    }
}





#include <stdio.h>
#include <stdlib.h>

void solveGauss(double **A, double *b, double *x, int s) {
    for (int j = 0; j < s - 1; j++) {
        for (int i = s - 1; i >= j + 1; i--) {
            double m = A[i][j] / A[j][j];
            for (int k = 0; k < s; k++) {
                A[i][k] -= m * A[j][k];
            }
            b[i] -= m * b[j];
        }
    }
    
    for (int i = 0; i < s; i++) {
        x[i] = 0.0;
    }
    x[s - 1] = b[s - 1] / A[s - 1][s - 1];
    
    for (int i = s - 2; i >= 0; i--) {
        double sum = 0.0;
        for (int j = s - 1; j >= i + 1; j--) {
            sum += A[i][j] * x[j];
        }
        x[i] = (b[i] - sum) / A[i][i];
    }
}


#include <stdio.h>
#include <stdlib.h> // For malloc, free, exit
#include <string.h> // For memcpy
#include <math.h>   // Although gcd is needed, it's often implemented manually for modular arithmetic

// Helper function to calculate the Greatest Common Divisor (GCD) of two numbers.
// This is needed for the modular inverse function.
int get_gcd(int a, int b) {
    a = (a > 0) ? a : -a; // Ensure numbers are non-negative for GCD calculation
    b = (b > 0) ? b : -b;
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


int modInverse(int a, int m) { //Helper function to calculate the modular multiplicative inverse of 'a' modulo 'm' and returns if exists.
    //Uses the Extended Euclidean Algorithm. Returns -1 if the inverse does not exist (i.e., if gcd(a, m) != 1).
    int m0 = m;
    int y = 0, x = 1;

    a = (a % m + m) % m; // Ensure 'a' is within the range [0, m-1]

    // Handle cases where inverse doesn't exist or is trivial
    if (m == 1) return 0; // No inverse mod 1
    if (a == 0) return -1; // Inverse of 0 doesn't exist

    while (a > 1) {  // Extended Euclidean Algorithm Finds x, y such that a*x + m*y = gcd(a, m)
        int q = a / m; // q is quotient
        int t = m;

        m = a % m; //m is remainder now, process same as Euclidean algorithm
        a = t;
        t = y;

        y = x - q * y; //Update y and x
        x = t;
    }
    if (x < 0) {
        x = x + m0; // Ensure x is positive
    }

    // Check if the inverse found is valid (only needed if not guaranteed gcd=1)
    // int gcd_check = get_gcd(a_original, m0); // Not needed here, logic above handles non-coprime
    // if (gcd_check != 1) return -1; // Should not happen if loop terminates correctly

    return x; // x is the modular inverse
}

int determinantOfMatrixUsingGaussian(int matrix[26][26]) { // Function to calculate the determinant of a 26x26 matrix modulo 26 using gaussian elimination.
    //Returns determinant or 0 if matrix is singular%26 (not invertible)
    int n=26;

    int temp_matrix[26][26]; //Create a local copy to avoid changing the original matrix passed in. We need to perform Gaussian elimination, which modifies the matrix.
    for (int i = 0; i < n; i++) { // Copy the input matrix to the temporary matrix
        for (int j = 0; j < n; j++) {
            temp_matrix[i][j] = (matrix[i][j] % 26 + 26) % 26; // Ensure initial values are 0-25 //just in case.
        }
    }

    int current_determinant = 1; 
    int num_row_swaps = 0;       // Count row swaps to adjust the sign later

    // Perform Forward Elimination to make the matrix upper triangular
    // Iterate through columns (pivot columns)
    for (int j = 0; j < n; j++) { // j is the current column index

        // --- Step 1: Find a pivot for the current column j ---
        int pivot_row = j; // Start looking for a pivot in the current row j
        // Find a row 'i' from 'j' downwards where the element temp_matrix[i][j] is non-zero and ideally coprime to 26 for modular inverse existence.
        while (pivot_row < n && temp_matrix[pivot_row][j] == 0) {
             pivot_row++; // Move to the next row if current element is 0
        }
        
        if (pivot_row == n) { // If no non-zero pivot is found in this column from row j downwards
            return 0;  // The matrix is singular modulo 26. Determinant is 0. This column is all zeros from row j downwards.
        }

        // --- Step 2: If the pivot is not in the current row j, swap rows ---
        if (pivot_row != j) {
            for (int k = 0; k < n; k++) { // Swap the current row j with the pivot_row
                int temp = temp_matrix[j][k];
                temp_matrix[j][k] = temp_matrix[pivot_row][k];
                temp_matrix[pivot_row][k] = temp;
            }
            num_row_swaps++; //swap counter++
            // Swapping rows multiplies the determinant by -1 (or 25 mod 26)
        }

        int pivot_element = temp_matrix[j][j]; // The pivot element is now temp_matrix[j][j]

        // --- Step 3: Check if the pivot element is coprime to 26 ---
        // For modular inverse to exist (needed for division in elimination)
        if (get_gcd(pivot_element, 26) != 1) {
             // The pivot is not coprime to 26 (divisible by 2 or 13).
             // The matrix is singular modulo 26. Determinant is 0.
             // print this for debugging to see why a matrix is rejected:
             // printf("Singular matrix: pivot %d not coprime to 26 at (%d,%d)\n", pivot_element, j, j);
             return 0;
        }

        // --- Step 4: Multiply the determinant by the pivot element ---
        current_determinant = (current_determinant * pivot_element) % 26;

        // --- Step 5: Eliminate elements below the pivot ---
        // Make all elements below the pivot temp_matrix[j][j] equal to zero.
        // Iterate through rows below the current pivot row 'j'
        for (int i = j + 1; i < n; i++) {
            int element_to_eliminate = temp_matrix[i][j]; // The element to make zero

            // If the element is already zero, no operation is needed for this row
            if (element_to_eliminate == 0) {
                continue;
            }

            // Calculate the multiplier 'm' = (element to eliminate) * (pivot's modular inverse) mod 26
            int pivot_inverse = modInverse(pivot_element, 26); // Get the modular inverse of the pivot
            int multiplier = (element_to_eliminate * pivot_inverse) % 26;

            // Perform the row operation: Row_i = (Row_i - multiplier * Row_j) mod 26
            // Apply this operation to elements in columns from j to the end of the row
            for (int k = j; k < n; k++) {
                int term_to_subtract = (multiplier * temp_matrix[j][k]) % 26;
                temp_matrix[i][k] = (temp_matrix[i][k] - term_to_subtract + 26) % 26; // Add 26 to handle potential negative results from subtraction
            }
        }
    }

    // --- Step 6: Adjust determinant sign based on row swaps ---
    // Each row swap multiplies the determinant by -1. Modulo 26, -1 is 25.
    if (num_row_swaps % 2 != 0) { // If an odd number of swaps occurred
        current_determinant = (26 - current_determinant) % 26; // This is equivalent to (25 * current_determinant) % 26
    }

    // Ensure the final determinant is in the range [0, 25]
    current_determinant = (current_determinant % 26 + 26) % 26;


    // The final determinant is the product of the diagonal elements after elimination,
    // adjusted for swaps. The loops already calculated this product step-by-step.

    return current_determinant; // Return the calculated determinant modulo 26
}

// You can now use this function in your makehillkey:
// int hillkey[26][26];
// int det;
// do {
//     // Fill hillkey with random numbers 0-25
//     // ...
//     det = determinantOfMatrixUsingGaussian(hillkey, 26);
// } while (det == 0); // Loop until an invertible matrix (det != 0 mod 26) is found