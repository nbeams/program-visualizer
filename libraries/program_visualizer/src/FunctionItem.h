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
    FunctionItem(QObject *programVisualizer, QTreeWidget *treeWidget, QString name);
    ~FunctionItem();
    VisualizerGraphicsScene* graphicsScene();
    QString name();
    QString definitionText();
    QList<FlowchartItem*>* flowchartList();

    void addFlowchartItem(FlowchartItem *item);
    void clear();
    void setDefinitionText(QString definition);
    void createFlowchart();
    void searchForKeywords(QString text, FlowchartItem *parentItem);
    QString findItemText(QString text, int *itemPos);
    bool checkItem(QString text, int beginPos, int testPos, int *endPos);

private:
    VisualizerGraphicsScene *m_graphicsScene;
    QString m_name;
    QString m_definitionText;
    QList<FlowchartItem*> *m_flowchartList;
    QTreeWidget *m_treeWidget;

};

#endif // FUNCTIONITEM_H
