#include "FlowchartItem.h"


FlowchartItem::FlowchartItem() : QGraphicsItem(0), m_parentItem(0), m_browserItem(),
    m_treeWidget(0), m_posInText(0), m_beginItemTextPos(0), m_endItemTextPos(0), m_endDoWhilePos(0),
    m_level(0), m_numberOfChildren(0)
{
    m_itemTextDocument = new QTextDocument();
    m_isHighlighted = false;
    m_hasParentItem = false;

    setAcceptsHoverEvents(true);
}

FlowchartItem::FlowchartItem(QTreeWidget *parentTreeWidget, int type, QString nameText):
    QGraphicsItem(0), m_parentItem(0), m_browserItem(0), m_treeWidget(0), m_posInText(0),
    m_beginItemTextPos(0), m_endItemTextPos(0), m_endDoWhilePos(0), m_level(0), m_numberOfChildren(0)
{

    m_treeWidget = parentTreeWidget;
    m_itemTextDocument = new QTextDocument();
    m_rectF = new QRectF();
    m_svgGraphics = new QSvgRenderer(QLatin1String("FlowchartItem.svg"));

    m_background = new QGraphicsSvgItem(this);
    m_background->setSharedRenderer(m_svgGraphics);
    m_background->setElementId("boxmiddle");

    m_backgroundLeft = new QGraphicsSvgItem(this);
    m_backgroundLeft->setSharedRenderer(m_svgGraphics);
    m_backgroundLeft->setElementId("boxleft");

    m_backgroundRight = new QGraphicsSvgItem(this);
    m_backgroundRight->setSharedRenderer(m_svgGraphics);
    m_backgroundRight->setElementId("boxright");

    m_textitem = new QGraphicsSimpleTextItem(this);

    m_fromPreviousArrow = new QGraphicsSvgItem(this);
    m_fromPreviousArrow->setSharedRenderer(m_svgGraphics);
    m_fromPreviousArrow->setElementId("frompreviousarrow");

    m_fromPreviousBlockArrowBottom = new QGraphicsSvgItem(this);
    m_fromPreviousBlockArrowBottom->setSharedRenderer(m_svgGraphics);
    m_fromPreviousBlockArrowBottom->setElementId("frompreviousblockarrowbottom");

    m_fromPreviousBlockArrowLine = new QGraphicsSvgItem(this);
    m_fromPreviousBlockArrowLine->setSharedRenderer(m_svgGraphics);
    m_fromPreviousBlockArrowLine->setElementId("frompreviousblockarrowline");

    m_fromParentArrow = new QGraphicsSvgItem(this);
    m_fromParentArrow->setSharedRenderer(m_svgGraphics);
    m_fromParentArrow->setElementId("fromparentarrow");

    m_toParentArrow = new QGraphicsSvgItem(this);
    m_toParentArrow->setSharedRenderer(m_svgGraphics);
    m_toParentArrow->setElementId("toparentarrow");

    m_type = type;
    m_nameText = nameText;
    m_isHighlighted = false;
    m_hasParentItem = false;
    m_isFunctionCall = false;
    m_scaled = false;

    setAcceptsHoverEvents(true);

}

