#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

#define MAX_INTS 1000000

long read_file(const char *filename, int *arr, long max_count) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    long n = 0;
    int x;
    while (n < max_count && fscanf(f, "%d", &x) == 1) arr[n++] = x;
    fclose(f);
    return n;
}

int main(void) {
    int arr[MAX_INTS];
    int all_passed = 1;

    printf("Starting all tests...\n");
    fflush(stdout);

    for (int t = 1; t <= 10; t++) {
        char infile[256], outfile[256];
        sprintf(infile, "test%02d.txt", t);
        sprintf(outfile, "test%02d-output.txt", t);

        long n = read_file(infile, arr, MAX_INTS);
        if (n == 0) {
            printf("Test %02d FAILED (input missing)\n", t);
            fflush(stdout);
            all_passed = 0;
            continue;
        }

        reset_lib();

        long idx = 0;
        int N = arr[idx++];
        for (int i = 0; i < N && idx+i <= n; i++) add_book(arr[idx+i]);
        idx += N;

        int Q = arr[idx++];
        int test_ok = 1;
        FILE *fout = fopen(outfile, "r");
        if (!fout) {
            printf("Test %02d FAILED (output missing)\n", t);
            all_passed = 0;
            continue;
        }

        for (int i = 0; i < Q && idx+i <= n; i++) {
            int id = arr[idx+i];
            char expected[16];
            if (!fgets(expected, sizeof(expected), fout)) {
                test_ok = 0;
                break;
            }
            expected[strcspn(expected, "\r\n")] = 0;
            if ((contains_book(id) && strcmp(expected, "YES") != 0) ||
                (!contains_book(id) && strcmp(expected, "NO") != 0)) {
                test_ok = 0;
                break;
            }
        }
        fclose(fout);

        if (test_ok) {
            printf("Test %02d validated\n", t);
        } else {
            printf("Test %02d FAILED\n", t);
            all_passed = 0;
        }
        fflush(stdout);
    }

    if (all_passed) printf("\nAll tests validated\n");
    else printf("\nSome tests failed\n");

    return 0;
}
