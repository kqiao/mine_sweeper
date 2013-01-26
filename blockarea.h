#ifndef BLOCKAREA_H
#define BLOCKAREA_H

#include <QWidget>
class QGridLayout;

class BlockArea : public QWidget
{
    Q_OBJECT
public:
    explicit BlockArea(int _r, int _c, int _m, QWidget *parent = 0);
    void set_block_area(int _r, int _c, int _m, const bool is_init);

protected:
    bool eventFilter(QObject *target, QEvent *event);

signals:
    void game_over(bool is_win);
    void set_current_mines(int n);
    
public slots:
    void slot_turn_over(bool);
    void slot_change_mines(int n);

private:
    QGridLayout * grid_layout;

    int row;
    int column;
    int total_mines;
    int left_mines;
    int total_blocks;
    int turn_over_blocks;

    bool game_over_flag;

    int calc_number(int x, int y);
    void try_to_turn_more_blocks(int x, int y);
};

#endif // BLOCKAREA_H
