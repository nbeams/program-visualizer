#ifndef PROGRAMVISUALIZER_H
#define PROGRAMVISUALIZER_H

#include <QtGui/QWidget>
#include <QTreeWidgetItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <QRectF>
#include <QList>
#include <QPointF>
#include <QLineF>
#include <QRegExp>

#include "SourceFile.h"
#include "BrowserItem.h"
#include "FunctionItem.h"

#include "VisualizerGraphicsScene.h"

namespace Ui {
    class ProgramVisualizer;
}

class ProgramVisualizer : public QWidget
{
    Q_OBJECT
public:
    ProgramVisualizer();
    ~ProgramVisualizer();

    void setCurrentFile(SourceFile *sourceFile);
    void run();

private:

    Ui::ProgramVisualizer *m_ui;
    SourceFile *m_currentSourceFile;
    FunctionItem *m_currentFunction;
    VisualizerGraphicsScene *m_currentScene;
    FlowchartItem *m_currentFlowchartItem;
    QList<FlowchartItem*> *m_flowchartList;
    QList<FunctionItem*> m_functionList;
    QString m_selectedText;

    void reset();
    int searchForFunctions(QString sourceFileText);
    QString findFunctionDefinition(QString sourceFileText, FunctionItem *item, int defPos);
    void closeEvent (QCloseEvent *event);

 private slots:
    void refresh();
    void setCurrentFlowchartItem();
    void setCurrentFunction(int index);
    void changeFunction();

};

#endif // PROGRAMVISUALIZER_H
