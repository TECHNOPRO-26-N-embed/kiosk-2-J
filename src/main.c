#include <stdio.h>


char item[][10] = {"apple", "banana", "orange"};
int number[] = {1, 2, 3};
int kosuu[] = {2, 3, 4};
int price[] = {100, 200, 300};
int itemcount = 3;

char days[] = "2026/05/18/14:00";

int waribiki = 50;
int  tax;


int main() {
    int total = 0;

    printf("==============================\n");

    for(int i = 0; i < itemcount; i++) {
        int subtotal = kosuu[i] * price[i];
        printf("%s %d %d %d\n", item[i], kosuu[i], price[i], subtotal);
        total += subtotal;
    }
    printf("==============================\n");
    printf("Total: %d\n", total);

    printf("END\n");
    return 0;
}