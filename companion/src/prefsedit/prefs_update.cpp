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

#include "prefs_update.h"
#include "ui_prefs_update.h"
#include "updates/updatefactories.h"
#include "updates/updateoptionsdialog.h"

#include <QMessageBox>

PrefsUpdatePanel::PrefsUpdatePanel(QWidget * parent, Firmware * fw, Board::Type & bd, Profile & prof, UpdateFactories * factories):
  PrefsPanel(parent, fw, bd, prof),
  ui(new Ui::PrefsUpdate),
  factories(factories)
{
  ui->setupUi(this);
  lock = true;

  ui->cboCheckFreq->addItems(AppData::updateCheckFreqsList());
  ui->cboCheckFreq->setValue((int)g.updateCheckFreq(), this);
  ui->cboCheckFreq->setBindSave([this] {
    g.updateCheckFreq((AppData::UpdateCheckFreq)this->ui->cboCheckFreq->currentData().toInt());
  });

  ui->btnResetToDefaults->setBindClicked([this] {
    if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Reset all update settings to defaults. Are you sure?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
      g.resetUpdatesSettings();
      QMessageBox::warning(this, CPN_STR_APP_NAME,
                           tr("Update settings have been reset. Please close and restart Companion to avoid unexpected behaviour!"));
      this->loadUpdatesTab();
    }
  });

  sectionFolders();
  sectionComponents();
  sectionOptions();
  sectionPostUpdate();

  update();
  shrink();
  lock = false;
}

PrefsUpdatePanel::~PrefsUpdatePanel()
{
  delete ui;
}

void PrefsUpdatePanel::onSDPathChanged()
{
  // change stuff
  update();
}

void PrefsUpdatePanel::save()
{
  QStringList msgs;

  if (leDownloadDir->text().isEmpty())
    msgs.append(tr("Download folder path missing!"));

  if (leDecompressDir->text().isEmpty())
    msgs.append(tr("Decompress folder path missing!"));

  if (leUpdateDir->text().isEmpty())
    msgs.append(tr("Update folder path missing!"));

  if (!chkDecompressDirUseDwnld->isChecked() &&
      leDecompressDir->text().trimmed() == leDownloadDir->text().trimmed())
    msgs.append(tr("Decompress and download folders have the same path!"));

  if (msgs.count() > 0) {
    QMessageBox::warning(this, CPN_STR_APP_NAME, tr("Update Preferences:\n%1").arg(msgs.join("\n|")));
  }

  AbstractPanel::save();
}

void PrefsUpdatePanel::sectionFolders()
{
  QGridLayout *layFolders = ui->csectFolders->start(tr("Folders"));
  row = col = 0;

  AutoLabel *lblDownloadDir = new AutoLabel(this, tr("Download"));
  layFolders->addWidget(lblDownloadDir, row, col++);

  leDownloadDir = new AutoLineEdit(this, true);
  leDownloadDir->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leDownloadDir->setValue(g.downloadDir(), this);
  leDownloadDir->setEditSignal(true);
  leDownloadDir->setBindSave([this] {
    g.downloadDir(this->leDownloadDir->text());
  });
  layFolders->addWidget(leDownloadDir, row, col++);

  AutoDirectorySelectButton *btnDownloadDir = new AutoDirectorySelectButton(this);
  btnDownloadDir->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnDownloadDir->setup(tr("Select download folder"), g.downloadDir(), leDownloadDir);;
  layFolders->addWidget(btnDownloadDir, row, col++);

  ++row; col = 0;
  AutoLabel *lblDecompressDir = new AutoLabel(this, tr("Decompress"));
  layFolders->addWidget(lblDecompressDir, row, col++);

  chkDecompressDirUseDwnld = new AutoCheckBox(this, tr("create sub-folders in Download folder"));
  chkDecompressDirUseDwnld->setValue(g.decompressDirUseDwnld());
  chkDecompressDirUseDwnld->setBindSave([this] {
    g.decompressDirUseDwnld(this->chkDecompressDirUseDwnld->isChecked());
  });
  layFolders->addWidget(chkDecompressDirUseDwnld, row, col++);

  ++row; col = 1; // skip col 0
  leDecompressDir = new AutoLineEdit(this, true);
  leDecompressDir->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leDecompressDir->setValue(g.decompressDir(), this);
  leDecompressDir->setEditSignal(true);
  leDecompressDir->setBindSave([this] {
    g.decompressDir(this->leDecompressDir->text());
  });
  layFolders->addWidget(leDecompressDir, row, col++);

  AutoDirectorySelectButton *btnDecompressDir = new AutoDirectorySelectButton(this);
  btnDecompressDir->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnDecompressDir->setup(tr("Select decompression folder"), g.decompressDir(), leDecompressDir);;
  layFolders->addWidget(btnDecompressDir, row, col++);

  ++row; col = 0;
  AutoLabel *lblUpdateDir = new AutoLabel(this, tr("Update"));
  layFolders->addWidget(lblUpdateDir, row, col++);

  chkUpdateDirUseSD = new AutoCheckBox(this, tr("use Radio Profile SD structure"));
  chkUpdateDirUseSD->setValue(g.updateDirUseSD());
  chkUpdateDirUseSD->setBindPostChanged([this] {;
    if (this->chkUpdateDirUseSD->isChecked())
      this->leUpdateDir->setValue("");

    update();
  });
  chkUpdateDirUseSD->setBindSave([this] {
    g.updateDirUseSD(this->chkUpdateDirUseSD->isChecked());
  });
  layFolders->addWidget(chkUpdateDirUseSD, row, col++);

  ++row; col = 1; // skip col 0
  leUpdateDir = new AutoLineEdit(this, true);
  leUpdateDir->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  leUpdateDir->setValue(g.updateDir(), this);
  leUpdateDir->setEditSignal(true);
  leUpdateDir->setBindText([this] {
    if (this->chkUpdateDirUseSD->isChecked())
      return "";
    return "";
  });
  leUpdateDir->setBindEnabled([this] { return !this->chkUpdateDirUseSD->isChecked() ;});
  leUpdateDir->setBindSave([this] {
    g.updateDir(this->leDecompressDir->text());
  });
  layFolders->addWidget(leUpdateDir, row, col++);

  AutoDirectorySelectButton *btnUpdateDir = new AutoDirectorySelectButton(this);
  btnUpdateDir->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  btnUpdateDir->setup(tr("Select update folder"), g.updateDir(), leUpdateDir);;
  btnUpdateDir->setBindEnabled([this] { return !this->chkUpdateDirUseSD->isChecked() ;});
  layFolders->addWidget(btnUpdateDir, row, col++);

  ui->csectFolders->finish(-1, -1, [this] { this->shrink(); });
}

