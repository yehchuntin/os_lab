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

## 📖 重要函數與巨集詳解

### 1. `perror()` - 印出錯誤訊息

**用途**：將系統錯誤訊息印到標準錯誤輸出（stderr）

```c
#include <stdio.h>
#include <errno.h>

perror("錯誤前綴");
```

**運作原理**：
- 自動讀取全域變數 `errno`（錯誤代碼）
- 轉換成人類可讀的錯誤訊息
- 格式：`你的訊息: 系統錯誤描述`

**範例**：
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        // fork 失敗
        perror("fork 失敗");
        // 可能輸出：fork 失敗: Resource temporarily unavailable
        return 1;
    }
    
    return 0;
}
```

**常見錯誤訊息**：
```c
perror("open");     // open: No such file or directory
perror("malloc");   // malloc: Cannot allocate memory
perror("fork");     // fork: Resource temporarily unavailable
```

---

### 2. `getpid()` 和 `getppid()` - 取得進程 ID

**用途**：取得進程識別碼

```c
#include <unistd.h>
#include <sys/types.h>

pid_t getpid(void);   // 取得自己的 PID
pid_t getppid(void);  // 取得父進程的 PID
```

**範例**：
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("我的 PID: %d\n", getpid());
    printf("我的父進程 PID: %d\n", getppid());
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程
        printf("\n[子進程]\n");
        printf("  我的 PID: %d\n", getpid());        // 新的 PID
        printf("  父進程 PID: %d\n", getppid());     // 原進程的 PID
    } else {
        // 父進程
        printf("\n[父進程]\n");
        printf("  我的 PID: %d\n", getpid());        // 不變
        printf("  子進程 PID: %d\n", pid);           // fork() 返回的
    }
    
    return 0;
}
```

**輸出範例**：
```
我的 PID: 1234
我的父進程 PID: 1000

[父進程]
  我的 PID: 1234
  子進程 PID: 1235

[子進程]
  我的 PID: 1235
  父進程 PID: 1234
```

**用途場景**：
```c
// 生成唯一的日誌檔名
char logfile[100];
sprintf(logfile, "log_%d.txt", getpid());

// 檢查進程關係
if (getppid() == 1) {
    printf("我的父進程是 init，我可能是孤兒進程\n");
}
```

---

### 3. `WIFEXITED()` 和 `WEXITSTATUS()` - 檢查退出狀態

**用途**：解析子進程的退出狀態

```c
#include <sys/wait.h>

WIFEXITED(status)    // 檢查是否正常退出（返回 true/false）
WEXITSTATUS(status)  // 取得退出碼（0-255）
```

**完整範例**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：執行任務
        printf("子進程工作中...\n");
        sleep(1);
        
        // 不同的退出方式
        exit(42);  // 正常退出，返回 42
        // return 42;  // 也可以用 return
        
    } else {
        // 父進程：等待並檢查狀態
        int status;
        wait(&status);
        //    ↑ status 存儲子進程的退出資訊
        
        printf("\n檢查子進程退出狀態：\n");
        
        // 1. 檢查是否正常退出
        if (WIFEXITED(status)) {
            printf("✅ 子進程正常退出\n");
            
            // 2. 取得退出碼
            int exit_code = WEXITSTATUS(status);
            printf("   退出碼: %d\n", exit_code);  // 輸出：42
            
            // 3. 根據退出碼判斷結果
            if (exit_code == 0) {
                printf("   狀態: 成功\n");
            } else {
                printf("   狀態: 失敗（錯誤碼 %d）\n", exit_code);
            }
        }
    }
    
    return 0;
}
```

**status 的結構**：
```
status (32 位元整數)
┌─────────────┬─────────────┬──────────┐
│ 未使用      │ 退出碼      │ 信號編號 │
│ 16 bits     │ 8 bits      │ 8 bits   │
└─────────────┴─────────────┴──────────┘
              ↑               ↑
         WEXITSTATUS()    WTERMSIG()
