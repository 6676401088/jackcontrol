/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

// Own includes
#include "About.h"
#include "SocketDialog.h"
#include "mainwidget.h"
#include "patchbay.h"
#include "socketlist.h"
#include "sockettreewidgetitem.h"

// Qt includes
#include <QMessageBox>
#include <QButtonGroup>
#include <QHeaderView>
#include <QRegExp>
#include <QPixmap>
#include <QMenu>

SocketDialog::SocketDialog (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QDialog(pParent, wflags)
{
	// Setup UI struct...
	m_ui.setupUi(this);

	m_pSocketList  = NULL;
	m_bSocketNew   = false;
	m_ppPixmaps    = NULL;
	m_iDirtyCount  = 0;

	// Setup time-display radio-button group.
	m_pSocketTypeButtonGroup = new QButtonGroup(this);
	m_pSocketTypeButtonGroup->addButton(m_ui.AudioRadioButton, 0);
	m_pSocketTypeButtonGroup->addButton(m_ui.MidiRadioButton,  1);
	m_pSocketTypeButtonGroup->addButton(m_ui.AlsaRadioButton,  2);
	m_pSocketTypeButtonGroup->setExclusive(true);

	// Plug list is not sortable.
	//m_ui.PlugListView->setSorting(-1);

	// Plug list view...
	QHeaderView *pHeader = m_ui.PlugListView->header();
//	pHeader->setDefaultAlignment(Qt::AlignLeft);
//	pHeader->setDefaultSectionSize(300);
#if QT_VERSION >= 0x050000
//	pHeader->setSectionResizeMode(QHeaderView::Custom);
	pHeader->setSectionsMovable(false);
#else
//	pHeader->setResizeMode(QHeaderView::Custom);
	pHeader->setMovable(false);
#endif
	pHeader->setStretchLastSection(true);

#ifndef CONFIG_JACK_MIDI
	m_ui.MidiRadioButton->setEnabled(false);
#endif
#ifndef CONFIG_ALSA_SEQ
	m_ui.AlsaRadioButton->setEnabled(false);
#endif

	// UI connections...

	connect(m_ui.PlugAddPushButton,
		SIGNAL(clicked()),
		SLOT(addPlug()));
	connect(m_ui.PlugRemovePushButton,
		SIGNAL(clicked()),
		SLOT(removePlug()));
	connect(m_ui.PlugEditPushButton,
		SIGNAL(clicked()),
		SLOT(editPlug()));
	connect(m_ui.PlugUpPushButton,
		SIGNAL(clicked()),
		SLOT(moveUpPlug()));
	connect(m_ui.PlugDownPushButton,
		SIGNAL(clicked()),
		SLOT(moveDownPlug()));
	connect(m_ui.PlugListView,
		SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(selectedPlug()));

	connect(m_ui.SocketNameLineEdit,
		SIGNAL(textChanged(const QString&)),
		SLOT(changed()));
	connect(m_ui.AudioRadioButton,
		SIGNAL(toggled(bool)),
		SLOT(socketTypeChanged()));
	connect(m_ui.MidiRadioButton,
		SIGNAL(toggled(bool)),
		SLOT(socketTypeChanged()));
	connect(m_ui.AlsaRadioButton,
		SIGNAL(toggled(bool)),
		SLOT(socketTypeChanged()));
	connect(m_ui.ExclusiveCheckBox,
		SIGNAL(toggled(bool)),
		SLOT(socketTypeChanged()));
	connect(m_ui.ClientNameComboBox,
		SIGNAL(editTextChanged(const QString&)),
		SLOT(clientNameChanged()));
	connect(m_ui.PlugNameComboBox,
		SIGNAL(editTextChanged(const QString&)),
		SLOT(changed()));

	connect(m_ui.PlugListView,
		SIGNAL(customContextMenuRequested(const QPoint&)),
		SLOT(customContextMenu(const QPoint&)));
	connect(m_ui.PlugListView->itemDelegate(),
		SIGNAL(commitData(QWidget*)),
		SLOT(changed()));
	connect(m_ui.SocketForwardComboBox,
		SIGNAL(activated(int)),
		SLOT(changed()));

	connect(m_ui.DialogButtonBox,
		SIGNAL(accepted()),
		SLOT(accept()));
	connect(m_ui.DialogButtonBox,
		SIGNAL(rejected()),
		SLOT(reject()));
}

SocketDialog::~SocketDialog ()
{
	delete m_pSocketTypeButtonGroup;
}

void SocketDialog::setSocketCaption ( const QString& sSocketCaption )
{
	m_ui.SocketTabWidget->setTabText(0, sSocketCaption);
	(m_ui.PlugListView->headerItem())->setText(0,
		sSocketCaption + ' ' + tr("Plugs / Ports"));
}

void SocketDialog::setSocketList (SocketList *pSocketList )
{
	m_pSocketList = pSocketList;
}

void SocketDialog::setSocketNew ( bool bSocketNew )
{
	m_bSocketNew = bSocketNew;
}

void SocketDialog::setPixmaps ( QPixmap **ppPixmaps )
{
	m_ppPixmaps = ppPixmaps;
}

void SocketDialog::setConnectCount ( int iConnectCount )
{
//	m_ui.SocketTypeGroupBox->setEnabled(iConnectCount < 1);
	if (iConnectCount) {
		switch (m_pSocketTypeButtonGroup->checkedId()) {
		case 0: // QJACKCTL_SOCKETTYPE_JACK_AUDIO
			m_ui.MidiRadioButton->setEnabled(false);
			m_ui.AlsaRadioButton->setEnabled(false);
			break;
		case 1: // QJACKCTL_SOCKETTYPE_JACK_MIDI
			m_ui.AudioRadioButton->setEnabled(false);
			m_ui.AlsaRadioButton->setEnabled(false);
			break;
		case 2: // QJACKCTL_SOCKETTYPE_ALSA_MIDI
			m_ui.AudioRadioButton->setEnabled(false);
			m_ui.MidiRadioButton->setEnabled(false);
			break;
		}
	}
	m_ui.ExclusiveCheckBox->setEnabled(iConnectCount < 2);

#ifndef CONFIG_JACK_MIDI
	m_ui.MidiRadioButton->setEnabled(false);
#endif
#ifndef CONFIG_ALSA_SEQ
	m_ui.AlsaRadioButton->setEnabled(false);
#endif
}

void SocketDialog::load(qjackctlPatchbaySocket *pSocket)
{
	m_ui.SocketNameLineEdit->setText(pSocket->name());

	QRadioButton *pRadioButton
		= static_cast<QRadioButton *> (
			m_pSocketTypeButtonGroup->button(pSocket->type()));
	if (pRadioButton)
		pRadioButton->setChecked(true);

	m_ui.ClientNameComboBox->setEditText(pSocket->clientName());
	m_ui.ExclusiveCheckBox->setChecked(pSocket->isExclusive());

	m_ui.PlugListView->clear();
	QTreeWidgetItem *pPlugItem = NULL;
	QStringListIterator iter(pSocket->pluglist());
	while (iter.hasNext()) {
		const QString& sPlugName = iter.next();
		pPlugItem = new QTreeWidgetItem(m_ui.PlugListView, pPlugItem);
		if (pPlugItem) {
			pPlugItem->setText(0, sPlugName);
			pPlugItem->setFlags(pPlugItem->flags() | Qt::ItemIsEditable);
		}
	}

	socketTypeChanged();

	int iItemIndex = 0;
	if (!pSocket->forward().isEmpty()) {
		int iItem = m_ui.SocketForwardComboBox->findText(pSocket->forward());
		if (iItem >= 0)
			iItemIndex = iItem;
	}
	m_ui.SocketForwardComboBox->setCurrentIndex(iItemIndex);

	m_iDirtyCount = 0;

	stabilizeForm();
}

void SocketDialog::save (qjackctlPatchbaySocket *pSocket )
{
	pSocket->setName(m_ui.SocketNameLineEdit->text());
	pSocket->setType(m_pSocketTypeButtonGroup->checkedId());
	pSocket->setClientName(m_ui.ClientNameComboBox->currentText());
	pSocket->setExclusive(m_ui.ExclusiveCheckBox->isChecked());

	pSocket->pluglist().clear();
	int iPlugCount = m_ui.PlugListView->topLevelItemCount();
	for (int iPlug = 0; iPlug < iPlugCount; ++iPlug) {
		QTreeWidgetItem *pItem = m_ui.PlugListView->topLevelItem(iPlug);
		pSocket->addPlug(pItem->text(0));
	}

	if (m_ui.SocketForwardComboBox->currentIndex() > 0)
		pSocket->setForward(m_ui.SocketForwardComboBox->currentText());
	else
		pSocket->setForward(QString::null);

	m_iDirtyCount = 0;
}

void SocketDialog::stabilizeForm ()
{
	m_ui.DialogButtonBox->button(
		QDialogButtonBox::Ok)->setEnabled(validateForm());

	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem) {
		int iItem = m_ui.PlugListView->indexOfTopLevelItem(pItem);
		int iItemCount = m_ui.PlugListView->topLevelItemCount();
		m_ui.PlugEditPushButton->setEnabled(true);
		m_ui.PlugRemovePushButton->setEnabled(true);
		m_ui.PlugUpPushButton->setEnabled(iItem > 0);
		m_ui.PlugDownPushButton->setEnabled(iItem < iItemCount - 1);
	} else {
		m_ui.PlugEditPushButton->setEnabled(false);
		m_ui.PlugRemovePushButton->setEnabled(false);
		m_ui.PlugUpPushButton->setEnabled(false);
		m_ui.PlugDownPushButton->setEnabled(false);
	}

	bool bEnabled = !m_ui.PlugNameComboBox->currentText().isEmpty();
	if (bEnabled) {
		bEnabled = (m_ui.PlugListView->findItems(
			m_ui.PlugNameComboBox->currentText(), Qt::MatchExactly).isEmpty());
	}
	m_ui.PlugAddPushButton->setEnabled(bEnabled);
}

