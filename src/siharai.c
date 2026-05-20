#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 10
#define DENOM_COUNT 10

// プロトタイプ宣言（main の前に関数を宣言）
void siharai(void);

// 商品リスト（固定）
const char *items[] = {"りんご", "パン", "牛乳", "チョコ"};
const int prices[] = {100, 150, 120, 200}; // 円
const int n_items = 4;

// 通貨（円）: 大きい順
int denoms[DENOM_COUNT] = {10000, 5000, 2000, 1000, 500, 100, 50, 10, 5, 1};

// allcoins は機械内にある各額面の枚数在庫（初期値）
int allcoins[DENOM_COUNT] = {5, 5, 2, 10, 20, 50, 50, 100, 100, 500};

// 関数：おつりを在庫を見て作れるか試す（貪欲法 + 在庫制約）
int try_make_change(int change, int temp_inv[], int change_dispensed[]) {
    int i;
    for (i = 0; i < DENOM_COUNT; i++) change_dispensed[i] = 0;
    for (i = 0; i < DENOM_COUNT; i++) {
        if (change <= 0) break;
        int use = change / denoms[i];
        if (use > temp_inv[i]) use = temp_inv[i];
        change_dispensed[i] = use;
        change -= use * denoms[i];
    }
    return (change == 0);
}

// ヘルパー：配列 a += b （長さ DENOM_COUNT）
void add_counts(int a[], int b[]) {
    for (int i = 0; i < DENOM_COUNT; i++) a[i] += b[i];
}

// ヘルパー：配列 a -= b
void sub_counts(int a[], int b[]) {
    for (int i = 0; i < DENOM_COUNT; i++) a[i] -= b[i];
}

// レシート表示（簡潔）
void print_receipt(int cart[]) {
    int total = 0;
    printf("\n--- レシート ---\n");
    for (int i = 0; i < n_items; i++) {
        if (cart[i] > 0) {
            int line = cart[i] * prices[i];
            printf("%s x%d = %d円\n", items[i], cart[i], line);
            total += line;
        }
    }
    if (total == 0) {
        printf("カートは空です。\n");
    } else {
        printf("合計: %d円\n", total);
    }
    printf("-----------------\n");
}

