#include <stdio.h>
#include "functions.h"

char item[][10] = {"apple", "banana", "orange"};
int number[] = {1, 2, 3};
int kosuu[] = {2, 3, 4};
int price[] = {100, 200, 300};
int itemcount = 3;

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
    return 0;
}

/*int main() {
    receipt();
    return 0;
}*/