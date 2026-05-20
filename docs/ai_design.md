# セルフレジ自動精算機 — 基本設計書

## 1. システム概要
C言語で作成するターミナル型セルフレジシステム。  
`scanf` / `printf` を使用し、CSVで商品・取引データを管理する。

---

# 2. ディレクトリ構成

```text
project/
├── src/
├── include/
├── data/
│   ├── products.csv
│   ├── transactions.csv
│   └── coins.csv
├── docs/
│   ├── ai_requirements.md
│   ├── comparison.md
│   └── ai_design.md
└── Makefile
```
# 2.1 画面遷移

画面遷移（簡潔版） Start（スタート画面） → Bag（レジ袋選択） → Product（商品スキャン/選択） → ManualInput（バーコードなしは手打ち/Pass入力） → CancelOption（キャンセル選択） → AgeCheck（年齢確認） → Payment（決済選択画面） → Receipt（レシート発行） → ChangeCheck（釣銭チェック） → Start（スタート画面）

---

# 3. データ設計

## products.csv

```csv
product_id,name,price,tax_rate,age_limit
1001,Apple,120,8,0
```

## transactions.csv

```csv
datetime,product_name,quantity,total,payment_method
```

---

# 4. 構造体設計

```c
typedef struct {
    char product_id[5];
    char name[100];
    int price;
    int tax_rate;
} Product;

typedef struct {
    Product product;
    int quantity;
} CartItem;
```

---

# 5. 機能設計

## 起動
- CSV読込
- メニュー表示

## 商品操作
- 商品ID入力
- カート追加
- 数量変更・削除

## 会計
- 小計計算
- 税率別集計（8% / 10%）
- 合計表示

## 支払
- 現金
- カード（疑似）
- 電子決済（疑似）

## ログ保存
- transactions.csv に追記

## 管理者機能
- 商品追加
- 商品編集
- 釣銭補充
- PIN認証

---

# 6. モジュール構成

| ファイル | 役割 |
|---|---|
| main.c | 全体制御 |
| csv.c | CSV処理 |
| product.c | 商品検索 |
| cart.c | カート管理 |
| payment.c | 支払 |
| tax.c | 税計算 |
| admin.c | 管理者機能 |

---

# 7. 関数例

```c
int load_products(const char *filename);
Product* find_product_by_id(const char *id);

void add_to_cart(Product product, int quantity);

int process_cash_payment(int total);

void save_transaction();
```

---

# 8. エラー処理

| エラー | 対応 |
|---|---|
| 商品未登録 | 再入力 |
| CSV読込失敗 | 起動停止 |
| 釣銭不足 | 支払拒否 |
| scanf失敗 | 再入力 |

---

# 9. テスト

- CSV読込確認
- 商品検索確認
- 税率計算確認
- 釣銭計算確認
- ログ保存確認

---
