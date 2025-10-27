#!/usr/bin/env python3
import sys
import argparse

def main():
    # 建立命令列參數解析器
    parser = argparse.ArgumentParser(description="簡單的 Python 測試腳本")
    parser.add_argument("--verbose", action="store_true", help="顯示詳細訊息")
    parser.add_argument("--output", type=str, help="輸出檔案名稱")

    # 解析參數
    args = parser.parse_args()

    # 若有 verbose 選項，印出詳細訊息
    if args.verbose:
        print("🔍 Verbose 模式開啟")
        print(f"📝 輸出檔案將寫入：{args.output}")
        print(f"📜 收到的 argv: {sys.argv}\n")

    # 寫入結果到指定檔案
    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write("這是 my_script.py 產生的結果！\n")
            f.write("參數：\n")
            for arg in sys.argv:
                f.write(f"  {arg}\n")
        print("✅ 已成功寫入結果到", args.output)
    else:
        print("⚠️ 沒有指定輸出檔案。")

if __name__ == "__main__":
    main()
