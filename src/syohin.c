#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "syohin.h"

// Remove newline chars / 末尾改行削除
static void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

// Safe line input / 安全な1行入力
static void read_line(char *buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

// Check 4-digit ID / 4桁ID判定
static int is_4digit_id(const char *s) {
    if (!s || strlen(s) != 4) return 0;
    for (int i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

// Parse integer >= 0 / 0以上整数変換
static int parse_non_negative_int(const char *s, int *out) {
    if (!s || *s == '\0') return 0;
    for (int i = 0; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    long v = strtol(s, NULL, 10);
    if (v < 0 || v > 1000000000L) return 0;
    *out = (int)v;
    return 1;
}

// Parse integer > 0 / 1以上整数変換
static int parse_positive_int(const char *s, int *out) {
    if (!parse_non_negative_int(s, out)) return 0;
    return *out > 0;
}

// Load products.csv / products.csv読み込み
int load_products_from_csv(KioskState *st, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("products.csv open failed");
        return 0;
    }

    char line[LINE_BUF];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (line[0] == '\0') continue;               // Skip empty / 空行スキップ
        if (strncmp(line, "id,", 3) == 0) continue;  // Skip header / ヘッダスキップ

        // CSV parse / CSV分解
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *price_s = strtok(NULL, ",");
        char *barcode = strtok(NULL, ",");
        char *age_s = strtok(NULL, ",");

        if (!id || !name || !price_s || !barcode || !age_s) continue;
        if (!is_4digit_id(id)) continue;

        int price = 0, age = 0;
        if (!parse_positive_int(price_s, &price)) continue;

        // CHANGED: age must be 0 or 20 / 変更: 年齢フラグは0または20
        if (!parse_non_negative_int(age_s, &age) || !(age == 0 || age == 20)) continue;

        if (st->product_count >= MAX_PRODUCTS) break;

        Product *p = &st->products[st->product_count++];
        memset(p, 0, sizeof(*p));
        strncpy(p->id, id, sizeof(p->id) - 1);
        strncpy(p->name, name, sizeof(p->name) - 1);
        strncpy(p->barcode, barcode, sizeof(p->barcode) - 1);
        p->price = price;
        p->age_restricted = age;
        loaded++;
    }

    fclose(fp);
    return loaded;
}

// Append product to products.csv / products.csvへ商品追記
int append_product_to_csv(const char *filename, const Product *p) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        perror("products.csv append failed");
        return 0;
    }
    fprintf(fp, "%s,%s,%d,%s,%d\n",
            p->id, p->name, p->price, p->barcode, p->age_restricted);
    fclose(fp);
    return 1;
}

// Load employees.csv / employees.csv読み込み
int load_employees_from_csv(KioskState *st, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("employees.csv open failed");
        return 0;
    }

    char line[LINE_BUF];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (line[0] == '\0') continue;
        if (strncmp(line, "login_id,", 9) == 0) continue; // header skip / ヘッダスキップ

        char *login_id = strtok(line, ",");
        char *password = strtok(NULL, ",");
        char *barcode = strtok(NULL, ",");
        char *name = strtok(NULL, ",");

        if (!login_id || !password || !barcode || !name) continue;
        if (st->employee_count >= MAX_EMPLOYEES) break;

        Employee *e = &st->employees[st->employee_count++];
        memset(e, 0, sizeof(*e));
        strncpy(e->login_id, login_id, sizeof(e->login_id) - 1);
        strncpy(e->password, password, sizeof(e->password) - 1);
        strncpy(e->barcode, barcode, sizeof(e->barcode) - 1);
        strncpy(e->name, name, sizeof(e->name) - 1);
        loaded++;
    }

    fclose(fp);
    return loaded;
}

// Find product by ID / IDで商品検索
Product *find_product_by_id(KioskState *st, const char *id) {
    for (int i = 0; i < st->product_count; i++) {
        if (strcmp(st->products[i].id, id) == 0) return &st->products[i];
    }
    return NULL;
}

// Find product by barcode / バーコードで商品検索
Product *find_product_by_barcode(KioskState *st, const char *barcode) {
    for (int i = 0; i < st->product_count; i++) {
        if (strcmp(st->products[i].barcode, barcode) == 0) return &st->products[i];
    }
    return NULL;
}

