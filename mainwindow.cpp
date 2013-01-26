#include "mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QStatusBar>
#include <QIcon>
#include <QKeySequence>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPixmap>
#include <QApplication>
#include <QMessageBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QSettings>

const QString g_software_version = "0.1";
const QString g_software_author = "kqiao";
const QString g_software_name = "mine sweeper";

const int g_no_record_time = 9999;
const QString g_no_record_name = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    ticker = new QTimer(this);  //ticker每用于每秒钟改变计时表的显示
    connect(ticker, SIGNAL(timeout()), this, SLOT(slot_timer()));
    ticker->setInterval(1000);  //设置时间间隔为1s
    time = new QTime;    //time是系统钟表，默认从(0, 0, 0, 0)开始计时
    time_label = new QLabel;
    left_mine_label = new QLabel;

    create_actions();
    create_menus();
    create_statusBar();

    /////////////////////读取QSetting类
    /*
    current_standard = 0;   //当前默认为 初级
    easy_record_name = middle_record_name = hard_record_name = "";
    easy_record_time = middle_record_time = hard_record_time = 9999;
    row = 9;
    column = 9;
    total_mines = 10;
    left_mine_label->setNum(10);
*/
    QCoreApplication::setApplicationName(g_software_name);
    QCoreApplication::setApplicationVersion(g_software_version);
    QCoreApplication::setOrganizationName(g_software_author);
    read_setting();

    ///////////////////////////


    block_area = new BlockArea(row, column, total_mines);
    connect(block_area, SIGNAL(game_over(bool)), this, SLOT(slot_game_over(bool)));
    connect(block_area, SIGNAL(set_current_mines(int)), left_mine_label, SLOT(setNum(int)));
    setCentralWidget(block_area);


    connect(show_all_mines, SIGNAL(triggered()), block_area, SLOT(slot_show_all_mines()));


    setWindowTitle(tr("Mine Sweeper"));
    setWindowIcon(QIcon(tr(":/images/game.png")));
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    //layout()返回Qlayout，表示布局管理类，是QBoxLayout的父类，sizeConstraint规定了几种大小设置方案
    //QLayout::SetFixedSize表示按照指定的大小，并且不可变

    slot_new_game();    //默认直接开始新游戏
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    write_setting();
}

void MainWindow::slot_timer()
{
    time_label->setNum(time->elapsed()/1000);
}

void MainWindow::slot_new_game()
{
    ticker->start();    //此时start使用interval中值重新开始，使用start(int msec)则使用msec值重新开始
    slot_timer();
    time_label->setNum(0);
    time->start();
    left_mine_label->setNum(total_mines);
    block_area->set_block_area(row, column, total_mines, false);
}

void MainWindow::slot_rank()
{
   QDialog dialog;
   dialog.setWindowTitle(tr("Rank"));
   QGridLayout *main_layout = new QGridLayout;
   main_layout->addWidget(new QLabel(tr("Standard")), 0, 0);
   main_layout->addWidget(new QLabel(tr("Score")), 0, 1);
   main_layout->addWidget(new QLabel(tr("Name")), 0, 2);

   main_layout->addWidget(new QLabel(tr("Easy")), 1, 0);
   main_layout->addWidget(new QLabel(tr("%1").arg(easy_record_time)), 1, 1);
   main_layout->addWidget(new QLabel(easy_record_name), 1, 2);

   main_layout->addWidget(new QLabel(tr("Middle")), 2, 0);
   main_layout->addWidget(new QLabel(tr("%1").arg(middle_record_time)), 2, 1);
   main_layout->addWidget(new QLabel(middle_record_name), 2, 2);

   main_layout->addWidget(new QLabel(tr("Hard")), 3, 0);
   main_layout->addWidget(new QLabel(tr("%1").arg(hard_record_time)), 3, 1);
   main_layout->addWidget(new QLabel(hard_record_name), 3, 2);

   QPushButton * recount_button = new QPushButton(tr("recount"));
   QPushButton * close_button = new QPushButton(tr("close"));
   connect(recount_button, SIGNAL(clicked()), &dialog, SLOT(accept()));
   connect(close_button, SIGNAL(clicked()), &dialog, SLOT(reject()));
   //QPushButton的clicked(bool checked)信号，在按下并释放鼠标左键时发出，也可以由setcheck()发出，当button为可以checkable时，就会设置checked为true。
   //dialog的accept()槽，会关闭当前对话框，并设置返回码QDialogCode为QDialog::Accepted，表示1.

   QHBoxLayout * bottom_layout = new QHBoxLayout;
   bottom_layout->addStretch();    //添加尺寸为0的可伸缩空间，使得按钮位于右方
   bottom_layout->addWidget(recount_button);
   bottom_layout->addWidget(close_button);

   QVBoxLayout * layout = new QVBoxLayout;
   layout->addLayout(main_layout);
   layout->addLayout(bottom_layout);
   dialog.setLayout(layout);

   if(dialog.exec() == QDialog::Accepted)
   {
       easy_record_time = middle_record_time = hard_record_time = 1111;
       easy_record_name = middle_record_name = hard_record_name = "";
   }
}

