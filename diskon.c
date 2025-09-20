#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

int main() {
    setlocale(LC_NUMERIC, ""); // aktifkan format ribuan

    char input[50];
    long totalBelanja;
    double diskon = 0, totalBayar;

    printf("Masukkan total belanja Anda (contoh: 150000 atau 550.000): Rp ");
    scanf("%s", input);

    // hapus titik ribuan
    for (size_t i = 0; i < strlen(input); i++) {
        if (input[i] == '.') {
            memmove(&input[i], &input[i + 1], strlen(input) - i);
            i--;
        }
    }

    totalBelanja = atol(input); // string → long

    // aturan diskon
    if (totalBelanja < 100000) {
        diskon = 0.10 * totalBelanja;
    } else if (totalBelanja <= 500000) {
        diskon = 0.15 * totalBelanja;
    } else {
        diskon = 0.25 * totalBelanja;
    }

    totalBayar = totalBelanja - diskon;

    // tampilkan hasil
    printf("\n=== Rincian Belanja ===\n");
    printf("Total belanja : Rp %'ld\n", totalBelanja);
    printf("Diskon        : Rp %'ld\n", (long)diskon);
    printf("Total bayar   : Rp %'ld\n", (long)totalBayar);

    return 0;
}
