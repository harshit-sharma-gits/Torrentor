//------------------------------------------------------------------------------
//	Copyright (c) 2012, Guido Pola.
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//
//	File Name:		MainWindow.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------


#include "Torrentor.h"

#include <Autolock.h>
#include <Application.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>

#include <StatusBar.h>
#include <StringView.h>
#include <ScrollView.h>

#include <Notification.h>

#include <ListView.h>
#include <ListItem.h>

#include <assert.h>

#include "MainWindow.h"


#include "ToolBar.h"

#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>

#include <ControlLook.h>


#include "TorrentRefFilter.h"

#include <SpaceLayoutItem.h>

#include "IconView.h"
#include "DownloadView.h"
#include "DownloadItem.h"

#include "InfoWindow.h"


#include "PreferencesWindow.h"



enum 
{
	MENU_FILE_OPEN_TORRENT		= 0x400000,
	MENU_FILE_OPEN_TORRENT_URL,
	MENU_EDIT_PREFERENCES,
	MENU_TORRENT_INSPECT,
	MENU_TORRENT_OPEN_DOWNLOAD,
};



class DownloadContainerScrollView : public BScrollView {
public:
	DownloadContainerScrollView(BView* target)
		:	BScrollView("DownloadScrollView", target, 0, false, true, B_NO_BORDER)
	{
	}

protected:
	virtual void DoLayout()
	{
		BScrollView::DoLayout();
		
		//
		// Tweak scroll bar layout to hide part of the frame for better looks.
		//
		BScrollBar* scrollBar = ScrollBar(B_VERTICAL);
		scrollBar->MoveBy(1, 1);
		scrollBar->ResizeBy(0, -B_V_SCROLL_BAR_WIDTH);
		Target()->ResizeBy(1, 0);
		
		//
		// Set the scroll steps
		if (BView* item = Target()->ChildAt(0)) 
		{
			scrollBar->SetSteps(item->MinSize().height + 1, item->MinSize().height + 1);
		}
	}
};


MainWindow::MainWindow(BRect frame) 
	: BWindow(frame, "Torrentor!", B_DOCUMENT_WINDOW, 
			  B_AUTO_UPDATE_SIZE_LIMITS |
			  B_ASYNCHRONOUS_CONTROLS | 
			  B_NOT_ZOOMABLE )
{
	SetPulseRate(1000000);
	
	//
	//
	//
	SetLayout(new BGroupLayout(B_VERTICAL, 0.0));
	
	CreateMenuBar();
	
	
	fDownloadView 	= new DownloadView;
	fDownloadScroll = new DownloadContainerScrollView(fDownloadView);
	
	
//	BStringView* fStatusText = new BStringView("status", "Laralala");
//	fStatusText->SetAlignment(B_ALIGN_LEFT);
//	fStatusText->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
///	fStatusText->SetExplicitMinSize(BSize(150, 12));

	
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 0.0)
		.Add(fMenuBar)
		.Add(fDownloadScroll)
/*
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
			.AddGlue()
			.Add(fRemoveMissingButton)
			.Add(fRemoveFinishedButton)
			.SetInsets(12, 5, 12, 5)
		)*/
	);


	//
	//
	//
	fOpenPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, true, NULL, 
								new TorrentRefFilter);
								
	//
	//
	//
	fPreferencesWindow = new PreferencesWindow();




	//
	CenterOnScreen();
}

void MainWindow::InitWindow()
{
}

