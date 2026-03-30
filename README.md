# FirefliesChicultureAutoWorker

### 這是一個專為篇篇流螢所開發的自動答題桌面工具。

> [!IMPORTANT]
> 本專案已停止支援，欲取得新版請前往 [這裡](https://github.com/132DC5B/FirefliesChicultureAutoWorker)

## 開始使用 (CLI)

您可以直接呼叫程式 `firefliesChicultureAutoWorker.exe` 及 `getAccessID.exe`。

### 1. 取得 Cookie (`getAccessID.exe`)

* 獲取學生 Cookie (預設)：`getAccessID.exe -s -em "你的信箱" -pw "你的密碼"`
* 獲取管理員 Cookie：`getAccessID.exe -a -em "你的信箱" -pw "你的密碼"`

### 2. 執行自動任務 (`firefliesChicultureAutoWorker.exe`)

取得對應的 Cookie 檔案 (`cookies_student.txt` / `cookies_admin.txt`) 後，可使用以下參數執行主程式：

```bash
firefliesChicultureAutoWorker.exe [-f | -nf <dates>] [-a] [-e]
```

* `-f` : 讀取同目錄下的 `date.txt` 檔案作為任務日期（預設行為）。
* `-nf <dates>` : 不讀取檔案，直接由參數傳入日期（例如 `-nf 2024-06-01,2024-06-10/2024-06-12`）。
* `-a` : 開啟隨機作答模式 (Random answer mode)。若無管理員 cookie 則必須加上此參數。
* `-e` : 停用額外閱讀提交 (Disable extra read)。

## 支援的日期格式 `date.txt`：
* **單一日期**：直接輸入日期，例如 `2024-06-01`
* **日期區間**：使用斜線 `/` 連接起始與結束日期，例如 `2024-06-01/2024-06-05`

您可以換行輸入多組不同的設定：
```text
2024-06-01
2024-06-10/2024-06-14
2024-06-18

```

>[!NOTE]
>*程式會自動判斷並忽略期間內的週末假日*
