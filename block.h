#ifndef BLOCK_H
#define BLOCK_H

#include <QLabel>
#include <QWidget>

class Block : public QLabel
{
    Q_OBJECT
public:
    explicit Block(bool is_mine, QWidget *parent = 0);
    void set_number(int n);

    bool is_mine();
    bool is_turn_over();
    void set_turn_over();
    
signals:
    void turn_over(bool is_mine);  //定义信号
    void change_mines(int n);
    
protected:
    void mousePressEvent(QMouseEvent *ev);
    //自定义按下鼠标的事件

private:
    int number;
    bool turn_over_flag;
    bool is_mine_flag;  //表示实际上该位置是不是雷
    bool mark_flag;     //表示玩家是否对该位置进行了插旗标记

    
};

#endif // BLOCK_H