```

**實際應用**：
```c
// 執行外部程式並檢查結果
pid_t pid = fork();
if (pid == 0) {
    execlp("grep", "grep", "pattern", "file.txt", NULL);
    exit(127);  // exec 失敗
} else {
    int status;
    wait(&status);
    
    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        
        if (code == 0) {
            printf("找到匹配\n");
        } else if (code == 1) {
            printf("沒找到匹配\n");
        } else if (code == 127) {
            printf("程式執行失敗\n");
        }
    }
}
```

---

### 4. `argv[]` - 命令列參數

**用途**：存儲傳遞給程式的參數

```c
int main(int argc, char *argv[])
//         ↑ 參數個數  ↑ 參數陣列
```

**結構說明**：
```
執行: ./program arg1 arg2 arg3

argc = 4

argv[0] = "./program"  ← 程式名稱（重要！）
argv[1] = "arg1"       ← 第一個參數
argv[2] = "arg2"       ← 第二個參數
argv[3] = "arg3"       ← 第三個參數
argv[4] = NULL         ← 結束標記
```

**範例程式**：
```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("總共有 %d 個參數\n\n", argc);
    
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    
    return 0;
}
```

**執行與輸出**：
```bash
$ ./program hello world 123

總共有 4 個參數

argv[0] = ./program
argv[1] = hello
argv[2] = world
argv[3] = 123
```

**為什麼 `argv[0]` 重要？**

在 `exec()` 系列函數中，`argv[0]` **通常應該是程式名稱**：

```c
// ✅ 正確：argv[0] 是程式名
execlp("ls", "ls", "-l", NULL);
//          ↑ 程式名  ↑ argv[0]

// ❌ 錯誤：會造成混淆
execlp("ls", "wrong_name", "-l", NULL);

// exec 系列函數的 argv[0] 規則：
execl("/bin/ls", "ls", "-l", NULL);
//    ↑ 實際路徑   ↑ argv[0]（顯示名稱）
```

**實際應用：建立符號連結**
```c
// 同一個程式，不同名稱，不同行為
int main(int argc, char *argv[]) {
    if (strcmp(argv[0], "compress") == 0) {
        // 壓縮模式
        compress_file();
    } else if (strcmp(argv[0], "decompress") == 0) {
        // 解壓模式
        decompress_file();
    }
}

// 使用方式：
// ln -s program compress
// ln -s program decompress
```

---

### 5. `exit()` - 結束進程

**用途**：終止程式並返回狀態碼

```c
#include <stdlib.h>

void exit(int status);  // 永不返回
```

**與 `return` 的區別**：

```c
int main() {
    if (fork() == 0) {
        printf("子進程\n");
        
        // 方法 1：exit()
        exit(42);  // ✅ 立即結束，不執行後續代碼
        
        // 方法 2：return
        return 42;  // ✅ 從 main 返回，效果相同
    }
    
    // 在函數中：
    void some_function() {
        if (error) {
            exit(1);    // ✅ 整個程式結束
            // return 1; // ❌ 只返回到調用者
        }
    }
    
    return 0;
}
```

**狀態碼慣例**：
```c
exit(0);   // 成功
exit(1);   // 一般錯誤
exit(2);   // 誤用命令
exit(126); // 命令無法執行
exit(127); // 命令找不到
exit(130); // 被 Ctrl+C 中斷
exit(255); // 退出碼超出範圍
```

**完整範例**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：模擬任務
        printf("子進程開始工作...\n");
        
        // 假設處理檔案
        FILE *fp = fopen("data.txt", "r");
        if (fp == NULL) {
            perror("無法開啟檔案");
            exit(1);  // 失敗，返回 1
        }
        
        // 處理資料...
        printf("處理完成\n");
        fclose(fp);
        
        exit(0);  // 成功，返回 0
        
    } else {
        // 父進程：檢查結果
        int status;
        wait(&status);
        
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            
            if (code == 0) {
                printf("✅ 任務成功\n");
            } else {
                printf("❌ 任務失敗，錯誤碼: %d\n", code);
            }
        }
    }
    
    return 0;
}
```

**exit() 會做什麼？**
1. 呼叫所有 `atexit()` 註冊的函數
2. 刷新（flush）所有開啟的 stdio 串流
3. 關閉所有開啟的檔案
4. 刪除 `tmpfile()` 建立的暫存檔
5. 返回退出碼給父進程

```c
void cleanup() {
    printf("清理資源...\n");
}

int main() {
    atexit(cleanup);  // 註冊清理函數
    
    printf("程式運行中\n");
    exit(0);  // 會先呼叫 cleanup()
}
```

---

