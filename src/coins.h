#ifndef COINS_H
#define COINS_H

#define MAX_DENOMS 16  // 金種最大数（拡張しやすく十分広く）
#define COIN_NAME_LEN 16

// コインの構造体（金種・枚数）
typedef struct {
    int value;      // 金種（例: 10000, 1000, ...）
    int count;      // 枚数
    char name[COIN_NAME_LEN]; // コイン名
} Coin;

// coins.csv の読み込み（成功:0, 失敗:-1など）
// coins ... Coin配列, n ... 読み込んだ金種数セットするポインタ
int loadCoins(const char *filename, Coin coins[], int *n);

// coins.csv への書き戻し（成功:0, 失敗:-1など）
int saveCoins(const char *filename, const Coin coins[], int n);

#endif