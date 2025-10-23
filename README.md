# fseek - 檔案指標定位

## 簡介

`fseek()` 是 C 標準函式庫中用於移動檔案讀寫位置指標的函數。它允許你在檔案中自由地跳轉到任意位置進行讀取或寫入操作，而不需要按順序處理檔案內容。

## 函數原型

```c
int fseek(FILE *stream, long offset, int whence);
```

## 參數說明

- **stream**：檔案指標（由 `fopen()` 返回）
- **offset**：偏移量，表示要移動的位元組數
  - 正數：向前移動
  - 負數：向後移動
  - 0：移動到參考點
- **whence**：參考點，決定從哪裡開始計算偏移量
  - `SEEK_SET` (0)：從檔案開頭開始
  - `SEEK_CUR` (1)：從目前位置開始
  - `SEEK_END` (2)：從檔案結尾開始

## 返回值

- 成功：返回 `0`
- 失敗：返回 `-1`，並設定 `errno`

## 使用範例

### 範例 1：讀取檔案第一個字元

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fptr = fopen("example.txt", "r");
    if (fptr == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // 移動到檔案開頭
    fseek(fptr, 0, SEEK_SET);

    char ch = fgetc(fptr);
    if (ch != EOF) {
        printf("First character in the file: %c\n", ch);
    }

    fclose(fptr);
    return 0;
}
```

**輸出：**
```
First character in the file: J
```

### 範例 2：跳到特定位置讀取

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fptr = fopen("example.txt", "r");
    if (fptr == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // 從開頭移動 7 個位元組
    fseek(fptr, 7, SEEK_SET);

    char ch = fgetc(fptr);
    if (ch != EOF) {
        printf("Character at position 7: %c\n", ch);
    }

    fclose(fptr);
    return 0;
}
```

### 範例 3：從目前位置移動

```c
// 從目前位置往後移動 3 個位元組
fseek(fptr, 3, SEEK_CUR);

// 從目前位置往前移動 3 個位元組
fseek(fptr, -3, SEEK_CUR);
```

### 範例 4：移動到檔案結尾

```c
// 移動到檔案結尾
fseek(fptr, 0, SEEK_END);

// 取得檔案大小
long fileSize = ftell(fptr);
printf("File size: %ld bytes\n", fileSize);
```

## 常見應用場景

1. **隨機存取檔案**：直接跳到檔案的特定位置讀取資料
2. **獲取檔案大小**：移動到檔案結尾後使用 `ftell()` 取得位置
3. **重複讀取**：將指標移回檔案開頭或特定位置重新讀取
4. **跳過不需要的資料**：快速跳過檔案中的某些部分

## 相關函數

- `ftell()`：返回目前檔案位置指標的位置
- `rewind()`：將檔案位置指標移回開頭（等同於 `fseek(fp, 0, SEEK_SET)`）
- `fgetpos()` / `fsetpos()`：更精確的位置操作函數

## 注意事項

1. **文字模式限制**：在文字模式下，`fseek()` 的行為可能因系統而異（特別是 Windows 的 CRLF 轉換）
2. **二進位模式建議**：對於需要精確位置控制的操作，建議使用二進位模式（`"rb"`, `"wb"` 等）
3. **錯誤檢查**：應該檢查 `fseek()` 的返回值以確保操作成功
4. **緩衝區問題**：在進行 `fseek()` 操作前，建議使用 `fflush()` 清空輸出緩衝區

## 測試檔案

**example.txt:**
```
Jim, Manager
Ting, Boss
```

使用上述範例程式碼測試不同的 `fseek()` 操作，觀察檔案指標的移動效果。