### 6. `wait(&status)` - 等待並取得狀態

**用途**：等待子進程結束，並取得退出資訊

```c
#include <sys/wait.h>

pid_t wait(int *status);
//          ↑ 指標，用來存儲子進程的退出狀態
```

**status 的用法**：

```c
int status;  // 定義變數來存儲狀態

wait(&status);  // 傳遞位址，讓 wait() 寫入資料
//   ↑ 取址運算子

// 現在 status 包含子進程的退出資訊
```

**完整範例**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("=== wait() 詳細示範 ===\n\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程
        printf("子進程 (PID:%d) 工作 3 秒...\n", getpid());
        sleep(3);
        printf("子進程完成\n");
        exit(5);  // 返回 5
        
    } else {
        // 父進程
        printf("父進程 (PID:%d) 等待子進程 (PID:%d)...\n", 
               getpid(), pid);
        
        int status;  // 1. 準備狀態變數
        
        pid_t finished_pid = wait(&status);  // 2. 等待（阻塞）
        //                        ↑ 傳遞位址
        
        printf("\n子進程 %d 已結束\n", finished_pid);
        
        // 3. 解析 status
        printf("\n狀態分析：\n");
        printf("  原始 status: 0x%x\n", status);
        
        if (WIFEXITED(status)) {
            printf("  ✅ 正常退出\n");
            printf("  退出碼: %d\n", WEXITSTATUS(status));
        }
        
        if (WIFSIGNALED(status)) {
            printf("  ⚠️  被信號終止\n");
            printf("  信號: %d\n", WTERMSIG(status));
        }
    }
    
    return 0;
}
```

**不使用 status（簡化版）**：
```c
// 如果不關心退出狀態
wait(NULL);  // 只等待，不取得狀態

// 等待多個子進程
for (int i = 0; i < num_children; i++) {
    wait(NULL);  // 簡單回收
}
```

---

### 7. `WIFSIGNALED()` 和 `WTERMSIG()` - 檢查信號終止

**用途**：檢查子進程是否被信號（signal）終止

```c
#include <sys/wait.h>

WIFSIGNALED(status)  // 是否被信號殺死？
WTERMSIG(status)     // 哪個信號？
```

**常見信號**：
```c
SIGKILL  (9)   // 強制終止（無法捕捉）
SIGTERM  (15)  // 要求終止（可捕捉）
SIGINT   (2)   // Ctrl+C
SIGSEGV  (11)  // 記憶體錯誤（Segmentation Fault）
SIGABRT  (6)   // abort() 呼叫
```

**範例程式**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    printf("=== 信號處理示範 ===\n\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：故意製造錯誤
        printf("子進程：我要做壞事了...\n");
        sleep(1);
        
        // 方法 1：觸發段錯誤
        // int *p = NULL;
        // *p = 42;  // SIGSEGV
        
        // 方法 2：主動終止
        raise(SIGTERM);  // 發送 SIGTERM 給自己
        
    } else {
        // 父進程：監控子進程
        int status;
        wait(&status);
        
        printf("\n=== 子進程終止分析 ===\n");
        
        // 檢查終止方式
        if (WIFEXITED(status)) {
            printf("方式: 正常退出\n");
            printf("退出碼: %d\n", WEXITSTATUS(status));
            
        } else if (WIFSIGNALED(status)) {
            printf("方式: 被信號終止 ⚠️\n");
            
            int sig = WTERMSIG(status);
            printf("信號編號: %d\n", sig);
            
            // 顯示信號名稱
            switch(sig) {
                case SIGTERM:
                    printf("信號名稱: SIGTERM (終止請求)\n");
                    break;
                case SIGKILL:
                    printf("信號名稱: SIGKILL (強制終止)\n");
                    break;
                case SIGSEGV:
                    printf("信號名稱: SIGSEGV (記憶體錯誤)\n");
                    break;
                case SIGINT:
                    printf("信號名稱: SIGINT (Ctrl+C)\n");
                    break;
                default:
                    printf("信號名稱: 未知\n");
            }
            
            // 檢查是否產生 core dump
            if (WCOREDUMP(status)) {
                printf("Core dump: 是\n");
            }
        }
    }
    
    return 0;
}
```

