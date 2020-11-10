#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../emu/et3400.h"
#include "../widgets/display.h"
#include "../widgets/keypad.h"
#include "../util/srec.h"
#include "about.h"
#include "debugger.h"
#include "settings.h"
#include "file.h"

#include <fstream>
#include <iostream>

#include <QMainWindow>
#include <QApplication>
#include <QTimer>
#include <QCloseEvent>
#include <QFile>
#include <QMenu>
#include <QMenuBar>

class MainWindow : public QMainWindow
{
  uint16_t ROM_SIZE = 0x400;
  const uint16_t ROM_ADDR = 0xFC00;

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

  void execute_emu();

private:
  long long last_cycles;

  Display *display;
  Keypad *keypad;
  SettingsDialog *settings_dialog;
  DebuggerDialog *debugger_dialog;
  et3400emu *emu;

  void load_ram();
  void save_ram();
  void load_brk();
  void save_brk();
  void load_map();
  void save_map();
  void show_about();
  void show_settings();
  void show_debugger();
  void updatecps();
  void fps();
};

#endif // MAINWINDOW_H