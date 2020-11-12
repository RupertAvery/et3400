#include "mainwindow.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  settings_dialog = new SettingsDialog(this);
  debugger_dialog = new DebuggerDialog;

  // Menu

  QAction *settings_action = new QAction("&Settings", this);
  QAction *about_action = new QAction("&About", this);
  QAction *debugger_action = new QAction("&Debugger", this);

  QAction *openRam_action = new QAction("&Load RAM", this);
  openRam_action->setShortcut(Qt::CTRL + Qt::Key_O);

  QAction *saveRam_action = new QAction("&Save RAM", this);
  saveRam_action->setShortcut(Qt::CTRL + Qt::Key_S);

  QAction *quit_action = new QAction("E&xit", this);
  quit_action->setShortcut(Qt::CTRL + Qt::Key_X);

  QMenu *file;
  file = menuBar()->addMenu("&File");
  file->addAction(openRam_action);
  file->addAction(saveRam_action);
  file->addSeparator();
  file->addAction(quit_action);

  menuBar()->addAction(debugger_action);

  QMenu *config_menu;
  config_menu = menuBar()->addMenu("&Config");
  config_menu->addAction(settings_action);

  QMenu *help_menu;
  help_menu = menuBar()->addMenu("&Help");
  help_menu->addAction(about_action);

  display = new Display;
  keypad = new Keypad;

  connect(openRam_action, &QAction::triggered, this, &MainWindow::load_ram);
  connect(saveRam_action, &QAction::triggered, this, &MainWindow::save_ram);
  connect(quit_action, &QAction::triggered, qApp, QApplication::quit);

  connect(debugger_action, &QAction::triggered, this, &MainWindow::show_debugger);
  connect(settings_action, &QAction::triggered, this, &MainWindow::show_settings);
  connect(about_action, &QAction::triggered, this, &MainWindow::show_about);

  // Layout
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

  setFixedSize(QSize(350, 500));

  execute_emu();

  last_cycles = 0;

  //setAttribute(Qt::WA_DeleteOnClose);
  //connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)) );

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::fps));
  timer->start(1000);
}

void MainWindow::fps()
{
  int cps = 0;
  if (last_cycles == 0)
  {
    cps = emu->total_cycles;
  }
  else
  {
    cps = emu->total_cycles - last_cycles;
  }

  std::cout << cps << std::endl;

  last_cycles = emu->total_cycles;
}

void MainWindow::show_debugger()
{
  debugger_dialog->set_emulator(emu);
  debugger_dialog->show();
}

void MainWindow::show_about()
{
  AboutDialog dialog;
  dialog.exec();
}

void MainWindow::show_settings()
{
  settings_dialog->set_emulator(emu);
  settings_dialog->show();
}

void MainWindow::load_ram()
{
  File::load_ram(this, emu);
  debugger_dialog->refresh();
  debugger_dialog->update_button_state();
}


void MainWindow::save_ram()
{
}

void MainWindow::load_brk()
{
}

void MainWindow::save_brk()
{
}

void MainWindow::load_map()
{
}

void MainWindow::save_map()
{
}


void MainWindow::updatecps()
{
  int cps = emu->get_cycles() - last_cycles;
  last_cycles = emu->get_cycles();
  cout << cps << endl;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  debugger_dialog->close();
}

MainWindow::~MainWindow()
{
  qDebug() << "Main destroy";
  emu->stop();
  delete emu;
  delete display;
  delete keypad;
  delete settings_dialog;
  delete debugger_dialog;
  qDebug() << "Main destroy done";
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
  switch (ev->key())
  {
  case Qt::Key_0:
    emu->keypad->press_key(keypad_io::Key0);
    break;
  case Qt::Key_1:
    emu->keypad->press_key(keypad_io::Key1);
    break;
  case Qt::Key_2:
    emu->keypad->press_key(keypad_io::Key2);
    break;
  case Qt::Key_3:
    emu->keypad->press_key(keypad_io::Key3);
    break;
  case Qt::Key_4:
    emu->keypad->press_key(keypad_io::Key4);
    break;
  case Qt::Key_5:
    emu->keypad->press_key(keypad_io::Key5);
    break;
  case Qt::Key_6:
    emu->keypad->press_key(keypad_io::Key6);
    break;
  case Qt::Key_7:
    emu->keypad->press_key(keypad_io::Key7);
    break;
  case Qt::Key_8:
    emu->keypad->press_key(keypad_io::Key8);
    break;
  case Qt::Key_9:
    emu->keypad->press_key(keypad_io::Key9);
    break;
  case Qt::Key_A:
    emu->keypad->press_key(keypad_io::KeyA);
    break;
  case Qt::Key_B:
    emu->keypad->press_key(keypad_io::KeyB);
    break;
  case Qt::Key_C:
    emu->keypad->press_key(keypad_io::KeyC);
    break;
  case Qt::Key_D:
    emu->keypad->press_key(keypad_io::KeyD);
    break;
  case Qt::Key_E:
    emu->keypad->press_key(keypad_io::KeyE);
    break;
  case Qt::Key_F:
    emu->keypad->press_key(keypad_io::KeyF);
    break;
  case Qt::Key_Escape:
    emu->keypad->press_key(keypad_io::KeyReset);
    break;
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
  switch (ev->key())
  {
  case Qt::Key_0:
    emu->keypad->release_key(keypad_io::Key0);
    break;
  case Qt::Key_1:
    emu->keypad->release_key(keypad_io::Key1);
    break;
  case Qt::Key_2:
    emu->keypad->release_key(keypad_io::Key2);
    break;
  case Qt::Key_3:
    emu->keypad->release_key(keypad_io::Key3);
    break;
  case Qt::Key_4:
    emu->keypad->release_key(keypad_io::Key4);
    break;
  case Qt::Key_5:
    emu->keypad->release_key(keypad_io::Key5);
    break;
  case Qt::Key_6:
    emu->keypad->release_key(keypad_io::Key6);
    break;
  case Qt::Key_7:
    emu->keypad->release_key(keypad_io::Key7);
    break;
  case Qt::Key_8:
    emu->keypad->release_key(keypad_io::Key8);
    break;
  case Qt::Key_9:
    emu->keypad->release_key(keypad_io::Key9);
    break;
  case Qt::Key_A:
    emu->keypad->release_key(keypad_io::KeyA);
    break;
  case Qt::Key_B:
    emu->keypad->release_key(keypad_io::KeyB);
    break;
  case Qt::Key_C:
    emu->keypad->release_key(keypad_io::KeyC);
    break;
  case Qt::Key_D:
    emu->keypad->release_key(keypad_io::KeyD);
    break;
  case Qt::Key_E:
    emu->keypad->release_key(keypad_io::KeyE);
    break;
  case Qt::Key_F:
    emu->keypad->release_key(keypad_io::KeyF);
    break;
  case Qt::Key_Escape:
    emu->keypad->release_key(keypad_io::KeyReset);
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

  emu = new et3400emu(keypad->device, display->device);

  emu->loadROM(ROM_ADDR, (uint8_t *)buffer, ROM_SIZE);
  emu->loadMap();

  debugger_dialog->set_emulator(emu);
  keypad->device->on_reset_press = [this] { emu->reset(); };
  emu->on_render_frame = [this] { display->update_display(); };

  emu->init();
  emu->start();
}