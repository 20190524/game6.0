#include "tower.h"
#include "enemy.h"
#include "bullet.h"
#include "mainwindow.h"
#include "utility.h"
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QVector2D>
#include <QtMath>


const QSize Tower::fixedSize(140,140);

Tower::Tower(QPoint pos,MainWindow *game,const QPixmap &sprite):
     t_attack(false)
    , t_attackRange(400)
    , t_attackDamage(10)
    , t_attackBreak(1000)
    , t_rotationSprite(0.0)
    , t_chooseEnemy(NULL)
    , t_game(game)
    , t_pos(pos)
    , t_sprite(sprite) {
    t_attackBreakTimer=new QTimer(this);
    connect(t_attackBreakTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

Tower::~Tower()
{
    delete t_attackBreakTimer;
    t_attackBreakTimer=NULL;
}

void Tower::attackEnemy() {
    t_attackBreakTimer->start(t_attackBreak);
}



void Tower::drawTower(QPainter *painter) {
    painter->save();
    painter->setPen(Qt::white);
    //painter->drawLine(t_pos.x(),t_pos.y(),t_pos.x()+t_attackRange,t_pos.y());
    //攻击范围的绘制

    static const QPoint offsetPoint (-fixedSize.width()/2,-fixedSize.height()/2);
    painter->translate(t_pos);
    painter->drawPixmap(offsetPoint,t_sprite);
    painter->restore();
}

void Tower::chooseEnemyforAttack(Enemy *enemy) {
    t_chooseEnemy = enemy;
    attackEnemy();
    t_chooseEnemy->getAttacked(this); //敌人类-确定被攻击的函数；
}

void Tower::enemyKilled() {
    if (t_chooseEnemy) t_chooseEnemy = NULL;
    t_attackBreakTimer->stop();
    t_rotationSprite=0.0;
}

void Tower::loseSightofEnmey() {
    t_chooseEnemy->getLostSight(this);//敌人类-离开攻击范围的函数；
    if (t_chooseEnemy) t_chooseEnemy = NULL;
    t_attackBreakTimer->stop();
    t_rotationSprite=0.0;
}

void Tower::shootWeapon()
{
    Bullet *bullet = new Bullet(t_pos, t_chooseEnemy->pos(),t_attackDamage, t_chooseEnemy, t_game);
    bullet->move();
    t_game->addBullet(bullet);
}

void Tower::checkEnemyInRange() {
    if (t_chooseEnemy)
    {
        // 这种情况下,需要旋转炮台对准敌人
        // 向量标准化
        QVector2D normalized(t_chooseEnemy->pos() - t_pos);
        normalized.normalize();
        //t_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) - 90;

        // 如果敌人脱离攻击范围
        if (!collisionWithCircle(t_pos,t_attackRange,t_chooseEnemy->pos()))
            loseSightofEnmey();
    }
    else
    {
        // 遍历敌人,看是否有敌人在攻击范围内
        QList<Enemy *> enemyList = t_game->enemyList1();
        foreach (Enemy *enemy, enemyList)
        {
            if (collisionWithCircle(t_pos, t_attackRange, enemy->pos()))
            {
                chooseEnemyforAttack(enemy);
                break;
            }
        }
    }
}
