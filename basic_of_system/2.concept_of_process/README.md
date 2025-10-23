# Linux 進程控制完整指南

> **fork()、exec()、wait()、system()** 四大核心函數詳解

## 📚 目錄

- [基本概念](#基本概念)
- [1. fork() - 創建進程](#1-fork---創建進程)
- [2. exec() 系列 - 執行新程序](#2-exec-系列---執行新程序)
- [3. wait() - 等待子進程](#3-wait---等待子進程)
- [4. system() - 執行系統命令](#4-system---執行系統命令)
- [組合使用場景](#組合使用場景)
- [實戰範例](#實戰範例)
- [常見問題](#常見問題)

---

## 基本概念

### 什麼是進程？

進程是程序的執行實例，每個進程有：
- 獨立的記憶體空間
- 唯一的進程 ID (PID)
- 自己的執行狀態

### 父子進程關係

```
父進程 (Parent Process)
    │
    ├─ fork() ─→ 子進程 (Child Process)
    │
    └─ wait() ←─ 子進程結束
```

---

## 1. fork() - 創建進程

### 📖 用途

**複製當前進程**，創建一個幾乎完全相同的子進程。

### 🔧 函數原型

```c
#include <unistd.h>
pid_t fork(void);
```

### 📊 返回值

| 返回值 | 意義 | 執行者 |
|--------|------|--------|
| > 0 | 子進程的 PID | 父進程 |
| 0 | 表示自己是子進程 | 子進程 |
| -1 | 創建失敗 | 父進程 |

### 💡 基本範例

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int count = 0;  // 測試變數
    
    printf("===== 程式開始 =====\n");
    printf("原始進程 PID: %d\n\n", getpid());
    
    // 創建子進程
    pid = fork();
    
    if (pid < 0) {
        // fork 失敗
        perror("fork 失敗");
        return 1;
        
    } else if (pid == 0) {
        // 這裡是子進程執行的代碼
        printf("👶 [子進程]\n");
        printf("   我的 PID: %d\n", getpid());
        printf("   我的父進程 PID: %d\n", getppid());
        printf("   fork() 返回值: %d\n", pid);
        
        // 修改變數（不會影響父進程）
        count = 100;
        printf("   我的 count: %d\n\n", count);
        
    } else {
        // 這裡是父進程執行的代碼
        printf("👨 [父進程]\n");
        printf("   我的 PID: %d\n", getpid());
        printf("   子進程 PID: %d\n", pid);
        printf("   fork() 返回值: %d\n", pid);
        printf("   我的 count: %d\n\n", count);
    }
    
    // 父子進程都會執行這段
    printf("進程 %d 說：再見！\n", getpid());
    
    return 0;
}
```

**編譯執行**：
```bash
gcc fork_demo.c -o fork_demo
./fork_demo
```

### 🎯 實際應用：多任務處理

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("🏭 工廠啟動，準備生產 3 個產品\n\n");
    
    // 創建 3 個工人（子進程）
    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：工人開始工作
            printf("👷 工人 %d (PID:%d) 開始生產產品\n", i, getpid());
            sleep(i);  // 模擬工作時間
            printf("✅ 工人 %d 完成產品！\n", i);
            return i;  // 返回產品編號
        }
    }
    
    // 父進程：老闆等待所有工人完成
    printf("👔 老闆等待所有工人完成...\n\n");
    
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t finished_pid = wait(&status);  // 等待任意子進程
        
        if (WIFEXITED(status)) {
            int product_id = WEXITSTATUS(status);
            printf("📦 收到產品 %d (來自 PID:%d)\n", product_id, finished_pid);
        }
    }
    
    printf("\n✨ 所有產品完成！工廠關閉\n");
    return 0;
}
```

---

## 2. exec() 系列 - 執行新程序

### 📖 用途

**用新程序替換**當前進程的代碼。執行成功後，原程序的代碼**完全消失**。

### 🔧 常用函數

```c
#include <unistd.h>

// l = list（參數列表）, v = vector（參數數組）
// p = path（在 PATH 中搜索）, e = environment（指定環境變數）

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
```

### 📊 函數對比

| 函數 | 路徑 | 參數形式 | 說明 |
|------|------|----------|------|
| `execl` | 絕對路徑 | 逐個列出 | 需要完整路徑 |
| `execlp` | 檔名 | 逐個列出 | 自動搜索 PATH |
| `execv` | 絕對路徑 | 數組 | 參數用陣列 |
| `execvp` | 檔名 | 數組 | PATH + 數組 |

### 💡 基本範例

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("🚀 程式即將變身...\n");
    printf("當前 PID: %d\n\n", getpid());
    
    // 方法 1：使用 execl（需要完整路徑）
    // execl("/bin/ls", "ls", "-l", "-h", NULL);
    
    // 方法 2：使用 execlp（自動搜索 PATH，推薦）
    execlp("ls", "ls", "-l", "-h", NULL);
    //     ^程式名  ^argv[0] ^argv[1] ^結束標記
    
    // 如果 exec 成功，下面的代碼永遠不會執行
    perror("❌ exec 失敗");
    return 1;
}
```

### 🎯 實際應用：執行外部程式

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("=== 照片處理程式 ===\n\n");
    
    char *input = "photo.jpg";
    char *output = "photo_resized.jpg";
    
    printf("📸 開始處理照片: %s\n", input);
    printf("🎯 輸出檔案: %s\n\n", output);
    
    // 使用 ImageMagick 的 convert 命令
    // 將照片縮小到 50%
    execlp("convert",           // 程式名（在 PATH 中搜索）
           "convert",           // argv[0]
           input,               // argv[1] 輸入檔案
           "-resize", "50%",    // argv[2], argv[3] 選項
           output,              // argv[4] 輸出檔案
           NULL);               // 結束標記
    
    // 如果執行到這裡，說明 exec 失敗了
    perror("❌ 無法執行 convert 命令");
    printf("💡 請確認已安裝 ImageMagick\n");
    return 1;
}
```

### 📦 使用數組傳遞參數

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    // 使用 execv 系列：參數用數組
    char *args[] = {
        "python3",              // argv[0] 程式名
        "my_script.py",         // argv[1] 腳本檔案
        "--verbose",            // argv[2] 選項
        "--output", "result.txt", // argv[3], argv[4]
        NULL                    // 結束標記
    };
    
    printf("🐍 執行 Python 腳本...\n\n");
    
    execvp(args[0], args);  // 第一個參數是程式名
    
    perror("❌ 執行失敗");
    return 1;
}
```

---

## 3. wait() - 等待子進程

### 📖 用途

父進程**等待子進程結束**，回收子進程資源，避免殭屍進程。

### 🔧 函數原型

```c
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

### 📊 參數說明

**wait()**
- 等待**任意**子進程結束
- 阻塞直到有子進程結束

**waitpid()**
- `pid > 0`：等待指定 PID 的子進程
- `pid = -1`：等待任意子進程（同 wait）
- `options = 0`：阻塞等待
- `options = WNOHANG`：非阻塞（立即返回）

### 💡 基本範例

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("👔 父進程 (PID:%d) 開始\n\n", getpid());
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程
        printf("👶 子進程 (PID:%d) 工作中...\n", getpid());
        sleep(2);  // 模擬工作
        printf("👶 子進程完成工作\n");
        exit(42);  // 返回退出碼 42
        
    } else {
        // 父進程
        printf("👔 父進程等待子進程 (PID:%d)...\n", pid);
        
        int status;
        pid_t finished_pid = wait(&status);
        //                   ↑ 存儲子進程的退出狀態
        
        printf("\n👔 子進程 %d 已結束\n", finished_pid);
        
        // 檢查子進程如何結束
        if (WIFEXITED(status)) {
            // 正常結束
            int exit_code = WEXITSTATUS(status);
            printf("   ✅ 正常結束，退出碼: %d\n", exit_code);
            
        } else if (WIFSIGNALED(status)) {
            // 被信號終止
            int signal = WTERMSIG(status);
            printf("   ⚠️  被信號 %d 終止\n", signal);
        }
    }
    
    return 0;
}
```

### 🎯 實際應用：批次任務管理

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_TASKS 5

int main() {
    printf("📋 任務管理器啟動\n");
    printf("準備執行 %d 個任務\n\n", NUM_TASKS);
    
    pid_t children[NUM_TASKS];
    time_t start_time = time(NULL);
    
    // 創建多個子進程執行任務
    for (int i = 0; i < NUM_TASKS; i++) {
        children[i] = fork();
        
        if (children[i] == 0) {
            // 子進程：執行任務
            printf("🔨 任務 %d 開始 (PID:%d)\n", i+1, getpid());
            
            // 模擬不同的工作時間
            int work_time = (rand() % 3) + 1;
            sleep(work_time);
            
            // 隨機成功或失敗
            int success = (rand() % 4) != 0;  // 75% 成功率
            
            if (success) {
                printf("✅ 任務 %d 完成\n", i+1);
                exit(0);  // 成功
            } else {
                printf("❌ 任務 %d 失敗\n", i+1);
                exit(1);  // 失敗
            }
        }
    }
    
    // 父進程：等待所有任務完成
    printf("\n⏳ 等待所有任務完成...\n\n");
    
    int success_count = 0;
    int failed_count = 0;
    
    for (int i = 0; i < NUM_TASKS; i++) {
        int status;
        pid_t finished = wait(&status);  // 等待任意子進程
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                success_count++;
            } else {
                failed_count++;
            }
        }
    }
    
    time_t end_time = time(NULL);
    
    // 顯示統計
    printf("\n" "═══════════════════════\n");
    printf("📊 任務統計報告\n");
    printf("═══════════════════════\n");
    printf("總任務數: %d\n", NUM_TASKS);
    printf("✅ 成功: %d\n", success_count);
    printf("❌ 失敗: %d\n", failed_count);
    printf("⏱️  總耗時: %ld 秒\n", end_time - start_time);
    printf("═══════════════════════\n");
    
    return 0;
}
```

### 🔍 waitpid 進階用法

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("👶 子進程工作中...\n");
        sleep(3);
        exit(0);
    } else {
        // 非阻塞檢查子進程狀態
        printf("👔 父進程繼續工作...\n");
        
        int status;
        pid_t result;
        
        for (int i = 0; i < 5; i++) {
            // WNOHANG：非阻塞，立即返回
            result = waitpid(pid, &status, WNOHANG);
            
            if (result == 0) {
                // 子進程還在運行
                printf("   檢查 %d: 子進程還在工作\n", i+1);
                sleep(1);
            } else {
                // 子進程已結束
                printf("   子進程完成！\n");
                break;
            }
        }
        
        // 最後確保子進程被回收
        if (result == 0) {
            printf("   阻塞等待子進程...\n");
            waitpid(pid, &status, 0);  // 阻塞等待
        }
    }
    
    return 0;
}
```

---

## 4. system() - 執行系統命令

### 📖 用途

**簡單快速**地執行 shell 命令，實際上是 `fork + exec + wait` 的封裝。

### 🔧 函數原型

```c
#include <stdlib.h>
int system(const char *command);
```

### 📊 返回值

- 成功：命令的退出狀態
- 失敗：-1

### 💡 基本範例

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== 使用 system() 執行命令 ===\n\n");
    
    // 範例 1：列出檔案
    printf("📁 當前目錄內容：\n");
    system("ls -lh");
    
    printf("\n");
    
    // 範例 2：查看系統資訊
    printf("💻 系統資訊：\n");
    system("uname -a");
    
    printf("\n");
    
    // 範例 3：執行複雜命令（管道）
    printf("📊 最大的 5 個檔案：\n");
    system("ls -lh | sort -k5 -hr | head -5");
    
    return 0;
}
```

### ⚠️ 安全性警告

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char filename[100];
    
    printf("輸入要刪除的檔案名: ");
    scanf("%99s", filename);
    
    // ❌ 危險！容易被注入攻擊
    char command[200];
    sprintf(command, "rm %s", filename);
    system(command);
    // 如果用戶輸入: "test.txt; rm -rf /"
    // 實際執行: rm test.txt; rm -rf /
    
    return 0;
}
```

**安全的做法**：

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char filename[100];
    
    printf("輸入要刪除的檔案名: ");
    scanf("%99s", filename);
    
    // ✅ 安全！使用 fork + exec
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：直接執行程式，不經過 shell
        execlp("rm", "rm", filename, NULL);
        perror("執行失敗");
        exit(1);
    } else {
        // 父進程：等待完成
        wait(NULL);
        printf("檔案刪除完成\n");
    }
    
    return 0;
}
```

