#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../emu/et3400.h"
#include "display.h"
#include "keypad.h"
#include "about.h"
#include "settings.h"

#include <fstream>
#include <QMainWindow>
#include <QApplication>
#include <QTimer>

class MainWindow : public QMainWindow
{
  uint16_t ROM_SIZE = 0x400;
  const uint16_t ROM_ADDR = 0xFC00;

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void keyPressEvent(QKeyEvent * ev) override;
  void keyReleaseEvent(QKeyEvent * ev) override;

  void execute_emu();

private:
  long long last_cycles;

  Display *display;
  Keypad *keypad;
  SettingsDialog *settings;
  et3400emu *emu;

  void load_ram();
  void show_about();
  void show_settings();
  void updatecps();
  void fps();
};

#endif // MAINWINDOW_H