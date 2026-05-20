#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ===== System limits =====
   MAX_PRODUCTS   : max number of products loaded from CSV
   MAX_CART_ITEMS : max number of unique products in cart
*/
#define MAX_PRODUCTS 1000
#define MAX_NAME_LEN 64
#define MAX_CART_ITEMS 300
#define LINE_BUF 512

/* Product master data (loaded from CSV)
   id            : 4-digit product id (e.g. "1001")
   name          : product name
   price         : price in yen
   barcode       : barcode text
   age_restricted: 0 = no age check, 1 = age check required
*/
typedef struct {
    char id[5];
    char name[MAX_NAME_LEN];
    int price;
    char barcode[32];
    int age_restricted;
} Product;

/* One line in cart:
   which product + how many units
*/
typedef struct {
    Product product;
    int quantity;
} CartItem;

/* Whole application state:
   - product master list
   - current cart
*/
typedef struct {
    Product products[MAX_PRODUCTS];
    int product_count;

    CartItem cart[MAX_CART_ITEMS];
    int cart_count;
} KioskState;

/* =========================
   Utility helper functions
   ========================= */

/* Remove trailing '\n' / '\r' from fgets input */
static void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

/* Safe line input wrapper (uses fgets) */
static void read_line(char *buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

/* Check if string is exactly 4 digits */
static int is_4digit_id(const char *s) {
    if (!s || strlen(s) != 4) return 0;
    for (int i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

/* Parse integer >= 0 from string */
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

/* Parse integer > 0 from string */
static int parse_positive_int(const char *s, int *out) {
    if (!parse_non_negative_int(s, out)) return 0;
    return *out > 0;
}

/* =========================
   Product CSV loading
   =========================
   CSV format:
   id,name,price,barcode,age
   1001,コーラ,120,111111,0
   1002,ビール,250,999999,1
*/
int load_products_from_csv(KioskState *st, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("CSV open failed");
        return 0;
    }

    char line[LINE_BUF];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);

        /* skip empty line */
        if (line[0] == '\0') continue;

        /* skip header row */
        if (strncmp(line, "id,", 3) == 0) continue;

        /* split CSV columns */
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *price_s = strtok(NULL, ",");
        char *barcode = strtok(NULL, ",");
        char *age_s = strtok(NULL, ",");

        /* require all 5 columns */
        if (!id || !name || !price_s || !barcode || !age_s) continue;

        /* validate id and numeric columns */
        if (!is_4digit_id(id)) continue;

        int price = 0, age = 0;
        if (!parse_positive_int(price_s, &price)) continue;
        if (!parse_non_negative_int(age_s, &age)) continue;
        if (!(age == 0 || age == 1)) continue;

        if (st->product_count >= MAX_PRODUCTS) break;

        /* store product in memory */
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

/* =========================
   Product search functions
   ========================= */

/* Search product by 4-digit ID */
Product *find_product_by_id(KioskState *st, const char *id) {
    for (int i = 0; i < st->product_count; i++) {
        if (strcmp(st->products[i].id, id) == 0) return &st->products[i];
    }
    return NULL;
}

/* Search product by barcode */
Product *find_product_by_barcode(KioskState *st, const char *barcode) {
    for (int i = 0; i < st->product_count; i++) {
        if (strcmp(st->products[i].barcode, barcode) == 0) return &st->products[i];
    }
    return NULL;
}

/* =========================
   Age confirmation logic
   ========================= */

/* If product is age-restricted (age=1), ask staff confirmation.
   return 1: allowed
   return 0: not allowed
*/
int staff_age_confirmation(const Product *p) {
    if (p->age_restricted == 0) return 1; /* no check needed */

    char buf[16];
    while (1) {
        printf("\n【年齢確認】この商品は年齢確認が必要です: %s\n", p->name);
        printf("お客様は18歳以上ですか？（店員確認）\n");
        printf("1 = はい / 0 = いいえ : ");
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "1") == 0) return 1; /* allow add */
        if (strcmp(buf, "0") == 0) return 0; /* deny add */

        printf("入力は 1 か 0 で入力してください\n");
    }
}

/* =========================
   Cart functions
   ========================= */

/* Find existing cart item index by product id */
int find_cart_index(KioskState *st, const char *id) {
    for (int i = 0; i < st->cart_count; i++) {
        if (strcmp(st->cart[i].product.id, id) == 0) return i;
    }
    return -1;
}

/* Add product to cart
   - if already exists: increase quantity
   - if new: append cart item
*/
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

/* Remove one cart line completely */
void remove_cart_item(KioskState *st, int index) {
    if (index < 0 || index >= st->cart_count) return;
    for (int i = index; i < st->cart_count - 1; i++) {
        st->cart[i] = st->cart[i + 1];
    }
    st->cart_count--;
}

/* Change quantity (qty <= 0 means delete item) */
void update_cart_quantity(KioskState *st, int index, int qty) {
    if (index < 0 || index >= st->cart_count) return;
    if (qty <= 0) remove_cart_item(st, index);
    else st->cart[index].quantity = qty;
}

/* Calculate total amount of cart */
int cart_total(KioskState *st) {
    int total = 0;
    for (int i = 0; i < st->cart_count; i++) {
        total += st->cart[i].product.price * st->cart[i].quantity;
    }
    return total;
}

/* Print cart contents */
void show_cart(KioskState *st) {
    printf("\n===== カート =====\n");
    if (st->cart_count == 0) {
        printf("（カートは空です）\n");
        return;
    }

    for (int i = 0; i < st->cart_count; i++) {
        int sub = st->cart[i].product.price * st->cart[i].quantity;
        printf("%d. [%s] %s x %d = %d円\n",
               i + 1,
               st->cart[i].product.id,
               st->cart[i].product.name,
               st->cart[i].quantity,
               sub);
    }
    printf("合計: %d円\n", cart_total(st));
}

/* =========================
   UI menus / flow
   ========================= */

/* Cart edit menu:
   1) quantity change
   2) delete item
   0) back
*/
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

/* Ask user to input barcode or 4-digit id
   Keep your requested prompt style:
   - バーコードを入力してください
   - 0 を入力すると終了します
*/
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

/* Ask quantity input (>=1) */
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

/* Main product add loop
   1) input product
   2) age confirmation if needed
   3) input quantity + add to cart
   4) continue / cart edit / checkout
*/
void product_add_loop(KioskState *st) {
    char cmd[32];

    while (1) {
        Product *p = input_product(st);
        if (!p) break; /* user selected finish */

        /* age check gate */
        if (!staff_age_confirmation(p)) {
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
            printf("3 = 会計へ進む\n");
            printf("選択してください: ");
            read_line(cmd, sizeof(cmd));

            if (strcmp(cmd, "1") == 0) break;
            if (strcmp(cmd, "2") == 0) {
                cart_edit_menu(st);
                continue;
            }
            if (strcmp(cmd, "3") == 0) return;
            printf("無効な選択です\n");
        }
    }
}

/* Entry point:
   - load product master from CSV
   - start add loop
   - show final checkout
*/
int main(void) {
    KioskState st;
    memset(&st, 0, sizeof(st));

    const char *csv_file = "products.csv";
    int loaded = load_products_from_csv(&st, csv_file);

    if (loaded <= 0) {
        printf("商品CSVの読み込みに失敗しました: %s\n", csv_file);
        return 1;
    }

    printf("商品マスタ読み込み完了: %d 件\n", loaded);

    product_add_loop(&st);

    printf("\n===== お会計 =====\n");
    show_cart(&st);
    printf("合計金額: %d円\n", cart_total(&st));
    printf("ありがとうございました\n");

    return 0;
}