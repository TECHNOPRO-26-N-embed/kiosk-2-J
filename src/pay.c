#include <stdio.h>
#include <string.h>

#define MAX_COINS 10
#define COINS_FILE "src/coins.csv"

/*
 * 金種情報を保存する構造体
 */
typedef struct {
    int moneyType;   // 金種
    int count;       // 枚数
} Coin;

/*
 * coins.csv を読み込む関数
 */
int loadCoins(Coin coins[]) {
    FILE *fp;
    int count = 0;
    char header[100];

    fp = fopen(COINS_FILE, "r");

    if (fp == NULL) {
        printf("coins.csv が見つかりません。\n");
        return 0;
    }

    /* ヘッダ行を読み飛ばす */
    fgets(header, sizeof(header), fp);

    while (fscanf(fp, "%d,%d\n",
                  &coins[count].moneyType,
                  &coins[count].count) == 2) {

        count++;

        if (count >= MAX_COINS) {
            break;
        }
    }

    fclose(fp);

    return count;
}

/*
 * coins.csv に保存する関数
 */
void saveCoins(Coin coins[], int count) {
    FILE *fp;
    int i;

    fp = fopen(COINS_FILE, "w");

    if (fp == NULL) {
        printf("coins.csv を保存できませんでした。\n");
        return;
    }

    fprintf(fp, "金種,枚数\n");

    for (i = 0; i < count; i++) {
        fprintf(fp, "%d,%d\n",
                coins[i].moneyType,
                coins[i].count);
    }

    fclose(fp);
}

/*
 * 釣銭を計算する関数
 * 貪欲法を使用する
 *
 * 成功: 1
 * 失敗: 0
 */
int calculateChange(Coin coins[],
                    int coinCount,
                    int change,
                    int useCoins[]) {

    int i;
    int need;

    /* 初期化 */
    for (i = 0; i < coinCount; i++) {
        useCoins[i] = 0;
    }

    /*
     * 大きい金種から順番に使用
     */
    for (i = 0; i < coinCount; i++) {

        need = change / coins[i].moneyType;

        /*
         * 在庫以上は使えない
         */
        if (need > coins[i].count) {
            need = coins[i].count;
        }

        useCoins[i] = need;

        change -= coins[i].moneyType * need;
    }

    /*
     * change が 0 なら成功
     */
    if (change == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * 現金支払処理
 *
 * total:
 * 支払う金額
 *
 * 成功: 1
 * 失敗: 0
 */
int cashPayment(int total) {

    Coin coins[MAX_COINS];

    int coinCount;

    /*
     * ユーザーが投入した金額
     */
    int insertedCoins[MAX_COINS] = {0};

    /*
     * 払い出す釣銭
     */
    int changeCoins[MAX_COINS] = {0};

    int insertedTotal = 0;

    int moneyType;
    int inputCount;

    int i;

    int found;

    int change;

    /*
     * coins.csv を読み込む
     */
    coinCount = loadCoins(coins);

    if (coinCount == 0) {
        printf("釣銭データがありません。\n");
        return 0;
    }

    printf("\n===== 現金支払 =====\n");

    printf("必要金額: %d円\n", total);

    printf("金種を入力してください。\n");

    printf("0 を入力すると終了します。\n");

    /*
     * お金を投入するループ
     */
    while (1) {

        printf("\n投入する金種: ");
        scanf("%d", &moneyType);

        /*
         * 0 で終了
         */
        if (moneyType == 0) {
            break;
        }

        printf("枚数: ");
        scanf("%d", &inputCount);

        found = 0;

        /*
         * 金種検索
         */
        for (i = 0; i < coinCount; i++) {

            if (coins[i].moneyType == moneyType) {

                insertedCoins[i] += inputCount;

                insertedTotal += moneyType * inputCount;

                found = 1;

                break;
            }
        }

        if (found == 0) {
            printf("使用できない金種です。\n");
        }

        printf("現在の投入金額: %d円\n",
               insertedTotal);

        /*
         * 必要金額以上なら終了
         */
        if (insertedTotal >= total) {
            break;
        }
    }

    /*
     * 金額不足
     */
    if (insertedTotal < total) {

        printf("\n投入金額が不足しています。\n");

        printf("支払を中断します。\n");

        printf("投入金を返却します。（疑似処理）\n");

        return 0;
    }

    /*
     * おつり計算
     */
    change = insertedTotal - total;

    printf("\nおつり: %d円\n", change);

    /*
     * 釣銭計算
     */
    if (calculateChange(coins,
                        coinCount,
                        change,
                        changeCoins) == 0) {

        printf("\n釣銭不足のため現金では支払えません。\n");

        printf("カード/電子/現金で正確な金額を投入してください。\n");

        printf("投入金を返却します。（疑似処理）\n");

        return 0;
    }

    /*
     * 在庫更新
     *
     * 入金:
     * +する
     *
     * 釣銭:
     * -する
     */
    for (i = 0; i < coinCount; i++) {

        coins[i].count += insertedCoins[i];

        coins[i].count -= changeCoins[i];
    }

    /*
     * 保存
     */
    saveCoins(coins, coinCount);

    printf("\n===== 支払成功 =====\n");

    /*
     * 釣銭表示
     */
    if (change > 0) {

        printf("\nおつり内訳:\n");

        for (i = 0; i < coinCount; i++) {

            if (changeCoins[i] > 0) {

                printf("%d円 : %d枚\n",
                       coins[i].moneyType,
                       changeCoins[i]);
            }
        }
    }

    return 1;
}

/*
 * カード支払
 */
int cardPayment(int total) {

    printf("\n===== カード支払 =====\n");

    printf("支払金額: %d円\n", total);

    printf("カード認証中...\n");

    printf("カード支払が完了しました。\n");

    return 1;
}

/*
 * 電子決済
 */
int electronicPayment(int total) {

    printf("\n===== 電子決済 =====\n");

    printf("支払金額: %d円\n", total);

    printf("電子決済認証中...\n");

    printf("電子決済が完了しました。\n");

    return 1;
}

/*
 * 支払方法選択
 */
int selectPayment(int total) {

    int choice;

    while (1) {

        printf("\n===== 支払方法選択 =====\n");

        printf("1. 現金\n");

        printf("2. カード\n");

        printf("3. 電子決済\n");

        printf("番号を入力してください: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1:

                if (cashPayment(total) == 1) {
                    return 1;
                }

                break;

            case 2:

                if (cardPayment(total) == 1) {
                    return 2;
                }

                break;

            case 3:

                if (electronicPayment(total) == 1) {
                    return 3;
                }

                break;

            default:

                printf("正しい番号を入力してください。\n");
        }
    }
}

/*
 * メイン関数
 */
int main(void) {

    /*
     * テスト用合計金額
     */
    int total = 850;

    int paymentMethod;

    paymentMethod = selectPayment(total);

    printf("\n支払完了\n");

    printf("支払方法番号: %d\n",
           paymentMethod);

    return 0;
}