FlowchartItem::FlowchartItem(QTreeWidget *parentTreeWidget, FlowchartItem *parentItem, int type, QString nameText):
        QGraphicsItem(0), m_parentItem(0), m_browserItem(0), m_treeWidget(0), m_posInText(0),
        m_beginItemTextPos(0), m_endItemTextPos(0), m_endDoWhilePos(0), m_level(0), m_numberOfChildren(0)
{
    m_parentItem = parentItem;
    m_itemTextDocument = new QTextDocument();
    m_treeWidget = parentTreeWidget;
    m_rectF = new QRectF();
    m_svgGraphics = new QSvgRenderer(QLatin1String("FlowchartItem.svg"));

    m_background = new QGraphicsSvgItem(this);
    m_background->setSharedRenderer(m_svgGraphics);
    m_background->setElementId("boxmiddle");

    m_backgroundLeft = new QGraphicsSvgItem(this);
    m_backgroundLeft->setSharedRenderer(m_svgGraphics);
    m_backgroundLeft->setElementId("boxleft");

    m_backgroundRight = new QGraphicsSvgItem(this);
    m_backgroundRight->setSharedRenderer(m_svgGraphics);
    m_backgroundRight->setElementId("boxright");

    m_textitem = new QGraphicsSimpleTextItem(this);

    m_fromPreviousArrow = new QGraphicsSvgItem(this);
    m_fromPreviousArrow->setSharedRenderer(m_svgGraphics);
    m_fromPreviousArrow->setElementId("frompreviousarrow");

    m_fromPreviousBlockArrowBottom = new QGraphicsSvgItem(this);
    m_fromPreviousBlockArrowBottom->setSharedRenderer(m_svgGraphics);
    m_fromPreviousBlockArrowBottom->setElementId("frompreviousblockarrowbottom");

    m_fromPreviousBlockArrowLine = new QGraphicsSvgItem(this);
    m_fromPreviousBlockArrowLine->setSharedRenderer(m_svgGraphics);
    m_fromPreviousBlockArrowLine->setElementId("frompreviousblockarrowline");

    m_fromParentArrow = new QGraphicsSvgItem(this);
    m_fromParentArrow->setSharedRenderer(m_svgGraphics);
    m_fromParentArrow->setElementId("fromparentarrow");

    m_toParentArrow = new QGraphicsSvgItem(this);
    m_toParentArrow->setSharedRenderer(m_svgGraphics);
    m_toParentArrow->setElementId("toparentarrow");

    m_type = type;
    m_nameText = nameText;
    m_isHighlighted = false;
    m_hasParentItem = true;
    m_isFunctionCall = false;
    m_scaled = false;

    setAcceptsHoverEvents(true);
}

FlowchartItem::~FlowchartItem()
{
}

FlowchartItem* FlowchartItem::parentItem()
{
    return m_parentItem;
}

FlowchartItem* FlowchartItem::previousItem()
{
    return m_previousItem;
}

int FlowchartItem::previousItemsTillLevel(int target_level)
{
    if(!previousItem())
        return 0;

    if(previousItem()->level() == target_level)
        return 1;

    return previousItem()->previousItemsTillLevel(target_level) + 1;
}

BrowserItem* FlowchartItem::browserItem()
{
    return m_browserItem;
}

int FlowchartItem::posInText()
{
    return m_posInText;
}

int FlowchartItem::beginItemTextPos()
{
    return m_beginItemTextPos;
}

int FlowchartItem::endItemTextPos()
{
    return m_endItemTextPos;
}

int FlowchartItem::endDoWhilePos()
{
    return m_endDoWhilePos;
}

int FlowchartItem::level()
{
    return m_level;
}

QString FlowchartItem::nameText()
{
    return m_nameText;
}

QString FlowchartItem::itemText()
{
    return m_itemText;
}

QTextDocument* FlowchartItem::itemTextDocument()
{
    return m_itemTextDocument;
}

QString FlowchartItem::itemType()
{
    return m_itemType;
}

bool FlowchartItem::isHighlighted()
{
    return m_isHighlighted;
}

bool FlowchartItem::isFunctionCall()
{
    return m_isFunctionCall;
}