bool SocketDialog::validateForm ()
{
	bool bValid = (m_iDirtyCount > 0);

	bValid = bValid && !m_ui.SocketNameLineEdit->text().isEmpty();
	bValid = bValid && !m_ui.ClientNameComboBox->currentText().isEmpty();
	bValid = bValid && (m_ui.PlugListView->topLevelItemCount() > 0);

	return bValid;
}

void SocketDialog::accept ()
{
	if (m_pSocketList == NULL)
		return;

	if (!validateForm())
		return;

	// Check if a socket with the same name already exists...
	if (m_bSocketNew) {
        QListIterator<SocketTreeWidgetItem *> iter(m_pSocketList->sockets());
		while (iter.hasNext()) {
			const QString& sSocketName = iter.next()->socketName();
			if (m_ui.SocketNameLineEdit->text() == sSocketName) {
				QMessageBox::critical(this,
					tr("Error") + " - " QJACKCTL_SUBTITLE1,
					tr("A socket named \"%1\" already exists.")
					.arg(sSocketName), QMessageBox::Cancel);
				// Reject.
				return;
			}
		}
	}

	QDialog::accept();
}

void SocketDialog::reject ()
{
	bool bReject = true;

	// Check if there's any pending changes...
	if (m_iDirtyCount > 0) {
		switch (QMessageBox::warning(this,
			tr("Warning") + " - " QJACKCTL_SUBTITLE1,
			tr("Some settings have been changed.\n\n"
			"Do you want to apply the changes?"),
			QMessageBox::Apply |
			QMessageBox::Discard |
			QMessageBox::Cancel)) {
		case QMessageBox::Apply:
			accept();
			return;
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bReject = false;
		}
	}

	if (bReject)
		QDialog::reject();
}


