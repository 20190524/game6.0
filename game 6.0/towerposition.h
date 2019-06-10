#ifndef TOWERPOSITION_H
#define TOWERPOSITION_H

#include <QPoint>
#include <QSize>
#include <QPixmap>

class towerposition
{
public:
    towerposition(QPoint pos,const QPixmap&sprite = QPixmap(":/pictures/posi.png") );
    bool canhaveTower();
    bool containTower(const QPoint &pos);
    const QPoint centerPos();
    void sethaveTower();


    void drawTower(QPainter *painter) const;

private:
    bool t_haveTower;
    QPoint t_pos;
    QPixmap t_sprite;

    static const QSize fixedSize;
};

#endif // TOWERPOSITION_H
