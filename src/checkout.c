\#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "syohin.h"

#define TAX_RATE 10

// ---------- helper ----------
static void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

static void read_line(char *buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

static int is_all_digits(const char *s) {
    if (!s || *s == '\0') return 0;
    for (int i = 0; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

static int is_digits_len(const char *s, int len) {
    return is_all_digits(s) && ((int)strlen(s) == len);
}

static int to_int_safe(const char *s, int *out) {
    if (!is_all_digits(s)) return 0;
    long v = strtol(s, NULL, 10);
    if (v < 0 || v > 1000000000L) return 0;
    *out = (int)v;
    return 1;
}

// ---------- payment methods ----------

// 1) Cash:
// ask cash input repeatedly until input == total
static void pay_cash_exact(int total) {
    char buf[64];
    int cash = 0;

    printf("\n現金を選びました。\n");

    while (1) {
        printf("現金金額を入力してください（合計: %d円）: ", total);
        read_line(buf, sizeof(buf));

        if (!to_int_safe(buf, &cash)) {
            printf("数字で入力してください。\n");
            continue;
        }

        if (cash == total) {
            printf("お支払いありがとうございました。\n");
            printf("決済完了。\n");
            return;
        } else {
            printf("入力金額が合計と一致しません。もう一度入力してください。\n");
        }
    }
}

// 2) Credit card:
// ask 10-digit card number + 4-digit PIN, then complete
static void pay_credit_card_simple(void) {
    char card[64];
    char pin[64];

    printf("\nカードを選びました。\n");

    while (1) {
        printf("カード番号（10桁）を入力してください: ");
        read_line(card, sizeof(card));
        if (is_digits_len(card, 10)) break;
        printf("カード番号は10桁の数字で入力してください。\n");
    }

    while (1) {
        printf("暗証番号（4桁PIN）を入力してください: ");
        read_line(pin, sizeof(pin));
        if (is_digits_len(pin, 4)) break;
        printf("PINは4桁の数字で入力してください。\n");
    }

    printf("payment complete\n");
}

// 3) Denshi money:
// if input 1 -> payment complete
// if input 2 -> tell user to touch and show "press 3 to return"
static int pay_denshi_money_flow(void) {
    char buf[32];

    printf("\n電子マネーを選びました。\n");

    while (1) {
        printf("入力してください（1=タッチ済み, 2=未タッチ）: ");
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "1") == 0) {
            printf("payment complete\n");
            return 1; // success
        }

        if (strcmp(buf, "2") == 0) {
            printf("タッチしてください。\n");
            printf("支払い方法選択に戻るには 3 を押してください。\n");
            read_line(buf, sizeof(buf));
            if (strcmp(buf, "3") == 0) {
                return 0; // go back to payment method selection
            } else {
                printf("無効な入力です。もう一度入力してください。\n");
                continue;
            }
        }

        printf("無効な入力です。1 または 2 を入力してください。\n");
    }
}

// ---------- checkout ----------
void checkout(KioskState *st) {
    int sub = 0;
    int tax = 0;
    int total = 0;
    char buf[32];

    for (int i = 0; i < st->cart_count; i++) {
        sub += st->cart[i].product.price * st->cart[i].quantity;
    }

    tax = (int)(floor(sub * TAX_RATE / 100.0 + 0.5));
    total = sub + tax;

    printf("\n==== お会計 ====\n");
    printf("小計（税抜）: %d円\n", sub);
    printf("消費税(%d%%): %d円\n", TAX_RATE, tax);
    printf("合計（税込）: %d円\n", total);

    while (1) {
        printf("\n支払い方法を選んでください：\n");
        printf("1 = 現金, 2 = クレジットカード, 3 = 電子マネー, 9 = 戻る\n");
        printf("選択: ");
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "9") == 0) {
            printf("支払い方法選択を終了しました。\n");
            return;
        }

        if (strcmp(buf, "1") == 0) {
            pay_cash_exact(total);
            return;
        }

        if (strcmp(buf, "2") == 0) {
            pay_credit_card_simple();
            return;
        }

        if (strcmp(buf, "3") == 0) {
            int done = pay_denshi_money_flow();
            if (done) return;   // payment complete
            continue;           // back to payment method selection
        }

        printf("無効な選択です。\n");
    }
}