**實際應用：超時終止**
```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：可能執行很久的任務
        printf("開始長時間任務...\n");
        sleep(100);  // 模擬長時間運行
        exit(0);
        
    } else {
        // 父進程：設定超時
        printf("等待 3 秒...\n");
        sleep(3);
        
        // 超時，強制終止子進程
        printf("超時！終止子進程\n");
        kill(pid, SIGTERM);  // 發送 SIGTERM
        
        sleep(1);  // 給它時間清理
        
        // 如果還沒結束，強制殺死
        kill(pid, SIGKILL);  // 發送 SIGKILL
        
        // 回收
        int status;
        wait(&status);
        
        if (WIFSIGNALED(status)) {
            printf("子進程被信號 %d 終止\n", WTERMSIG(status));
        }
    }
    
    return 0;
}
```

---

### 8. `sleep()` - 暫停執行

**用途**：讓程式暫停指定秒數

```c
#include <unistd.h>

unsigned int sleep(unsigned int seconds);
// 返回值：如果被中斷，返回剩餘秒數
```

**基本用法**：
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("開始\n");
    
    sleep(3);  // 暫停 3 秒
    
    printf("3 秒後\n");
    
    return 0;
}
```

**更精確的時間控制**：
```c
#include <unistd.h>

// 微秒級暫停（1 微秒 = 0.000001 秒）
usleep(500000);  // 0.5 秒

// 奈秒級暫停（更精確）
#include <time.h>
struct timespec ts = {.tv_sec = 1, .tv_nsec = 500000000};
nanosleep(&ts, NULL);  // 1.5 秒
```

**實際應用：模擬工作**
```c
#include <stdio.h>
#include <unistd.h>

void simulate_work(const char *task, int seconds) {
    printf("⏳ %s (預計 %d 秒)...\n", task, seconds);
    
    for (int i = 1; i <= seconds; i++) {
        sleep(1);
        printf("   進度: %d/%d\n", i, seconds);
    }
    
    printf("✅ %s 完成\n\n", task);
}

int main() {
    simulate_work("下載檔案", 3);
    simulate_work("處理資料", 2);
    simulate_work("上傳結果", 2);
    
    return 0;
}
```

**被信號中斷**：
```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handle_signal(int sig) {
    printf("\n收到信號 %d\n", sig);
}

int main() {
    signal(SIGINT, handle_signal);  // 捕捉 Ctrl+C
    
    printf("睡眠 10 秒（按 Ctrl+C 中斷）\n");
    
    unsigned int remaining = sleep(10);
    
    if (remaining > 0) {
        printf("被中斷，還剩 %u 秒\n", remaining);
    } else {
        printf("完整睡眠完成\n");
    }
    
    return 0;
}
```

---

### 9. `sprintf()` - 格式化字串

**用途**：將格式化的資料寫入字串（類似 printf，但輸出到字串）

```c
#include <stdio.h>

int sprintf(char *str, const char *format, ...);
//          ↑ 目標字串  ↑ 格式字串  ↑ 參數
```

**基本範例**：
```c
#include <stdio.h>

int main() {
    char buffer[100];
    
    // 基本用法
    sprintf(buffer, "Hello, %s!", "World");
    printf("%s\n", buffer);  // Hello, World!
    
    // 組合多個變數
    int age = 25;
    char name[] = "Alice";
    sprintf(buffer, "%s is %d years old", name, age);
    printf("%s\n", buffer);  // Alice is 25 years old
    
    // 數字格式化
    sprintf(buffer, "編號: %04d", 42);
    printf("%s\n", buffer);  // 編號: 0042
    
    return 0;
}
```

**⚠️ 危險用法（容易被攻擊）**：

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char filename[100];
    char command[256];
    
    printf("輸入檔案名: ");
    scanf("%s", filename);
    
    // ❌ 危險！
    sprintf(command, "rm %s", filename);
    system(command);
    
    // 如果用戶輸入: test.txt; rm -rf /
    // 實際執行: rm test.txt; rm -rf /
    // 災難！
    
    return 0;
}
```

**✅ 安全的替代方案**：

```c
// 方法 1：使用 snprintf（限制長度）
char command[256];
snprintf(command, sizeof(command), "rm %s", filename);
//       ↑ 防止緩衝區溢出

// 方法 2：驗證輸入
if (strchr(filename, ';') || strchr(filename, '|')) {
    fprintf(stderr, "非法字元\n");
    return 1;
}

// 方法 3：使用 fork + exec（最安全）
pid_t pid = fork();
if (pid == 0) {
    execlp("rm", "rm", filename, NULL);  // 不經過 shell
    exit(1);
}
wait(NULL);
```