// Find employee by login id / ログインIDで従業員検索
Employee *find_employee_by_login(KioskState *st, const char *login_id) {
    for (int i = 0; i < st->employee_count; i++) {
        if (strcmp(st->employees[i].login_id, login_id) == 0) return &st->employees[i];
    }
    return NULL;
}

// Find employee by barcode / バーコードで従業員検索
Employee *find_employee_by_barcode(KioskState *st, const char *barcode) {
    for (int i = 0; i < st->employee_count; i++) {
        if (strcmp(st->employees[i].barcode, barcode) == 0) return &st->employees[i];
    }
    return NULL;
}

// Admin login (ID + password) / 管理者ログイン（ID+パスワード）
int admin_login(KioskState *st, Employee **out_emp) {
    char id[64], pass[64];

    printf("\n=== 管理者ログイン ===\n");
    printf("社員IDを入力してください: ");
    read_line(id, sizeof(id));
    printf("パスワードを入力してください: ");
    read_line(pass, sizeof(pass));

    Employee *e = find_employee_by_login(st, id);
    if (!e) {
        printf("社員IDが見つかりません\n");
        return 0;
    }
    if (strcmp(e->password, pass) != 0) {
        printf("パスワードが違います\n");
        return 0;
    }

    *out_emp = e;
    printf("ログイン成功: %s\n", e->name);
    return 1;
}

// Add new product from admin menu / 管理メニューから新商品追加
void admin_add_product(KioskState *st, const char *products_csv) {
    if (st->product_count >= MAX_PRODUCTS) {
        printf("商品上限に達しています\n");
        return;
    }

    Product np;
    char buf[128];
    memset(&np, 0, sizeof(np));

    while (1) {
        printf("新しい商品ID（4桁）: ");
        read_line(buf, sizeof(buf));
        if (!is_4digit_id(buf)) {
            printf("4桁数字で入力してください\n");
            continue;
        }
        if (find_product_by_id(st, buf)) {
            printf("そのIDは既に存在します\n");
            continue;
        }
        strncpy(np.id, buf, sizeof(np.id) - 1);
        break;
    }

    printf("商品名: ");
    read_line(np.name, sizeof(np.name));
    if (np.name[0] == '\0') {
        printf("商品名は必須です\n");
        return;
    }

    while (1) {
        int price;
        printf("値段: ");
        read_line(buf, sizeof(buf));
        if (!parse_positive_int(buf, &price)) {
            printf("値段は1以上の整数\n");
            continue;
        }
        np.price = price;
        break;
    }

    while (1) {
        printf("バーコード: ");
        read_line(np.barcode, sizeof(np.barcode));
        if (np.barcode[0] == '\0') {
            printf("バーコードは必須です\n");
            continue;
        }
        if (find_product_by_barcode(st, np.barcode)) {
            printf("そのバーコードは既に存在します\n");
            continue;
        }
        break;
    }

    while (1) {
        int age;
        // CHANGED: prompt 0/20 / 変更: 表示を0/20へ
        printf("年齢確認フラグ (0=不要,20=必要): ");
        read_line(buf, sizeof(buf));

        // CHANGED: validate 0 or 20 / 変更: 0または20のみ許可
        if (!parse_non_negative_int(buf, &age) || !(age == 0 || age == 20)) {
            printf("0 または 20 を入力してください\n");
            continue;
        }
        np.age_restricted = age;
        break;
    }

    if (!append_product_to_csv(products_csv, &np)) {
        printf("CSV追加に失敗しました\n");
        return;
    }

    st->products[st->product_count++] = np;
    printf("商品を追加しました: [%s] %s\n", np.id, np.name);
}

// Admin menu / 管理者メニュー
void admin_menu(KioskState *st, const char *products_csv) {
    Employee *emp = NULL;
    if (!admin_login(st, &emp)) return;

    char cmd[32];
    while (1) {
        printf("\n=== 管理者メニュー ===\n");
        printf("1 = 商品追加 (products.csv)\n");
        printf("0 = 戻る\n");
        printf("選択: ");
        read_line(cmd, sizeof(cmd));

        if (strcmp(cmd, "0") == 0) return;
        if (strcmp(cmd, "1") == 0) admin_add_product(st, products_csv);
        else printf("無効な選択です\n");
    }
}

