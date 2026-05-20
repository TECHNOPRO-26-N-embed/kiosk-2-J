#include <stdio.h>
#include <stdlib.h>
#include "coins.h"

int loadCoins(const char *filename, Coin coins[], int *n) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    char line[64];
    int idx = 0;

    // 1行目: ヘッダスキップ
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return -1; }

    while (fgets(line, sizeof(line), fp) && idx < MAX_DENOMS) {
        int value, count;
        if (sscanf(line, "%d,%d", &value, &count) == 2) {
            coins[idx].value = value;
            coins[idx].count = count;
            idx++;
        }
    }
    fclose(fp);
    *n = idx;
    return 0;
}

// --- ここ修正ポイント ---
// fprintf(fp, "金種,枚数\n"); を各エディタでcp932(Shift-JIS)で保存しておく
int saveCoins(const char *filename, const Coin coins[], int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    // ↓ ※このファイル自体（.c）をShift-JISで必ず保存すること。もしUTF-8環境なら "denom,count\n" 英語ヘッダも一つの対策
    fprintf(fp, "金種,枚数\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d,%d\n", coins[i].value, coins[i].count);
    }
    fclose(fp);
    return 0;
}