//
//	File ->
//			- Open Torrent
//			- Open From URL
//			---------------
//			- Pause all torrents
//			- Resume all torrents
//			---------------
//			Quit
//	---------------------------------
//	Edit ->
//			-
//
//	---------------------------------
//
void MainWindow::CreateMenuBar()
{
	//
	//
	//
	fMenuBar = new BMenuBar(BRect(0, 0, 0, 0), "MainMenuBar");
	
	BMenu* menu = new BMenu("File");
	
	//
	//
	//
	//menu->AddItem(new BMenuItem("New Torrent", NULL));
	menu->AddItem(new BMenuItem("Open Torrent", new BMessage(MENU_FILE_OPEN_TORRENT)));
	menu->AddItem(new BMenuItem("Open From URL", NULL));
	menu->AddItem(new BSeparatorItem);
	menu->AddItem(new BMenuItem("Start All Torrents", NULL));
	menu->AddItem(new BMenuItem("Pause All Torrent", NULL));
	menu->AddItem(new BSeparatorItem);
	menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED)));
	
	//menu->AddItem(new BMenuItem("Open" B_UTF8_ELLIPSIS, new BMessage(MENU_FILE_OPEN), 'O'));
	
	//
	//
	//
	fMenuBar->AddItem(menu);
	
	//
	//
	//
	menu = new BMenu("Edit");
	menu->AddItem(new BMenuItem("Select All", NULL));
	menu->AddItem(new BMenuItem("Deselect All", NULL));
	menu->AddItem(new BSeparatorItem);
	menu->AddItem(new BMenuItem("Preferences", new BMessage(MENU_EDIT_PREFERENCES)));
/*
	//
	//
	//
	BMenuItem* item = new BMenuItem("Preferences", new BMessage(MENU_EDIT_PREFERENCES));
	
	item->SetTarget(be_app);
	
	menu->AddItem(item);
*/
	fMenuBar->AddItem(menu);
	
	
	//
	//
	//
	menu = new BMenu("Torrent");
	menu->AddItem(new BMenuItem("Inspect", new BMessage(MENU_TORRENT_INSPECT)));
	menu->AddItem(new BMenuItem("Open download folder", new BMessage(MENU_TORRENT_OPEN_DOWNLOAD)));
	//menu->AddItem(new BSeparatorItem);
	//menu->AddItem(new BMenuItem("Select All", NULL));
	
	fMenuBar->AddItem(menu);
	
	
	//
	//
	//
	menu = new BMenu("View");
	
	
	fMenuBar->AddItem(menu);
	
	
	//
	//
	//
	menu = new BMenu("Help");
	
	
	fMenuBar->AddItem(menu);
}






void MainWindow::MessageReceived(BMessage* message)
{
	switch( message->what )
	{
	case MENU_FILE_OPEN_TORRENT:
		fOpenPanel->Show();
		break;
	case MENU_EDIT_PREFERENCES:
		OpenPreferencesWindow();
		break;
	case MENU_TORRENT_INSPECT:
		OnTorrentInspect();
		break;
	case MENU_TORRENT_OPEN_DOWNLOAD:
		OpenTorrentDownloadFolder();
		break;
	default:
		BWindow::MessageReceived(message);
		break;
	}
}

bool MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}


void MainWindow::OnTorrentInspect()
{
	
	if( const DownloadItem* item = fDownloadView->ItemSelected() )
	{
		
		//
		// Get the torrent object.
		//	
		const TorrentObject* torrent = item->GetTorrentObject();
	
		//
		// @TODO: check if inspect window is already created.
		//
		InfoWindow* window = new InfoWindow(torrent);
	
		window->Show();
	
	}
}


void MainWindow::OpenPreferencesWindow()
{
//	BAutolock _Lock(fPreferencesWindow);
	
	
	if (fPreferencesWindow->IsHidden())
		fPreferencesWindow->Show();
	else
		fPreferencesWindow->Activate();
}

const char* kTrackerSignature = "application/x-vnd.Be-TRAK";

void MainWindow::OpenTorrentDownloadFolder()
{
/*
	if( const DownloadItem* item = fDownloadView->ItemSelected() )
	{
		const TorrentObject* torrent = item->GetTorrentObject();

			
		entry_ref ref;
		status_t status = get_ref_for_path(torrent->GetDownloadDir(), &ref);
		

		
		if( status == B_OK )
		{
			BMessage message(B_REFS_RECEIVED);
			message.AddRef("refs", &ref);

			BMessenger messenger(kTrackerSignature);
			messenger.SendMessage(&message);
	
		}
	}
*/
}

void MainWindow::Update()
{
//	if( fListView->LockLooper() ) 
//	{
//		fListView->Invalidate();
//		fListView->UnlockLooper();
//	}
}


void MainWindow::AddTorrent(TorrentObject* torrentObject)
{
	torrentObject->Update();
	
	//
	// @TODO: fDownloadView->AddItem(torrentObject);
	//
	fDownloadView->AddItem(torrentObject);
}