// Find cart line by product ID / 商品IDでカート行検索
int find_cart_index(KioskState *st, const char *id) {
    for (int i = 0; i < st->cart_count; i++) {
        if (strcmp(st->cart[i].product.id, id) == 0) return i;
    }
    return -1;
}

// Add to cart / カート追加
void add_to_cart(KioskState *st, Product *p, int qty) {
    int idx = find_cart_index(st, p->id);
    if (idx >= 0) {
        st->cart[idx].quantity += qty;
        return;
    }
    if (st->cart_count >= MAX_CART_ITEMS) {
        printf("カート上限です\n");
        return;
    }
    st->cart[st->cart_count].product = *p;
    st->cart[st->cart_count].quantity = qty;
    st->cart_count++;
}

// Remove cart line / カート行削除
void remove_cart_item(KioskState *st, int index) {
    if (index < 0 || index >= st->cart_count) return;
    for (int i = index; i < st->cart_count - 1; i++) st->cart[i] = st->cart[i + 1];
    st->cart_count--;
}

// Update quantity / 数量更新（0以下で削除）
void update_cart_quantity(KioskState *st, int index, int qty) {
    if (index < 0 || index >= st->cart_count) return;
    if (qty <= 0) remove_cart_item(st, index);
    else st->cart[index].quantity = qty;
}

// Calculate total / 合計計算
int cart_total(KioskState *st) {
    int total = 0;
    for (int i = 0; i < st->cart_count; i++) {
        total += st->cart[i].product.price * st->cart[i].quantity;
    }
    return total;
}

// Display cart / カート表示
void show_cart(KioskState *st) {
    printf("\n===== カート =====\n");
    if (st->cart_count == 0) {
        printf("（カートは空です）\n");
        return;
    }

    for (int i = 0; i < st->cart_count; i++) {
        int sub = st->cart[i].product.price * st->cart[i].quantity;
        printf("%d. [%s] %s x %d = %d円\n",
               i + 1, st->cart[i].product.id, st->cart[i].product.name,
               st->cart[i].quantity, sub);
    }
    printf("合計: %d円\n", cart_total(st));
}

// Age confirmation (employee barcode only) / 年齢確認（従業員バーコードのみ）
int staff_age_confirmation(KioskState *st, const Product *p) {
    // CHANGED: only 20 requires confirmation / 変更: 20のときだけ確認する
    if (p->age_restricted == 0) return 1;
    if (p->age_restricted != 20) return 1; // unknown value -> allow / 想定外値は許可（必要なら拒否に変更）

    char emp_barcode[64];
    char decision[16];

    while (1) {
        printf("\n【年齢確認】この商品は年齢確認が必要です: %s\n", p->name);
        printf("店員バーコードをスキャンしてください: ");
        read_line(emp_barcode, sizeof(emp_barcode));

        Employee *e = find_employee_by_barcode(st, emp_barcode);
        if (!e) {
            printf("店員バーコードが無効です\n");
            continue;
        }

        printf("確認担当: %s\n", e->name);
        printf("お客様は18歳以上ですか？ 1=はい / 0=いいえ : ");
        read_line(decision, sizeof(decision));

        if (strcmp(decision, "1") == 0) return 1;
        if (strcmp(decision, "0") == 0) return 0;
        printf("入力は 1 か 0 を入力してください\n");
    }
}

// Input quantity / 数量入力
int input_quantity() {
    char q[32];
    int qty;
    while (1) {
        printf("数量を入力してください: ");
        read_line(q, sizeof(q));
        if (parse_positive_int(q, &qty)) return qty;
        printf("数量は1以上の整数で入力してください\n");
    }
}

// Input product by barcode or 4-digit ID / バーコードまたは4桁ID入力
Product *input_product(KioskState *st) {
    char key[64];
    while (1) {
        printf("\nバーコードを入力してください\n");
        printf("0 を入力すると終了します\n");
        printf("（4桁の商品IDでも可）\n");

        read_line(key, sizeof(key));
        if (key[0] == '\0') continue;
        if (strcmp(key, "0") == 0) return NULL;

        Product *p = NULL;
        if (is_4digit_id(key)) p = find_product_by_id(st, key);
        if (!p) p = find_product_by_barcode(st, key);

        if (!p) {
            printf("商品未登録です\n");
            continue;
        }

        printf("商品: [%s] %s / %d円\n", p->id, p->name, p->price);
        return p;
    }
}

