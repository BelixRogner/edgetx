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

  row = col = 0;
  ui->csectGeneral->setTitle(tr("General"));
  QGridLayout *layGeneral = new QGridLayout();
  // show splash on start
  AutoLabel *lblSplash = new AutoLabel(this, tr("Splash"));
  layGeneral->addWidget(lblSplash, row, col++);
  chkSplash = new AutoCheckBox(this);
  chkSplash->setValue(g.showSplash(), this);
  chkSplash->setBindSave([this] {
    g.showSplash(this->chkSplash->isChecked());
  });
  layGeneral->addWidget(chkSplash, row, col++);

  newRow();



  addHSpring(layGeneral, col, row);
  ui->csectGeneral->setContentLayout(*layGeneral);
  ui->csectGeneral->setBindResize([this] { this->shrink(); });

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