// Dirty up the current form.
void SocketDialog::changed ()
{
	m_iDirtyCount++;
	stabilizeForm();
}


// Add new Plug to socket list.
void SocketDialog::addPlug ()
{
	if (m_ppPixmaps == NULL)
		return;

	QString sPlugName = m_ui.PlugNameComboBox->currentText();
	if (!sPlugName.isEmpty()) {
		QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
		if (pItem)
			pItem->setSelected(false);
		pItem = new QTreeWidgetItem(m_ui.PlugListView, pItem);
		if (pItem) {
			pItem->setText(0, sPlugName);
			pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
			QPixmap *pXpmPlug = NULL;
			switch (m_pSocketTypeButtonGroup->checkedId()) {
			case 0: // QJACKCTL_SOCKETTYPE_JACK_AUDIO
				pXpmPlug = m_ppPixmaps[QJACKCTL_XPM_AUDIO_PLUG];
				break;
			case 1: // QJACKCTL_SOCKETTYPE_JACK_MIDI
			case 2: // QJACKCTL_SOCKETTYPE_ALSA_MIDI
				pXpmPlug = m_ppPixmaps[QJACKCTL_XPM_MIDI_PLUG];
				break;
			}
			if (pXpmPlug)
				pItem->setIcon(0, QIcon(*pXpmPlug));
			pItem->setSelected(true);
			m_ui.PlugListView->setCurrentItem(pItem);
		}
		m_ui.PlugNameComboBox->setEditText(QString::null);
	}

	clientNameChanged();
}