// Cart edit menu / カート編集メニュー
void cart_edit_menu(KioskState *st) {
    char buf[64];
    while (1) {
        show_cart(st);
        if (st->cart_count == 0) return;

        printf("\n1 = 数量変更\n");
        printf("2 = 商品を削除\n");
        printf("0 = 戻る\n");
        printf("選択してください: ");
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "0") == 0) return;

        if (strcmp(buf, "1") == 0) {
            char no_s[32], qty_s[32];
            int no, qty;

            printf("変更したい商品の番号を入力してください: ");
            read_line(no_s, sizeof(no_s));
            if (!parse_positive_int(no_s, &no) || no < 1 || no > st->cart_count) {
                printf("無効な番号です\n");
                continue;
            }

            printf("新しい数量を入力してください（0で削除）: ");
            read_line(qty_s, sizeof(qty_s));

            if (strcmp(qty_s, "0") == 0) {
                update_cart_quantity(st, no - 1, 0);
                printf("削除しました\n");
                continue;
            }

            if (!parse_positive_int(qty_s, &qty)) {
                printf("無効な数量です\n");
                continue;
            }

            update_cart_quantity(st, no - 1, qty);
            printf("更新しました\n");
        } else if (strcmp(buf, "2") == 0) {
            char no_s[32];
            int no;
            printf("削除したい商品の番号を入力してください: ");
            read_line(no_s, sizeof(no_s));
            if (!parse_positive_int(no_s, &no) || no < 1 || no > st->cart_count) {
                printf("無効な番号です\n");
                continue;
            }
            remove_cart_item(st, no - 1);
            printf("商品を削除しました\n");
        } else {
            printf("無効な選択です\n");
        }
    }
}

// Main operation loop / メイン処理ループ
void product_add_loop(KioskState *st, const char *products_csv) {
    char cmd[32];

    while (1) {
        printf("\n=== メインメニュー ===\n");
        printf("1 = 商品スキャン開始\n");
        printf("9 = 管理者メニュー\n");
        printf("0 = 終了\n");
        printf("選択: ");
        read_line(cmd, sizeof(cmd));

        if (strcmp(cmd, "0") == 0) return;
        if (strcmp(cmd, "9") == 0) {
            admin_menu(st, products_csv);
            continue;
        }
        if (strcmp(cmd, "1") != 0) {
            printf("無効な選択です\n");
            continue;
        }

        while (1) {
            Product *p = input_product(st);
            if (!p) break;

            if (!staff_age_confirmation(st, p)) {
                printf("年齢確認NGのため、この商品は追加できません\n");
                continue;
            }

            int qty = input_quantity();
            add_to_cart(st, p, qty);
            printf("%s をカートに追加しました\n", p->name);

            while (1) {
                show_cart(st);
                printf("\n1 = 続けて入力\n");
                printf("2 = カート編集\n");
                printf("3 = メインメニューへ戻る\n");
                printf("選択してください: ");
                read_line(cmd, sizeof(cmd));

                if (strcmp(cmd, "1") == 0) break;
                if (strcmp(cmd, "2") == 0) {
                    cart_edit_menu(st);
                    continue;
                }
                if (strcmp(cmd, "3") == 0) goto BACK_MAIN;
                printf("無効な選択です\n");
            }
        }
BACK_MAIN:
        ;
    }
}

// Program entry / プログラム開始
int syohin() {
    KioskState st;
    memset(&st, 0, sizeof(st));

    const char *products_csv = "products.csv";
    const char *employees_csv = "employees.csv";

    int loaded_products = load_products_from_csv(&st, products_csv);
    int loaded_employees = load_employees_from_csv(&st, employees_csv);

    if (loaded_products <= 0) {
        printf("商品CSVの読み込みに失敗しました: %s\n", products_csv);
        return 1;
    }
    if (loaded_employees <= 0) {
        printf("社員CSVの読み込みに失敗しました: %s\n", employees_csv);
        return 1;
    }

    printf("商品マスタ読み込み完了: %d 件\n", loaded_products);
    printf("社員マスタ読み込み完了: %d 件\n", loaded_employees);

    product_add_loop(&st, products_csv);

    printf("\n===== お会計 =====\n");
    show_cart(&st);
    printf("合計金額: %d円\n", cart_total(&st));
    printf("ありがとうございました\n");
    return 0;
}

int main() {
    syohin();
    return 0;
}