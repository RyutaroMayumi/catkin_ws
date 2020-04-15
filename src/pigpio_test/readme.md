## Lチカの手順
- [参考](https://mozyanari.hatenablog.com/entry/2019/06/27/114108)
1. ラズパイの37番ピン（GPIO26）をLEDの＋極、39番ピン（Ground）をLEDの－極に接続する。ただし、LEDと100Ω程度の抵抗を直列につないでおく。
2. roscore を起動する
3. 以下のコマンドで pigpio サービスを起動する
    ```
    sudo pigpiod
    ```
4. 以下のコマンドでノードを起動する
    ```
    rosrun pigpio_test pigpio_test
    ```
5. 以下のコマンドで /led トピックを送信する
    ```
    rostopic pub -1 /led std_msgs/Bool "true"
    ```
6. LED が点灯する
7. 以下のコマンドで /led トピックを送信する
    ```
    rostopic pub -1 /led std_msgs/Bool "false"
    ```
8. LED が消灯する