bool FlowchartItem::hasParentItem()
{
    return m_hasParentItem;
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

void FlowchartItem::setPosInText(int pos)
{
    m_posInText = pos;
}

void FlowchartItem::setBeginItemTextPos(int pos)
{
    m_beginItemTextPos = pos;
}

void FlowchartItem::setEndItemTextPos(int pos)
{
    m_endItemTextPos = pos;
}

void FlowchartItem::setEndDoWhilePos(int pos)
{
    m_endDoWhilePos = pos;
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

void FlowchartItem::setPreviousItem(FlowchartItem *item)
{
    m_previousItem = item;
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

void FlowchartItem::setHighlighted(bool highlight)
{
    m_isHighlighted = highlight;
}

void FlowchartItem::setIsFunctionCall(bool isFunctionCall)
{
    m_isFunctionCall = isFunctionCall;
}

void FlowchartItem::setItemText(QString text)
{
    m_itemText = text;
    m_itemTextDocument->setPlainText(m_itemText);
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

void FlowchartItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_browserItem->setBackgroundColor(0, Qt::blue);
}

void FlowchartItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_browserItem->setBackgroundColor(0, Qt::white);
}

void FlowchartItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   //double xscale;

    /*Determine size of rectangle needed based on length of text*/
    QRectF boundingRect;
    boundingRect = painter->boundingRect(*m_rectF, Qt::AlignCenter, m_nameText);

    if(boundingRect.width() < 100)
    {
        boundingRect.setWidth(100);
    }

    m_rectF->setWidth(boundingRect.width() + 5);
    m_rectF->setHeight(boundingRect.height());

    if(! m_scaled)
    {
    //m_background->scale(1.0/xscale, 1.0);
        //xscale = (boundingRect.width()+5.0)/m_background->boundingRect().width();
        m_xscale = (boundingRect.width())/50.0;
        //m_background->setTransformOriginPoint(-16, 0);
        m_background->scale(m_xscale, 1.0);

        if(m_previousItem)
        {
            m_fromPreviousBlockArrowLine->scale(1, (previousItemsTillLevel(m_level)-1) * 55/65);
        }

        m_scaled = true;
    }

    m_background->setZValue(-1);
    m_background->setPos((m_backgroundLeft->boundingRect().width() ) - boundingRect.width()/50.0 , 0); // we subtract 1 scaled pixel for some odd reason...

    m_backgroundLeft->setPos(0, 0);
    m_backgroundRight->setPos((m_backgroundLeft->boundingRect().width() ) - boundingRect.width()/50.0 + m_background->boundingRect().width() * boundingRect.width()/50.0 - boundingRect.width()/50.0, 0); // we subtract another scaled pixel for some odd reason


     /*Use size and location to create connection points for visualization*/
    m_leftConnectionPoint.setX((m_rectF->topLeft().x() + m_rectF->bottomLeft().x())/2);
    m_leftConnectionPoint.setY((m_rectF->topLeft().y() + m_rectF->bottomLeft().y())/2);

    m_rightConnectionPoint.setX((m_rectF->topRight().x() + m_rectF->bottomRight().x())/2);
    m_rightConnectionPoint.setY((m_rectF->topRight().y() + m_rectF->bottomRight().y())/2);

    //painter->drawRect(*m_rectF);
    //painter->drawText(*m_rectF, m_nameText);

    m_textitem->setText(m_nameText);
    m_textitem->setPos(15, 15);
    m_textitem->setZValue(2);

    m_fromParentArrow->setVisible(m_parentItem && m_previousItem == m_parentItem);
    m_fromParentArrow->setPos(-25 -6, -8);

    if(m_previousItem)
    {
        m_fromPreviousArrow->setVisible(m_previousItem->level() == level());
        m_fromPreviousArrow->setPos(-25 -0, -18);

        m_fromPreviousBlockArrowBottom->setVisible(m_previousItem->level() > level());
        m_fromPreviousBlockArrowBottom->setPos(25, -63);

        m_fromPreviousBlockArrowLine->setVisible(m_previousItem->level() > level() && previousItemsTillLevel(m_level) > 1);
        m_fromPreviousBlockArrowLine->setPos(25, -55*(previousItemsTillLevel(m_level)-1)-8 - (previousItemsTillLevel(m_level)-1) * 55/65/2 ); // we subtract 0.5 scaled pixels here for some very odd reason
    }
    else
    {
        m_fromPreviousArrow->setVisible(false);
        m_fromPreviousBlockArrowBottom->setVisible(false);
        m_fromPreviousBlockArrowLine->setVisible(false);
    }

}

QRectF FlowchartItem::boundingRect() const
{
  //return *m_rectF;
    QRectF bounds= m_background->boundingRect();

    bounds.setLeft(0);
    //bounds.setRight(m_backgroundRight->boundingRect().right());
    bounds.setWidth(2 * m_backgroundLeft->boundingRect().width() + 50 * m_xscale);

    return bounds;
}