**實用範例**：
```c
#include <stdio.h>
#include <time.h>

int main() {
    char filename[100];
    
    // 1. 生成帶時間戳的檔名
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    sprintf(filename, "backup_%04d%02d%02d_%02d%02d%02d.tar.gz",
            t->tm_year + 1900, 
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
    
    printf("檔名: %s\n", filename);
    // backup_20250101_143025.tar.gz
    
    // 2. 建立 SQL 查詢
    char query[256];
    int user_id = 123;
    sprintf(query, "SELECT * FROM users WHERE id = %d", user_id);
    printf("SQL: %s\n", query);
    
    // 3. 格式化路徑
    char path[256];
    sprintf(path, "/home/%s/documents/%s", "user", "file.txt");
    printf("路徑: %s\n", path);
    
    return 0;
}
```

**sprintf vs snprintf vs asprintf**：

```c
char buf[50];

// sprintf - 不檢查長度（危險）
sprintf(buf, "%s", very_long_string);  // 可能溢出！

// snprintf - 安全版本
snprintf(buf, sizeof(buf), "%s", very_long_string);  // ✅ 安全

// asprintf - 自動分配記憶體（GNU 擴展）
char *result;
asprintf(&result, "%s", very_long_string);
// 使用 result...
free(result);  // 記得釋放
```

---

## 📊 函數對照表

| 函數/巨集 | 標頭檔 | 用途 | 返回值 |
|----------|--------|------|--------|
| `perror()` | `<stdio.h>` | 印出錯誤訊息 | void |
| `getpid()` | `<unistd.h>` | 取得自己的 PID | pid_t |
| `getppid()` | `<unistd.h>` | 取得父進程 PID | pid_t |
| `WIFEXITED()` | `<sys/wait.h>` | 是否正常退出 | int (bool) |
| `WEXITSTATUS()` | `<sys/wait.h>` | 取得退出碼 | int (0-255) |
| `WIFSIGNALED()` | `<sys/wait.h>` | 是否被信號終止 | int (bool) |
| `WTERMSIG()` | `<sys/wait.h>` | 取得終止信號 | int |
| `exit()` | `<stdlib.h>` | 結束進程 | 不返回 |
| `sleep()` | `<unistd.h>` | 暫停 (秒) | unsigned int |
| `sprintf()` | `<stdio.h>` | 格式化字串 | int (寫入字元數) |

---

---

## 🎓 快速查詢指南

### 常用函數速查

```c
// === 進程管理 ===
pid_t pid = fork();              // 創建子進程
pid_t my_pid = getpid();         // 取得自己的 PID
pid_t parent_pid = getppid();    // 取得父進程 PID
exit(0);                         // 結束進程（成功）
exit(1);                         // 結束進程（失敗）

// === 執行程式 ===
execlp("ls", "ls", "-l", NULL);  // 執行外部程式（PATH搜索）
execl("/bin/ls", "ls", "-l", NULL);  // 執行程式（完整路徑）
system("ls -l");                 // 執行 shell 命令

// === 等待子進程 ===
int status;
wait(&status);                   // 等待任意子進程
waitpid(pid, &status, 0);        // 等待特定子進程
waitpid(-1, &status, WNOHANG);   // 非阻塞檢查

// === 狀態檢查 ===
if (WIFEXITED(status)) {         // 是否正常退出？
    int code = WEXITSTATUS(status);  // 取得退出碼
}
if (WIFSIGNALED(status)) {       // 是否被信號終止？
    int sig = WTERMSIG(status);  // 取得信號編號
}

// === 錯誤處理 ===
perror("錯誤訊息");              // 印出系統錯誤
if (pid < 0) {
    perror("fork");
    exit(1);
}

// === 字串處理 ===
char buf[100];
sprintf(buf, "格式 %d", 123);    // 格式化字串
snprintf(buf, sizeof(buf), "安全格式 %s", str);  // 安全版本

// === 時間控制 ===
sleep(3);                        // 暫停 3 秒
usleep(500000);                  // 暫停 0.5 秒（微秒）
```

