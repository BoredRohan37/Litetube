/****************************************************************************
** Meta object code from reading C++ file 'PlayerControls.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/PlayerControls.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlayerControls.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClickSlider_t {
    QByteArrayData data[1];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClickSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClickSlider_t qt_meta_stringdata_ClickSlider = {
    {
QT_MOC_LITERAL(0, 0, 11) // "ClickSlider"

    },
    "ClickSlider"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClickSlider[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ClickSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ClickSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<QSlider::staticMetaObject>(),
    qt_meta_stringdata_ClickSlider.data,
    qt_meta_data_ClickSlider,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ClickSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClickSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClickSlider.stringdata0))
        return static_cast<void*>(this);
    return QSlider::qt_metacast(_clname);
}

int ClickSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSlider::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_PlayerControls_t {
    QByteArrayData data[12];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlayerControls_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlayerControls_t qt_meta_stringdata_PlayerControls = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PlayerControls"
QT_MOC_LITERAL(1, 15, 16), // "toggleFullscreen"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 14), // "requestCompact"
QT_MOC_LITERAL(4, 48, 13), // "nextRequested"
QT_MOC_LITERAL(5, 62, 12), // "statsToggled"
QT_MOC_LITERAL(6, 75, 2), // "on"
QT_MOC_LITERAL(7, 78, 10), // "onPosition"
QT_MOC_LITERAL(8, 89, 3), // "pos"
QT_MOC_LITERAL(9, 93, 3), // "dur"
QT_MOC_LITERAL(10, 97, 17), // "buildSettingsMenu"
QT_MOC_LITERAL(11, 115, 12) // "buildSubMenu"

    },
    "PlayerControls\0toggleFullscreen\0\0"
    "requestCompact\0nextRequested\0statsToggled\0"
    "on\0onPosition\0pos\0dur\0buildSettingsMenu\0"
    "buildSubMenu"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerControls[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,
       5,    1,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   55,    2, 0x08 /* Private */,
      10,    0,   60,    2, 0x08 /* Private */,
      11,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    8,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlayerControls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerControls *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleFullscreen(); break;
        case 1: _t->requestCompact(); break;
        case 2: _t->nextRequested(); break;
        case 3: _t->statsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onPosition((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: _t->buildSettingsMenu(); break;
        case 6: _t->buildSubMenu(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::toggleFullscreen)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::requestCompact)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::nextRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::statsToggled)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlayerControls::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlayerControls.data,
    qt_meta_data_PlayerControls,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlayerControls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerControls::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerControls.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlayerControls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void PlayerControls::toggleFullscreen()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PlayerControls::requestCompact()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PlayerControls::nextRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PlayerControls::statsToggled(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
