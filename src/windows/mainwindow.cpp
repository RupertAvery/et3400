#include "mainwindow.h"
#include "../util/srec.h"
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QtWidgets>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  QAction *open = new QAction("&Load .S19", this);
  QAction *quit = new QAction("&Quit", this);
  QAction *about = new QAction("&About", this);

  QMenu *file;
  file = menuBar()->addMenu("&File");
  file->addAction(open);
  file->addSeparator();
  file->addAction(quit);

  QMenu *help;
  help = menuBar()->addMenu("&Help");
  help->addAction(about);

  display = new Display;
  keypad = new Keypad;

  connect(quit, &QAction::triggered, qApp, QApplication::quit);
  connect(open, &QAction::triggered, this, &MainWindow::load_ram);
  connect(about, &QAction::triggered, this, &MainWindow::show_about);

  QGridLayout *mainLayout = new QGridLayout;

  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 1);
  mainLayout->setRowStretch(0, 1);
  mainLayout->setRowStretch(1, 3);
  mainLayout->addWidget(display, 0, 0, 1, 2, Qt::AlignTop);
  mainLayout->addWidget(keypad, 1, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

  auto central = new QWidget;
  central->setLayout(mainLayout);

  setCentralWidget(central);

  setWindowTitle(tr("ET-3400 Emulator"));
  last_cycles = 0;

  this->setFixedSize(QSize(350, 500));

  execute_emu();
}

void MainWindow::show_about()
{
  AboutDialog dialog;
  dialog.exec();
}

void MainWindow::load_ram()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open .S19 File"), "", tr("Assembly Files (*.obj *.s19)"));

  std::vector<srec_block> *blocks = new std::vector<srec_block>;

  // pause emulation to avoid overwriting memory while executing
  emu->stop();

  // load S19 blocks
  if (Srec::Read(fileName, blocks))
  {
    uint8_t *memory = emu->get_memory();

    // write blocks to memory
    for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
    {
      for (int i = 0; i < it->bytecount; ++i)
      {
        memcpy(&memory[it->address], it->data, it->bytecount);
      }
    }
  }

  // clean up
  for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
  {
    free(it->data);
  }

  free(blocks);

  // reset and resume emulation
  emu->reset();
  emu->start();
}

void MainWindow::updatecps()
{
  int cps = emu->get_cycles() - last_cycles;
  last_cycles = emu->get_cycles();
  cout << cps << endl;
}

MainWindow::~MainWindow()
{
  emu->stop();
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
  switch (ev->key())
  {
  case Qt::Key_0:
    emu->press_key(et3400emu::Key0);
    break;
  case Qt::Key_1:
    emu->press_key(et3400emu::Key1);
    break;
  case Qt::Key_2:
    emu->press_key(et3400emu::Key2);
    break;
  case Qt::Key_3:
    emu->press_key(et3400emu::Key3);
    break;
  case Qt::Key_4:
    emu->press_key(et3400emu::Key4);
    break;
  case Qt::Key_5:
    emu->press_key(et3400emu::Key5);
    break;
  case Qt::Key_6:
    emu->press_key(et3400emu::Key6);
    break;
  case Qt::Key_7:
    emu->press_key(et3400emu::Key7);
    break;
  case Qt::Key_8:
    emu->press_key(et3400emu::Key8);
    break;
  case Qt::Key_9:
    emu->press_key(et3400emu::Key9);
    break;
  case Qt::Key_A:
    emu->press_key(et3400emu::KeyA);
    break;
  case Qt::Key_B:
    emu->press_key(et3400emu::KeyB);
    break;
  case Qt::Key_C:
    emu->press_key(et3400emu::KeyC);
    break;
  case Qt::Key_D:
    emu->press_key(et3400emu::KeyD);
    break;
  case Qt::Key_E:
    emu->press_key(et3400emu::KeyE);
    break;
  case Qt::Key_F:
    emu->press_key(et3400emu::KeyF);
    break;
  case Qt::Key_Escape:
    emu->press_key(et3400emu::KeyReset);
    break;
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
  switch (ev->key())
  {
  case Qt::Key_0:
    emu->release_key(et3400emu::Key0);
    break;
  case Qt::Key_1:
    emu->release_key(et3400emu::Key1);
    break;
  case Qt::Key_2:
    emu->release_key(et3400emu::Key2);
    break;
  case Qt::Key_3:
    emu->release_key(et3400emu::Key3);
    break;
  case Qt::Key_4:
    emu->release_key(et3400emu::Key4);
    break;
  case Qt::Key_5:
    emu->release_key(et3400emu::Key5);
    break;
  case Qt::Key_6:
    emu->release_key(et3400emu::Key6);
    break;
  case Qt::Key_7:
    emu->release_key(et3400emu::Key7);
    break;
  case Qt::Key_8:
    emu->release_key(et3400emu::Key8);
    break;
  case Qt::Key_9:
    emu->release_key(et3400emu::Key9);
    break;
  case Qt::Key_A:
    emu->release_key(et3400emu::KeyA);
    break;
  case Qt::Key_B:
    emu->release_key(et3400emu::KeyB);
    break;
  case Qt::Key_C:
    emu->release_key(et3400emu::KeyC);
    break;
  case Qt::Key_D:
    emu->release_key(et3400emu::KeyD);
    break;
  case Qt::Key_E:
    emu->release_key(et3400emu::KeyE);
    break;
  case Qt::Key_F:
    emu->release_key(et3400emu::KeyF);
    break;
  case Qt::Key_Escape:
    emu->release_key(et3400emu::KeyReset);
    break;
  }
}

void MainWindow::execute_emu()
{
  QFile file(":/rom/monitor.bin");

  char *buffer;

  if (!file.open(QIODevice::ReadOnly))
  {
    throw -10010;
  }
  else
  {
    buffer = (char *)malloc(ROM_SIZE);

    file.read(buffer, ROM_SIZE);
  }

  file.close();

  emu = new et3400emu;

  emu->loadROM(ROM_ADDR, (uint8_t *)buffer, ROM_SIZE);

  display->set_memory(emu->get_memory());
  keypad->set_emulator(emu);

  emu->init();
  emu->start();
}