### 🎯 實際應用：備份腳本

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    printf("💾 自動備份程式\n\n");
    
    // 獲取當前時間作為備份檔名
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char backup_name[100];
    
    sprintf(backup_name, "backup_%04d%02d%02d_%02d%02d%02d.tar.gz",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    
    printf("📦 建立備份: %s\n", backup_name);
    
    // 使用 tar 打包
    char command[256];
    sprintf(command, "tar -czf %s *.c *.h 2>/dev/null", backup_name);
    
    int result = system(command);
    
    if (result == 0) {
        printf("✅ 備份成功！\n\n");
        
        // 顯示備份檔案資訊
        sprintf(command, "ls -lh %s", backup_name);
        system(command);
    } else {
        printf("❌ 備份失敗\n");
    }
    
    return 0;
}
```

---

## 組合使用場景

### 場景 1：fork + exec + wait（標準組合）

**用途**：執行外部程式並等待結果

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("🎬 視頻轉換程式\n\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：執行 ffmpeg 轉換視頻
        printf("🔄 開始轉換視頻...\n");
        
        execlp("ffmpeg",
               "ffmpeg",
               "-i", "input.mp4",      // 輸入檔案
               "-vcodec", "libx264",   // 視頻編碼
               "-acodec", "aac",       // 音頻編碼
               "output.mp4",           // 輸出檔案
               NULL);
        
        // 如果 exec 失敗
        perror("❌ 無法執行 ffmpeg");
        return 1;
        
    } else if (pid > 0) {
        // 父進程：等待轉換完成
        printf("⏳ 等待轉換完成...\n\n");
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("✅ 視頻轉換成功！\n");
        } else {
            printf("❌ 轉換失敗\n");
        }
        
    } else {
        perror("fork 失敗");
        return 1;
    }
    
    return 0;
}
```

