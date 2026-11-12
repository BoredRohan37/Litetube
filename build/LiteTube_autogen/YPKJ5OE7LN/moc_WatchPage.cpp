/****************************************************************************
** Meta object code from reading C++ file 'WatchPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/WatchPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WatchPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WatchPage_t {
    QByteArrayData data[18];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WatchPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WatchPage_t qt_meta_stringdata_WatchPage = {
    {
QT_MOC_LITERAL(0, 0, 9), // "WatchPage"
QT_MOC_LITERAL(1, 10, 10), // "toggleMini"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 16), // "toggleFullscreen"
QT_MOC_LITERAL(4, 39, 9), // "onLoading"
QT_MOC_LITERAL(5, 49, 9), // "VideoInfo"
QT_MOC_LITERAL(6, 59, 1), // "v"
QT_MOC_LITERAL(7, 61, 6), // "onMeta"
QT_MOC_LITERAL(8, 68, 14), // "ResolvedStream"
QT_MOC_LITERAL(9, 83, 1), // "s"
QT_MOC_LITERAL(10, 85, 8), // "onFailed"
QT_MOC_LITERAL(11, 94, 3), // "msg"
QT_MOC_LITERAL(12, 98, 7), // "onEnded"
QT_MOC_LITERAL(13, 106, 12), // "onSearchDone"
QT_MOC_LITERAL(14, 119, 2), // "id"
QT_MOC_LITERAL(15, 122, 16), // "QList<VideoInfo>"
QT_MOC_LITERAL(16, 139, 5), // "items"
QT_MOC_LITERAL(17, 145, 4) // "next"

    },
    "WatchPage\0toggleMini\0\0toggleFullscreen\0"
    "onLoading\0VideoInfo\0v\0onMeta\0"
    "ResolvedStream\0s\0onFailed\0msg\0onEnded\0"
    "onSearchDone\0id\0QList<VideoInfo>\0items\0"
    "next"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WatchPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   51,    2, 0x08 /* Private */,
       7,    1,   54,    2, 0x08 /* Private */,
      10,    1,   57,    2, 0x08 /* Private */,
      12,    0,   60,    2, 0x08 /* Private */,
      13,    3,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 15, QMetaType::QString,   14,   16,   17,

       0        // eod
};

void WatchPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WatchPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleMini(); break;
        case 1: _t->toggleFullscreen(); break;
        case 2: _t->onLoading((*reinterpret_cast< const VideoInfo(*)>(_a[1]))); break;
        case 3: _t->onMeta((*reinterpret_cast< const ResolvedStream(*)>(_a[1]))); break;
        case 4: _t->onFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onEnded(); break;
        case 6: _t->onSearchDone((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QList<VideoInfo>(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< VideoInfo >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<VideoInfo> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WatchPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WatchPage::toggleMini)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WatchPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WatchPage::toggleFullscreen)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WatchPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_WatchPage.data,
    qt_meta_data_WatchPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WatchPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WatchPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WatchPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int WatchPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void WatchPage::toggleMini()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void WatchPage::toggleFullscreen()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
