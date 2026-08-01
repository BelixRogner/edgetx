/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "prefs_app.h"
#include "ui_prefs_app.h"
#include "appdata.h"

PrefsAppPanel::PrefsAppPanel(QWidget * parent, Firmware * fw, Board::Type & bd, Profile & prof) :
  PrefsPanel(parent, fw, bd, prof),
  ui(new Ui::PrefsApp)
{
  ui->setupUi(this);
  lock = true;

  QGridLayout *layGeneral = ui->csectGeneral->start(tr("General"));
  row = col = 0;
  AutoLabel *lblShowSplash = new AutoLabel(this, tr("Show splash screen"));
  layGeneral->addWidget(lblShowSplash, row, col++);
  chkSplash = new AutoCheckBox(this);
  chkSplash->setValue(g.showSplash(), this);
  chkSplash->setBindSave([this] {
    g.showSplash(this->chkSplash->isChecked());
  });
  layGeneral->addWidget(chkSplash, row, col++);

  newRow();
  AutoLabel *lblProfPrmpt = new AutoLabel(this, tr("Prompt for radio profile on startup"));
  layGeneral->addWidget(lblProfPrmpt, row, col++);
  chkProfPrmpt = new AutoCheckBox(this);
  chkProfPrmpt->setValue(g.promptProfile(), this);
  chkProfPrmpt->setBindSave([this] {
    g.promptProfile(this->chkProfPrmpt->isChecked());
  });
  layGeneral->addWidget(chkProfPrmpt, row, col++);

  newRow();
  AutoLabel *lblRecentFiles = new AutoLabel(this, tr("Most recently used files"));
  layGeneral->addWidget(lblRecentFiles, row, col++);
  sbxRecentFiles = new AutoSpinBox(this);
  sbxRecentFiles->setRange(1, 50);
  sbxRecentFiles->setValue(g.historySize());
  sbxRecentFiles->setBindSave([this] {
    g.historySize(this->sbxRecentFiles->value());
  });
  layGeneral->addWidget(sbxRecentFiles, row, col++);
  ui->csectGeneral->finish(row, col, [this] { this->shrink(); });

  QGridLayout *layModelActions = ui->csectModelActions->start(tr("Model Actions"));
  row = col = 0;
  AutoLabel *lblModelNew = new AutoLabel(this, tr("New"));
  layModelActions->addWidget(lblModelNew, row, col++);
  cboModelNew = new AutoComboBox(this);
  cboModelNew->addItems(AppData::newModelActionsList());
  cboModelNew->setValue((int)g.newModelAction(), this);
  cboModelNew->setBindSave([this] {
    g.newModelAction((AppData::NewModelAction)this->cboModelNew->currentData().toInt());
  });
  layModelActions->addWidget(cboModelNew, row, col++);

  newRow();
  AutoLabel *lblModelDelete = new AutoLabel(this, tr("Delete"));
  layModelActions->addWidget(lblModelDelete, row, col++);
  chkModelDelete = new AutoCheckBox(this);
  chkModelDelete->setValue(g.removeModelSlots(), this);
  chkModelDelete->setBindSave([this] {
    g.removeModelSlots(this->chkModelDelete->isChecked());
  });
  layModelActions->addWidget(chkModelDelete, row, col++);
  ui->csectModelActions->finish(row, col, [this] { this->shrink(); });


  update();
  shrink();
  lock = false;
}

PrefsAppPanel::~PrefsAppPanel()
{
  delete ui;
}

void PrefsAppPanel::save()
{
  AbstractPanel::save();
}

void PrefsAppPanel::update()
{
  AbstractPanel::update();
}