void SocketDialog::editPlug ()
{
	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem)
		m_ui.PlugListView->editItem(pItem, 0);

	clientNameChanged();
}

void SocketDialog::removePlug ()
{
	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem)
		delete pItem;

	clientNameChanged();
}

void SocketDialog::moveUpPlug ()
{
	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem) {
		int iItem = m_ui.PlugListView->indexOfTopLevelItem(pItem);
		if (iItem > 0) {
			pItem->setSelected(false);
			pItem = m_ui.PlugListView->takeTopLevelItem(iItem);
			m_ui.PlugListView->insertTopLevelItem(iItem - 1, pItem);
			pItem->setSelected(true);
			m_ui.PlugListView->setCurrentItem(pItem);
		}
	}

	changed();
}

void SocketDialog::moveDownPlug ()
{
	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem) {
		int iItem = m_ui.PlugListView->indexOfTopLevelItem(pItem);
		int iItemCount = m_ui.PlugListView->topLevelItemCount();
		if (iItem < iItemCount - 1) {
			pItem->setSelected(false);
			pItem = m_ui.PlugListView->takeTopLevelItem(iItem);
			m_ui.PlugListView->insertTopLevelItem(iItem + 1, pItem);
			pItem->setSelected(true);
			m_ui.PlugListView->setCurrentItem(pItem);
		}
	}

	changed();
}

void SocketDialog::selectedPlug ()
{
	QTreeWidgetItem *pItem = m_ui.PlugListView->currentItem();
	if (pItem)
		m_ui.PlugNameComboBox->setEditText(pItem->text(0));

	stabilizeForm();
}

void SocketDialog::activateAddPlugMenu ( QAction *pAction )
{
	int iIndex = pAction->data().toInt();
	if (iIndex >= 0 && iIndex < m_ui.PlugNameComboBox->count()) {
		m_ui.PlugNameComboBox->setCurrentIndex(iIndex);
		addPlug();
	}
}

