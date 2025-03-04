#ifndef GAMESCENE_H
#define GAMESCENE_H
#include <QMouseEvent>
#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <stack>
#include<vector>
#include "Stone.h"
namespace Ui {
class Gamescene;
}

class Gamescene : public QWidget
{
    Q_OBJECT

public:
    explicit Gamescene(QWidget *parent = nullptr);
    ~Gamescene();
    virtual void paintEvent(QPaintEvent *event);

    //象棋的棋盘的坐标转换成界面坐标
    QPoint center(int row, int col);
    //寻找棋子的位置
    QPoint center(int id);

    //绘画单个具体的棋子
    void drawStone(QPainter&painter,int id);

    //界面坐标转换成棋盘的行列值[获取鼠标点击的像素坐标，是位于棋盘的哪一个行列值]
    bool getRowCol(QPoint pt, int& row, int& col);

    //鼠标点击事件
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void StoneMove(int row,int col);//移动函数

    //象棋移动的规则[将  士  象  马  车  炮  兵]
    virtual bool canMove(int moveId, int killId, int row, int col);
    virtual bool canMoveJIANG(int moveId, int killId, int row, int col);
    virtual bool canMoveSHI(int moveId, int killId, int row, int col);
    virtual bool canMoveXIANG(int moveId, int killId, int row, int col);
    bool canMoveMA(int moveId, int killId, int row, int col);
    bool canMoveCHE(int moveId, int killId, int row, int col);
    bool canMovePAO(int moveId, int killId, int row, int col);
    virtual bool canMoveBING(int moveId, int killId, int row, int col);

    std::stack<std::vector<Stone>> Regret;//悔棋栈，用来存储每一步的棋盘现状，便于悔棋
    void Regretstone();//悔棋函数
    void Regretpacket();//把当前棋盘装入悔棋栈的函数


    Stone stone[32];
    int r;//棋子半径
    int offset;//距离界面的边距
    int d;//间距
    int selectid;//选中棋子则大于0，否则为-1;
    //因为棋子有32个，要记录选中的棋子0-31,所以不能用bool类型
    int clicked;//用鼠标选中棋子的id
    bool redtrue;//红旗先下标志
    int gz=100;//棋盘向右移动100
    //bool flag=false;//用于判断上一回合是否被将军，用于区别被绝杀还是送将

    //设置背景音乐
    QMediaPlayer *scenemusic =nullptr;//播放器类
    QAudioOutput *control=nullptr;//控制输出类

    //设置棋子音效
    QSoundEffect *stonemovemusic;//移动棋子
    QSoundEffect *stoneeatmusic;//吃子
    //设置将军音效
    QSoundEffect *attackmusic;
    //设置将死音效
    QSoundEffect *Lorekmusic;

    //处理被将军的情况
    virtual void reset();
    bool face();
    virtual bool isDefeated(bool f);//判断送将和将军
    bool isOver();
    bool isAttack();
    void whoWin();
    void winMessageBox(QString title, QString msg);
    virtual bool tryflee();//尝试逃离

    bool isdead(int id);
    int getStoneId(int row,int col);
    //车 炮 的功能辅助函数   判断两个点是否在一个直线上面,且返回直线之间的棋子个数
    int getStoneCountAtLine(int row1,int col1,int row2,int col2);

    Ui::Gamescene *ui;
};

#endif // GAMESCENE_H
