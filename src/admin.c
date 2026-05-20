#include <stdio.h>
#include <string.h>

#define ADMIN_PIN 1234

#define PRODUCTS_FILE "products.csv"
#define COINS_FILE "coins.csv"
#define TRANSACTIONS_FILE "transactions.csv"

#define MAX_PRODUCTS 1500
#define MAX_LOGS 2000
#define MAX_COINS 10

/*
 * 商品情報を保存する構造体
 * products.csv の1行分のデータを入れるために使う。
 *
 * CSV形式：
 * 商品ID,商品名,値段,税率,対象年齢
 */
typedef struct {
    char id[20];            // 商品ID
    char name[50];          // 商品名
    int price;              // 値段
    char barcode[30];       // バーコード
    int taxRate;            // 税率
    int ageLimit;           // 対象年齢
} Product;

/*
 * 釣銭情報を保存する構造体
 * coins.csv の1行分のデータを入れるために使う。
 *
 * CSV形式：
 * 金種,枚数
 */
typedef struct {
    int moneyType;          // 金種（例：1000円、500円、100円）
    int count;              // 枚数
} Coin;

/*
 * 取引ログを保存する構造体
 * transactions.csv の1行分のデータを入れるために使う。
 *
 * CSV形式：
 * 商品名,個数,税率,合計,支払方法,日にちと時間
 */
typedef struct {
    char productName[50];   // 商品名
    int quantity;           // 個数
    int taxRate;            // 税率
    int total;              // 合計金額
    char payment[20];       // 支払方法
    char datetime[50];      // 日にちと時間
} Transaction;

/* 関数の宣言 */
int adminLogin(void);
void adminMenu(void);

int loadProducts(Product products[]);
void saveProducts(Product products[], int count);
void showProducts(Product products[], int count);
void addProduct(void);
void changeProductPrice(void);

int loadCoins(Coin coins[]);
void saveCoins(Coin coins[], int count);
void showCoins(Coin coins[], int count);
void addCoins(void);

int loadTransactions(Transaction logs[]);
void showTransactionLogs(void);
void showSalesReport(void);
void showTaxReport(void);

/*
 * メイン関数
 * 今回はメンバー5の担当部分だけを確認できるように、
 * 起動後すぐ管理者メニューを表示する。
 */
int main(void) {
    adminMenu();
    return 0;
}

/*
 * 管理者PINを確認する関数
 * 正しいPINが入力された場合は1を返す。
 * 間違っている場合は0を返す。
 */
int adminLogin(void) {
    int pin;

    printf("===== 管理者ログイン =====\n");
    printf("PINを入力してください: ");
    scanf("%d", &pin);

    if (pin == ADMIN_PIN) {
        printf("ログイン成功しました。\n");
        return 1;
    } else {
        printf("PINが違います。管理者メニューを終了します。\n");
        return 0;
    }
}

/*
 * 管理者メニューを表示する関数
 * PIN認証に成功したあと、商品編集、釣銭補充、ログ確認、レポート表示を行う。
 */
void adminMenu(void) {
    int choice;

    if (adminLogin() == 0) {
        return;
    }

    while (1) {
        printf("\n===== 管理者メニュー =====\n");
        printf("1. 商品一覧を表示\n");
        printf("2. 商品を追加\n");
        printf("3. 商品価格を変更\n");
        printf("4. 釣銭一覧を表示\n");
        printf("5. 釣銭を補充\n");
        printf("6. 取引ログを表示\n");
        printf("7. 売上レポートを表示\n");
        printf("8. 税率別レポートを表示\n");
        printf("9. 終了\n");
        printf("番号を選択してください: ");

        scanf("%d", &choice);

        if (choice == 1) {
            Product products[MAX_PRODUCTS];
            int count = loadProducts(products);
            showProducts(products, count);
        } else if (choice == 2) {
            addProduct();
        } else if (choice == 3) {
            changeProductPrice();
        } else if (choice == 4) {
            Coin coins[MAX_COINS];
            int count = loadCoins(coins);
            showCoins(coins, count);
        } else if (choice == 5) {
            addCoins();
        } else if (choice == 6) {
            showTransactionLogs();
        } else if (choice == 7) {
            showSalesReport();
        } else if (choice == 8) {
            showTaxReport();
        } else if (choice == 9) {
            printf("管理者メニューを終了します。\n");
            break;
        } else {
            printf("正しい番号を入力してください。\n");
        }
    }
}

/*
 * products.csv から商品情報を読み込む関数
 * 読み込んだ商品数を返す。
 */
int loadProducts(Product products[]) {
    FILE *fp;
    int count = 0;
    char header[200];

    fp = fopen(PRODUCTS_FILE, "r");

    if (fp == NULL) {
        printf("products.csv が見つかりません。\n");
        return 0;
    }

    /* ヘッダ行を読み飛ばす */
    fgets(header, sizeof(header), fp);

    while (fscanf(fp, "%19[^,],%49[^,],%d,%29[^,],%d,%d\n",
              products[count].id,
              products[count].name,
              &products[count].price,
              products[count].barcode,
              &products[count].taxRate,
              &products[count].ageLimit) == 6) {

      count++;

    if (count >= MAX_PRODUCTS) {
        printf("商品数が上限に達しました。\n");
        break;
        }
    }

    fclose(fp);
    return count;
}