### 場景 2：多進程並行處理

**用途**：同時處理多個任務

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_WORKERS 4

// 模擬處理資料
void process_data(int worker_id, int start, int end) {
    printf("👷 工人 %d: 處理資料 %d-%d\n", worker_id, start, end);
    sleep(2);  // 模擬處理時間
    printf("✅ 工人 %d: 完成\n", worker_id);
}

int main() {
    printf("🏭 資料處理工廠啟動\n");
    printf("📊 使用 %d 個工人並行處理\n\n", NUM_WORKERS);
    
    int total_data = 1000;
    int chunk_size = total_data / NUM_WORKERS;
    
    // 創建多個工人（子進程）
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：處理一部分資料
            int start = i * chunk_size;
            int end = (i == NUM_WORKERS - 1) ? total_data : (i + 1) * chunk_size;
            
            process_data(i + 1, start, end);
            exit(0);
        }
    }
    
    // 父進程：等待所有工人完成
    for (int i = 0; i < NUM_WORKERS; i++) {
        wait(NULL);
    }
    
    printf("\n🎉 所有資料處理完成！\n");
    return 0;
}
```

### 場景 3：簡易 Shell 實現

**用途**：實現命令行解析器

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 256
#define MAX_ARGS 10

int main() {
    char command[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    
    printf("🐚 簡易 Shell (輸入 'exit' 退出)\n\n");
    
    while (1) {
        // 顯示提示符
        printf("myshell> ");
        fflush(stdout);
        
        // 讀取命令
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // 移除換行符
        command[strcspn(command, "\n")] = 0;
        
        // 檢查退出命令
        if (strcmp(command, "exit") == 0) {
            printf("👋 再見！\n");
            break;
        }
        
        // 解析命令（簡單版本：只處理第一個單詞）
        args[0] = strtok(command, " ");
        int i = 1;
        while ((args[i] = strtok(NULL, " ")) != NULL && i < MAX_ARGS - 1) {
            i++;
        }
        args[i] = NULL;
        
        if (args[0] == NULL) {
            continue;  // 空命令
        }
        
        // 執行命令
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：執行命令
            execvp(args[0], args);
            
            // 如果執行到這裡，說明命令不存在
            printf("❌ 命令不存在: %s\n", args[0]);
            exit(1);
            
        } else if (pid > 0) {
            // 父進程：等待命令完成
            int status;
            waitpid(pid, &status, 0);
            
        } else {
            perror("fork 失敗");
        }
    }
    
    return 0;
}
```

