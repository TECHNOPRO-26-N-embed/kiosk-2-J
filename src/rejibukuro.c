#include <stdio.h>

int main(void) {
    int rezibukuro;
    printf("レジ袋を使用しますか？\n");
    printf("1. はい\n");
    printf("2. いいえ\n");
    getchar();
    printf("何個必要ですか？1個当たり5円です。\n");
    scanf("%d", &rezibukuro);
    printf("%d個のレジ袋をカートに入れました。\n", rezibukuro);
    return 0;
}