/*
 * 商品情報を products.csv に保存する関数
 * 既存の内容を上書きして保存する。
 */
void saveProducts(Product products[], int count) {
    FILE *fp;
    int i;

    fp = fopen(PRODUCTS_FILE, "w");

    if (fp == NULL) {
        printf("products.csv を保存できませんでした。\n");
        return;
    }

    fprintf(fp, "id,name,price,barcode,tax,age\n");

    for (i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%d,%s,%d,%d\n",
                products[i].id,
                products[i].name,
                products[i].price,
                products[i].barcode,
                products[i].taxRate,
                products[i].ageLimit);
    }

    fclose(fp);
}

/*
 * 商品一覧を表示する関数
 */
void showProducts(Product products[], int count) {
    int i;

    if (count == 0) {
        printf("表示できる商品がありません。\n");
        return;
    }

    printf("\n===== 商品一覧 =====\n");

    for (i = 0; i < count; i++) {
        printf("%d件目\n", i + 1);
        printf("商品ID: %s\n", products[i].id);
        printf("商品名: %s\n", products[i].name);
        printf("値段: %d円\n", products[i].price);
        printf("バーコード: %s\n", products[i].barcode);
        printf("税率: %d%%\n", products[i].taxRate);
        printf("対象年齢: %d歳\n", products[i].ageLimit);
        printf("--------------------\n");
    }
}

/*
 * 商品を追加する関数
 * 入力された商品情報を products.csv に追加する。
 */
void addProduct(void) {
    Product products[MAX_PRODUCTS];
    int count;

    count = loadProducts(products);

    if (count >= MAX_PRODUCTS) {
        printf("これ以上商品を追加できません。\n");
        return;
    }

    printf("\n===== 商品追加 =====\n");

    printf("商品IDを入力してください: ");
    scanf("%s", products[count].id);

    printf("商品名を入力してください: ");
    scanf("%s", products[count].name);

    printf("値段を入力してください: ");
    scanf("%d", &products[count].price);

    printf("バーコードを入力してください: ");
    scanf("%s", products[count].barcode);

    printf("税率を入力してください（8 または 10）: ");
    scanf("%d", &products[count].taxRate);

    printf("対象年齢を入力してください（制限なしは0）: ");
    scanf("%d", &products[count].ageLimit);

    count++;

    saveProducts(products, count);

    printf("商品を追加しました。\n");
}

/*
 * 商品価格を変更する関数
 * 商品IDを検索し、該当商品の価格を変更する。
 */
void changeProductPrice(void) {
    Product products[MAX_PRODUCTS];
    int count;
    int i;
    int found = 0;
    char targetId[20];
    int newPrice;

    count = loadProducts(products);

    if (count == 0) {
        printf("商品データがありません。\n");
        return;
    }

    printf("\n===== 商品価格変更 =====\n");
    printf("価格を変更する商品IDを入力してください: ");
    scanf("%s", targetId);

    for (i = 0; i < count; i++) {
        if (strcmp(products[i].id, targetId) == 0) {
            printf("現在の商品名: %s\n", products[i].name);
            printf("現在の価格: %d円\n", products[i].price);

            printf("新しい価格を入力してください: ");
            scanf("%d", &newPrice);

            products[i].price = newPrice;
            found = 1;
            break;
        }
    }

    if (found == 1) {
        saveProducts(products, count);
        printf("価格を変更しました。\n");
    } else {
        printf("指定された商品IDは見つかりませんでした。\n");
    }
}

/*
 * coins.csv から釣銭情報を読み込む関数
 * 読み込んだ金種の数を返す。
 */
int loadCoins(Coin coins[]) {
    FILE *fp;
    int count = 0;
    char header[100];

    fp = fopen(COINS_FILE, "r");

    if (fp == NULL) {
        printf("coins.csv が見つかりません。\n");
        return 0;
    }

    /* ヘッダ行を読み飛ばす */
    fgets(header, sizeof(header), fp);

    while (fscanf(fp, "%d,%d\n",
                  &coins[count].moneyType,
                  &coins[count].count) == 2) {
        count++;

        if (count >= MAX_COINS) {
            printf("釣銭データが上限に達しました。\n");
            break;
        }
    }

    fclose(fp);
    return count;
}

/*
 * 釣銭情報を coins.csv に保存する関数
 */
void saveCoins(Coin coins[], int count) {
    FILE *fp;
    int i;

    fp = fopen(COINS_FILE, "w");

    if (fp == NULL) {
        printf("coins.csv を保存できませんでした。\n");
        return;
    }

    fprintf(fp, "金種,枚数\n");

    for (i = 0; i < count; i++) {
        fprintf(fp, "%d,%d\n", coins[i].moneyType, coins[i].count);
    }

    fclose(fp);
}