### 場景 4：守護進程（Daemon）

**用途**：創建後台服務

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void create_daemon() {
    pid_t pid;
    
    // 第一次 fork
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    if (pid > 0) {
        exit(0);  // 父進程退出
    }
    
    // 子進程成為會話領導者
    if (setsid() < 0) {
        exit(1);
    }
    
    // 第二次 fork（可選，確保不會獲得控制終端）
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    if (pid > 0) {
        exit(0);
    }
    
    // 改變工作目錄
    chdir("/");
    
    // 關閉標準輸入輸出
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // 重新導向到 /dev/null
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

int main() {
    printf("🚀 創建守護進程...\n");
    
    create_daemon();
    
    // 守護進程在後台運行
    FILE *log = fopen("/tmp/daemon.log", "a");
    
    for (int i = 0; i < 10; i++) {
        fprintf(log, "守護進程運行中... %d\n", i);
        fflush(log);
        sleep(5);
    }
    
    fclose(log);
    return 0;
}
```

---

## 實戰範例

### 🎮 範例：遊戲啟動器

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int id;
    char *name;
    char *command;
} Game;

int main() {
    Game games[] = {
        {1, "文字編輯器", "gedit"},
        {2, "計算機", "gnome-calculator"},
        {3, "終端機", "gnome-terminal"}
    };
    int num_games = 3;
    
    printf("🎮 遊戲啟動器\n");
    printf("═══════════════════════\n");
    
    // 顯示遊戲列表
    for (int i = 0; i < num_games; i++) {
        printf("%d. %s\n", games[i].id, games[i].name);
    }
    printf("0. 退出\n");
    printf("═══════════════════════\n");
    
    while (1) {
        printf("\n請選擇遊戲 (0-%d): ", num_games);
        int choice;
        scanf("%d", &choice);
        
        if (choice == 0) {
            printf("👋 再見！\n");
            break;
        }
        
        if (choice < 1 || choice > num_games) {
            printf("❌ 無效的選擇\n");
            continue;
        }
        
        Game selected = games[choice - 1];
        printf("🚀 啟動 %s...\n", selected.name);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：執行遊戲
            execlp(selected.command, selected.command, NULL);
            
            printf("❌ 無法啟動遊戲\n");
            exit(1);
            
        } else if (pid > 0) {
            // 父進程：不等待，讓遊戲在背景執行
            printf("✅ 遊戲已啟動 (PID: %d)\n", pid);
            printf("💡 遊戲在背景執行，你可以繼續選擇其他遊戲\n");
            
        } else {
            perror("fork 失敗");
        }
    }
    
    // 清理所有子進程
    printf("\n🧹 清理背景進程...\n");
    while (waitpid(-1, NULL, WNOHANG) > 0);
    
    return 0;
}
```

### 📸 範例：批次照片處理

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_FILES 100

int main() {
    printf("📸 批次照片處理工具\n\n");
    
    // 模擬照片列表
    char *photos[] = {
        "photo1.jpg",
        "photo2.jpg", 
        "photo3.jpg",
        "photo4.jpg"
    };
    int num_photos = 4;
    
    printf("📋 找到 %d 張照片\n", num_photos);
    printf("🔄 開始批次處理...\n\n");
    
    // 為每張照片創建一個處理進程
    for (int i = 0; i < num_photos; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：處理一張照片
            printf("👷 進程 %d: 處理 %s\n", getpid(), photos[i]);
            
            // 生成輸出檔名
            char output[100];
            sprintf(output, "resized_%s", photos[i]);
            
            // 使用 ImageMagick convert 調整大小
            // execlp("convert", "convert", photos[i], 
            //        "-resize", "800x600", output, NULL);
            
            // 模擬處理（因為可能沒裝 ImageMagick）
            sleep(1);
            printf("✅ 進程 %d: %s 處理完成\n", getpid(), photos[i]);
            
            exit(0);
        }
    }
    
    // 父進程：等待所有照片處理完成
    int completed = 0;
    while (completed < num_photos) {
        int status;
        pid_t finished = wait(&status);
        
        if (finished > 0) {
            completed++;
            printf("📊 進度: %d/%d\n", completed, num_photos);
        }
    }
    
    printf("\n🎉 所有照片處理完成！\n");
    return 0;
}
```

### 🔐 範例：多進程密碼破解（教育用途）

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PASSWORD "1234"
#define NUM_WORKERS 4
#define RANGE_START 0
#define RANGE_END 10000

// 嘗試破解密碼的函數
void try_crack(int worker_id, int start, int end) {
    printf("🔍 工人 %d: 搜索範圍 %d-%d\n", worker_id, start, end);
    
    for (int i = start; i < end; i++) {
        char guess[10];
        sprintf(guess, "%d", i);
        
        if (strcmp(guess, PASSWORD) == 0) {
            printf("🎯 工人 %d: 找到密碼！%s\n", worker_id, guess);
            exit(0);  // 找到就退出
        }
        
        // 模擬每次嘗試的時間
        usleep(100);
    }
    
    printf("❌ 工人 %d: 範圍內未找到\n", worker_id);
    exit(1);  // 未找到
}

int main() {
    printf("🔐 多進程密碼破解演示\n");
    printf("🎯 目標密碼: [隱藏]\n");
    printf("👷 使用 %d 個工人並行搜索\n\n", NUM_WORKERS);
    
    int range_size = (RANGE_END - RANGE_START) / NUM_WORKERS;
    pid_t workers[NUM_WORKERS];
    
    // 創建多個工人進程
    for (int i = 0; i < NUM_WORKERS; i++) {
        int start = RANGE_START + i * range_size;
        int end = (i == NUM_WORKERS - 1) ? RANGE_END : start + range_size;
        
        workers[i] = fork();
        
        if (workers[i] == 0) {
            // 子進程：嘗試破解
            try_crack(i + 1, start, end);
        }
    }
    
    // 父進程：等待第一個找到密碼的工人
    int found = 0;
    for (int i = 0; i < NUM_WORKERS; i++) {
        int status;
        pid_t finished = wait(&status);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("\n✅ 密碼已被進程 %d 破解！\n", finished);
            found = 1;
            
            // 終止其他工人
            for (int j = 0; j < NUM_WORKERS; j++) {
                if (workers[j] != finished) {
                    kill(workers[j], SIGTERM);
                }
            }
            break;
        }
    }
    
    if (!found) {
        printf("\n❌ 所有工人都未找到密碼\n");
    }
    
    // 清理剩餘進程
    while (wait(NULL) > 0);
    
    return 0;
}
```