---

## 🔍 除錯技巧

### 1. 追蹤進程創建

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void print_process_info(const char *label) {
    printf("[%s] PID:%d, PPID:%d\n", label, getpid(), getppid());
}

int main() {
    print_process_info("程式開始");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        print_process_info("子進程");
        sleep(1);
    } else {
        print_process_info("父進程");
        wait(NULL);
    }
    
    print_process_info("結束");
    return 0;
}
```

### 2. 使用 strace 追蹤系統調用

```bash
# 追蹤程式的所有系統調用
strace ./your_program

# 只看 fork/exec/wait 相關
strace -e trace=fork,execve,wait4 ./your_program

# 追蹤子進程
strace -f ./your_program

# 輸出到檔案
strace -o trace.log ./your_program
```

### 3. 檢查殭屍進程

```bash
# 列出所有進程狀態
ps aux | head -1; ps aux | grep Z

# 查看進程樹
pstree -p

# 監控進程
top
# 按下 'f' 選擇顯示欄位，找到 S (state)
# Z = 殭屍進程
```

### 4. GDB 調試多進程

```bash
# 啟動 GDB
gdb ./your_program

# 設定跟隨模式
(gdb) set follow-fork-mode child   # 跟隨子進程
(gdb) set follow-fork-mode parent  # 跟隨父進程（預設）

# 設定分離模式
(gdb) set detach-on-fork off  # 同時調試父子進程

# 查看所有進程
(gdb) info inferiors

# 切換進程
(gdb) inferior 2

# 在 fork 處設定斷點
(gdb) break fork
(gdb) run
```

### 5. 日誌記錄最佳實踐

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

void log_message(const char *level, const char *msg) {
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    timestr[24] = '\0';  // 移除換行
    
    fprintf(stderr, "[%s] [%s] PID:%d - %s\n", 
            timestr, level, getpid(), msg);
}

int main() {
    log_message("INFO", "程式啟動");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        log_message("DEBUG", "子進程開始執行");
        sleep(1);
        log_message("INFO", "子進程完成");
        exit(0);
    } else if (pid > 0) {
        log_message("DEBUG", "父進程等待子進程");
        wait(NULL);
        log_message("INFO", "父進程完成");
    } else {
        log_message("ERROR", "fork 失敗");
        exit(1);
    }
    
    return 0;
}
```

---

## 💡 進階主題

### 1. 進程間通信（IPC）

#### 管道（Pipe）

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipefd[2];  // pipefd[0]=讀端, pipefd[1]=寫端
    char buffer[100];
    
    // 創建管道
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：寫入資料
        close(pipefd[0]);  // 關閉讀端
        
        const char *msg = "Hello from child!";
        write(pipefd[1], msg, strlen(msg) + 1);
        
        close(pipefd[1]);
        exit(0);
        
    } else {
        // 父進程：讀取資料
        close(pipefd[1]);  // 關閉寫端
        
        read(pipefd[0], buffer, sizeof(buffer));
        printf("父進程收到: %s\n", buffer);
        
        close(pipefd[0]);
        wait(NULL);
    }
    
    return 0;
}
```

#### 共享記憶體

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main() {
    // 創建共享記憶體
    int *shared = mmap(NULL, sizeof(int),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS,
                       -1, 0);
    
    if (shared == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    *shared = 0;  // 初始值
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：寫入
        for (int i = 0; i < 5; i++) {
            (*shared)++;
            printf("子進程: shared = %d\n", *shared);
            sleep(1);
        }
        exit(0);
        
    } else {
        // 父進程：讀取
        wait(NULL);
        printf("父進程: 最終值 = %d\n", *shared);
        
        // 清理
        munmap(shared, sizeof(int));
    }
    
    return 0;
}
```

### 2. 信號處理

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t got_signal = 0;

void signal_handler(int signum) {
    printf("\n收到信號 %d\n", signum);
    got_signal = 1;
}

