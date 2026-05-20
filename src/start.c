#include <stdio.h>
#include <stdlib.h>
#include "coins.h" // coins.csv を扱う自作ヘッダ(前の例を参照)

#define COIN_FILE "data/coins.csv"

void show_start_screen(void) {
    printf("*** セルフレジへようこそ ***\n");
    printf("[1] 会計開始\n");
    printf("[2] 管理者メニュー\n");
    printf("[9] 終了\n");
}

void dummy_coin_refill(Coin coins[], int n) {
    printf("釣銭補充（ダミー）\n");
    for (int i = 0; i < n; ++i) {
        printf("%s(%d円): 追加枚数> ", coins[i].name, coins[i].value);
        int add = 0;
        scanf("%d", &add);
        if (add > 0)
            coins[i].count += add;
    }
    printf("補充が完了しました。\n");
}

int start(void) {
    Coin coins[MAX_DENOMS];
    int ncoins = 0;

    if (loadCoins(COIN_FILE, coins, &ncoins) != 0) {
        fprintf(stderr, "coins.csv の読み込みに失敗しました。\n");
        fprintf(stderr, "ファイルを確認してください。\n");
        return 1;
    }

    printf("--- 起動時：coins.csv を読み込んでレジ金を確認 ---\n");
    for (int i = 0; i < ncoins; ++i) {
        printf("%s(%d円): %d枚\n", coins[i].name, coins[i].value, coins[i].count);
    }

    show_start_screen();

    int menu = 0;
    while (1) {
        printf("メニュー選択 > ");
        if (scanf("%d", &menu) != 1) {
            printf("入力エラー 再入力してください。\n");
            while(getchar() != '\n'); // 入力残りをクリア
            continue;
        }
        switch(menu) {
            case 1: printf("会計を開始します。（ここに遷移）\n"); break;
            case 2: 
                printf("管理者モードに入ります。\n");
                dummy_coin_refill(coins, ncoins);
                if (saveCoins(COIN_FILE, coins, ncoins) == 0)
                    printf("coins.csv を保存しました。\n");
                else
                    printf("coins.csv 保存失敗！\n");
                break;
            case 9: printf("終了します。\n"); return 0;
            default: printf("無効な選択肢です。\n"); break;
        }
    }
    return 0;
}