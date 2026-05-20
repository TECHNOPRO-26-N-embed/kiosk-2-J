#include <stdio.h>

int money = 100000;
int pass;

int turisenki() {
    printf("釣銭機を確認中...\n");
    if (money > 95000) {
        printf(":ERROR:\n釣銭機がいっぱいです\n");
        printf("店員を呼んでください\n");
        while (1){
            printf("管理者パスワードを入力してください\n");
            scanf("%d", &pass);
            if (pass == 1234) {
                printf("釣銭機から55,000円を取り出しました\n");
                money = money - 55000;
                break;
            } else {
                printf("パスワードが違います\n");
            }
        }
    } else {
        printf("釣銭機は正常に動作しています\n");
    }
    printf("=============================\n");
}

int main() {
    turisenki();
    return 0;
}