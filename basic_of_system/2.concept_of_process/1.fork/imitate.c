#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(void) {
    pid_t pid;
    int worker;

    // 提示 + 立即刷新，避免 fork 後重複 flush 或看不到提示
    printf("How many worker do you want to ask? ");
    fflush(stdout);

    if (scanf("%d", &worker) != 1 || worker <= 0) {
        fprintf(stderr, "Invalid worker count.\n");
        return 1;
    }

    // 這個種子只影響父行程；子行程內會再各自播種
    srand((unsigned)time(NULL));

    for (int i = 1; i <= worker; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            // 視情況中止或繼續；這裡選擇中止
            return 1;
        }
        if (pid == 0) {
            // 子行程：各自播種，確保亂數不同
            unsigned seed = (unsigned)(time(NULL) ^ getpid() ^ (i << 16));
            srand(seed); // or srand(i) that is more easy

            printf("Worker%d (PID:%d) start working\n", i, (int)getpid());

            int delay = rand() % 6;      // 0~5 秒
            sleep(delay);

            printf("Worker%d complete the product\n", i);

            // _exit 避免重覆 flush 父行程繼承來的 stdio 緩衝
            _exit(i & 0xFF);             // 只保留 0~255
        }
        // 父行程：繼續產生下一位 worker
    }

    printf("Boss is waiting the whole worker\n");

    int collected = 0;
    while (collected < worker) {
        int status = 0;
        pid_t finished = wait(&status);
        if (finished == -1) {
            // 沒有更多子行程可回收（或出錯）
            perror("wait");
            break;
        }

        if (WIFEXITED(status)) {
            int product_id = WEXITSTATUS(status);
            printf("Receive product No.%d (from PID: %d)\n",
                   product_id, (int)finished);
        } else if (WIFSIGNALED(status)) {
            printf("Worker (PID:%d) terminated by signal %d\n",
                   (int)finished, WTERMSIG(status));
        }
        collected++;
    }

    printf("All product are done\n");
    return 0;
}