void SocketDialog::customContextMenu ( const QPoint& pos )
{
	int iItem = 0;
	int iItemCount = 0;
	QTreeWidgetItem *pItem = m_ui.PlugListView->itemAt(pos);
	if (pItem == NULL)
		pItem = m_ui.PlugListView->currentItem();
	if (pItem) {
		iItem = m_ui.PlugListView->indexOfTopLevelItem(pItem);
		iItemCount = m_ui.PlugListView->topLevelItemCount();
	}

	QMenu menu(this);
	QAction *pAction;

	// Build the add plug sub-menu...
	QMenu *pAddPlugMenu = menu.addMenu(
		QIcon(":/images/add1.png"), tr("Add Plug"));
	int iIndex = 0;
	for (iIndex = 0; iIndex < m_ui.PlugNameComboBox->count(); iIndex++) {
		pAction = pAddPlugMenu->addAction(
			m_ui.PlugNameComboBox->itemText(iIndex));
		pAction->setData(iIndex);
	}
	connect(pAddPlugMenu,
		SIGNAL(triggered(QAction*)),
		SLOT(activateAddPlugMenu(QAction*)));
	pAddPlugMenu->setEnabled(iIndex > 0);
	// Build the plug context menu...
	bool bEnabled = (pItem != NULL);
	pAction = menu.addAction(QIcon(":/images/edit1.png"),
		tr("Edit"), this, SLOT(editPlug()));
	pAction->setEnabled(bEnabled);
	pAction = menu.addAction(QIcon(":/images/remove1.png"),
		tr("Remove"), this, SLOT(removePlug()));
	pAction->setEnabled(bEnabled);
	menu.addSeparator();
	pAction = menu.addAction(QIcon(":/images/up1.png"),
		tr("Move Up"), this, SLOT(moveUpPlug()));
	pAction->setEnabled(bEnabled && iItem > 0);
	pAction = menu.addAction(QIcon(":/images/down1.png"),
		tr("Move Down"), this, SLOT(moveDownPlug()));
	pAction->setEnabled(bEnabled && iItem < iItemCount - 1);

	menu.exec((m_ui.PlugListView->viewport())->mapToGlobal(pos));
}

void SocketDialog::updateJackClients ( int iSocketType )
{
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return;

	const char *pszJackPortType = JACK_DEFAULT_AUDIO_TYPE;
	int iPixmap = QJACKCTL_XPM_AUDIO_CLIENT;
#ifdef CONFIG_JACK_MIDI
	if (iSocketType == QJACKCTL_SOCKETTYPE_JACK_MIDI) {
		pszJackPortType = JACK_DEFAULT_MIDI_TYPE;
		iPixmap = QJACKCTL_XPM_MIDI_CLIENT;
	}
#endif

	bool bReadable = m_pSocketList->isReadable();
	const QIcon icon(*m_ppPixmaps[iPixmap]);

	// Grab all client ports.
	const char **ppszClientPorts = jack_get_ports(
		pJackClient, NULL, pszJackPortType,
		(bReadable ? JackPortIsOutput : JackPortIsInput));
	if (ppszClientPorts) {
		int iClientPort = 0;
		while (ppszClientPorts[iClientPort]) {
			QString sClientPort = QString::fromUtf8(ppszClientPorts[iClientPort]);
			int iColon = sClientPort.indexOf(':');
			if (iColon >= 0) {
				QString sClientName
					= ClientAlias::escapeRegExpDigits(
						sClientPort.left(iColon));
				bool bExists = false;
				for (int i = 0;
					i < m_ui.ClientNameComboBox->count() && !bExists; i++)
					bExists = (sClientName == m_ui.ClientNameComboBox->itemText(i));
				if (!bExists) {
					m_ui.ClientNameComboBox->addItem(icon, sClientName);
				}
			}
			iClientPort++;
		}
		::free(ppszClientPorts);
	}
}

