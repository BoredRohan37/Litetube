/****************************************************************************
** Meta object code from reading C++ file 'MpvController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/playback/MpvController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MpvController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MpvController_t {
    QByteArrayData data[18];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MpvController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MpvController_t qt_meta_stringdata_MpvController = {
    {
QT_MOC_LITERAL(0, 0, 13), // "MpvController"
QT_MOC_LITERAL(1, 14, 15), // "positionChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 3), // "pos"
QT_MOC_LITERAL(4, 35, 3), // "dur"
QT_MOC_LITERAL(5, 39, 12), // "pauseChanged"
QT_MOC_LITERAL(6, 52, 6), // "paused"
QT_MOC_LITERAL(7, 59, 16), // "bufferingChanged"
QT_MOC_LITERAL(8, 76, 9), // "buffering"
QT_MOC_LITERAL(9, 86, 10), // "fileLoaded"
QT_MOC_LITERAL(10, 97, 13), // "playbackEnded"
QT_MOC_LITERAL(11, 111, 13), // "errorOccurred"
QT_MOC_LITERAL(12, 125, 3), // "msg"
QT_MOC_LITERAL(13, 129, 13), // "tracksChanged"
QT_MOC_LITERAL(14, 143, 6), // "tracks"
QT_MOC_LITERAL(15, 150, 11), // "inputAction"
QT_MOC_LITERAL(16, 162, 6), // "action"
QT_MOC_LITERAL(17, 169, 8) // "onWakeup"

    },
    "MpvController\0positionChanged\0\0pos\0"
    "dur\0pauseChanged\0paused\0bufferingChanged\0"
    "buffering\0fileLoaded\0playbackEnded\0"
    "errorOccurred\0msg\0tracksChanged\0tracks\0"
    "inputAction\0action\0onWakeup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MpvController[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       5,    1,   64,    2, 0x06 /* Public */,
       7,    1,   67,    2, 0x06 /* Public */,
       9,    0,   70,    2, 0x06 /* Public */,
      10,    0,   71,    2, 0x06 /* Public */,
      11,    1,   72,    2, 0x06 /* Public */,
      13,    1,   75,    2, 0x06 /* Public */,
      15,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QVariantList,   14,
    QMetaType::Void, QMetaType::QString,   16,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void MpvController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MpvController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->positionChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 1: _t->pauseChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->bufferingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->fileLoaded(); break;
        case 4: _t->playbackEnded(); break;
        case 5: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->tracksChanged((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 7: _t->inputAction((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onWakeup(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MpvController::*)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MpvController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::pauseChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MpvController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::bufferingChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MpvController::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::fileLoaded)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MpvController::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::playbackEnded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MpvController::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::errorOccurred)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MpvController::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::tracksChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MpvController::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvController::inputAction)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MpvController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MpvController.data,
    qt_meta_data_MpvController,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MpvController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MpvController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MpvController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MpvController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void MpvController::positionChanged(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MpvController::pauseChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MpvController::bufferingChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MpvController::fileLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MpvController::playbackEnded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MpvController::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MpvController::tracksChanged(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MpvController::inputAction(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