// 元の大きな main を siharai 関数に移動（void 関数）
void siharai(void) {
    // 乱数初期化（カード承認の簡易乱数用）
    srand((unsigned int)time(NULL));

    // カートの宣言（値は最初に 0 にしておく）
    int cart[MAX_ITEMS];
    int i;
    for (i = 0; i < n_items; i++) cart[i] = 0; // 最初の初期化（レシート後にもリセットあり）

    int running = 1;
    int choice;

    while (running) {
        // メニュー表示（商品選択）
        printf("\n--- セルフレジ ---\n");
        for (i = 0; i < n_items; i++) {
            printf("%d: %s %d円\n", i + 1, items[i], prices[i]);
        }
        printf("0: 会計（次に進む）\n");      // 次に進むは 0
        printf("9: 最初に戻る（カートをクリア）\n"); // 最初に戻るは 9
        printf("番号を入力してください: ");

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("数字で入力してください。\n");
            continue;
        }

        if (choice == 0) {
            // 会計へ（支払い処理）
            int total = 0;
            for (i = 0; i < n_items; i++) total += cart[i] * prices[i];

            if (total == 0) {
                printf("カートは空です。商品を選んでください。\n");
                continue;
            }

            // 支払方法選択ループ
            int payment_done = 0;
            while (!payment_done) {
                int pay_choice;
                printf("\n合計 %d 円です。\n", total);
                printf("1: 現金\n2: カード/電子\n9: 最初に戻る（支払中止、カートクリア）\n");
                printf("支払方法を選んでください: ");
                if (scanf("%d", &pay_choice) != 1) {
                    int c; while ((c = getchar()) != '\n' && c != EOF) { }
                    printf("数字で入力してください。\n");
                    continue;
                }

                if (pay_choice == 9) {
                    // 最初に戻る：カートをクリアして支払中止
                    for (i = 0; i < n_items; i++) cart[i] = 0; // カートクリア（最初に戻る）
                    printf("最初に戻りました。カートをクリアしました。\n");
                    payment_done = 1; // 戻ってメニューへ
                    break;
                }
                else if (pay_choice == 1) {
                    // 現金支払処理
                    int inserted_sum = 0;
                    int inserted_counts[DENOM_COUNT];
                    for (i = 0; i < DENOM_COUNT; i++) inserted_counts[i] = 0;

                    printf("\n現金投入口へ（額面を入力して投入してください）。\n");
                    printf("利用可能な額面：");
                    for (i = 0; i < DENOM_COUNT; i++) {
                        printf("%d円", denoms[i]);
                        if (i < DENOM_COUNT - 1) printf(", ");
                    }
                    printf("\n");
                    printf("9 を入力すると支払中止（投入金を返却）して最初に戻ります。\n");

                    // 投入ループ
                    while (inserted_sum < total) {
                        int put;
                        printf("残額 %d 円。投入額を入力してください: ", total - inserted_sum);
                        if (scanf("%d", &put) != 1) {
                            int c; while ((c = getchar()) != '\n' && c != EOF) { }
                            printf("数字で入力してください。\n");
                            continue;
                        }
                        if (put == 9) {
                            // キャンセル
                            if (inserted_sum > 0) {
                                printf("支払中止。投入金 %d 円を返却します（疑似）。\n", inserted_sum);
                            } else {
                                printf("支払中止しました。\n");
                            }
                            // 支払中断、カートはクリア（最初に戻るルール）
                            for (i = 0; i < n_items; i++) cart[i] = 0;
                            payment_done = 1;
                            break;
                        }
                        // 入力された金額が既知の額面か確認
                        int idx = -1;
                        for (i = 0; i < DENOM_COUNT; i++) {
                            if (put == denoms[i]) { idx = i; break; }
                        }
                        if (idx == -1) {
                            printf("その額面は受け付けません。上の額面を入力してください。\n");
                            continue;
                        }
                        // 受け取り（まだ allcoins に反映しないで inserted_counts に記録）
                        inserted_counts[idx] += 1;
                        inserted_sum += put;
                        printf("%d円を受け取りました。合計投入 %d 円。\n", put, inserted_sum);
                    } // 投入ループ終了

                    if (payment_done) break; // キャンセルでメニューへ戻る

                    // 合計投入 >= total
                    int change = inserted_sum - total;

                    // 在庫を含めた一時的な在庫を作る（allcoins + inserted_counts）
                    int temp_inv[DENOM_COUNT];
                    for (i = 0; i < DENOM_COUNT; i++) temp_inv[i] = allcoins[i] + inserted_counts[i];

                    int change_dispensed[DENOM_COUNT];
                    int ok = try_make_change(change, temp_inv, change_dispensed);

                    if (!ok) {
                        // 釣銭不足のため現金では支払えない（指示のメッセージ）
                        printf("釣銭不足のため現金では支払えません。カード/電子/現金で正確な金額を投入してください\n");
                        // 支払を中断し、投入金を返却する（疑似）
                        printf("投入金 %d 円を返却します（疑似）。\n", inserted_sum);
                        // そのまま支払方法選択へ戻す（allcoins は更新しない）
                        continue;
                    } else {
                        // 成功：allcoins を更新（受け取り分を加え、払い出した釣銭を差し引く）
                        for (i = 0; i < DENOM_COUNT; i++) {
                            allcoins[i] = temp_inv[i] - change_dispensed[i];
                        }
                        // 釣銭を表示
                        if (change > 0) {
                            printf("おつり %d 円\n", change);
                            for (i = 0; i < DENOM_COUNT; i++) {
                                if (change_dispensed[i] > 0) {
                                    printf("%d円 x %d\n", denoms[i], change_dispensed[i]);
                                }
                            }
                        } else {
                            printf("ちょうどです。ありがとうございました。\n");
                        }
                        // 支払成功 -> 取引確定
                        printf("現金支払が成功しました。\n");
                        // レシート表示（取引確定後）
                        print_receipt(cart);
                        // ここで変数の初期化（レシート後）：カートをリセット
                        for (i = 0; i < n_items; i++) cart[i] = 0; // ← 変数の初期化はレシート後
                        payment_done = 1;
                        break;
                    }

                } // 現金処理 end

                else if (pay_choice == 2) {
                    // カード/電子（疑似承認）
                    printf("\nカード/電子の処理を開始します（疑似承認）...\n");
                    // 簡易ランダム承認（成功率 85%）
                    int r = rand() % 100;
                    if (r < 85) {
                        // 承認成功
                        printf("承認成功。\n");
                        // 取引確定
                        print_receipt(cart);
                        // 変数初期化（レシート後）
                        for (i = 0; i < n_items; i++) cart[i] = 0; // ← ここでリセット
                        payment_done = 1;
                        break;
                    } else {
                        // 承認失敗
                        printf("承認失敗しました。支払方法の選択に戻ります。\n");
                        continue; // 支払方法選択へ戻る
                    }
                }
                else {
                    printf("正しい番号を入力してください。\n");
                }
            } // 支払方法選択ループ end
        }
        else if (choice == 9) {
            // 最初に戻る（メニュー）
            for (i = 0; i < n_items; i++) cart[i] = 0; // カートクリア（最初に戻る）
            printf("最初に戻りました。カートをクリアしました。\n");
        }
        else if (1 <= choice && choice <= n_items) {
            // 商品を1つ追加
            cart[choice - 1] += 1;
            printf("%s をカートに追加しました。現在 %d 個です。\n", items[choice - 1], cart[choice - 1]);
            printf("続ける場合は商品番号、会計は0、最初に戻るは9を入力してください。\n");
        }
        else {
            printf("正しい番号を入力してください。\n");
        }
    } // while running
}
 
// 実行エントリポイントはここだけ
int main(void) {
    siharai(); // 実際の処理は siharai 関数で行う
    return 0;
}