#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QFileInfo>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected:
	void openFolder();
	void addImages();
	void cleanImages();
	void saveTo();

	void loadImages(const QList<QFileInfo> list);

	void updateWorkStatu();
	bool isImage(const QFileInfo& info);

private:
	QPushButton* open_dir;
	QPushButton* open_images;
	QPushButton* clean_imange;
	QPushButton* save_to;
	QListWidget* image_list_widget;

	QList<QFileInfo> image_list;
	bool working;
	QSize image_size;
};
#endif // MAINWINDOW_H
