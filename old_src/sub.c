#include <stdio.h>
#include <string.h>
char item[][10] = {"apple", "banana", "orange"};
int number[] = {1, 2, 3};
int kosuu[] = {2, 3, 4};
int price[] = {100, 200, 300};
int itemcount = 3;

int money;
int discount = 50;


int receipt() {
    int total = 0;

    printf("=============レシート=============\n");

    for(int i = 0; i < itemcount; i++) {
        int subtotal = kosuu [i] * price[i];
        printf("%s     %d個     ￥%d\n", item[i], kosuu[i], subtotal);
        total += subtotal;
    }
    printf("---------------------------------\n");
    int tax = total * 0.10;
    printf("消費税: ￥%d\n", tax);
    total += tax;
    printf("割引: ￥%d\n", discount);
    total -= discount;
    printf("合計: ￥%d\n", total);

    printf("=================================\n");
}

 typedef struct {
    char name[20];
    int barcode;
    int price;
} Product;
int start() {

    printf("画面をタッチ\n"); 
}
int rejibukuro() {
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
}

int barcode() {

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
}
int age_main() {
    struct Product{
        char name[20];
        int number;
        int price;
        int type;
        
    };
    struct Product Product[]={
        {"林檎", 11100, 200, 0},
        {"葡萄", 22200, 300, 0},
        {"お酒", 33300, 400, 1}     //ここの０，１で年齢判定も使いたい
    };
    
    int x;  //ここより下の年齢判定は使いたい。
    scanf("%d",&x);
    for(int i=0;i<3;i++){
        if(x==Product[i].number){
            printf("%s %d\n",Product[i].name,Product[i].price);
            if(Product[i].type == 1){
                printf("年齢確認\n");
                printf("年齢を入力してください\n");
                int age;
                scanf("%d",&age);
                if(age < 20){
                    printf("購入できません\n");
                    return 0;
                }
            }
        }
        
    }
}
int main (void) {
    start();
    rejibukuro();
    barcode();
    //age_main();
    receipt(); 
    return 0;
}