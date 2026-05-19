#include <stdio.h>

int main(void) {
    int rezibukuro;
    int choice;
    printf("レジ袋を使用しますか？\n");
    printf("1. はい\n");
    printf("2. いいえ\n");
    scanf("%d", &choice);
    getchar();
    if (choice == 1){
         printf("何個必要ですか？1個当たり5円です。\n");
        scanf("%d", &rezibukuro);
        printf("%d個のレジ袋をカートに入れました。\n", rezibukuro);
    }else if (choice == 2){
        printf("レジ袋は使用しません。\n");
        printf("このまま買い物を続けます。\n");
    }
   
    return 0;
}