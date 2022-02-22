#pragma once

#include <QVideoWidget>

class QMimeData;

class VideoPlayerWidget : public QVideoWidget
{
	Q_OBJECT
public:
	VideoPlayerWidget(QWidget * parent = nullptr)
		: QVideoWidget(parent)
	{
		QPalette p = palette();
		p.setColor(QPalette::Window, Qt::black);
		setPalette(p);
	}
};
