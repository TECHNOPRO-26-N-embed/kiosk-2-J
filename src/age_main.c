#include <stdio.h>
#include <string.h>
int main() {
    struct Product{
        char name[20];
        int number;
        int price;
        int type;
        
    };
    struct Product Product[]={
        {"林檎", 11100, 200, 0},
        {"葡萄", 22200, 300, 0},
        {"お酒", 33300, 400, 1}
    };
    
    int x;
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
    return 0;
}