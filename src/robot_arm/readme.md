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
            
