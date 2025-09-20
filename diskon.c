#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void format_rupiah(double amount, char *out, size_t out_size) {
    if (amount < 0) {
        snprintf(out, out_size, "-Rp 0,00");
        return;
    }

    // Pisah bagian integer dan desimal (2 digit)
    unsigned long long rup = (unsigned long long) amount;
    int cents = (int)round((amount - (double)rup) * 100.0);

    if (cents == 100) { // pembulatan misal 0.999... -> +1 rupiah
        rup += 1;
        cents = 0;
    }

    // ubah rupiah ke string dan sisipkan titik setiap 3 digit dari kanan
    char tmp[64];
    snprintf(tmp, sizeof tmp, "%llu", rup);

    char formatted[80];
    int len = (int)strlen(tmp);
    int outi = 0;
    int first_group = len % 3;
    if (first_group == 0) first_group = 3;

    // tulis angka pertama (1-3 digit)
    int i = 0;
    for (; i < first_group && tmp[i] != '\0' && outi < (int)sizeof(formatted)-1; ++i)
        formatted[outi++] = tmp[i];

    // sisipkan titik sebelum setiap grup selanjutnya
    for (; tmp[i] != '\0' && outi < (int)sizeof(formatted)-1; ++i) {
        if ((len - i) % 3 == 0) {
            if (outi < (int)sizeof(formatted)-1) formatted[outi++] = '.';
        }
        if (outi < (int)sizeof(formatted)-1) formatted[outi++] = tmp[i];
    }
    formatted[outi] = '\0';

    // gabungkan dengan desimal memakai koma (Indonesia)
    snprintf(out, out_size, "Rp %s,%02d", formatted, cents);
}

int main(void) {
    char buf[256];
    char clean[256];
    int dot_count = 0;
    int i, j = 0;
    double totalBelanja;
    char *endptr;
    int sign_before_digit = 0; // -1 jika ada '-', 1 jika '+', 0 jika tidak ada

    printf("Masukkan total belanja (boleh 'Rp', titik ribuan, atau koma desimal): Rp ");
    if (!fgets(buf, sizeof buf, stdin)) {
        printf("Gagal membaca input.\n");
        return 1;
    }
    buf[strcspn(buf, "\r\n")] = 0; // hapus newline

    // deteksi tanda +/- sebelum digit pertama
    for (i = 0; buf[i] != '\0'; ++i) {
        if (isdigit((unsigned char)buf[i])) break;
        if (buf[i] == '-') { sign_before_digit = -1; break; }
        if (buf[i] == '+') { sign_before_digit = 1; break; }
    }

    // hitung titik dan cek koma
    for (i = 0; buf[i] != '\0'; ++i) if (buf[i] == '.') ++dot_count;
    int has_comma = (strchr(buf, ',') != NULL);

    // simpan tanda minus jika ada
    if (sign_before_digit == -1) {
        clean[j++] = '-';
    }

    // bersihkan string: keep digits, satu '.' untuk desimal, atau convert ',' -> '.'
    for (i = 0; buf[i] != '\0' && j < (int)sizeof clean - 1; ++i) {
        char c = buf[i];
        if (isdigit((unsigned char)c)) {
            clean[j++] = c;
        } else if (c == ',') {
            // koma dianggap desimal -> ubah ke titik untuk strtod
            clean[j++] = '.';
        } else if (c == '.') {
            if (has_comma) {
                // jika ada koma, titik dianggap pemisah ribuan -> skip
                continue;
            } else if (dot_count > 1) {
                // lebih dari 1 titik -> titik pemisah ribuan -> skip
                continue;
            } else {
                // tepat 1 titik dan tidak ada koma -> titik desimal -> keep
                clean[j++] = '.';
            }
        } else {
            // skip (Rp, spasi, huruf, tanda lain)
            continue;
        }
    }
    clean[j] = '\0';

    if (j == 0 || (j == 1 && clean[0] == '-')) {
        printf("Input tidak mengandung angka. Contoh valid: 1500000 atau Rp 1.000.000,50\n");
        return 1;
    }

    totalBelanja = strtod(clean, &endptr);
    if (endptr == clean) {
        printf("Konversi gagal. Pastikan format input benar.\n");
        return 1;
    }

    if (totalBelanja < 0) {
        printf("Total belanja tidak boleh negatif.\n");
        return 1;
    }

    // hitung diskon
    double diskon = 0.0;
    if (totalBelanja < 100000.0) {
        diskon = 0.10 * totalBelanja;
    } else if (totalBelanja <= 500000.0) {
        diskon = 0.15 * totalBelanja;
    } else {
        diskon = 0.25 * totalBelanja;
    }
    double totalBayar = totalBelanja - diskon;

    // formatting rupiah
    char s_total[80], s_diskon[80], s_bayar[80];
    format_rupiah(totalBelanja, s_total, sizeof s_total);
    format_rupiah(diskon, s_diskon, sizeof s_diskon);
    format_rupiah(totalBayar, s_bayar, sizeof s_bayar);

    // tampilkan hasil
    printf("\n=== Rincian Belanja ===\n");
    printf("Total belanja : %s\n", s_total);
    printf("Diskon        : %s\n", s_diskon);
    printf("Total bayar   : %s\n", s_bayar);

    return 0;
}
