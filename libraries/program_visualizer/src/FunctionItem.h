#ifndef FUNCTIONITEM_H
#define FUNCTIONITEM_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTreeWidget>
#include <QRegExp>

#include "VisualizerGraphicsScene.h"
#include "FlowchartItem.h"

class FunctionItem : public QObject
{
public:
    FunctionItem(QObject *programVisualizer, QTreeWidget *treeWidget, QString prototype);
    ~FunctionItem();
    VisualizerGraphicsScene* graphicsScene();
    QString prototype();
    QString name();
    QString type();
    QString definitionText();
    QList<FlowchartItem*>* flowchartList();

    void addFlowchartItem(FlowchartItem *item);
    void clear();
    void setDefinitionText(QString definition);
    void setName(QString name);
    void setType(QString type);

    int createFlowchart();
    int searchForKeywords(QString text, FlowchartItem *parentItem);
    QString findItemText(QString text, int *itemPos);
    FlowchartItem* findCase(QString text, FlowchartItem *parentItem, int *itemPos, bool *newItemFound);
    void handleFunctionCall(QString nameext, FlowchartItem *item, int *endOfFunctionCall);
    bool findNext(QString text, QRegExp regExp, int *searchPos);
    bool checkItem(QString text, int beginPos, int testPos, int *endPos);

private:
    VisualizerGraphicsScene *m_graphicsScene;
    QString m_prototype;
    QString m_name;
    QString m_type;
    QString m_definitionText;
    QList<FlowchartItem*> *m_flowchartList;
    QTreeWidget *m_treeWidget;

};

#endif // FUNCTIONITEM_H
