## 问题收集


### GameAf
* 控制台隐藏, 使用了FreeConsole进行隐藏, 并且编译项目仍然是控制台程序(不使用应用程序的原因: 1. 应用程序显示的控制台无法让utf8使其作用, 或许可以作用, 但是太麻烦了)， 利用了关闭窗口后会失去焦点事件(无法直接检测窗口关闭事件), 利用IsWindow检测窗口是否关闭, 用来减缓查询api次数

### Audio
* 部分mp3音频 MCI不支持播放
  - 似乎是存在元数据(比如图片信息), 安装ffmpeg，使用``ffmpeg -i "Evan Call - For 1000 Years.mp3" -vn -c:a copy "output.mp3`` 去掉所有的视频流生成新的mp3解决问题
  - 名称里带空格也识别不出来
* MCI播放音频时不时会出现卡顿现象
  * 来回open似乎可以解决此问题?很奇怪
* wav格式无法使用repeat指定命令其重复播放(mp3可以)