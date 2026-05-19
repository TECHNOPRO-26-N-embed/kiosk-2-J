#include <stdio.h>

typedef struct {
    char name[20];
    int barcode;
    int price;
} Product;

int main() {

    Product products[3] = {
        {"コカ・コーラ", 111111, 120},
        {"ペプシ", 222222, 150},
        {"ファンタ", 333333, 200}
    };

    // cart quantity
    int cart[3] = {0, 0, 0};

    int input;

    while (1) {

        printf("\nバーコードを入力してください\n");
        printf("0 を入力すると終了します\n");

        scanf("%d", &input);

        // finish
        if (input == 0) {
            break;
        }

        int found = 0;

        // search product
        for (int i = 0; i < 3; i++) {

            if (input == products[i].barcode) {

                cart[i]++;

                printf("%s をカートに追加しました\n",
                       products[i].name);

                found = 1;

                break;
            }
        }

        // if not found
        if (found == 0) {
            printf("商品が見つかりません\n");
        }

        // show cart
        printf("\n===== カート =====\n");

        int total = 0;

        for (int i = 0; i < 3; i++) {

            if (cart[i] > 0) {

                printf("%d. %s x %d = %d円\n",
                       i + 1,
                       products[i].name,
                       cart[i],
                       cart[i] * products[i].price);

                total += cart[i] * products[i].price;
            }
        }

        printf("合計: %d円\n", total);

        // menu
        int choice;

        printf("\n1 = 商品を減らす\n");
        printf("2 = 商品を削除\n");
        printf("3 = 続ける\n");

        scanf("%d", &choice);

        // reduce
        if (choice == 1) {

            int item;

            printf("減らしたい商品の番号を入力してください\n");
            scanf("%d", &item);

            if (item >= 1 && item <= 3) {

                if (cart[item - 1] > 0) {

                    cart[item - 1]--;

                    printf("1個減らしました\n");

                } else {
                   
                    printf("カートにありません\n");
                }
            }
        }

        // delete
        else if (choice == 2) {

            int item;

            printf("削除したい商品の番号を入力してください\n");
            scanf("%d", &item);

            if (item >= 1 && item <= 3) {

                cart[item - 1] = 0;

                printf("商品を削除しました\n");

            } else {
                printf("無効な番号です\n");
            }
        }

    // final cart
    printf("\n===== お会計 =====\n");   //ここから年齢確認に移す。

    int finalTotal = 0;

    for (int i = 0; i < 3; i++) {

        if (cart[i] > 0) {

            printf("%s x %d = %d円\n",
                   products[i].name,
                   cart[i],
                   cart[i] * products[i].price);

            finalTotal += cart[i] * products[i].price;
        }
    }

    printf("合計金額: %d円\n", finalTotal);

    printf("ありがとうございました\n");

    
 }
    return 0;
}