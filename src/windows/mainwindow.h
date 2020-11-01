#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../emu/et3400.h"
#include "display.h"

#include <fstream>
#include <QMainWindow>
#include <QApplication>

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
  int last_cycles;
  void load_ram();
  void updatecps();
  Display *display;
  et3400emu *emu;
};

#endif // MAINWINDOW_H