---

## 常見問題

### ❓ Q1: fork() 後父子進程的執行順序？

**A**: 不確定！由作業系統調度決定。不要假設執行順序。

```c
pid_t pid = fork();
if (pid == 0) {
    printf("子進程\n");  // 可能先執行
} else {
    printf("父進程\n");  // 也可能先執行
}
```

### ❓ Q2: fork() 會複製什麼？

**A**: 幾乎所有東西，但是**獨立副本**：

✅ 複製的：
- 程式碼
- 資料段（變數）
- 堆疊
- 堆積（heap）
- 檔案描述符（但共享檔案偏移量）

❌ 不複製的：
- 進程 ID
- 父進程 ID
- 鎖（locks）

### ❓ Q3: 什麼是殭屍進程？

**A**: 子進程結束了，但父進程沒有調用 `wait()` 回收資源。

```c
// ❌ 錯誤：創建殭屍進程
pid_t pid = fork();
if (pid == 0) {
    exit(0);  // 子進程結束
}
// 父進程沒有 wait()，子進程變成殭屍！

// ✅ 正確：回收子進程
pid_t pid = fork();
if (pid == 0) {
    exit(0);
} else {
    wait(NULL);  // 回收資源
}
```

查看殭屍進程：
```bash
ps aux | grep Z
```

