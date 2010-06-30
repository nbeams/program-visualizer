#ifndef VISUALIZERGRAPHICSSCENE_H
#define VISUALIZERGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsItem>
#include <QGraphicsTextItem>

#include "FlowchartItem.h"

class VisualizerGraphicsScene : public QGraphicsScene
{
public:
    VisualizerGraphicsScene(QObject *parent = 0);

private:
    FlowchartItem *m_selectedFCItem;

};

#endif // VISUALIZERGRAPHICSSCENE_H
