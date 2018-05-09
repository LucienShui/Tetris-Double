#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <cstring>
#include <ctime>

enum direction{UP,DOWN,LEFT,RIGHT,SPACE};//定义枚举

typedef struct {
    int pos_x; //方块x坐标
    int pos_y; //方块y坐标
}REC_Point;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

signals:
    void GameOverSignal();    //游戏结束信号
    void CurrentScore();   //游戏当前分数

public slots:
    void StartGame();   //游戏开始
    void PlayGame();    //恢复游戏
    void PauseGame();   //暂停游戏
    void SetSpeed(int level);//设置速度等级
    //void SetHard(int level);//设置是否增加难度
    void GameOver();    //游戏结束
public:

    explicit MainWindow(QWidget *parent = 0); //构造函数
    ~MainWindow(); //析构函数
    void weizhi(int x,int y,int tx,direction direct); //位置变换
    void paintEvent(QPaintEvent *);  //场景绘制
    void trychange(int x,int y,int num,REC_Point temppoint[4]);
    void change(int x,int y,int num,int state);     //方块变换
    void clear(int x,int y,int num);        //清除当前位置信息
    void move(int hang,direction direct);//部分移动
    void SetLevel(int);//设置随机行行数并加上随机行标记
    void UseLevel();//设置随机行并消除随机行标记

protected:

    void timerEvent(QTimerEvent *event);  //定时设置
    void keyPressEvent(QKeyEvent *event); //键盘事件

private slots:
    void on_actionPause_P_triggered();

    void on_action1_triggered();

    void on_actionStart_S_triggered();

    void on_action2_triggered();

    void on_action3_triggered();

    void on_action4_triggered();

    void on_action5_triggered();

    void on_action1_2_triggered();

    void on_action2_2_triggered();

    void on_action3_2_triggered();

    void on_action4_2_triggered();

    void on_action5_2_triggered();

    void on_actionAbout_triggered();

    void on_actionHow_to_Play_triggered();

    void on_actionRed_triggered();

    void on_actionYellow_2_triggered();

    void on_actionBlue_2_triggered();

    void on_actionGreen_triggered();

    void on_actionRed_2_triggered();

    void on_actionYellow_3_triggered();

    void on_actionBlue_3_triggered();

    void on_actionGreen_2_triggered();

private:
    int MaxScore; //历史最高分数
    int HardLevel;//难度等级
    int SCENE_H;  //场景行数
    int SCENE_W;  //场景列数
    int REC_SIZE; //方块大小
    int scene_num[1007][1007];//场景数据
    REC_Point rpoint[2][4]; //定义四个方块的位置
    int REC_tx[4];//定义方块的图形
    int timerid1,timerid2;//记录两个定时器id
    int speed_ms,fresh_ms;//speed_ms记录下落速度,fresh_ms设置窗体刷新速度
    int score;//分数
    int LevelFive;//进入level5之后辅助计分变量
    bool uselevel[6];//是否已进入对应难度等级
    bool Pauseflag;//暂停标志
    bool Levelflag;//随机行标记
    bool Startflag;//是否已经开始游戏
    QColor color1, color2;//方块颜色
    Ui::MainWindow *ui;
    //Query *query;
};

#endif // MAINWINDOW_H