int main() {
    // 設定信號處理器
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // kill 命令
    
    printf("程式運行中... (按 Ctrl+C 測試)\n");
    printf("PID: %d\n", getpid());
    
    while (!got_signal) {
        printf("工作中...\n");
        sleep(2);
    }
    
    printf("正在清理資源...\n");
    sleep(1);
    printf("程式正常退出\n");
    
    return 0;
}
```

### 3. 進程池實現

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define POOL_SIZE 4
#define TASK_COUNT 10

void worker_process(int worker_id) {
    printf("👷 工人 %d 啟動 (PID:%d)\n", worker_id, getpid());
    
    while (1) {
        // 實際應用中，這裡會從隊列獲取任務
        sleep(1);
        printf("工人 %d 處理任務\n", worker_id);
    }
}

int main() {
    pid_t workers[POOL_SIZE];
    
    printf("🏭 創建進程池 (大小: %d)\n", POOL_SIZE);
    
    // 創建工人進程
    for (int i = 0; i < POOL_SIZE; i++) {
        workers[i] = fork();
        
        if (workers[i] == 0) {
            // 子進程
            worker_process(i + 1);
            exit(0);
        } else if (workers[i] < 0) {
            perror("fork");
            exit(1);
        }
    }
    
    // 主進程：分配任務
    printf("📋 主進程分配任務...\n");
    sleep(5);  // 模擬運行
    
    // 終止所有工人
    printf("🛑 關閉進程池\n");
    for (int i = 0; i < POOL_SIZE; i++) {
        kill(workers[i], SIGTERM);
        waitpid(workers[i], NULL, 0);
    }
    
    printf("✅ 進程池已關閉\n");
    return 0;
}
```

### 4. 守護進程完整實現

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>

void daemonize() {
    pid_t pid;
    
    // 1. Fork 並讓父進程退出
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    
    // 2. 創建新會話
    if (setsid() < 0) exit(EXIT_FAILURE);
    
    // 3. 忽略信號
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    // 4. 再次 Fork
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    
    // 5. 設定文件權限掩碼
    umask(0);
    
    // 6. 改變工作目錄
    chdir("/");
    
    // 7. 關閉文件描述符
    for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
        close(fd);
    }
    
    // 8. 重定向標準輸入/輸出/錯誤到 /dev/null
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");
}

int main() {
    daemonize();
    
    // 使用 syslog 記錄日誌
    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "守護進程啟動");
    
    // 守護進程主循環
    while (1) {
        syslog(LOG_INFO, "守護進程運行中...");
        sleep(60);  // 每分鐘記錄一次
    }
    
    closelog();
    return 0;
}
```

---

## 📈 性能優化

### 1. 減少 fork 開銷

```c
// ❌ 低效：頻繁 fork
for (int i = 0; i < 1000; i++) {
    if (fork() == 0) {
        process_task(i);
        exit(0);
    }
    wait(NULL);
}

// ✅ 高效：使用進程池
#define WORKERS 8
for (int i = 0; i < WORKERS; i++) {
    if (fork() == 0) {
        // 工人進程持續處理任務
        while (has_tasks()) {
            process_task();
        }
        exit(0);
    }
}
```

### 2. 使用 vfork（謹慎使用）

```c
// vfork：不複製記憶體，更快但危險
pid_t pid = vfork();
if (pid == 0) {
    // ⚠️ 只能調用 exec 或 _exit
    execl("/bin/ls", "ls", NULL);
    _exit(1);  // 必須用 _exit，不能用 exit
}
```

### 3. Copy-on-Write 優化

```c
// fork 使用 COW（寫時複製）
// 只有寫入時才真正複製記憶體

int huge_array[1000000];  // 大陣列

pid_t pid = fork();
if (pid == 0) {
    // 只讀：不會複製記憶體
    printf("%d\n", huge_array[0]);
    
    // 寫入：才複製這一頁記憶體
    huge_array[0] = 42;
}
```

---

## 🔒 安全性考量

### 1. 避免命令注入

```c
// ❌ 危險
char cmd[256];
sprintf(cmd, "rm %s", user_input);
system(cmd);  // 容易被注入

// ✅ 安全
pid_t pid = fork();
if (pid == 0) {
    execlp("rm", "rm", user_input, NULL);  // 不經過 shell
    exit(1);
}
wait(NULL);
```

### 2. 驗證輸入

```c
#include <ctype.h>
#include <string.h>

int is_safe_filename(const char *filename) {
    // 只允許字母、數字、底線、點、減號
    for (int i = 0; filename[i]; i++) {
        char c = filename[i];
        if (!isalnum(c) && c != '_' && c != '.' && c != '-') {
            return 0;
        }
    }
    
    // 不允許路徑遍歷
    if (strstr(filename, "..") != NULL) {
        return 0;
    }
    
    return 1;
}
```

### 3. 限制資源使用

```c
#include <sys/resource.h>

