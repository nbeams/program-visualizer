#ifndef FLOWCHARTITEM_H
#define FLOWCHARTITEM_H

#include <QRectF>
#include <QPointF>
#include <QTreeWidget>
#include <QGraphicsItem>
#include <QPainter>
#include <QTextDocument>

#include <QGraphicsSimpleTextItem>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>

#include "BrowserItem.h"

class FlowchartItem : public QGraphicsItem
{

public:

    FlowchartItem();
    FlowchartItem(QTreeWidget *parentTreeWidget = 0, int type = 0, QString nameText = 0);
    FlowchartItem(QTreeWidget *parentTreeWidget = 0, FlowchartItem *parentItem = 0, int type = 0, QString nameText = 0);
    ~FlowchartItem();

    FlowchartItem* parentItem();
    FlowchartItem* previousItem();
    int previousItemsTillLevel(int target_level);
    BrowserItem* browserItem();
    int posInText();
    int beginItemTextPos();
    int endItemTextPos();
    int endDoWhilePos();
    int level();
    QString nameText();
    QString itemText();
    QTextDocument* itemTextDocument();
    QString itemType();
    bool isHighlighted();
    bool isFunctionCall();
    bool hasParentItem();
    QString functionName();
    QPointF leftConnectionPoint();
    QPointF rightConnectionPoint();
    QRectF* rectF();


    void setPosInText(int pos);
    void setBeginItemTextPos(int pos);
    void setEndItemTextPos(int pos);
    void setEndDoWhilePos(int pos);
    void setLevel(int level);
    int numberOfChildren();
    void setNumberOfChildren(int number);
    void createBrowserItem();
    void setNameText(QString name);
    void setItemType(QString type);
    void setHighlighted(bool highlight);
    void setIsFunctionCall(bool isFunctionCall);
    void setItemText(QString text);
    void setFunctionName(QString functionName);
    void setLocation(float topLeftX, float topLeftY);
    void setPreviousItem(FlowchartItem*);

private:
    FlowchartItem *m_parentItem;
    FlowchartItem *m_previousItem;
    BrowserItem *m_browserItem;
    QString m_nameText;
    QString m_itemText;
    QTextDocument *m_itemTextDocument;
    QString m_itemType;
    bool m_isHighlighted;
    bool m_isFunctionCall;
    QString m_functionName;
    QTreeWidget *m_treeWidget;
    QRectF *m_rectF;
    QSvgRenderer *m_svgGraphics;
    QGraphicsSvgItem *m_background, *m_backgroundLeft, *m_backgroundRight, *m_toParentArrow,
        *m_fromParentArrow, *m_fromPreviousArrow, *m_fromPreviousBlockArrowBottom, *m_fromPreviousBlockArrowLine;
    QGraphicsSimpleTextItem *m_textitem;
    QPointF m_leftConnectionPoint;
    QPointF m_rightConnectionPoint;
    int m_type;
    int m_posInText;
    int m_beginItemTextPos;
    int m_endItemTextPos;
    int m_endDoWhilePos;
    int m_level;
    int m_numberOfChildren;
    bool m_hasParentItem;
    bool m_scaled;
    double m_xscale;

    /*Pure virtual function from QGraphicsItem that must be reimplemented*/
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

signals:
    void flowchartItemClicked(BrowserItem *parent);


};

#endif // FLOWCHARTITEM_H
