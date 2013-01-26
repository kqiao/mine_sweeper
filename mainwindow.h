#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "blockarea.h"
class QMenu;
class QAction;
class QActionGroup;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void slot_new_game();
    void slot_rank();

    void slot_show_status(bool);
    void slot_standard_modes(QAction *);
    void slot_game_over(bool is_win);

    void slot_about_mine();

    void slot_timer();

private:
    QMenu * game_menu;
    QMenu * setting_menu;
    QMenu * help_menu;

    QAction * new_game;
    QAction * rank;
    QAction * exit;

    QAction * show_status;
    QAction * easy_mode;
    QAction * middle_mode;
    QAction * hard_mode;
    QAction * custom_mode;
    QActionGroup * standard_modes;

    QAction * about_mine;
    QAction * about_qt;
    QLabel * time_label;
    QLabel * left_mine_label;

    QTimer * ticker;
    QTime * time;
    BlockArea * block_area;

    int easy_record_time;
    int middle_record_time;
    int hard_record_time;
    QString easy_record_name;
    QString middle_record_name;
    QString hard_record_name;

    int row;
    int column;
    int total_mines;

    int current_standard;

    void create_menus();
    void create_actions();
    void create_statusBar();

    void read_setting();
    void write_setting();
};

#endif // MAINWINDOW_H