void MainWindow::slot_show_status(bool checked)
{
    if(checked)
        statusBar()->show();
    else
        statusBar()->hide();
}

void MainWindow::slot_standard_modes(QAction *action)
{
    if(action == easy_mode){
        current_standard = 0;
        row = 9;
        column = 9;
        total_mines = 10;
        slot_new_game();
    }else if(action == middle_mode){
        current_standard = 1;
        row = 16;
        column = 16;
        total_mines = 40;
        slot_new_game();
    }else if(action == hard_mode){
        current_standard = 2;
        row = 16;
        column = 30;
        total_mines = 99;
        slot_new_game();
    }else if(action == custom_mode){
        QDialog dialog;
        dialog.setWindowTitle(tr("Set standard mode"));
        QHBoxLayout * row_layout = new QHBoxLayout;
        row_layout->addWidget(new QLabel(tr("The height: ")));
        QSpinBox * row_spinBox = new QSpinBox;
        row_spinBox->setRange(1, 24);
        row_spinBox->setValue(row);
        row_layout->addWidget(row_spinBox);

        QHBoxLayout * column_layout = new QHBoxLayout;
        column_layout->addWidget(new QLabel(tr("The width: ")));
        QSpinBox * column_spinBox = new QSpinBox;
        column_spinBox->setRange(1, 30);
        column_spinBox->setValue(column);
        column_layout->addWidget(column_spinBox);

        QHBoxLayout * mine_layout = new QHBoxLayout;
        mine_layout->addWidget(new QLabel(tr("The mines: ")));
        QSpinBox * mine_spinBox = new QSpinBox;
        mine_spinBox->setRange(1, 600);
        mine_spinBox->setValue(total_mines);
        mine_layout->addWidget(mine_spinBox);

        QHBoxLayout * bottom_layout = new QHBoxLayout;
        bottom_layout->addStretch();
        QDialogButtonBox * dialog_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(dialog_buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        connect(dialog_buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
        bottom_layout->addWidget(dialog_buttonBox);

        QVBoxLayout * main_layout = new QVBoxLayout(&dialog);
        main_layout->addLayout(row_layout);
        main_layout->addLayout(column_layout);
        main_layout->addLayout(mine_layout);
        main_layout->addLayout(bottom_layout);
        if(dialog.exec() == QDialog::Accepted)
        {
            if(row_spinBox->value()*column_spinBox->value() > mine_spinBox->value())
            {
                current_standard = 3;
                row = row_spinBox->value();
                column = column_spinBox->value();
                total_mines = mine_spinBox->value();
            }
            else
                QMessageBox::warning(this, tr("Warning"), tr("total mines must less than blocks"));
            slot_new_game();    //只有选择了OK，才会开始新的一局。
        }
    }
}

void MainWindow::slot_game_over(bool is_win)
{
    ticker->stop();
    QString name;
    if(is_win){
    switch(current_standard){
    case 0:
            if(time_label->text().toInt() < easy_record_time){
                name = QInputDialog::getText(this, tr("Please enter your name"),tr("You create a record. Please enter your name"));
                if(!name.isEmpty()){
                    easy_record_time = time_label->text().toInt();
                    easy_record_name = name;
                }
            }
            break;
    case 1:
        if(time_label->text().toInt() < middle_record_time){
            name = QInputDialog::getText(this, tr("Please enter your name"),tr("You create a record. Please enter your name"));
            if(!name.isEmpty()){
                middle_record_time = time_label->text().toInt();
                middle_record_name = name;
            }
        }
        break;
    case 2:
        if(time_label->text().toInt() < middle_record_time){
            name = QInputDialog::getText(this, tr("Please enter your name"),tr("You create a record. Please enter your name"));
            if(!name.isEmpty()){
                middle_record_time = time_label->text().toInt();
                middle_record_name = name;
            }
        }
        break;
    }
    QMessageBox::information(this, tr("Result"), tr("You win"));
  }else{
        QMessageBox::information(this,tr("Result"),tr("You lose"));
    }
}

void MainWindow::slot_about_mine()
{
    QString introduction(
                "<h2>"+tr("About Mine Sweeper")+"</h2>"
                +"<p>"+tr("This game is played by revealing squares of the grid,")
                +tr("squares of the grid, typically by clicking them with a mouse.")
                +tr("If a square containing a mine is revealed, the player loses the game. ")
                +tr("Otherwise, a digit is revealed in the square, indicating the number of adjacent squares (out of the possible eight) that contain mines.")
                +tr("if this number is zero then the square appears blank, and the surrounding squares are automatically also revealed.")+"</p>"
                +"<br />"+tr("Easy mode: ")+"9*9 grid, 10 mines."
                +"<br />"+tr("Middle mode: ")+"16*16 grid, 40 mines."
                +"<br />"+tr("Hard mode: ")+"16*30 grid, 99 mines."
                +"<br />"+tr("Custom mode: ")+tr("Hight: ")+"1-24."+tr(" Width: ")+"1-30."+tr(" Mines: ")+"10-600."
                +"<p>"+tr("Please see ")+"<a href=http://www.baidu.com>http://www.baidu.com</a>"+ tr(" for an overview")+"</p>"
                +"<br />"+tr("Version: ")+g_software_version
                +"<br />"+tr("Author: ")+g_software_author);
    QMessageBox::about(this, tr("About Mine"), introduction);
}

void MainWindow::create_menus()
{
    game_menu=menuBar()->addMenu(tr("&Game"));
    game_menu->addAction(new_game);
    game_menu->addAction(rank);
    game_menu->addAction(exit);

    setting_menu=menuBar()->addMenu(tr("&Setting"));
    setting_menu->addAction(show_status);
    setting_menu->addSeparator();
    setting_menu->addAction(easy_mode);
    setting_menu->addAction(middle_mode);
    setting_menu->addAction(hard_mode);
    setting_menu->addAction(custom_mode);

    help_menu=menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(about_mine);
    help_menu->addAction(about_qt);
    help_menu->addAction(show_all_mines);
}

void MainWindow::create_actions()
{
    new_game = new QAction(QIcon(tr(":/images/new_game.png")), tr("&New Game"), this);
    new_game->setShortcut(QKeySequence::New);
    connect(new_game, SIGNAL(triggered()), this, SLOT(slot_new_game()));

    rank = new QAction(QIcon(tr(":/images/rank.png")), tr("&Rank"), this);
    rank->setShortcut(tr("Ctrl+R"));    //设置快捷键,setShortcut参数是QkeySequence，"Ctrl+R"可以被隐式转换为前者。
    connect(rank, SIGNAL(triggered()), this, SLOT(slot_rank()));

    exit = new QAction(QIcon(tr(":/images/exit.png")), tr("E&xit"), this);
    exit->setShortcut(tr("Ctrl+Q"));                            //windows下QKeySequence::Quit没有快捷键
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));    //exit动作直接连接QWidget的close()槽

    show_status = new QAction(tr("Show Status Bar"), this);
    show_status->setCheckable(true);    //QAction的checked默认是不可检测的，这里打开设置
    connect(show_status, SIGNAL(toggled(bool)), this, SLOT(slot_show_status(bool)));
    //QAction的toggled(bool checked)信号，每当ischeced()状态改变时得到该信号，可能是用户点击，也可能是setchecked()引起

    easy_mode = new QAction(QIcon(tr(":/images/easy_standard.png")), tr("Easy"), this);
    easy_mode->setCheckable(true);
    middle_mode = new QAction(QIcon(tr(":/images/middle_standard.png")), tr("Middle"), this);
    middle_mode->setCheckable(true);
    hard_mode = new QAction(QIcon(tr(":/images/hard_standard.png")), tr("Hard"), this);
    hard_mode->setCheckable(true);
    custom_mode = new QAction(QIcon(tr(":/images/custom_standard.png")), tr("Custom"), this);
    custom_mode->setCheckable(true);
    standard_modes = new QActionGroup(this);    //使用QAction组，里面的action在同一时刻只有一个激活
    standard_modes->addAction(easy_mode);
    standard_modes->addAction(middle_mode);
    standard_modes->addAction(hard_mode);
    standard_modes->addAction(custom_mode);
    connect(standard_modes, SIGNAL(triggered(QAction*)), this, SLOT(slot_standard_modes(QAction*)));

    about_mine = new QAction(QIcon(tr(":/images/game.png")), tr("About mine"), this);
    connect(about_mine, SIGNAL(triggered()), this, SLOT(slot_about_mine()));
    about_qt = new QAction(QIcon(tr(":/images/qt.png")), tr("About qt"), this);
    connect(about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    show_all_mines = new QAction(tr("Show all mines"), this);
}

void MainWindow::create_statusBar()
{
    //添加永久组件，”永久“表示组件位于右侧，故不会被临时消息覆盖.
    //addPermanentWidget的参数为QWidget*,故需要传递QLabel*,用QLabel来显示图片
   // QStatusBar * status_bar = statusBar();
    statusBar()->addPermanentWidget(time_label);
    QLabel *clock_label = new QLabel;
    clock_label->setPixmap(QPixmap(tr(":/images/clock.jpg")));
    statusBar()->addPermanentWidget(clock_label);

    QLabel *mine_label = new QLabel;
    mine_label->setPixmap(QPixmap(tr(":/images/game.png")));
    statusBar()->addWidget(mine_label);
    statusBar()->addWidget(left_mine_label);
}

void MainWindow::read_setting()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size").toSize());
    move(settings.value("pos").toPoint());
    bool show_statusBar = settings.value("showStatusBar").toBool();
    show_status->setChecked(show_statusBar);
    slot_show_status(show_statusBar);
    settings.endGroup();

    settings.beginGroup("GameSetting");
    current_standard = settings.value("current_standard").toInt();
    switch(current_standard){
        case 0:easy_mode->setChecked(true);break;
        case 1:middle_mode->setChecked(true);break;
        case 2:hard_mode->setChecked(true);break;
        case 3:custom_mode->setChecked(true);break;
    }
    row=settings.value("row").toInt()==0?9:settings.value("row").toInt();
    column=settings.value("column").toInt()==0?9:settings.value("column").toInt();
    total_mines=settings.value("total_mines").toInt()==0?10:settings.value("total_mines").toInt();
    settings.endGroup();

    settings.beginGroup("Rank");
    easy_record_time=settings.value("easy_record_time").toInt()==0?g_no_record_time:settings.value("easy_record_time").toInt();
    middle_record_time=settings.value("middle_record_time").toInt()==0?g_no_record_time:settings.value("middle_record_time").toInt();
    hard_record_time=settings.value("hard_record_time").toInt()==0?g_no_record_time:settings.value("hard_record_time").toInt();
    easy_record_name=settings.value("easy_record_name").toString()==""?g_no_record_name:settings.value("easy_record_name").toString();
    middle_record_name=settings.value("middle_record_name").toString()==""?g_no_record_name:settings.value("middle_record_name").toString();
    hard_record_name=settings.value("hard_record_name").toString()==""?g_no_record_name:settings.value("hard_record_name").toString();
    settings.endGroup();
}

void MainWindow::write_setting()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size",size());
    settings.setValue("pos", pos());
    settings.setValue("showStatusBar",show_status->isChecked());
    settings.endGroup();

    settings.beginGroup("GameSetting");
    settings.setValue("current_standard",current_standard);
    settings.setValue("row",row);
    settings.setValue("column",column);
    settings.setValue("total_mines",total_mines);
    settings.endGroup();

    settings.beginGroup("Rank");
    settings.setValue("easy_record_time",easy_record_time);
    settings.setValue("middle_record_time",middle_record_time);
    settings.setValue("hard_record_time",hard_record_time);
    settings.setValue("easy_record_name",easy_record_name);
    settings.setValue("middle_record_name",middle_record_name);
    settings.setValue("hard_record_name",hard_record_name);
    settings.endGroup();

}
