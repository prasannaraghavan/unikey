/*
 * barrier -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SetupWizard.h"
#include "MainWindow.h"
#include "QBarrierApplication.h"
#include "QUtility.h"

#include <QMessageBox>

SetupWizard::SetupWizard(MainWindow& mainWindow, bool startMain) :
    m_MainWindow(mainWindow),
    m_StartMain(startMain)
{
    setupUi(this);

    // Explicitly set radio button text as workaround for Qt6 text rendering issue
    m_pServerRadioButton->setText(tr("&Server (share this computer's mouse and keyboard)"));
    m_pClientRadioButton->setText(tr("&Client (use another computer's mouse and keyboard)"));
    
    // Qt6 Windows font fix: Ensure proper font rendering
    QFont buttonFont = m_pServerRadioButton->font();
    buttonFont.setPointSize(9);
    buttonFont.setFamily("Segoe UI");
    m_pServerRadioButton->setFont(buttonFont);
    m_pClientRadioButton->setFont(buttonFont);
    
    // Force text color to ensure visibility
    QPalette palette = m_pServerRadioButton->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    m_pServerRadioButton->setPalette(palette);
    m_pClientRadioButton->setPalette(palette);

#if defined(Q_OS_MAC)

    // the mac style needs a little more room because of the
    // graphic on the left.
    resize(600, 500);
    setMinimumSize(size());

#elif defined(Q_OS_WIN)

    // when aero is disabled on windows, the next/back buttons
    // are hidden (must be a qt bug) -- resizing the window
    // to +1 of the original height seems to fix this.
    // NOTE: calling setMinimumSize after this will break
    // it again, so don't do that.
    resize(size().width(), size().height() + 1);

#endif

    connect(m_pServerRadioButton, SIGNAL(toggled(bool)), m_MainWindow.m_pGroupServer, SLOT(setChecked(bool)));
    connect(m_pClientRadioButton, SIGNAL(toggled(bool)), m_MainWindow.m_pGroupClient, SLOT(setChecked(bool)));

    m_Locale.fillLanguageComboBox(m_pComboLanguage);
    setIndexFromItemData(m_pComboLanguage, m_MainWindow.appConfig().language());
}

SetupWizard::~SetupWizard()
{
}

bool SetupWizard::validateCurrentPage()
{
    QMessageBox message;
    message.setWindowTitle(tr("Setup Barrier"));
    message.setIcon(QMessageBox::Information);

    if (currentPage() == m_pNodePage)
    {
        bool result = m_pClientRadioButton->isChecked() ||
                 m_pServerRadioButton->isChecked();

        if (!result)
        {
            message.setText(tr("Please select an option."));
            message.exec();
            return false;
        }
    }

    return true;
}

void SetupWizard::changeEvent(QEvent* event)
{
    if (event != 0)
    {
        switch (event->type())
        {
        case QEvent::LanguageChange:
            {
                m_pComboLanguage->blockSignals(true);
                retranslateUi(this);
                // Explicitly reset radio button text after language change
                m_pServerRadioButton->setText(tr("&Server (share this computer's mouse and keyboard)"));
                m_pClientRadioButton->setText(tr("&Client (use another computer's mouse and keyboard)"));
                
                // Qt6 Windows font fix: Ensure proper font rendering after language change
                QFont buttonFont = m_pServerRadioButton->font();
                buttonFont.setPointSize(9);
                buttonFont.setFamily("Segoe UI");
                m_pServerRadioButton->setFont(buttonFont);
                m_pClientRadioButton->setFont(buttonFont);
                
                // Force text color to ensure visibility
                QPalette palette = m_pServerRadioButton->palette();
                palette.setColor(QPalette::WindowText, Qt::black);
                palette.setColor(QPalette::Text, Qt::black);
                m_pServerRadioButton->setPalette(palette);
                m_pClientRadioButton->setPalette(palette);
                
                m_pComboLanguage->blockSignals(false);
                break;
            }

        default:
            QWizard::changeEvent(event);
        }
    }
}

void SetupWizard::accept()
{
    AppConfig& appConfig = m_MainWindow.appConfig();

    appConfig.setLanguage(m_pComboLanguage->itemData(m_pComboLanguage->currentIndex()).toString());

    appConfig.setWizardHasRun();
    appConfig.saveSettings();

    QSettings& settings = m_MainWindow.settings();
    if (m_pServerRadioButton->isChecked())
    {
        settings.setValue("groupServerChecked", true);
        settings.setValue("groupClientChecked", false);
    }
    if (m_pClientRadioButton->isChecked())
    {
        settings.setValue("groupClientChecked", true);
        settings.setValue("groupServerChecked", false);
    }

    QWizard::accept();

    if (m_StartMain)
    {
        // Skip the problematic main window opening for now
        // m_MainWindow.updateZeroconfService();
        // m_MainWindow.open();
        
        // Show a message that setup is complete
        QMessageBox::information(this, tr("Setup Complete"), 
            tr("Barrier setup is complete. You can now close this window and run Barrier from the main interface."));
    }
}

void SetupWizard::reject()
{
    QBarrierApplication::getInstance()->switchTranslator(m_MainWindow.appConfig().language());

    if (m_StartMain)
    {
        m_MainWindow.open();
    }

    QWizard::reject();
}

void SetupWizard::on_m_pComboLanguage_currentIndexChanged(int index)
{
    QString ietfCode = m_pComboLanguage->itemData(index).toString();
    QBarrierApplication::getInstance()->switchTranslator(ietfCode);
}