void SocketDialog::updateAlsaClients ( int iSocketType )
{
	if (iSocketType != QJACKCTL_SOCKETTYPE_ALSA_MIDI)
		return;

#ifdef CONFIG_ALSA_SEQ

//    MainWidget *pMainForm = MainWidget::getInstance();
//	if (pMainForm == NULL)
//		return;

//	snd_seq_t *pAlsaSeq = pMainForm->alsaSeq();
//	if (pAlsaSeq == NULL)
//		return;

//	bool bReadable = m_pSocketList->isReadable();
//	const QIcon icon(*m_ppPixmaps[QJACKCTL_XPM_MIDI_CLIENT]);

//	// Readd all subscribers...
//	snd_seq_client_info_t *pClientInfo;
//	snd_seq_port_info_t   *pPortInfo;
//	unsigned int uiAlsaFlags;
//	if (bReadable)
//		uiAlsaFlags = SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ;
//	else
//		uiAlsaFlags = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

//	snd_seq_client_info_alloca(&pClientInfo);
//	snd_seq_port_info_alloca(&pPortInfo);
//	snd_seq_client_info_set_client(pClientInfo, -1);
//	while (snd_seq_query_next_client(pAlsaSeq, pClientInfo) >= 0) {
//		int iAlsaClient = snd_seq_client_info_get_client(pClientInfo);
//		QString sClient
//			= qjackctlClientAlias::escapeRegExpDigits(
//				QString::fromUtf8(snd_seq_client_info_get_name(pClientInfo)));
//		if (iAlsaClient > 0) {
//			bool bExists = false;
//			snd_seq_port_info_set_client(pPortInfo, iAlsaClient);
//			snd_seq_port_info_set_port(pPortInfo, -1);
//			while (!bExists
//				&& snd_seq_query_next_port(pAlsaSeq, pPortInfo) >= 0) {
//				unsigned int uiPortCapability
//					= snd_seq_port_info_get_capability(pPortInfo);
//				if (((uiPortCapability & uiAlsaFlags) == uiAlsaFlags) &&
//					((uiPortCapability & SND_SEQ_PORT_CAP_NO_EXPORT) == 0)) {
//					for (int i = 0;
//						i < m_ui.ClientNameComboBox->count() && !bExists; i++)
//						bExists = (sClient == m_ui.ClientNameComboBox->itemText(i));
//					if (!bExists) {
//						m_ui.ClientNameComboBox->addItem(icon, sClient);
//						bExists = true;
//					}
//				}
//			}
//		}
//	}

#endif	// CONFIG_ALSA_SEQ
}

