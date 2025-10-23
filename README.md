# File I/O 完整指南

## 總目錄

### [1. fseek - 檔案指標定位](#1-fseek---檔案指標定位-1)
- [1.1 簡介](#11-簡介)
- [1.2 函數原型](#12-函數原型)
- [1.3 參數說明](#13-參數說明)
- [1.4 返回值](#14-返回值)
- [1.5 使用範例](#15-使用範例)
- [1.6 常見應用場景](#16-常見應用場景)
- [1.7 相關函數](#17-相關函數)
- [1.8 注意事項](#18-注意事項)

### [2. stat - 檔案資訊查詢](#2-stat---檔案資訊查詢-1)
- [2.1 簡介](#21-簡介)
- [2.2 函數原型](#22-函數原型)
- [2.3 參數說明](#23-參數說明)
- [2.4 返回值](#24-返回值)
- [2.5 struct stat 結構體](#25-struct-stat-結構體)
- [2.6 常用欄位說明](#26-常用欄位說明)
- [2.7 檔案類型判斷巨集](#27-檔案類型判斷巨集)
- [2.8 使用範例](#28-使用範例)
- [2.9 常見應用場景](#29-常見應用場景)
- [2.10 相關函數](#210-相關函數)
- [2.11 注意事項](#211-注意事項)

---

# 1. fseek - 檔案指標定位

## 1.1 簡介

`fseek()` 是 C 標準函式庫中用於移動檔案讀寫位置指標的函數。它允許你在檔案中自由地跳轉到任意位置進行讀取或寫入操作，而不需要按順序處理檔案內容。

## 1.2 函數原型

```c
int fseek(FILE *stream, long offset, int whence);
```

## 1.3 參數說明

- **stream**：檔案指標（由 `fopen()` 返回）
- **offset**：偏移量，表示要移動的位元組數
  - 正數：向前移動
  - 負數：向後移動
  - 0：移動到參考點
- **whence**：參考點，決定從哪裡開始計算偏移量
  - `SEEK_SET` (0)：從檔案開頭開始
  - `SEEK_CUR` (1)：從目前位置開始
  - `SEEK_END` (2)：從檔案結尾開始

## 1.4 返回值

- 成功：返回 `0`
- 失敗：返回 `-1`，並設定 `errno`

## 1.5 使用範例

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

## 1.6 常見應用場景

1. **隨機存取檔案**：直接跳到檔案的特定位置讀取資料
2. **獲取檔案大小**：移動到檔案結尾後使用 `ftell()` 取得位置
3. **重複讀取**：將指標移回檔案開頭或特定位置重新讀取
4. **跳過不需要的資料**：快速跳過檔案中的某些部分

## 1.7 相關函數

- `ftell()`：返回目前檔案位置指標的位置
- `rewind()`：將檔案位置指標移回開頭（等同於 `fseek(fp, 0, SEEK_SET)`）
- `fgetpos()` / `fsetpos()`：更精確的位置操作函數

## 1.8 注意事項

1. **文字模式限制**：在文字模式下，`fseek()` 的行為可能因系統而異（特別是 Windows 的 CRLF 轉換）
2. **二進位模式建議**：對於需要精確位置控制的操作，建議使用二進位模式（`"rb"`, `"wb"` 等）
3. **錯誤檢查**：應該檢查 `fseek()` 的返回值以確保操作成功
4. **緩衝區問題**：在進行 `fseek()` 操作前，建議使用 `fflush()` 清空輸出緩衝區

**測試檔案 (example.txt):**
```
Jim, Manager
Ting, Boss
```

---

# 2. stat - 檔案資訊查詢

## 2.1 簡介

`stat()` 是 POSIX 系統呼叫，用於取得檔案或目錄的詳細資訊（metadata），包括檔案大小、類型、權限、修改時間等。這是一個非常實用的函數，可以在不開啟檔案的情況下獲取檔案的各種屬性。

## 2.2 函數原型

```c
#include <sys/stat.h>
#include <sys/types.h>

int stat(const char *pathname, struct stat *statbuf);
```

## 2.3 參數說明

- **pathname**：檔案或目錄的路徑（字串）
- **statbuf**：指向 `struct stat` 結構體的指標，用於存放檔案資訊

## 2.4 返回值

- 成功：返回 `0`
- 失敗：返回 `-1`，並設定 `errno`

## 2.5 struct stat 結構體

```c
struct stat {
    dev_t     st_dev;      // 裝置 ID
    ino_t     st_ino;      // inode 編號
    mode_t    st_mode;     // 檔案類型和權限
    nlink_t   st_nlink;    // 硬連結數量
    uid_t     st_uid;      // 擁有者的 user ID
    gid_t     st_gid;      // 擁有者的 group ID
    dev_t     st_rdev;     // 特殊檔案的裝置 ID
    off_t     st_size;     // 檔案大小（位元組）
    blksize_t st_blksize;  // I/O 區塊大小
    blkcnt_t  st_blocks;   // 已分配的區塊數
    time_t    st_atime;    // 最後存取時間
    time_t    st_mtime;    // 最後修改時間
    time_t    st_ctime;    // 最後狀態變更時間
};
```

## 2.6 常用欄位說明

| 欄位 | 說明 |
|------|------|
| `st_size` | 檔案大小（位元組數） |
| `st_mode` | 檔案類型和權限 |
| `st_mtime` | 最後修改時間（檔案內容） |
| `st_atime` | 最後存取時間 |
| `st_ctime` | 最後狀態變更時間（權限、擁有者等） |

## 2.7 檔案類型判斷巨集

```c
S_ISREG(m)   // 是否為一般檔案
S_ISDIR(m)   // 是否為目錄
S_ISLNK(m)   // 是否為符號連結
S_ISCHR(m)   // 是否為字元裝置
S_ISBLK(m)   // 是否為區塊裝置
S_ISFIFO(m)  // 是否為 FIFO (named pipe)
S_ISSOCK(m)  // 是否為 socket
```

## 2.8 使用範例

### 範例 1：基本資訊查詢

```c
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main() {
    struct stat info;

    // 呼叫 stat()
    if (stat("example.txt", &info) == 0) {
        // 印出檔案大小
        printf("File size: %ld bytes\n", info.st_size);

        // 印出修改時間
        printf("Last modified: %s", ctime(&info.st_mtime));

        // 判斷是否為一般檔案
        if (S_ISREG(info.st_mode))
            printf("Type: Regular file\n");
        else if (S_ISDIR(info.st_mode))
            printf("Type: Directory\n");
        else
            printf("Type: Other\n");
    } else {
        perror("stat");
    }

    return 0;
}
```

**輸出範例：**
```
File size: 25 bytes
Last modified: Wed Oct 22 10:30:45 2025
Type: Regular file
```

### 範例 2：檢查目錄

```c
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    struct stat folder;

    if (stat("example", &folder) == 0) {
        if (S_ISDIR(folder.st_mode)) {
            printf("Type: Directory\n");
        } else {
            printf("Not a directory.\n");
        }
    } else {
        perror("stat (example)");
    }

    return 0;
}
```

### 範例 3：檢查檔案是否存在

```c
#include <stdio.h>
#include <sys/stat.h>

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main() {
    if (file_exists("example.txt")) {
        printf("File exists!\n");
    } else {
        printf("File does not exist.\n");
    }

    return 0;
}
```

### 範例 4：比較檔案修改時間

```c
#include <stdio.h>
#include <sys/stat.h>

int main() {
    struct stat file1, file2;

    stat("file1.txt", &file1);
    stat("file2.txt", &file2);

    if (file1.st_mtime > file2.st_mtime) {
        printf("file1.txt is newer\n");
    } else if (file1.st_mtime < file2.st_mtime) {
        printf("file2.txt is newer\n");
    } else {
        printf("Both files have the same modification time\n");
    }

    return 0;
}
```

## 2.9 常見應用場景

1. **檢查檔案是否存在**：在開啟檔案前先確認
2. **取得檔案大小**：用於記憶體分配或進度顯示
3. **判斷路徑類型**：確認是檔案還是目錄
4. **檢查檔案權限**：確認是否有讀寫權限
5. **比較檔案時間**：用於備份、同步等操作
6. **目錄遍歷**：配合 `opendir()` 和 `readdir()` 使用

## 2.10 相關函數

- `fstat()`：類似 `stat()`，但使用檔案描述符而非路徑
- `lstat()`：類似 `stat()`，但不會追蹤符號連結
- `access()`：檢查檔案存取權限
- `chmod()`：修改檔案權限
- `utime()`：修改檔案時間

## 2.11 注意事項

1. **符號連結**：`stat()` 會追蹤符號連結到實際檔案，使用 `lstat()` 可以獲取連結本身的資訊
2. **權限問題**：如果沒有足夠權限存取檔案或父目錄，`stat()` 會失敗
3. **錯誤處理**：務必檢查返回值，使用 `perror()` 或 `strerror(errno)` 顯示錯誤訊息
4. **跨平台**：`stat()` 是 POSIX 標準，在 Windows 上需要使用 `_stat()` 或其他替代方案
5. **目錄大小**：目錄的 `st_size` 通常不代表其內容的總大小，而是目錄結構本身的大小

**測試檔案結構：**
```
basic_of_system/1.file_IO/
├── 3.fseek/
│   ├── main1.c
│   ├── main2.c
│   └── example.txt
└── 4.stat/
    ├── main.c
    └── example/
        └── example.txt
```

---

## 編譯與執行

### fseek 範例
```bash
cd basic_of_system/1.file_IO/3.fseek
gcc main1.c -o fseek_demo1
./fseek_demo1
```

### stat 範例
```bash
cd basic_of_system/1.file_IO/4.stat
gcc main.c -o stat_demo
./stat_demo
```

---

## 總結

本指南涵蓋了兩個重要的檔案操作函數：

- **fseek()**：用於精確控制檔案讀寫位置，實現隨機存取
- **stat()**：用於獲取檔案元數據，無需開啟檔案即可查詢資訊

這兩個函數是系統程式設計中不可或缺的工具，掌握它們能讓你更有效地處理檔案操作。