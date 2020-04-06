## 
- check_urdf コマンド
    - デフォルトでは check_urdf コマンドがインストールされていないため、以下のコマンドでインストールする
        ```
        $ sudo apt install liburdfdom-tools
        ```
- rviz でリンク1より先が表示できないエラー
    - "Could not find the GUI, install the 'joint_state_publisher_gui' package" と怒られているので、パッケージをインストールする
    - 特に他の依存関係はなくビルド成功
    - 表示も正常に行えるようになった
- joint_state_publisher のGUIが表示されない
    - デフォルトで設定がTrueになっているが表示されない
    - 代わりに、rqt の Message Publisher (rqt_publisher) を使用できる
    - ただし、display.lauch を編集して、デフォルトの joint_state_publisher を起動しないようにしておく必要がある
        - 以下のコマンドで、launch ファイルをエディタで開いて編集する
            ```
            $ sudo chmod 777 /path/to/launch/file
            $ code -r /path/to/launch/file
            ```
        - 以下の行を削除する
            ```
            <node name="joint_state_publisher" pkg="joint_state_publisher" type="joint_state_publisher" />
            ```
        - 編集が終わったらアクセス権をもとに戻しておく
            ```
            $ sudo chmod 644 /path/to/launch/file
            ```
    - また、時刻が加算されないと反映されない
        - Header の sec パラメータを加算する