void SocketDialog::socketTypeChanged ()
{
	if (m_ppPixmaps == NULL)
		return;
	if (m_pSocketList == NULL)
		return;

	QString sOldClientName = m_ui.ClientNameComboBox->currentText();

	m_ui.ClientNameComboBox->clear();

	QPixmap *pXpmSocket = NULL;
	QPixmap *pXpmPlug   = NULL;

	bool bReadable = m_pSocketList->isReadable();
	int iSocketType = m_pSocketTypeButtonGroup->checkedId();
	switch (iSocketType) {
	case 0: // QJACKCTL_SOCKETTYPE_JACK_AUDIO
		if (m_ui.ExclusiveCheckBox->isChecked())
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_AUDIO_SOCKET_X];
		else
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_AUDIO_SOCKET];
		m_ui.SocketTabWidget->setTabIcon(0, QIcon(*pXpmSocket));
		pXpmPlug = m_ppPixmaps[QJACKCTL_XPM_AUDIO_PLUG];
		updateJackClients(QJACKCTL_SOCKETTYPE_JACK_AUDIO);
		break;
	case 1: // QJACKCTL_SOCKETTYPE_JACK_MIDI
		if (m_ui.ExclusiveCheckBox->isChecked())
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET_X];
		else
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET];
		m_ui.SocketTabWidget->setTabIcon(0, QIcon(*pXpmSocket));
		pXpmPlug = m_ppPixmaps[QJACKCTL_XPM_MIDI_PLUG];
		updateJackClients(QJACKCTL_SOCKETTYPE_JACK_MIDI);
		break;
	case 2: // QJACKCTL_SOCKETTYPE_ALSA_MIDI
		if (m_ui.ExclusiveCheckBox->isChecked())
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET_X];
		else
			pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET];
		m_ui.SocketTabWidget->setTabIcon(0, QIcon(*pXpmSocket));
		pXpmPlug = m_ppPixmaps[QJACKCTL_XPM_MIDI_PLUG];
		updateAlsaClients(QJACKCTL_SOCKETTYPE_ALSA_MIDI);
		break;
	}

	m_ui.ClientNameComboBox->setEditText(sOldClientName);
	clientNameChanged();

	if (pXpmPlug) {
		int iItemCount = m_ui.PlugListView->topLevelItemCount();
		for (int iItem = 0; iItem < iItemCount; ++iItem) {
			QTreeWidgetItem *pItem = m_ui.PlugListView->topLevelItem(iItem);
			pItem->setIcon(0, QIcon(*pXpmPlug));
		}
	}

	// Now the socket forward list...
	m_ui.SocketForwardComboBox->clear();
	m_ui.SocketForwardComboBox->addItem(tr("(None)"));
	if (!bReadable) {
        QListIterator<SocketTreeWidgetItem *> iter(m_pSocketList->sockets());
		while (iter.hasNext()) {
            SocketTreeWidgetItem *pSocketItem = iter.next();
			if (pSocketItem->socketType() == iSocketType
				&& pSocketItem->socketName() != m_ui.SocketNameLineEdit->text()) {
				switch (iSocketType) {
				case 0: // QJACKCTL_SOCKETTYPE_JACK_AUDIO
					if (pSocketItem->isExclusive())
						pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_AUDIO_SOCKET_X];
					else
						pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_AUDIO_SOCKET];
					break;
				case 1: // QJACKCTL_SOCKETTYPE_JACK_MIDI
				case 2: // QJACKCTL_SOCKETTYPE_ALSA_MIDI
					if (pSocketItem->isExclusive())
						pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET_X];
					else
						pXpmSocket = m_ppPixmaps[QJACKCTL_XPM_MIDI_SOCKET];
					break;
				}
				m_ui.SocketForwardComboBox->addItem(
					QIcon(*pXpmSocket), pSocketItem->socketName());
			}
		}
	}

	bool bEnabled = (m_ui.SocketForwardComboBox->count() > 1);
	m_ui.SocketForwardTextLabel->setEnabled(bEnabled);
	m_ui.SocketForwardComboBox->setEnabled(bEnabled);
}

void SocketDialog::updateJackPlugs ( int iSocketType )
{
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return;

	const char *pszJackPortType = JACK_DEFAULT_AUDIO_TYPE;
	int iPixmap = QJACKCTL_XPM_AUDIO_PLUG;
#ifdef CONFIG_JACK_MIDI
	if (iSocketType == QJACKCTL_SOCKETTYPE_JACK_MIDI) {
		pszJackPortType = JACK_DEFAULT_MIDI_TYPE;
		iPixmap = QJACKCTL_XPM_MIDI_PLUG;
	}
#endif

	QString sClientName = m_ui.ClientNameComboBox->currentText();
	if (sClientName.isEmpty())
		return;
	QRegExp rxClientName(sClientName);

	bool bReadable = m_pSocketList->isReadable();
	const QIcon icon(*m_ppPixmaps[iPixmap]);
	const char **ppszClientPorts = jack_get_ports(
		pJackClient, NULL, pszJackPortType,
		(bReadable ? JackPortIsOutput : JackPortIsInput));
	if (ppszClientPorts) {
		int iClientPort = 0;
		while (ppszClientPorts[iClientPort]) {
			QString sClientPort = QString::fromUtf8(ppszClientPorts[iClientPort]);
			int iColon = sClientPort.indexOf(':');
			if (iColon >= 0 && rxClientName.exactMatch(sClientPort.left(iColon))) {
				QString sPort
					= ClientAlias::escapeRegExpDigits(
						sClientPort.right(sClientPort.length() - iColon - 1));
				if (m_ui.PlugListView->findItems(sPort, Qt::MatchExactly).isEmpty())
					m_ui.PlugNameComboBox->addItem(icon, sPort);
			}
			iClientPort++;
		}
		::free(ppszClientPorts);
	}
}