/*
 * 釣銭一覧を表示する関数
 */
void showCoins(Coin coins[], int count) {
    int i;

    if (count == 0) {
        printf("表示できる釣銭データがありません。\n");
        return;
    }

    printf("\n===== 釣銭一覧 =====\n");

    for (i = 0; i < count; i++) {
        printf("%d円: %d枚\n", coins[i].moneyType, coins[i].count);
    }
}

/*
 * 釣銭を補充する関数
 * 指定した金種の枚数を増やす。
 */
void addCoins(void) {
    Coin coins[MAX_COINS];
    int count;
    int i;
    int targetMoney;
    int addCount;
    int found = 0;

    count = loadCoins(coins);

    if (count == 0) {
        printf("釣銭データがありません。\n");
        return;
    }

    printf("\n===== 釣銭補充 =====\n");
    showCoins(coins, count);

    printf("補充する金種を入力してください（例：100）: ");
    scanf("%d", &targetMoney);

    printf("追加する枚数を入力してください: ");
    scanf("%d", &addCount);

    for (i = 0; i < count; i++) {
        if (coins[i].moneyType == targetMoney) {
            coins[i].count += addCount;
            found = 1;
            break;
        }
    }

    if (found == 1) {
        saveCoins(coins, count);
        printf("釣銭を補充しました。\n");
    } else {
        printf("指定された金種は見つかりませんでした。\n");
    }
}

/*
 * transactions.csv から取引ログを読み込む関数
 * 読み込んだログ件数を返す。
 */
int loadTransactions(Transaction logs[]) {
    FILE *fp;
    int count = 0;
    char header[200];

    fp = fopen(TRANSACTIONS_FILE, "r");

    if (fp == NULL) {
        printf("transactions.csv が見つかりません。\n");
        return 0;
    }

    /* ヘッダ行を読み飛ばす */
    fgets(header, sizeof(header), fp);

    while (fscanf(fp, "%49[^,],%d,%d,%d,%19[^,],%49[^\n]\n",
                  logs[count].productName,
                  &logs[count].quantity,
                  &logs[count].taxRate,
                  &logs[count].total,
                  logs[count].payment,
                  logs[count].datetime) == 6) {
        count++;

        if (count >= MAX_LOGS) {
            printf("取引ログが上限に達しました。\n");
            break;
        }
    }

    fclose(fp);
    return count;
}

/*
 * 取引ログを一覧表示する関数
 */
void showTransactionLogs(void) {
    Transaction logs[MAX_LOGS];
    int count;
    int i;

    count = loadTransactions(logs);

    if (count == 0) {
        printf("表示できる取引ログがありません。\n");
        return;
    }

    printf("\n===== 取引ログ一覧 =====\n");

    for (i = 0; i < count; i++) {
        printf("%d件目\n", i + 1);
        printf("商品名: %s\n", logs[i].productName);
        printf("個数: %d\n", logs[i].quantity);
        printf("税率: %d%%\n", logs[i].taxRate);
        printf("合計: %d円\n", logs[i].total);
        printf("支払方法: %s\n", logs[i].payment);
        printf("日時: %s\n", logs[i].datetime);
        printf("--------------------\n");
    }
}

/*
 * 売上レポートを表示する関数
 * 取引件数、販売個数、総売上を集計する。
 */
void showSalesReport(void) {
    Transaction logs[MAX_LOGS];
    int count;
    int i;
    int totalSales = 0;
    int totalQuantity = 0;

    count = loadTransactions(logs);

    if (count == 0) {
        printf("集計できる取引ログがありません。\n");
        return;
    }

    for (i = 0; i < count; i++) {
        totalSales += logs[i].total;
        totalQuantity += logs[i].quantity;
    }

    printf("\n===== 売上レポート =====\n");
    printf("取引件数: %d件\n", count);
    printf("販売個数: %d個\n", totalQuantity);
    printf("総売上: %d円\n", totalSales);
}

/*
 * 税率別レポートを表示する関数
 * 8%対象売上と10%対象売上を分けて表示する。
 */
void showTaxReport(void) {
    Transaction logs[MAX_LOGS];
    int count;
    int i;
    int tax8Sales = 0;
    int tax10Sales = 0;
    int otherSales = 0;

    count = loadTransactions(logs);

    if (count == 0) {
        printf("集計できる取引ログがありません。\n");
        return;
    }

    for (i = 0; i < count; i++) {
        if (logs[i].taxRate == 8) {
            tax8Sales += logs[i].total;
        } else if (logs[i].taxRate == 10) {
            tax10Sales += logs[i].total;
        } else {
            otherSales += logs[i].total;
        }
    }

    printf("\n===== 税率別レポート =====\n");
    printf("8%%対象売上: %d円\n", tax8Sales);
    printf("10%%対象売上: %d円\n", tax10Sales);
    printf("その他税率売上: %d円\n", otherSales);
}
