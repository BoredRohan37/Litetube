#pragma once
#include <QString>
#include <QIcon>
namespace Theme {
QIcon appIcon();
inline QString stylesheet() {
    return R"(
* { font-family: "Segoe UI", "Tahoma", sans-serif; font-size: 13px; }
QWidget { background: #0f0f0f; color: #f1f1f1; }
QDialog, QMainWindow { background: #0f0f0f; }
QScrollArea, QScrollArea > QWidget > QWidget { background: #0f0f0f; border: none; }
QListWidget { background: #0f0f0f; border: none; outline: 0; }
QLineEdit, QComboBox, QSpinBox, QPlainTextEdit, QTextBrowser { background: #121212; border: 1px solid #303030; border-radius: 4px; padding: 5px 8px; selection-background-color: #cc0000; }
QLineEdit:focus, QComboBox:focus { border-color: #3ea6ff; }
QComboBox QAbstractItemView { background: #212121; selection-background-color: #3d3d3d; outline: 0; }
QPushButton, QToolButton { background: #272727; border: none; border-radius: 16px; padding: 7px 14px; }
QPushButton:hover, QToolButton:hover { background: #3f3f3f; }
QPushButton:disabled { color: #777; }
QPushButton:checked, QToolButton:checked { background: #f1f1f1; color: #0f0f0f; }
QPushButton#primary { background: #f1f1f1; color: #0f0f0f; font-weight: 600; }
QPushButton#primary:hover { background: #d9d9d9; }
QPushButton#subscribe { background: #f1f1f1; color: #0f0f0f; font-weight: 600; }
QPushButton#subscribe:checked { background: #272727; color: #f1f1f1; }
QPushButton#red { background: #cc0000; color: white; font-weight: 600; }
QPushButton#chip { background: #272727; border-radius: 8px; padding: 6px 12px; }
QPushButton#chip:checked { background: #f1f1f1; color: #0f0f0f; }
QPushButton#flat { background: transparent; color: #3ea6ff; }
QToolButton#ctl { background: transparent; font-size: 16px; padding: 4px 8px; border-radius: 6px; }
QToolButton#ctl:hover { background: #2a2a2a; }
QToolButton#icon { background: transparent; font-size: 18px; padding: 6px 10px; border-radius: 18px; }
QToolButton#icon:hover { background: #272727; }
QSlider::groove:horizontal { height: 4px; background: #4a4a4a; border-radius: 2px; }
QSlider::sub-page:horizontal { background: #ff0000; border-radius: 2px; }
QSlider::handle:horizontal { background: #ff0000; width: 12px; margin: -4px 0; border-radius: 6px; }
QScrollBar:vertical { background: transparent; width: 12px; }
QScrollBar::handle:vertical { background: #4a4a4a; border-radius: 6px; min-height: 30px; margin: 2px; }
QScrollBar::handle:vertical:hover { background: #6a6a6a; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; width: 0; }
QScrollBar:horizontal { background: transparent; height: 12px; }
QScrollBar::handle:horizontal { background: #4a4a4a; border-radius: 6px; margin: 2px; }
QMenu { background: #212121; border: 1px solid #303030; padding: 6px 0; }
QMenu::item { padding: 7px 28px; }
QMenu::item:selected { background: #3d3d3d; }
QToolTip { background: #3a3a3a; color: #fff; border: none; padding: 4px; }
QTabWidget::pane { border: none; }
QTabBar::tab { background: transparent; padding: 8px 16px; color: #aaa; }
QTabBar::tab:selected { color: #fff; border-bottom: 2px solid #fff; }
QLabel#h1 { font-size: 20px; font-weight: 600; }
QLabel#h2 { font-size: 16px; font-weight: 600; }
QLabel#muted { color: #aaaaaa; }
QWidget#topbar { background: #0f0f0f; border-bottom: 1px solid #272727; }
QListWidget#nav { background: #0f0f0f; padding: 6px; }
QListWidget#nav::item { padding: 9px 12px; border-radius: 10px; }
QListWidget#nav::item:hover { background: #272727; }
QListWidget#nav::item:selected { background: #272727; font-weight: 600; }
QFrame#card { background: #181818; border-radius: 10px; }
)";
}
}
