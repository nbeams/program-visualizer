/****************************************************************************
** Meta object code from reading C++ file 'ProgramVisualizer.h'
**
** Created: Thu Jul 8 08:34:30 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ProgramVisualizer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProgramVisualizer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProgramVisualizer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      25,   18,   18,   18, 0x08,
      35,   18,   18,   18, 0x08,
      67,   61,   18,   18, 0x08,
      91,   18,   18,   18, 0x08,
     108,   18,   18,   18, 0x08,
     125,   18,   18,   18, 0x08,
     145,   18,   18,   18, 0x08,
     160,   18,   18,   18, 0x08,
     179,   18,   18,   18, 0x08,
     201,   18,   18,   18, 0x08,
     232,  220,   18,   18, 0x08,
     280,  220,   18,   18, 0x08,
     339,  321,   18,   18, 0x08,
     367,  362,   18,   18, 0x08,
     395,  390,   18,   18, 0x08,
     421,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProgramVisualizer[] = {
    "ProgramVisualizer\0\0run()\0refresh()\0"
    "setCurrentFlowchartItem()\0index\0"
    "setCurrentFunction(int)\0changeFunction()\0"
    "jumpToPosition()\0setUpdateDecision()\0"
    "enableUpdate()\0updateSourceFile()\0"
    "checkCursorPosition()\0checkHighlighted()\0"
    "item,column\0setHighlightedBrowserItem(QTreeWidgetItem*,int)\0"
    "browserItemChanged(QTreeWidgetItem*,int)\0"
    "pos,removed,added\0setOffset(int,int,int)\0"
    "font\0setItemTextFont(QFont)\0size\0"
    "setItemTextPointSize(int)\0setBrowserFont()\0"
};

const QMetaObject ProgramVisualizer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ProgramVisualizer,
      qt_meta_data_ProgramVisualizer, 0 }
};

const QMetaObject *ProgramVisualizer::metaObject() const
{
    return &staticMetaObject;
}

void *ProgramVisualizer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProgramVisualizer))
        return static_cast<void*>(const_cast< ProgramVisualizer*>(this));
    return QWidget::qt_metacast(_clname);
}

int ProgramVisualizer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: run(); break;
        case 1: refresh(); break;
        case 2: setCurrentFlowchartItem(); break;
        case 3: setCurrentFunction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: changeFunction(); break;
        case 5: jumpToPosition(); break;
        case 6: setUpdateDecision(); break;
        case 7: enableUpdate(); break;
        case 8: updateSourceFile(); break;
        case 9: checkCursorPosition(); break;
        case 10: checkHighlighted(); break;
        case 11: setHighlightedBrowserItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: browserItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: setOffset((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: setItemTextFont((*reinterpret_cast< QFont(*)>(_a[1]))); break;
        case 15: setItemTextPointSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: setBrowserFont(); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
