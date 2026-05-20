#include <stdio.h>
#include <math.h>
#include <string.h>
#include "syohin.h"

#define TAX_RATE 10  // 例：全商品10%とする（もし商品ごとに税率を持たせたい場合はstruct Productに追加してね）

// カート合計・税額・会計表示と支払い方法選択
void checkout(KioskState *st) {
    int sub = 0;       // 小計（税抜き金額）
    int tax = 0;       // 税額
    int total = 0;     // 合計金額（税込み）
    char buf[32];      // 入力バッファ
    int cmd = 0;       // 支払い決定用
    

    // --- 1. 小計計算 ---
    for (int i = 0; i < st->cart_count; i++) {
        sub += st->cart[i].product.price * st->cart[i].quantity;
    }

    // --- 2. 税額計算（四捨五入）---
    //   ※ 全商品「同じ税率」想定。税額 = floor(小計 * 税率 / 100 + 0.5)
    tax = (int)(floor(sub * TAX_RATE / 100.0 + 0.5));

    // --- 3. 合計（税込） ---
    total = sub + tax;

    // --- 4. 結果表示 ---
    printf("\n==== お会計 ==== \n");
    printf("小計（税抜）: %d円\n", sub);
    printf("消費税(%d%%): %d円\n", TAX_RATE, tax);
    printf("合計（税込）: %d円\n", total);

    // --- 5. 支払い方法選択 ---
    while (1) {
        printf("\n支払い方法を選んでください：\n");
        printf("1 = 現金, 2 = カード（疑似）, 3 = 電子（疑似）, 9 = 最初に戻る\n");
        printf("選択: ");
        fgets(buf, sizeof(buf), stdin);  // バッファサイズに注意
        buf[strcspn(buf, "\n")] = 0;     // 改行除去

        if (strcmp(buf, "9") == 0) {
            printf("最初に戻ります。\n");
            return; // メインメニューなどに戻る
        }
        if (strcmp(buf, "1") == 0) {
            printf("現金支払いを選びました。\n");
            break; // この先、現金処理担当に渡す
        }
        if (strcmp(buf, "2") == 0) {
            printf("カード（疑似）支払いを選びました。\n");
            break; // この先、カード擬似処理担当に渡す
        }
        if (strcmp(buf, "3") == 0) {
            printf("電子（疑似）支払いを選びました。\n");
            break; // この先、電子擬似処理担当に渡す
        }
        printf("無効な選択です。もう一度数字で選んでください。\n");
    }

    // --- 6. ここから支払い処理担当の関数を呼び出してね！ ---
}
int main() {
    KioskState st;
    memset(&st, 0, sizeof(st));

    // ここで商品や従業員のCSVを読み込んでね！

    // カートに商品を追加する処理もここで呼び出してね！

    // 最後にお会計処理
    checkout(&st);

    return 0;
}