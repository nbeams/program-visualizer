#include "FlowchartItem.h"


FlowchartItem::FlowchartItem() : QGraphicsItem(0), m_parentItem(0), m_browserItem(),
    m_treeWidget(0), m_level(0), m_numberOfChildren(0)
{

}

FlowchartItem::FlowchartItem(QTreeWidget *parentTreeWidget, int type, QString nameText):
    QGraphicsItem(0), m_parentItem(0), m_browserItem(0), m_treeWidget(0), m_level(0), m_numberOfChildren(0)
{

    m_treeWidget = parentTreeWidget;
    m_rectF = new QRectF();
/*    m_svgGraphics = new QSvgRenderer(QLatin1String("FlowchartItem.svg"));

    m_background = new QGraphicsSvgItem(this);
    m_background->setSharedRenderer(m_svgGraphics);
    m_background->setElementId("boxmiddle");

    m_backgroundLeft = new QGraphicsSvgItem(this);
    m_backgroundLeft->setSharedRenderer(m_svgGraphics);
    m_backgroundLeft->setElementId("boxleft");

    m_backgroundRight = new QGraphicsSvgItem(this);
    m_backgroundRight->setSharedRenderer(m_svgGraphics);
    m_backgroundRight->setElementId("boxright");

    m_textitem = new QGraphicsSimpleTextItem(this); */
    m_type = type;
    m_nameText = nameText;
    m_hasParentItem = false;
    m_isFunctionCall = false;
 //   m_scaled = false;

}

FlowchartItem::FlowchartItem(QTreeWidget *parentTreeWidget, FlowchartItem *parentItem, int type, QString nameText):
        QGraphicsItem(0), m_parentItem(0), m_browserItem(0), m_treeWidget(0), m_level(0), m_numberOfChildren(0)
{
    m_parentItem = parentItem;
    m_treeWidget = parentTreeWidget;
    m_rectF = new QRectF();
  /*  m_svgGraphics = new QSvgRenderer(QLatin1String("FlowchartItem.svg"));

    m_background = new QGraphicsSvgItem(this);
    m_background->setSharedRenderer(m_svgGraphics);
    m_background->setElementId("boxmiddle");

    m_backgroundLeft = new QGraphicsSvgItem(this);
    m_backgroundLeft->setSharedRenderer(m_svgGraphics);
    m_backgroundLeft->setElementId("boxleft");

    m_backgroundRight = new QGraphicsSvgItem(this);
    m_backgroundRight->setSharedRenderer(m_svgGraphics);
    m_backgroundRight->setElementId("boxright");

    m_textitem = new QGraphicsSimpleTextItem(this); */
    m_type = type;
    m_nameText = nameText;
    m_hasParentItem = true;
    m_isFunctionCall = false;
 //   m_scaled = false;
}

FlowchartItem::~FlowchartItem()
{
}

FlowchartItem* FlowchartItem::parentItem()
{
    return m_parentItem;
}

BrowserItem* FlowchartItem::browserItem()
{
    return m_browserItem;
}

QString FlowchartItem::nameText()
{
    return m_nameText;
}

QString FlowchartItem::itemText()
{
    return m_itemText;
}

QString FlowchartItem::itemType()
{
    return m_itemType;
}

bool FlowchartItem::isFunctionCall()
{
    return m_isFunctionCall;
}

QString FlowchartItem::functionName()
{
    return m_functionName;
}

QPointF FlowchartItem::leftConnectionPoint()
{
    return m_leftConnectionPoint;
}

QPointF FlowchartItem::rightConnectionPoint()
{
    return m_rightConnectionPoint;
}

QRectF* FlowchartItem::rectF()
{
    return m_rectF;
}

int FlowchartItem::level()
{
    return m_level;
}

void FlowchartItem::setLevel(int level)
{
    m_level = level;
}

int FlowchartItem::numberOfChildren()
{
    return m_numberOfChildren;
}

void FlowchartItem::setNumberOfChildren(int number)
{
    m_numberOfChildren = number;
}

void FlowchartItem::createBrowserItem()
{
    if(m_hasParentItem)
        m_browserItem = new BrowserItem(m_parentItem->browserItem(), m_type, m_nameText);
    else
        m_browserItem = new BrowserItem(m_treeWidget, m_type, m_nameText);
}

void FlowchartItem::setNameText(QString name)
{
    m_nameText = name;
}

void FlowchartItem::setItemType(QString text)
{
    m_itemType = text;
}

void FlowchartItem::setIsFunctionCall(bool isFunctionCall)
{
    m_isFunctionCall = isFunctionCall;
}

void FlowchartItem::setItemText(QString text)
{
    m_itemText = text;
}

void FlowchartItem::setFunctionName(QString functionName)
{
    m_functionName = functionName;
}

void FlowchartItem::setLocation(float topLeftX, float topLeftY)
{
    /*Set the size and topLeft members*/
    QPointF topLeftPoint(topLeftX, topLeftY);
    setPos(topLeftPoint);
    m_rectF->setTopLeft(topLeftPoint);

}

void FlowchartItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /*Clear old selection in the QTreeWidget*/
    m_treeWidget->clearSelection();

    /*Set new selection -- this causes the QTreeWidget to emit a signal to ProgramVisualizer*/
    m_browserItem->setSelected(true);

    /*Show changes on graphics scene*/
   // update(*m_rectF);
}

void FlowchartItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    double xscale;

    /*Determine size of rectangle needed based on length of text*/
    QRectF boundingRect;
    boundingRect = painter->boundingRect(*m_rectF, Qt::AlignCenter, m_nameText);
    m_rectF->setWidth(boundingRect.width() + 5);
    m_rectF->setHeight(boundingRect.height());

/*    if(! m_scaled)
    {
    //m_background->scale(1.0/xscale, 1.0);
        //xscale = (boundingRect.width()+5.0)/m_background->boundingRect().width();
        xscale = (boundingRect.width())/50.0;
        //m_background->setTransformOriginPoint(-16, 0);
        m_background->scale(xscale, 1.0);
        m_scaled = true;
    }

     // This fixes the spacing of flowchart items... or maybe not
    //m_rectF->setWidth(m_background->boundingRect().width());
    //m_rectF->setHeight(m_background->boundingRect().height());

    m_background->setZValue(-1);
    m_background->setPos((m_backgroundLeft->boundingRect().width() ) - boundingRect.width()/50.0 , 0); // we subtract 1 scaled pixel for some odd reason...

    m_backgroundLeft->setPos(0, 0);
    m_backgroundRight->setPos((m_backgroundLeft->boundingRect().width() ) - boundingRect.width()/50.0 + m_background->boundingRect().width() * boundingRect.width()/50.0 - boundingRect.width()/50.0, 0); // we subtract another scaled pixel for some odd reason */



     /*Use size and location to create connection points for visualization*/
    m_leftConnectionPoint.setX((m_rectF->topLeft().x() + m_rectF->bottomLeft().x())/2);
    m_leftConnectionPoint.setY((m_rectF->topLeft().y() + m_rectF->bottomLeft().y())/2);

    m_rightConnectionPoint.setX((m_rectF->topRight().x() + m_rectF->bottomRight().x())/2);
    m_rightConnectionPoint.setY((m_rectF->topRight().y() + m_rectF->bottomRight().y())/2);

    painter->drawRect(*m_rectF);
    painter->drawText(*m_rectF, m_nameText);

 /*   m_textitem->setText(m_nameText);
    m_textitem->setPos(15, 15);
    m_textitem->setZValue(2); */
}

QRectF FlowchartItem::boundingRect() const
{
   return *m_rectF;
 //  return m_background->boundingRect();
}
