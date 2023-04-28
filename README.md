# **CircusEasyUART**
## 使用方式

---

### Include CircusUart Library

```arduino
#include <CircusUart.h>
```

### 建立 CircusUart Object

建立一或多個 CircusUart Object，並在建構時傳入要使用的序列埠 Object，後續將會從此序列埠收發資料。

```arduino
CircusUart uart(Serial2);
```

### 初始化

在使用 CircusEasyUart 之前，需要先初始化目標序列埠。

Ex: 使用 Serial2 作為輸入輸出序列埠，在 Setup() 方法中使用 Serial2.begin() 初始化序列埠。

```arduino
void setup()
{
    Serial2.begin(57600);
}
```

### 註冊事件

在 Setup() 方法中註冊事件，以便在接收到指定訊息時執行相應的操作。例如，在這個例子中，使用 uart.on() 函數註冊了名為 "Temp" 和 "Humi" 的事件，並以匿名方式分別指定了它們的處理函數（Callback Function），如下所示：

```arduino
void setup()
{
    // ...
		// 當指定 Serial 收到 "Temp:23.4\r\n" 將會自動呼叫此事件，其中匿名方法傳入參數(*data)為"23.4"
		// Output: Temperature: 23.4
    uart.on("Temp", ':', [](const char *data) {
        Serial.print("Temperature: ");
        Serial.println(data);
    });
		// 當指定 Serial 收到 "Humi:56.7\r\n" 將會自動呼叫此事件，其中匿名方法傳入參數(*data)為"56.7"
		// Output: Humidity: 56.7
    uart.on("Humi", ':', [](const char *data) {
        Serial.print("Humidity: ");
        Serial.println(data);
    });
    // ...
}
```

### 發送命令

可以使用 send() 函數發送數據。例如，在這個例子中，每隔一秒鐘就發送一個名為 "Motor1" 的事件，值為 "128"：

```arduino
void loop()
{
    static uint32_t timer = 0;
    if (millis() > timer) {
        timer = millis() + 1000;
        uart.send("Motor1", ':', "128"); // 等同於 Serial.println("Motor1:128");
    }
}
```

### 定時輪巡

需要定時呼叫 loop 方法檢查是否有收到新資料，盡可能的即時呼叫此方法，如下所示：

```arduino
void loop()
{
    uart.loop();
}
```

### 許可證

CircusEasyUART 基於 MIT 許可證發布。請參閱 LICENSE 文件以獲取更多詳細信息。
