/****************************************************************************
** Meta object code from reading C++ file 'VideoGrid.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/VideoGrid.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoGrid.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoGrid_t {
    QByteArrayData data[10];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoGrid_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoGrid_t qt_meta_stringdata_VideoGrid = {
    {
QT_MOC_LITERAL(0, 0, 9), // "VideoGrid"
QT_MOC_LITERAL(1, 10, 13), // "playRequested"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "VideoInfo"
QT_MOC_LITERAL(4, 35, 1), // "v"
QT_MOC_LITERAL(5, 37, 17), // "loadMoreRequested"
QT_MOC_LITERAL(6, 55, 15), // "removeRequested"
QT_MOC_LITERAL(7, 71, 3), // "row"
QT_MOC_LITERAL(8, 75, 8), // "showMenu"
QT_MOC_LITERAL(9, 84, 3) // "pos"

    },
    "VideoGrid\0playRequested\0\0VideoInfo\0v\0"
    "loadMoreRequested\0removeRequested\0row\0"
    "showMenu\0pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoGrid[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,
       6,    1,   38,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    9,

       0        // eod
};

void VideoGrid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VideoGrid *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->playRequested((*reinterpret_cast< const VideoInfo(*)>(_a[1]))); break;
        case 1: _t->loadMoreRequested(); break;
        case 2: _t->removeRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< VideoInfo >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VideoGrid::*)(const VideoInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoGrid::playRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VideoGrid::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoGrid::loadMoreRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VideoGrid::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoGrid::removeRequested)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VideoGrid::staticMetaObject = { {
    QMetaObject::SuperData::link<QListWidget::staticMetaObject>(),
    qt_meta_stringdata_VideoGrid.data,
    qt_meta_data_VideoGrid,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VideoGrid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoGrid::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoGrid.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int VideoGrid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void VideoGrid::playRequested(const VideoInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VideoGrid::loadMoreRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void VideoGrid::removeRequested(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
