## ハードウェア
- [Raspberry Pi 4](https://www.amazon.co.jp/【国内正規代理店品】Raspberry-Pi4-ModelB-ラズベリーパイ4-技適対応品/dp/B081YD3VL5/ref=pd_aw_sbs_147_2/355-3642707-0969305?_encoding=UTF8&pd_rd_i=B081YD3VL5&pd_rd_r=198d8230-6677-4d1a-9a22-4a762da6b1d7&pd_rd_w=wOBTW&pd_rd_wg=GgPO4&pf_rd_p=bff3a3a6-0f6e-4187-bd60-25e75d4c1c8f&pf_rd_r=N93TP7A532W3P6E245W3&psc=1&refRID=N93TP7A532W3P6E245W3)
- [6DOF ロボットアーム](https://www.amazon.co.jp/gp/product/B07M7TK6KR/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
- [0-48V/10A AC/DC変換変圧器](https://www.amazon.co.jp/gp/product/B07538V92C/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)
- [12A 電源コード](https://www.amazon.co.jp/gp/product/B00FIXSEO8/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)
- [サーボモーターコネクター延長ケーブル](https://www.amazon.co.jp/gp/product/B07PFCJ96G/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1)
- [ブレッドボード](https://www.amazon.co.jp/ELEGOO-400タイポイント-ブレッドボード3PCS-、Arduino用ジャンパーワイヤ-4電源レール/dp/B071CVLWZ2/ref=sr_1_3?__mk_ja_JP=%E3%82%AB%E3%82%BF%E3%82%AB%E3%83%8A&dchild=1&keywords=%E3%83%96%E3%83%AC%E3%83%83%E3%83%89%E3%83%9C%E3%83%BC%E3%83%89&qid=1586906713&s=instant-video&sr=8-3)
- [ジャンプワイヤー](https://www.amazon.co.jp/gp/product/B00J3E1206/ref=ppx_yo_dt_b_asin_title_o00_s01?ie=UTF8&psc=1)
- [フレキシブルジャンプワイヤ―](https://www.amazon.co.jp/ELEGOO-Arduino用-フレキシブル-ジャンパーワイヤ130pcs-forブレッドボード/dp/B0713S9M4M/ref=pd_bxgy_img_3/355-3642707-0969305?_encoding=UTF8&pd_rd_i=B0713S9M4M&pd_rd_r=556f2840-f242-4b58-8c12-c22a4864bd24&pd_rd_w=64xt3&pd_rd_wg=mkCfE&pf_rd_p=3ed3024e-aea2-49a7-ac95-7612edc065d7&pf_rd_r=4Y01HP9A9ME1JCCNFCKZ&psc=1&refRID=4Y01HP9A9ME1JCCNFCKZ)
- [GPIO ブレイクアウトボード](https://www.amazon.co.jp/gp/product/B07P6PJT4Z/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
- [DCジャックDIP化キット (10A)](http://akizukidenshi.com/catalog/g/gK-08415/)
- [DCプラグ (5A)](http://akizukidenshi.com/catalog/g/gC-09311/)
    - または詳細スペックは不明だが使いやすい[こっち](http://akizukidenshi.com/catalog/g/gC-08847/)
- [ビニル平行電線 (0.5～1.25SQ程度)](https://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=76UE-4GK2)
- [サーボテスター（必須ではない）](https://www.amazon.co.jp/gp/product/B07R8SBS5S/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&psc=1)
- その他
    - LED, 抵抗器 (200～300Ω程度), ピンヘッダ

## ソフトウェア
### 環境構築
- Raspberry Pi への ROS インストール -> [参考](https://qiita.com/Ninagawa_Izumi/items/063d9d4910a19e9fcdec)
- Windows との連携
    - VSCode との連携
        - SSH Development をインストール
        - Windows上で、以下のコマンドを実行してキーファイルを生成する（事前にGitのインストールが必要）
            ```
            $ ssh-keygen -t rsa -b 4096 -C "your_mail@address"
            ```
        - このへんを[参考](https://qiita.com/dearmarie/items/4e8c7bb5d71d4f626b74)にして接続
    - [GUIの表示](https://www.atmarkit.co.jp/ait/articles/1812/06/news040.html)
        - [この設定も必要](https://qiita.com/masa-tu/items/20111878476bb97a9d07)
        - 出力先のモニターを変更する場合は、~/.bashrc の以下の記述を変更する
            ```
            DISPLAY=xxx.xxx.xxx.xxx:0.0
            ```
    - ROS の追加機能をビルドする
        - opencv, gtk をインストールしておく
            ```
            $ sudo apt install libopencv-dev
            $ sudo apt install libgtk-3-dev
            ```
            - ビルド中に「... not found」のエラーが出たらインストールする
        - [image_common](https://github.com/ros-perception/image_common)
        - [vision_opencv](https://github.com/ros-perception/vision_opencv)
        - [geometry2](https://github.com/ros/geometry2)
        - [image_pipeline](https://github.com/ros-perception/image_pipeline)
        - [usb_cam](https://github.com/ros-drivers/usb_cam)
        - ビルド時に OpenCV 関連のエラーが出る場合は、/usr/share/OpenCV 以下に cmake の config ファイルがあるかどうかを確認

### モデルの作成
- [参考](https://qiita.com/yoneken/items/ed2e5edf3aa4e0d8d2e3)
- [複数のリンクを同時に動かす](https://answers.ros.org/question/341365/urdf-attaching-two-joints-to-one-link-for-a-gripper/)
- check_urdf コマンド
    - デフォルトでは check_urdf コマンドがインストールされていないため、以下のコマンドでインストールする
        ```
        $ sudo apt install liburdfdom-tools
        ```
- rviz でリンク1より先が表示できないエラー
    - "Could not find the GUI, install the 'joint_state_publisher_gui' package" と怒られているので、パッケージをビルドする
        - [joint_state_publisher](https://github.com/ros/joint_state_publisher)
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
    - 原因が判明
        - GUI が表示されない原因は、display.launch ファイルが間違っている（joint_state_publisher_gui が使われていない）ため
        - オリジナル
            ```
            <launch>

            <arg name="model" default="$(find urdf_tutorial)/urdf/01-myfirst.urdf"/>
            <arg name="gui" default="true" />
            <arg name="rvizconfig" default="$(find urdf_tutorial)/rviz/urdf.rviz" />

            <param name="robot_description" command="$(find xacro)/xacro.py $(arg model)" />
            <param name="use_gui" value="$(arg gui)"/>

            <node name="joint_state_publisher" pkg="joint_state_publisher" type="joint_state_publisher" />
            <node name="robot_state_publisher" pkg="robot_state_publisher" type="state_publisher" />
            <node name="rviz" pkg="rviz" type="rviz" args="-d $(arg rvizconfig)" required="true" />

            </launch>
            ```
        - [最新版](https://github.com/ros/urdf_tutorial/blob/master/launch/display.launch)
            ```
            <launch>

            <arg name="model" default="$(find urdf_tutorial)/urdf/01-myfirst.urdf"/>
            <arg name="gui" default="true" />
            <arg name="rvizconfig" default="$(find urdf_tutorial)/rviz/urdf.rviz" />

            <param name="robot_description" command="$(find xacro)/xacro $(arg model)" />

            <node if="$(arg gui)" name="joint_state_publisher" pkg="joint_state_publisher_gui" type="joint_state_publisher_gui" />
            <node unless="$(arg gui)" name="joint_state_publisher" pkg="joint_state_publisher" type="joint_state_publisher" />
            <node name="robot_state_publisher" pkg="robot_state_publisher" type="robot_state_publisher" />
            <node name="rviz" pkg="rviz" type="rviz" args="-d $(arg rvizconfig)" required="true" />

            </launch>
            ```
    - グリッパーモデルの表示
        - [ココ](http://wiki.ros.org/urdf/Tutorials/Building%20a%20Visual%20Robot%20Model%20with%20URDF%20from%20Scratch)を参照
    - カメラモデルの表示
        - [ココ](https://github.com/OUXT-Polaris/ros_ship_packages) からSTLモデルを借用
        
### GPIO によるサーボモーターの制御
- [参考](https://mozyanari.hatenablog.com/entry/2019/07/25/113932)

### 認識機能の導入
- [参考](https://www.youtube.com/watch?v=aimSGOAUI8Y)
- 以下のコマンドで実行
    ```
    $ cd ~/Workspace/git/tflite1
    $ source tflite-env/bin/activate
    $ python3 TFLite_detection_webcam.py --modeldir=Sample_TFLite_model --resolution=640x480
    ```
- OpenCVを使ったカメラ画像の取得
    - [参考](https://qiita.com/vs4sh/items/4a9ce178f1b2fd26ea30)
- tflite on raspberry pi のC++サンプル
    - [参考1 : 数字認識](https://qiita.com/iwatake2222/items/4d198f6203348ef7fd31)
    - [参考2 : カメラ画像の認識](https://github.com/mattn/webcam-detect-tflite/blob/master/main.cxx)
- ROSへの組み込み（OpenCV + ROS）
    - [参考 : image_trasport の使い方](https://ya10345.hatenablog.com/entry/2019/05/19/031443)

