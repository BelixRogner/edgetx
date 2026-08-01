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

  newRow();
  AutoLabel *lblModelNew = new AutoLabel(this, tr("New models action"));
  layGeneral->addWidget(lblModelNew, row, col++);
  cboModelNew = new AutoComboBox(this);
  cboModelNew->addItems(AppData::newModelActionsList());
  cboModelNew->setValue((int)g.newModelAction(), this);
  cboModelNew->setBindSave([this] {
    g.newModelAction((AppData::NewModelAction)this->cboModelNew->currentData().toInt());
  });
  layGeneral->addWidget(cboModelNew, row, col++);

  newRow();
  AutoLabel *lblModelDelete = new AutoLabel(this, tr("Remove empty slot on model delete (B&W radios)"));
  layGeneral->addWidget(lblModelDelete, row, col++);
  chkModelDelete = new AutoCheckBox(this);
  chkModelDelete->setValue(g.removeModelSlots(), this);
  chkModelDelete->setBindSave([this] {
    g.removeModelSlots(this->chkModelDelete->isChecked());
  });
  layGeneral->addWidget(chkModelDelete, row, col++);
  ui->csectGeneral->finish(row, col, [this] { this->shrink(); });

  QGridLayout *layProfiles = ui->csectProfiles->start(tr("Radio Profiles"));
  row = col = 0;

  chkProfileTop = new AutoCheckBox(this, tr("Move selected profile to the top of the list"));
  chkProfileTop->setValue(g.sortProfiles(), this);
  chkProfileTop->setBindSave([this] {
    g.sortProfiles(this->chkProfileTop->isChecked());
  });
  layProfiles->addWidget(chkProfileTop, row, col++);

  newRow();
  chkBackupFirmware = new AutoCheckBox(this, tr("Prompt to backup current firmware before flashing new firmware"));
  chkBackupFirmware->setValue(g.enableBackup(), this);
  chkBackupFirmware->setBindSave([this] {
    g.enableBackup(this->chkBackupFirmware->isChecked());
  });
  layProfiles->addWidget(chkBackupFirmware, row, col++);

  newRow();
  chkUseSavedSettings = new AutoCheckBox(this, tr("Use radio settings backup for new models and settings files"));
  chkUseSavedSettings->setValue(g.useSavedSettings(), this);
  chkUseSavedSettings->setBindSave([this] {
    g.useSavedSettings(this->chkUseSavedSettings->isChecked());
  });
  layProfiles->addWidget(chkUseSavedSettings, row, col++);

  newRow();
  chkConfirmDeleteWrite = new AutoCheckBox(this, tr("Do not prompt to delete exisiting models on write to radio or SD path"));
  chkConfirmDeleteWrite->setValue(g.confirmWriteModelsAndSettings(), this);
  chkConfirmDeleteWrite->setBindSave([this] {
    g.confirmWriteModelsAndSettings(this->chkConfirmDeleteWrite->isChecked());
  });
  layProfiles->addWidget(chkConfirmDeleteWrite, row, col++);

  ui->csectProfiles->finish(row, col, [this] { this->shrink(); });

  QGridLayout *layFolders = ui->csectFolders->start(tr("Folders"));
  row = col = 0;

  AutoLabel *lblSDPath = new AutoLabel(this, tr("Models"));
  layFolders->addWidget(lblSDPath, row, col++);

  leModelsPath = new AutoLineEdit(this, true);
  leModelsPath->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leModelsPath->setValue(g.modelsDir(), this);
  leModelsPath->setEditSignal(true);
  leModelsPath->setBindSave([this] {
    g.modelsDir(this->leModelsPath->text());
  });
  layFolders->addWidget(leModelsPath, row, col++);

  AutoDirectorySelectButton *btnModelsPath = new AutoDirectorySelectButton(this);
  btnModelsPath->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnModelsPath->setup(tr("Select models folder"), g.modelsDir(), leModelsPath);;
  layFolders->addWidget(btnModelsPath, row, col++);

  newRow();
  AutoLabel *lblBackupsPath = new AutoLabel(this, tr("Backups"));
  layFolders->addWidget(lblBackupsPath, row, col++);

  leBackupsPath = new AutoLineEdit(this, true);
  leBackupsPath->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leBackupsPath->setValue(g.backupDir(), this);
  leBackupsPath->setEditSignal(true);
  leBackupsPath->setBindSave([this] {
    g.backupDir(this->leBackupsPath->text());
  });
  layFolders->addWidget(leBackupsPath, row, col++);

  AutoDirectorySelectButton *btnBackupsPath = new AutoDirectorySelectButton(this);
  btnBackupsPath->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnBackupsPath->setup(tr("Select backups folder"), g.backupDir(), leBackupsPath);;
  layFolders->addWidget(btnBackupsPath, row, col++);

  newRow();
  AutoLabel *lblLogsPath = new AutoLabel(this, tr("Logs"));
  layFolders->addWidget(lblLogsPath, row, col++);

  leLogsPath = new AutoLineEdit(this, true);
  leLogsPath->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leLogsPath->setValue(g.logDir(), this);
  leLogsPath->setEditSignal(true);
  leLogsPath->setBindSave([this] {
    g.logDir(this->leLogsPath->text());
  });
  layFolders->addWidget(leLogsPath, row, col++);

  AutoDirectorySelectButton *btnLogsPath = new AutoDirectorySelectButton(this);
  btnLogsPath->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnLogsPath->setup(tr("Select logs folder"), g.logDir(), leLogsPath);;
  layFolders->addWidget(btnLogsPath, row, col++);

  ui->csectFolders->finish(-1, -1, [this] { this->shrink(); });

  QGridLayout *layLogging = ui->csectLogging->start(tr("Logging"));
  row = col = 0;

  AutoLabel *lblLogApp = new AutoLabel(this, tr("Application (Companion/Simulator)"));
  layLogging->addWidget(lblLogApp, row, col++);

  chkLogApp = new AutoCheckBox(this);
  chkLogApp->setValue(g.appDebugLog(), this);
  chkLogApp->setBindSave([this] {
    g.appDebugLog(this->chkLogApp->isChecked());
  });
  layLogging->addWidget(chkLogApp, row, col++);

  newRow();
  AutoLabel *lblLogFW = new AutoLabel(this, tr("Radio Firmware (in Simulator)"));
  layLogging->addWidget(lblLogFW, row, col++);

  chkLogFW = new AutoCheckBox(this);
  chkLogFW->setValue(g.fwTraceLog(), this);
  chkLogFW->setBindSave([this] {
    g.fwTraceLog(this->chkLogFW->isChecked());
  });
  layLogging->addWidget(chkLogFW, row, col++);

  newRow();
  AutoLabel *lblLogLevel = new AutoLabel(this, tr("Level"));
  layLogging->addWidget(lblLogLevel, row, col++);

  cboLogLevel = new AutoComboBox(this);
  cboLogLevel->addItems(AppData::updateLogLevelsList());
  cboLogLevel->setValue((int)g.updLogLevel(), this);
  cboLogLevel->setBindSave([this] {
    g.updLogLevel(this->cboLogLevel->currentData().toInt());
  });
  layLogging->addWidget(cboLogLevel, row, col++);

  newRow();
  AutoLabel *lblLogVerbose = new AutoLabel(this, tr("Verbose"));
  layLogging->addWidget(lblLogVerbose, row, col++);

  chkLogVerbose = new AutoCheckBox(this);
  chkLogVerbose->setValue(g.logVerbose(), this);
  chkLogVerbose->setBindSave([this] {
    g.logVerbose(this->chkLogVerbose->isChecked());
  });
  layLogging->addWidget(chkLogVerbose, row, col++);

  ui->csectLogging->finish(row, col, [this] { this->shrink(); });

  QGridLayout *layGoogle = ui->csectGoogleEarth->start(tr("Google Earth"));
  row = col = 0;

  AutoLabel *lblGoogleExe = new AutoLabel(this, tr("Executable"));
  layGoogle->addWidget(lblGoogleExe, row, col++);

  leGoogleExe = new AutoLineEdit(this, true);
  leGoogleExe->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leGoogleExe->setValue(g.gePath(), this);
  leGoogleExe->setEditSignal(true);
  leGoogleExe->setBindSave([this] {
    g.gePath(this->leGoogleExe->text());
  });
  layGoogle->addWidget(leGoogleExe, row, col++);

  AutoFileSelectButton *btnGoogleExe = new AutoFileSelectButton(this);
  btnGoogleExe->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnGoogleExe->setup(tr("Select Google Earth executable"), g.gePath(), "", leGoogleExe);;
  layGoogle->addWidget(btnGoogleExe, row, col++);

  ui->csectGoogleEarth->finish(-1, -1, [this] { this->shrink(); });

  QGridLayout *laySplash = ui->csectSplash->start(tr("Splash Screens (B&&W radios)"));
  row = col = 0;

  AutoLabel *lblSplashLibraries = new AutoLabel(this, tr("Libraries"));
  laySplash->addWidget(lblSplashLibraries, row, col++);

  cboSplashLibraries = new AutoComboBox(this);
  cboSplashLibraries->addItems(AppData::splashLibsList());
  cboSplashLibraries->setValue(g.embedSplashes(), this);
  cboSplashLibraries->setBindSave([this] {
    g.embedSplashes(this->cboSplashLibraries->currentData().toInt());
  });
  laySplash->addWidget(cboSplashLibraries, row, col++);

  newRow();
  AutoLabel *lblSplashUserPath = new AutoLabel(this, tr("User folder"));
  laySplash->addWidget(lblSplashUserPath, row, col++);

  leSplashUserPath = new AutoLineEdit(this, true);
  leSplashUserPath->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leSplashUserPath->setValue(g.libDir(), this);
  leSplashUserPath->setEditSignal(true);
  leSplashUserPath->setBindSave([this] {
    g.libDir(this->leSplashUserPath->text());
  });
  laySplash->addWidget(leSplashUserPath, row, col++);

  AutoDirectorySelectButton *btnSplashUser = new AutoDirectorySelectButton(this);
  btnSplashUser->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnSplashUser->setup(tr("Select user splash folder"), g.libDir(), leSplashUserPath);;
  laySplash->addWidget(btnSplashUser, row, col++);

  ui->csectSplash->finish(-1, -1, [this] { this->shrink(); });

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
