/****************************************************************************
** Meta object code from reading C++ file 'CommentsWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/CommentsWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommentsWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CommentsWidget_t {
    QByteArrayData data[11];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CommentsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CommentsWidget_t qt_meta_stringdata_CommentsWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CommentsWidget"
QT_MOC_LITERAL(1, 15, 13), // "seekRequested"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "seconds"
QT_MOC_LITERAL(4, 38, 8), // "onLoaded"
QT_MOC_LITERAL(5, 47, 2), // "id"
QT_MOC_LITERAL(6, 50, 14), // "QList<Comment>"
QT_MOC_LITERAL(7, 65, 1), // "c"
QT_MOC_LITERAL(8, 67, 4), // "next"
QT_MOC_LITERAL(9, 72, 8), // "onFailed"
QT_MOC_LITERAL(10, 81, 3) // "msg"

    },
    "CommentsWidget\0seekRequested\0\0seconds\0"
    "onLoaded\0id\0QList<Comment>\0c\0next\0"
    "onFailed\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CommentsWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,   32,    2, 0x08 /* Private */,
       9,    2,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 6, QMetaType::QString,    5,    7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    5,   10,

       0        // eod
};

void CommentsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommentsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->seekRequested((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->onLoaded((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QList<Comment>(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->onFailed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommentsWidget::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommentsWidget::seekRequested)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CommentsWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CommentsWidget.data,
    qt_meta_data_CommentsWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CommentsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommentsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CommentsWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CommentsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CommentsWidget::seekRequested(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