### ❓ Q4: fork() 和 exec() 的區別？

| 特性 | fork() | exec() |
|------|--------|--------|
| 作用 | 複製進程 | 替換程式 |
| 進程數 | +1 | 不變 |
| PID | 新 PID | 不變 |
| 記憶體 | 複製 | 全部替換 |
| 返回 | 有返回值 | 不返回（成功時）|

### ❓ Q5: system() vs fork+exec，如何選擇？

**使用 system()**：
- ✅ 快速簡單
- ✅ 需要 shell 功能（管道、重定向）
- ❌ 安全性較低
- ❌ 效能較差

**使用 fork+exec**：
- ✅ 更安全
- ✅ 更高效
- ✅ 更靈活的控制
- ❌ 代碼較長

```c
// system: 簡單但不安全
system("ls -l | grep txt");

// fork+exec: 安全但複雜
pid_t pid = fork();
if (pid == 0) {
    execlp("ls", "ls", "-l", NULL);
    exit(1);
}
wait(NULL);
```

### ❓ Q6: 如何避免殭屍進程？

**方法 1**：使用 `wait()` 或 `waitpid()`

```c
pid_t pid = fork();
if (pid > 0) {
    wait(NULL);  // 等待子進程
}
```

**方法 2**：忽略 SIGCHLD 信號

