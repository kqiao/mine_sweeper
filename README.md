扫雷
==========================

  该项目是一个用Qt实现的类似于windows下的扫雷游戏。

1.基本逻辑：

  1）左键点击一个雷区，如果是地雷，则游戏结束；如果是数字，表示周围8个位置有多少地雷，如果数字是0，自动扩展周围区域
  
  2）右键点击时，插旗标记是地雷，再次右键取消标记；
  
  3）打开所有非地雷区，游戏通关。
  
2.设置 简单、中等、困难三个模式，也可以自定义模式，设置雷区大小和地雷数。

3.记录三个模式下各自的最好成绩，并在下次打开时保持。

4.（个人爱好）添加了个“一键显示所有地雷”的帮组按钮