void PrefsUpdatePanel::sectionComponents()
{
  QGridLayout *layComponents = ui->csectComponents->start(tr("Components"));
  row = 0; col = 1;  //  leave col 0 blank

  QLabel *lblCheck = new QLabel(tr("Check"));
  layComponents->addWidget(lblCheck, row, col++);

  QLabel *lblReleaseChannel = new QLabel(tr("Release channel"));
  layComponents->addWidget(lblReleaseChannel, row, col++);

  col++;  // options button

  QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layComponents->addItem(spacer, row, col++);

  QMapIterator<QString, int> it(factories->sortedComponentsList());

  while (it.hasNext()) {
    it.next();
    int i = it.value();

    row++; col = 0;

    lblName[i] = new AutoLabel();
    layComponents->addWidget(lblName[i], row, col++);

    chkCheckForUpdate[i] = new AutoCheckBox(this);
    chkCheckForUpdate[i]->setStyleSheet("spacing: 10px"); // workaround Qt 6.9.0 Qt::AlignHCenter causes text to overlap checkbox rhs
    layComponents->addWidget(chkCheckForUpdate[i], row, col++);
    layComponents->setAlignment(chkCheckForUpdate[i], Qt::AlignHCenter);

    cboReleaseChannel[i] = new AutoComboBox(this);
    cboReleaseChannel[i]->addItems(ComponentData::releaseChannelsList());
    layComponents->addWidget(cboReleaseChannel[i], row, col++);

    btnComponentOptions[i] = new AutoPushButton(this, tr("Options"));
    connect(btnComponentOptions[i], &QPushButton::clicked, [=]() {
      UpdateOptionsDialog *dlg = new UpdateOptionsDialog(this, factories->instance(i), i, false);
      dlg->exec();
      dlg->deleteLater();
    });
    layComponents->addWidget(btnComponentOptions[i], row, col++);
  }

  while (it.hasNext()) {
    it.next();
    int i = it.value();

    lblName[i]->setText(it.key());
    chkCheckForUpdate[i]->setChecked(g.component[i].checkForUpdate());
    cboReleaseChannel[i]->setCurrentIndex(g.component[i].releaseChannel());
  }

  ui->csectComponents->finish(row, col, [this] { this->shrink(); });

  while (it.hasNext()) {
    it.next();
    int i = it.value();

    g.component[i].checkForUpdate(chkCheckForUpdate[i]->isChecked());
    g.component[i].releaseChannel((ComponentData::ReleaseChannel)cboReleaseChannel[i]->currentIndex());
  }

}