```c
#include <signal.h>

signal(SIGCHLD, SIG_IGN);  // 自動回收子進程

pid_t pid = fork();
// 不需要 wait()
```

**方法 3**：使用信號處理器

```c
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    signal(SIGCHLD, sigchld_handler);
    
    // 創建子進程...
}
```

### ❓ Q7: fork() 失敗的常見原因？

1. **進程數限制**
```bash
ulimit -u  # 查看最大進程數
```

2. **記憶體不足**

3. **PID 用盡**

**處理方式**：
```c
pid_t pid = fork();
if (pid < 0) {
    if (errno == EAGAIN) {
        fprintf(stderr, "進程數達到上限\n");
    } else if (errno == ENOMEM) {
        fprintf(stderr, "記憶體不足\n");
    }
}
```

---

## 📚 進階主題

### 進程間通信（IPC）

fork() 創建的父子進程可以透過多種方式通信：

1. **管道（Pipe）**
```c
int pipefd[2];
pipe(pipefd);

if (fork() == 0) {
    close(pipefd[0]);  // 關閉讀端
    write(pipefd[1], "Hello", 5);
    close(pipefd[1]);
} else {
    close(pipefd[1]);  // 關閉寫端
    char buf[10];
    read(pipefd[0], buf, 5);
    close(pipefd[0]);
}
```

2. **共享記憶體**
3. **訊號（Signal）**
4. **Socket**

### 進程池（Process Pool）

預先創建一組進程，重複使用：

```c
#define POOL_SIZE 5

void worker_process() {
    while (1) {
        // 等待任務...
        // 處理任務...
    }
}

int main() {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (fork() == 0) {
            worker_process();
            exit(0);
        }
    }
    
    // 主進程分配任務...
    
    return 0;
}
```

---

## 🔗 參考資源

### 線上文件
- [Linux man pages](https://man7.org/)
- [fork(2)](https://man7.org/linux/man-pages/man2/fork.2.html)
- [exec(3)](https://man7.org/linux/man-pages/man3/exec.3.html)
- [wait(2)](https://man7.org/linux/man-pages/man2/wait.2.html)

### 推薦書籍
- *Advanced Programming in the UNIX Environment* (APUE)
- *The Linux Programming Interface*

### 調試工具
```bash
# 查看進程
ps aux

# 進程樹
pstree

# 即時監控
top / htop

# 追蹤系統調用
strace ./your_program
```

---

## 📝 編譯與執行

```bash
# 編譯
gcc your_program.c -o your_program

# 執行
./your_program

# 帶調試訊息編譯
gcc -g -Wall your_program.c -o your_program

# 使用 gdb 調試
gdb ./your_program
```

---

## ⚠️ 注意事項

1. **總是檢查返回值**
```c
pid_t pid = fork();
if (pid < 0) {
    perror("fork");
    exit(1);
}
```

2. **避免 fork 炸彈**
```c
// ❌ 危險！無限創建進程
while(1) fork();
```

3. **正確處理檔案描述符**
```c
int fd = open("file.txt", O_RDONLY);
fork();
// 父子進程都有這個 fd，要小心關閉
```

4. **exec 後要處理錯誤**
```c
execlp("program", "program", NULL);
// 如果執行到這裡，說明 exec 失敗了
perror("exec");
exit(1);
```

---

## 🎓 總結

| 函數 | 目的 | 常見搭配 |
|------|------|----------|
| **fork()** | 創建子進程 | wait() |
| **exec()** | 執行新程式 | fork() |
| **wait()** | 回收子進程 | fork() |
| **system()** | 快速執行命令 | 單獨使用 |

**黃金組合**：`fork() + exec() + wait()` = 安全、靈活、可控的進程管理

**記住**：
- fork 創造新生命 👶
- exec 改變身份 🎭
- wait 負責善後 ⏳
- system 一鍵搞定 🔘

---

## 💡 最佳實踐

1. ✅ 總是用 `wait()` 回收子進程
2. ✅ 檢查所有系統調用的返回值
3. ✅ 安全性要求高時避免使用 `system()`
4. ✅ 使用 `execlp()` 而不是 `execl()`（自動搜索 PATH）
5. ✅ 在子進程中正確處理錯誤並 `exit()`
6. ✅ 注意檔案描述符的繼承和關閉
7. ✅ 使用 `waitpid()` 的 `WNOHANG` 選項實現非阻塞檢查

---

**Happy Coding! 🚀**