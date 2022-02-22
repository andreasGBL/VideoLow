#pragma once

#include <QLabel>
#include "structs.h"
#include "videolowwindow.h"


class QMimeData;

namespace acceptedFileTypes {
	QString const MP4("mp4");
	QString const ALL[1] = { MP4 };
}

class FileDropWidget : public QLabel
{
	Q_OBJECT
public:
	explicit FileDropWidget(QWidget * parent = nullptr);
	void setVideoLowWindowPointer(VideoLowWindow const * vlw);
	void setPreviewLabelPointer(QLabel * preview)
	{
		previewLabel = preview;
	}
	void passDropEvent(QDropEvent * event)
	{
		dropEvent(event);
	}
public slots:
	void clear();

signals:
	void newVideoFileDropped(const Video filePath);

protected:
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dragMoveEvent(QDragMoveEvent * event) override;
	void dragLeaveEvent(QDragLeaveEvent * event) override;
	void dropEvent(QDropEvent * event) override;

	bool acceptMimeType(QMimeData const * mimeData);

private:
	bool hasDrop = false;

	QString breakLines(QString str, int maxChars);
	VideoLowWindow const * videoLowWindow;
	QLabel * previewLabel;
	void connectSlots();
};

