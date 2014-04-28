#include "mainwindow.h"

#define CAMERA_NAME						"USB 2861 Video"
//#define CAMERA_NAME						"iLook 300"
//#define CAMERA_NAME						"ASUS USB2.0 Webcam"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), task(NULL), camera(NULL), image_capture(NULL)
{
	ui.setupUi(this);
	setWindowTitle("");
	Initialize();
	connect(ui.ScreenshotButton, SIGNAL(clicked(bool)), SLOT(TakeScreenshot(void)));
	connect(ui.CreateAction, SIGNAL(triggered()), SLOT(CreateNewTask()));
	ui.CreateAction->setShortcut(tr("Ctrl+N"));
	connect(ui.SaveAction, SIGNAL(triggered()), SLOT(SaveTask()));
	ui.SaveAction->setShortcut(tr("Ctrl+S"));
	ui.ButtonsGroupBox->setAlignment(Qt::AlignHCenter);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::Initialize()
{
	QVideoWidget *wdgt = NULL;
	QString description;
	QString tst;
	foreach(const QByteArray &device_name, QCamera::availableDevices()) {
		description = QCamera::deviceDescription(device_name);
		if (description == CAMERA_NAME) {
			camera = new QCamera(device_name);
			if (camera) {
				camera->setViewfinder((QVideoWidget *)ui.ScreenWidget);
				camera->setCaptureMode(QCamera::CaptureStillImage);
				image_capture = new QCameraImageCapture(camera);
				image_capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
				
				QImageEncoderSettings image_settings;
				image_settings.setCodec("image/jpeg");
				image_settings.setResolution(640, 480);
				image_capture->setEncodingSettings(image_settings);

				camera->start();
			} else {
				return false;
			}
		}
	}
	return true;
}


bool MainWindow::CreateButtons(const Template &t)
{
	if (task) {
		DestroyButtons();
		foreach (const BUTTON button, *t.Buttons()) {
			CmdButton *cmd_button = new CmdButton(button.command, ui.ButtonsGroupBox);
			if (cmd_button) {
				buttons.append(cmd_button);
				cmd_button->setText(button.name);
				ui.ButtonsHLayout->addWidget(cmd_button);
				cmd_button->show();
				connect(cmd_button, SIGNAL(SendCommand(QString)), SLOT(SendCommand(QString)));
			}
		}
	}
	return false;
}


void MainWindow::DestroyButtons(void)
{
	foreach (CmdButton *button, buttons) {
		delete button;
	}
	buttons.clear();
}


bool MainWindow::CreateNewTask(void)
{
	NewTaskDialog dlg;
	if (dlg.exec() == QDialog::Accepted) {
		SetTask(dlg.NewTask());
		return true;
	}
	return false;
}

bool MainWindow::SaveTask( void )
{
	if (task) {
		return task->Save();
	}
	return false;
}

void MainWindow::SetTask(Task *new_task)
{
	//
	// SaveCurrentTask();
	//

	if (new_task) {
		task = new_task;
		setWindowTitle(QString::fromLocal8Bit("������: ") + task->Name());
		ui.TaskTreeWidget->setColumnCount(1);
		ui.TaskTreeWidget->setHeaderLabel(QString::fromLocal8Bit("������"));

		QTreeWidgetItem *task_item = new QTreeWidgetItem(ui.TaskTreeWidget);
		task_item->setText(0, task->Name());
		task_item->setFlags(Qt::ItemIsEnabled);
		task_item->setExpanded(true);

		QTreeWidgetItem *task_description = new QTreeWidgetItem(task_item);
		task_description->setText(0, QString::fromLocal8Bit("��������: ") + task->Description());
		task_description->setFlags(Qt::ItemIsEnabled);

		foreach(const Template &t, *task->Templates()) {
			QTreeWidgetItem *template_item = new QTreeWidgetItem(task_item);
			template_item->setText(0, t.Name() + ", " + t.Description());
			template_item->setFlags(Qt::ItemIsEnabled);
			template_item->setExpanded(true);
			foreach (const SECTION &section, *t.Sections()) {
				QTreeWidgetItem *section_item = new QTreeWidgetItem(template_item);
				section_item->setText(0, section.name);
			}
		}
		CreateButtons((*task->Templates())[0]);
	}
}

void MainWindow::TakeScreenshot(void)
{
	static unsigned int counter = 0;
	if (task) {
		QString picture_name = task->ResDirectory() + "/" + QString::number(counter, 10) + ".jpg";
		camera->searchAndLock();
		image_capture->capture(picture_name);
		camera->unlock();
		counter++;
	}
}

bool MainWindow::SendCommand(QString command)
{
	if (control_unit.IsOpened()) {
		control_unit.SenCommand(command);
		return true;
	}
	return false;
}
