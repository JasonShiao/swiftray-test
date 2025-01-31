#pragma once

#include <QDialog>
#include <widgets/base-container.h>

namespace Ui {
  class MachineMonitor;
}

class MachineMonitor : public QDialog, BaseContainer {
Q_OBJECT

public:
  explicit MachineMonitor(QWidget *parent = nullptr);

  ~MachineMonitor();

private:
  Ui::MachineMonitor *ui;
};