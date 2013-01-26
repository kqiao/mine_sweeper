#include "blockarea.h"
#include <QGridLayout>
#include "block.h"
#include <QTime>
#include <QEvent>
#include <QQueue>
#include <QPair>

BlockArea::BlockArea(int _r, int _c, int _m, QWidget *parent) :
    QWidget(parent)
{
    set_block_area(_r, _c, _m, true);
}

void BlockArea::set_block_area(int _r, int _c, int _m, const bool is_init)
{
    if(!is_init)
    {
        for(int i = 0; i < row; i++)
            for(int j = 0; j < column; j++)
                delete static_cast<Block*>(grid_layout->itemAtPosition(i,j)->widget());
        delete grid_layout;
    }
    row = _r;
    column = _c;
    total_mines = _m;
    left_mines = _m;
    total_blocks = row * column;
    turn_over_blocks = 0;
    game_over_flag = false;     //在每次新游戏中，game_over_flag都要重新设置为false。

    bool *mine_flag = new bool[total_blocks];   //变量做长度也能成功？？？
    for(int i = 0; i < total_blocks; i++)
        mine_flag[i] = false;
    //产生total_mines个随机数作为雷区的分布
    QTime cur_time = QTime::currentTime();
    qsrand(cur_time.msec()+cur_time.second()*1000);
    int i = 0;
    //这种布雷的方法没有采用shuffle的方法优秀，因为当雷数过多时，可能一直得到与前面重复的随机数
    while(i < total_mines)
    {
        int n = qrand()%total_blocks;
        if(!mine_flag[n])
        {
            mine_flag[n] = true;
            i++;
        }
    }
    //在上面随机产生的点布雷，只有当完成布雷后才能计算其余位置的数字
    grid_layout = new QGridLayout(this);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
            grid_layout->addWidget(new Block(mine_flag[i*column+j]), i, j);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
        {
            Block * cur_block = static_cast<Block*>(grid_layout->itemAtPosition(i,j)->widget());
            cur_block->set_number(calc_number(i, j));
            connect(cur_block, SIGNAL(turn_over(bool)), this, SLOT(slot_turn_over(bool)));
            connect(cur_block, SIGNAL(change_mines(int)), this, SLOT(slot_change_mines(int)));
            cur_block->installEventFilter(this);
        }
    delete []mine_flag;
}
//事件过滤器，用于当游戏结束后，不能再对blockArea区域的鼠标事件进行响应
bool BlockArea::eventFilter(QObject *target, QEvent *event)
{
    if(game_over_flag)
        if(event->type()==QEvent::MouseButtonPress)
            return true;
    return QWidget::eventFilter(target, event);
}

//计算当前位置应该填入的数字，注意不需要判断该位置是否有雷，因为当有雷时，显示的一定是有雷的图片
int BlockArea::calc_number(int x, int y)
{
    int count = 0;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(x-1+i>=0 && x-1+i<row && y-1+j>=0 && y-1+j<column)
                if(static_cast<Block*>(grid_layout->itemAtPosition(x-1+i, y-1+j)->widget())->is_mine())
                    count++;
    return count;
}

void BlockArea::slot_turn_over(bool is_mine)
{
    if(is_mine){
        game_over_flag = true;
        emit game_over(false);      //以失败结束游戏
    }else{
        ++turn_over_blocks;
        if(turn_over_blocks == total_blocks-total_mines){
            game_over_flag = true;
            emit game_over(true);       //成功结束游戏
        }else{
            for(int i = 0; i < row; i++)
                for(int j = 0; j < column; j++)
                {
                    Block * current = static_cast<Block*>(grid_layout->itemAtPosition(i,j)->widget());
                    if(sender()== current && calc_number(i, j)==0)
                        try_to_turn_more_blocks(i,j);
                }
        }
    }
}

void BlockArea::slot_change_mines(int n)
{
    left_mines += n;
    emit set_current_mines(left_mines);
}

//扩展更多块的原则是：检查当前位置的数值是否为0，如果为0，就扩展它周围的8个位置
//此时这8个位置一定都是数字，并且要先判断该位置是否被turn_over过，如果没有，则查看数字并选择是否扩展
//如果在8个位置中还发现了数值为0的块，就把它加入队列，继续扩展直到队列为空
void BlockArea::try_to_turn_more_blocks(int x, int y)
{
    QQueue<QPair<int, int> > queue;
    queue.enqueue(qMakePair(x, y));
    QPair<int, int> pair;
    while(!queue.isEmpty())
    {
        pair = queue.dequeue();
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
            {
                //pair.first为public变量
                if(pair.first-1+i>=0 && pair.first-1+i<row && pair.second-1+j>=0 && pair.second-1+j<column){
                    Block * current = static_cast<Block*>(grid_layout->itemAtPosition(pair.first-1+i, pair.second-1+j)->widget());
                    if(!current->is_turn_over()){
                        current->set_turn_over();
                        ++turn_over_blocks;
                        if(calc_number(pair.first-1+i, pair.second-1+j) == 0)
                            queue.enqueue(qMakePair(pair.first-1+i, pair.second-1+j));
                    }
                }
            }
    }
}