void SocketDialog::updateAlsaPlugs ( int iSocketType )
{
	if (iSocketType != QJACKCTL_SOCKETTYPE_ALSA_MIDI)
		return;

#ifdef CONFIG_ALSA_SEQ

//    MainWidget *pMainForm = MainWidget::getInstance();
//	if (pMainForm == NULL)
//		return;

//	snd_seq_t *pAlsaSeq = pMainForm->alsaSeq();
//	if (pAlsaSeq == NULL)
//		return;

//	QString sClientName = m_ui.ClientNameComboBox->currentText();
//	if (sClientName.isEmpty())
//		return;
//	QRegExp rxClientName(sClientName);

//	bool bReadable = m_pSocketList->isReadable();
//	const QIcon icon(*m_ppPixmaps[QJACKCTL_XPM_MIDI_PLUG]);

//	// Fill sequencer plugs...
//	snd_seq_client_info_t *pClientInfo;
//	snd_seq_port_info_t   *pPortInfo;
//	unsigned int uiAlsaFlags;
//	if (bReadable)
//		uiAlsaFlags = SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ;
//	else
//		uiAlsaFlags = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

//	snd_seq_client_info_alloca(&pClientInfo);
//	snd_seq_port_info_alloca(&pPortInfo);
//	snd_seq_client_info_set_client(pClientInfo, -1);
//	while (snd_seq_query_next_client(pAlsaSeq, pClientInfo) >= 0) {
//		int iAlsaClient = snd_seq_client_info_get_client(pClientInfo);
//		QString sClient = QString::fromUtf8(
//			snd_seq_client_info_get_name(pClientInfo));
//		if (iAlsaClient > 0 && rxClientName.exactMatch(sClient)) {
//			snd_seq_port_info_set_client(pPortInfo, iAlsaClient);
//			snd_seq_port_info_set_port(pPortInfo, -1);
//			while (snd_seq_query_next_port(pAlsaSeq, pPortInfo) >= 0) {
//				unsigned int uiPortCapability
//					= snd_seq_port_info_get_capability(pPortInfo);
//				if (((uiPortCapability & uiAlsaFlags) == uiAlsaFlags) &&
//					((uiPortCapability & SND_SEQ_PORT_CAP_NO_EXPORT) == 0)) {
//					QString sPort
//						= qjackctlClientAlias::escapeRegExpDigits(
//							QString::fromUtf8(snd_seq_port_info_get_name(pPortInfo)));
//					if (m_ui.PlugListView->findItems(sPort, Qt::MatchExactly).isEmpty())
//						m_ui.PlugNameComboBox->addItem(icon, sPort);
//				}
//			}
//		}
//	}

#endif	// CONFIG_ALSA_SEQ
}

void SocketDialog::clientNameChanged ()
{
	if (m_ppPixmaps == NULL)
		return;
	if (m_pSocketList == NULL)
		return;

	m_ui.PlugNameComboBox->clear();

	int iSocketType = m_pSocketTypeButtonGroup->checkedId();
	switch (iSocketType) {
	case 0:
		updateJackPlugs(QJACKCTL_SOCKETTYPE_JACK_AUDIO);
		break;
	case 1:
		updateJackPlugs(QJACKCTL_SOCKETTYPE_JACK_MIDI);
		break;
	case 2:
		updateAlsaPlugs(QJACKCTL_SOCKETTYPE_ALSA_MIDI);
		break;
	}

	changed();
}
