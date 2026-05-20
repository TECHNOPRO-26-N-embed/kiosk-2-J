// ===== Limits / 上限設定 =====
#define MAX_PRODUCTS 1500      // Max products from CSV / CSVから読み込む最大商品数
#define MAX_NAME_LEN 64        // Max product name length / 商品名の最大長
#define MAX_CART_ITEMS 300     // Max unique cart lines / カート内の最大商品行数
#define MAX_EMPLOYEES 200      // Max employees from CSV / CSVから読み込む最大従業員数
#define LINE_BUF 512           // Input line buffer / 1行読み込みバッファ

// Product data / 商品データ
typedef struct {
    char id[5];                // 4-digit product ID / 4桁商品ID
    char name[MAX_NAME_LEN];   // Product name / 商品名
    int price;                 // Price in yen / 価格（円）
    char barcode[32];          // Product barcode / 商品バーコード
    int age_restricted;        // 0=no check, 1=age check needed / 0=不要,1=年齢確認必要
} Product;

// Cart line / カート1行
typedef struct {
    Product product;           // Product info / 商品情報
    int quantity;              // Quantity / 数量
} CartItem;

// Employee data / 従業員データ
typedef struct {
    char login_id[32];         // Admin login ID / 管理ログインID
    char password[32];         // Admin password / 管理パスワード
    char barcode[32];          // Employee barcode for age check / 年齢確認用バーコード
    char name[32];             // Employee name / 従業員名
} Employee;

// Application state / アプリ全体状態
typedef struct {
    Product products[MAX_PRODUCTS];  // Product master / 商品マスタ
    int product_count;               // Number of products / 商品数

    CartItem cart[MAX_CART_ITEMS];   // Cart data / カートデータ
    int cart_count;                  // Number of cart lines / カート行数

    Employee employees[MAX_EMPLOYEES]; // Employee master / 従業員マスタ
    int employee_count;                // Number of employees / 従業員数
} KioskState;
