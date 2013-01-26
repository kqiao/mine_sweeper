#include "block.h"
#include <QPixmap>
#include <QMouseEvent>

Block::Block(bool is_mine, QWidget *parent) :
    QLabel(parent)
{
    is_mine_flag = is_mine;
    turn_over_flag = false;
    mark_flag = false;
    number = -1;
    setPixmap(QPixmap(":/images/normal.png"));
}

void Block::set_number(int n)
{
    number = n;
}

void Block::set_tip()
{
    is_get_tip = true;
}

bool Block::is_mine()
{
    return is_mine_flag;
}

bool Block::is_turn_over()
{
    return turn_over_flag;
}

bool Block::is_marked()
{
    return mark_flag;
}

void Block::set_turn_over()
{
    if(!turn_over_flag)
    {
        turn_over_flag = true;
        if(is_mine_flag)
            setPixmap(QPixmap(tr(":/images/mine.png")));
        else
            setPixmap(QPixmap(tr(":/images/mine_%1.png").arg(number)));
    }
}

void Block::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()== Qt::LeftButton){   //QMouseEvent->button()返回当前按下哪个按键
        if(!turn_over_flag && !mark_flag){  //只有没有被翻过的块或没有插旗的块才允许被翻面
            turn_over_flag = true;
            if(is_mine_flag){
                setPixmap(QPixmap(tr(":/images/mine.png")));
             //   update();   //立即更新该位置的图像
                emit turn_over(true);   //参数表示是否是雷
            }else{
                setPixmap(QPixmap(tr(":/images/mine_%1.png").arg(number)));
              //  update();
                emit turn_over(false);
            }
        }
    }else if(ev->button() == Qt::RightButton){  //该情况下不要设置turn_over_flag为true。
        if(!turn_over_flag){
            if(!mark_flag){     //normal下的变为插上旗
                mark_flag = true;
                if(is_get_tip)
                    setPixmap(QPixmap(tr(":/images/flag_tip.png")));
                else
                    setPixmap(QPixmap(tr(":/images/flag.png")));
                emit change_mines(1);
            }else{          //已经插过旗的，变为normal状态
                mark_flag = false;
                if(is_get_tip)
                    setPixmap(QPixmap(tr(":/images/normal_tip.png")));
                else
                    setPixmap(QPixmap(tr(":/images/normal.png")));
                emit change_mines(-1);
            }
            update();
        }
    }
    QLabel::mousePressEvent(ev);
}
