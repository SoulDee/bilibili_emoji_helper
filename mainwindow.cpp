#include "mainwindow.h"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QBuffer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	open_dir(new QPushButton(tr("open folder"))),
	open_images(new QPushButton(tr("add images"))),
	clean_imange(new QPushButton(tr("clean images"))),
	save_to(new QPushButton(tr("save to..."))),
	image_list_widget(new QListWidget),
	working(false),
	image_size(162, 162)
{
	setWindowTitle(tr("Bilibili Emoji Helper"));
	setMinimumSize(QSize(800, 800));

	image_list_widget->setViewMode(QListWidget::IconMode);
	image_list_widget->setIconSize(QSize(100, 100));
	image_list_widget->setSpacing(10);
	image_list_widget->setResizeMode(QListView::Adjust);
	image_list_widget->setMovement(QListView::Static);

	// Layout
	auto central_widget = new QWidget;

	auto button_layout = new QHBoxLayout;
	button_layout->addWidget(open_dir);
	button_layout->addWidget(open_images);
	button_layout->addStretch();
	button_layout->addWidget(clean_imange);
	button_layout->addWidget(save_to);

	auto main_layout = new QVBoxLayout;
	main_layout->addLayout(button_layout);
	main_layout->addWidget(image_list_widget);

	central_widget->setLayout(main_layout);
	setCentralWidget(central_widget);

	connect(open_dir, &QPushButton::clicked, this, &MainWindow::openFolder);
	connect(open_images, &QPushButton::clicked, this, &MainWindow::addImages);
	connect(clean_imange, &QPushButton::clicked, this, &MainWindow::cleanImages);
	connect(save_to, &QPushButton::clicked, this, &MainWindow::saveTo);
}

MainWindow::~MainWindow() = default;

void MainWindow::openFolder()
{
	auto path = QFileDialog::getExistingDirectory(this, "Select Folder", "./");

	if (!path.isEmpty()) {
		QDir root_dir(path);
		QList<QFileInfo> files = root_dir.entryInfoList(QDir::Filter::Files);
		loadImages(files);
	}
}

void MainWindow::addImages()
{
	auto files = QFileDialog::getOpenFileNames(this, "Select Images", "./", "Images (*.png *.jpg *jpeg)");

	if (!files.isEmpty()) {
		QList<QFileInfo> file_info_list;

		for (auto file : files) {
			file_info_list.append(QFileInfo(file));
		}

		loadImages(file_info_list);
	}
}

void MainWindow::cleanImages()
{
	image_list.clear();
	image_list_widget->clear();
}

void MainWindow::saveTo()
{
	auto target = QFileDialog::getExistingDirectory(this, "Save to", "./");

	if (target.isEmpty())
		return;

	working = true;
	updateWorkStatu();

	for (auto file : image_list) {
		QImage image = QImage(file.filePath()).scaled(image_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		QPixmap pixmap(image_size);
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);

		QPoint point(0, 0);

		if (image.width() > image.height()) {
			point.setY((image_size.height() - image.height()) / 2);
		}
		else if(image.width() < image.height()) {
			point.setX((image_size.width() - image.width()) / 2);
		}

		painter.drawImage(point, image);

		pixmap.save(target + "/" + file.baseName() + ".png", "PNG");
	}

	working = false;
	updateWorkStatu();

	QMessageBox::information(this, tr("successful"), tr("save successful."));
}

void MainWindow::loadImages(const QList<QFileInfo> list)
{
	for (const auto& file : list) {
		if (isImage(file) && !image_list.contains(file)) {
			image_list.append(file);

			auto icon = QIcon(file.filePath());
			auto item = new QListWidgetItem;
			item->setIcon(icon);
			item->setText(file.fileName());

			image_list_widget->addItem(item);
		}

	}
}

void MainWindow::updateWorkStatu()
{
	open_dir->setDisabled(working);
	open_images->setDisabled(working);
	clean_imange->setDisabled(working);
	save_to->setDisabled(working);

	if (working) {
		setWindowTitle(tr("Working..."));
	}
	else {
		setWindowTitle(tr("Bilibili Emoji Helper"));
	}
}

bool MainWindow::isImage(const QFileInfo & info)
{
	static QMimeDatabase mime_db;
	QMimeType mime = mime_db.mimeTypeForFile(info);
	return mime.name().startsWith("image/");
}