void PrefsUpdatePanel::sectionOptions()
{
  QGridLayout *layOptions = ui->csectOptions->start(tr("Options"));
  row = col = 0;

  AutoLabel *lblDelDownloads = new AutoLabel(this, tr("Delete downloads"));
  layOptions->addWidget(lblDelDownloads, row, col++);

  chkDelDownloads = new AutoCheckBox(this);
  chkDelDownloads->setValue(g.updDelDownloads());
  chkDelDownloads->setBindEnabled([this] {
    return this->chkDecompressDirUseDwnld->isChecked();
  });
  chkDelDownloads->setBindSave([this] {
    g.updDelDownloads(this->chkDelDownloads->isChecked());
  });
  layOptions->addWidget(chkDelDownloads, row, col++);

  row++; col = 0;
  AutoLabel *lblDelDecompress = new AutoLabel(this, tr("Delete decompressions"));
  layOptions->addWidget(lblDelDecompress, row, col++);

  chkDelDecompress = new AutoCheckBox(this);
  chkDelDecompress->setValue(g.updDelDecompress());
  chkDelDecompress->setBindSave([this] {
    g.updDelDecompress(this->chkDelDecompress->isChecked());
  });
  chkDelDecompress->setBindEnabled([this] {
    return this->chkDecompressDirUseDwnld->isChecked();
  });
  chkDelDecompress->setBindPostChanged([this] {
    if (!this->chkDelDecompress->isChecked()) {
      if (this->chkDecompressDirUseDwnld->isChecked()) {
        this->chkDelDownloads->setEnabled(false);
        this->chkDelDownloads->setValue(false);
      }
    } else {
      this->chkDelDownloads->setEnabled(true);
    }

    update();
  });
  layOptions->addWidget(chkDelDecompress, row, col++);

  row++; col = 0;
  QLabel *lblLogLevel = new QLabel(tr("Log level"), this);
  layOptions->addWidget(lblLogLevel, row, col++);

  cboLogLevel = new AutoComboBox(this);
  cboLogLevel->addItems(AppData::updateLogLevelsList());
  cboLogLevel->setValue(g.updLogLevel(), this);
  cboLogLevel->setBindSave([this] {
    g.updLogLevel(this->cboLogLevel->currentData().toInt());
  });
  layOptions->addWidget(cboLogLevel, row, col++);

  ui->csectOptions->finish(row, col, [this] { this->shrink(); });
}

void PrefsUpdatePanel::sectionPostUpdate()
{
  QGridLayout *layPostUpdate = ui->csectPostUpdate->start(tr("Post Update"));
  row = col = 0;

  chkPrmptFlash = new AutoCheckBox(this, tr("Prompt to flash firmware"));
  chkPrmptFlash->setValue(profile.burnFirmware());
  chkPrmptFlash->setBindSave([this] {
    profile.burnFirmware(this->chkPrmptFlash->isChecked());
  });
  layPostUpdate->addWidget(chkPrmptFlash, row, col++);

  row++; col = 0;
  chkPrmptSDSync = new AutoCheckBox(this, tr("Prompt to run SD Sync"));
  chkPrmptSDSync->setValue(profile.runSDSync());
  chkPrmptSDSync->setBindSave([this] {
    profile.runSDSync(this->chkPrmptSDSync->isChecked());
  });
  layPostUpdate->addWidget(chkPrmptSDSync, row, col++);

  row++; col = 0;
  chkPrmptCpnInstall = new AutoCheckBox(this, tr("Prompt to run Companion installer"));
  chkPrmptCpnInstall->setValue(g.runAppInstaller());
  chkPrmptCpnInstall->setBindSave([this] {
    g.runAppInstaller(this->chkPrmptCpnInstall->isChecked());
  });
  layPostUpdate->addWidget(chkPrmptCpnInstall, row, col++);

  ui->csectPostUpdate->finish(row, col, [this] { this->shrink(); });
}

void PrefsUpdatePanel::update()
{
  AbstractPanel::update();
}

void PrefsUpdatePanel::loadUpdatesTab()
{
  cboCheckFreq->setCurrentIndex(g.updateCheckFreq());
  chkDelDownloads->setChecked(g.updDelDownloads());
  chkDelDecompress->setChecked(g.updDelDecompress());
  leDownloadDir->setText(g.downloadDir());
  //  trigger toggled signal by changing design value and then setting to saved value
  chkDecompressDirUseDwnld->setChecked(!chkDecompressDirUseDwnld->isChecked());
  chkDecompressDirUseDwnld->setChecked(g.decompressDirUseDwnld());

  if (g.currentProfile().sdPath().trimmed().isEmpty())
    chkUpdateDirUseSD->setEnabled(false);
  else
    chkUpdateDirUseSD->setEnabled(true);

  if (g.updateDirUseSD() && g.currentProfile().sdPath().trimmed().isEmpty()) {
    g.updateDirUseSD(false);
    g.updateDirReset();
  }

  if (g.updateDirUseSD()) {
    //  trigger toggled signal by changing design value and then setting to saved value
    chkUpdateDirUseSD->setChecked(!chkUpdateDirUseSD->isChecked());
    chkUpdateDirUseSD->setChecked(g.updateDirUseSD());
  }
  else
    chkUpdateDirUseSD->setChecked(false);

  chkDelDownloads->setChecked(g.updDelDownloads());
  cboLogLevel->setCurrentIndex(g.updLogLevel());

  QMapIterator<QString, int> it(factories->sortedComponentsList());

  while (it.hasNext()) {
    it.next();
    int i = it.value();

    lblName[i]->setText(it.key());
    chkCheckForUpdate[i]->setChecked(g.component[i].checkForUpdate());
    cboReleaseChannel[i]->setCurrentIndex(g.component[i].releaseChannel());
  }
}
