#ifndef PROGRAMVISUALIZER_H
#define PROGRAMVISUALIZER_H

#include <QtGui/QWidget>
#include <QAction>
#include <QTreeWidgetItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QRectF>
#include <QList>
#include <QPointF>
#include <QPoint>
#include <QRegExp>

#include "SourceFile.h"
#include "BrowserItem.h"
#include "FunctionItem.h"
#include "UpdateDialog.h"
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

    QAction *visualizerAction() {return &m_action;}

private:

    Ui::ProgramVisualizer *m_ui;
    SourceFile *m_currentSourceFile;
    FunctionItem *m_currentFunction;
    VisualizerGraphicsScene *m_currentScene;
    FlowchartItem *m_currentFlowchartItem;
    UpdateDialog *m_updateSourceWarning;
    QList<FlowchartItem*> *m_flowchartList;
    QList<FunctionItem*> m_functionList;
    QTextDocument *m_currentItemText;
    int m_updateDecision;
    QList<QPoint> m_offsetList;
    QAction m_action;

    void reset();
    int searchForFunctions(QString sourceFileText);
    QString findFunctionDefinition(QString sourceFileText, FunctionItem *item, int defPos);
    void closeEvent (QCloseEvent *event);

 private slots:

    void run();
    void refresh();
    void setCurrentFlowchartItem();
    void setCurrentFunction(int index);
    void changeFunction();

    void jumpToPosition();

    void setUpdateDecision();
    void enableUpdate();
    void updateSourceFile();

    void checkCursorPosition();
    void checkHighlighted();
    void setHighlightedBrowserItem(QTreeWidgetItem *item, int column);
    void browserItemChanged(QTreeWidgetItem *item, int column);
    void setOffset(int pos, int removed, int added);

    void setItemTextFont(QFont font);
    void setItemTextPointSize(int size);
    void setBrowserFont();


};

#endif // PROGRAMVISUALIZER_H
