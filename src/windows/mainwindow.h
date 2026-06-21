#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../emu/et3400.h"
#include "../widgets/display.h"
#include "../widgets/keypad.h"
#include "../util/srec.h"
#include "../util/settings.h"
#include "about.h"
#include "debugger.h"
#include "settings.h"
#include "tips.h"
#include "file.h"
#include "save.h"

#include <fstream>
#include <iostream>

#include <QMainWindow>
#include <QApplication>
#include <QTimer>
#include <QCloseEvent>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
  uint16_t MONITOR_SIZE = 0x400;
  uint16_t FANTOMII_SIZE = 0x800;
  uint16_t TINYBASIC_SIZE = 0x800;

  const uint16_t MONITOR_ADDR = 0xFC00;
  const uint16_t FANTOMII_ADDR = 0x1400;
  const uint16_t TINYBASIC_ADDR = 0x1C00;

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

  void start();

  void setAddress(std::string address);
  void setSpeed(std::string speed);
  void setShowDebugger(bool show);
  void setLabel(std::string labelFile);
  void setRAM(std::string file);
  void setROM(std::string file);

  LoadSettings load_ram_settings{"RAM", 0x0000, 0x1FFF};
  SaveSettings save_ram_settings{"", 0x0000, 0x01FF};
  LoadSettings load_rom_settings{"Monitor ROM", 0xFC00, 0xFFFF};

  static const int AUTOSTART_DELAY_MS = 500;
  static const int AUTOSTART_KEY_HOLD_MS = 200;

private:
  long long last_cycles;
  Settings settings;
  Display *display = nullptr;
  Keypad *keypad = nullptr;
  SettingsDialog *settings_dialog = nullptr;
  DebuggerDialog *debugger_dialog = nullptr;
  et3400emu *emu = nullptr;
  uint16_t start_address = 0;
  bool has_start_address = false;
  bool showDebugger = false;

  void init_emu();
  void execute_emu();
  void load_rom();
  void load_ram();
  void save_ram();
  void show_about();
  void show_settings();
  void show_debugger();
  void show_tips();
  void updatecps();
  void fps();
  void autostart_sequence();
  void load_devices();

};

#endif // MAINWINDOW_H