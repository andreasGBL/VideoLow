#pragma once

#include <QLabel>
class VideoLowWindow;

struct Video;

class QString;
class QMimeData;

namespace acceptedFileTypes {
	QString const MP4("mp4");
	QString const MP3("mp3");
	QString const ALL[2] = { MP4, MP3 };
}

class FileDropWidget : public QLabel
{
	Q_OBJECT
public:
	explicit FileDropWidget(QWidget * parent = nullptr);
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
	void newVideoFileDropped(Video const & video);

protected:
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dragMoveEvent(QDragMoveEvent * event) override;
	void dragLeaveEvent(QDragLeaveEvent * event) override;
	void dropEvent(QDropEvent * event) override;

	bool acceptMimeType(QMimeData const * mimeData);

private:
	bool hasDrop = false;

	QString extractParameter(QString const& input, QString parameter);
	QString extractParameterRegex(QString const& input, QString pattern);
	QString breakLines(QString str, int maxChars);
	QLabel * previewLabel;
};

