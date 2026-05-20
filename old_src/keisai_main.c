#include <stdio.h>
int pay_sun=4000;
int main() {
    printf("お支払方法を選んでください\n");
    printf("1.現金\n");
    printf("2.クレジットカード\n"); 
    printf("3.QRコード\n");
    int x;
    scanf("%d",&x);
    switch (x)
    {
        case 1:
            printf("現金を投入してください。\n");
            while(1){
                int money;      //釣銭機の関係で現金とそれ以外を変数分けたい。
                scanf("%d",&money);
                if(money >= pay_sun){
                    printf("お釣りは%d円です。\n", money - pay_sun);
                    break;
                }
                else{
                    printf("金額が足りません。再度投入してください。\n");
                }
            }
            break;
        case 2:
            printf("クレジットカードを挿入してください。\n");
            break;
        case 3:
            printf("QRコードを読み取ってください。\n");
            break;
    }
    return 0;
}