void limit_resources() {
    struct rlimit limit;
    
    // 限制 CPU 時間（秒）
    limit.rlim_cur = 5;
    limit.rlim_max = 5;
    setrlimit(RLIMIT_CPU, &limit);
    
    // 限制記憶體（位元組）
    limit.rlim_cur = 100 * 1024 * 1024;  // 100 MB
    limit.rlim_max = 100 * 1024 * 1024;
    setrlimit(RLIMIT_AS, &limit);
    
    // 限制進程數
    limit.rlim_cur = 10;
    limit.rlim_max = 10;
    setrlimit(RLIMIT_NPROC, &limit);
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        limit_resources();
        // 執行不受信任的代碼...
        exit(0);
    }
    wait(NULL);
    return 0;
}
```

---

## 🎯 實戰項目

### 項目 1：簡易 Web 服務器

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 10

void handle_client(int client_fd) {
    char buffer[1024];
    read(client_fd, buffer, sizeof(buffer));
    
    // 簡單的 HTTP 回應
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<h1>Hello from fork()!</h1>"
        "<p>Process ID: %d</p>";
    
    char html[2048];
    sprintf(html, response, getpid());
    write(client_fd, html, strlen(html));
    
    close(client_fd);
}

void sigchld_handler(int sig) {
    // 回收殭屍進程
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    
    // 設定信號處理
    signal(SIGCHLD, sigchld_handler);
    
    // 創建 socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 綁定地址
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, BACKLOG);
    
    printf("🌐 服務器運行在 http://localhost:%d\n", PORT);
    
    // 主循環
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        
        // 為每個客戶端創建子進程
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程處理客戶端
            close(server_fd);  // 不需要監聽 socket
            handle_client(client_fd);
            exit(0);
            
        } else if (pid > 0) {
            // 父進程繼續接受連接
            close(client_fd);  // 父進程不處理客戶端
            
        } else {
            perror("fork");
        }
    }
    
    close(server_fd);
    return 0;
}
```

### 項目 2：並行文件搜索

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

void search_in_file(const char *filename, const char *pattern) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    
    char line[1024];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        if (strstr(line, pattern)) {
            printf("[PID %d] %s:%d: %s", getpid(), filename, line_num, line);
        }
    }
    
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "用法: %s <搜索字串>\n", argv[0]);
        return 1;
    }
    
    const char *pattern = argv[1];
    DIR *dir = opendir(".");
    struct dirent *entry;
    int child_count = 0;
    
    printf("🔍 搜索 '%s' 中...\n\n", pattern);
    
    while ((entry = readdir(dir)) != NULL) {
        // 只處理 .c 和 .txt 文件
        if (strstr(entry->d_name, ".c") || strstr(entry->d_name, ".txt")) {
            
            pid_t pid = fork();
            
            if (pid == 0) {
                // 子進程搜索一個文件
                search_in_file(entry->d_name, pattern);
                exit(0);
                
            } else if (pid > 0) {
                child_count++;
            }
        }
    }
    
    closedir(dir);
    
    // 等待所有子進程
    for (int i = 0; i < child_count; i++) {
        wait(NULL);
    }
    
    printf("\n✅ 搜索完成\n");
    return 0;
}
```

---

## 📚 延伸閱讀

### 推薦書籍
1. **Advanced Programming in the UNIX Environment (APUE)** - W. Richard Stevens
2. **The Linux Programming Interface** - Michael Kerrisk
3. **Unix Network Programming** - W. Richard Stevens

### 線上資源
- [Linux Man Pages](https://man7.org/linux/man-pages/)
- [fork(2) manual](https://man7.org/linux/man-pages/man2/fork.2.html)
- [exec(3) manual](https://man7.org/linux/man-pages/man3/exec.3.html)
- [wait(2) manual](https://man7.org/linux/man-pages/man2/wait.2.html)

### 相關主題
- 執行緒 (Threads) vs 進程 (Processes)
- 協程 (Coroutines)
- 非同步 I/O
- 容器化技術 (Docker, LXC)

---

**Happy Coding! 🚀**