#pragma once

#include "filedropwidget.h"
#include <QDragEnterEvent>
#include <QMimeData>

class FilePreviewWidget : public FileDropWidget
{
	Q_OBJECT
public:
	explicit FilePreviewWidget(QWidget * parent = nullptr)
		: FileDropWidget(parent)
	{
	}
	void setVideoLowWindowPointer(VideoLowWindow const * vlw) = delete;
	void setPreviewLabelPointer(QLabel * preview) = delete;

	void setDropWidgetPointer(FileDropWidget * drop)
	{
		dropWidget = drop;
	}


protected:
	void dragEnterEvent(QDragEnterEvent * event) override
	{
		if (acceptMimeType(event->mimeData())) {
			event->acceptProposedAction();
		}
	}
	void dragMoveEvent(QDragMoveEvent * event) override
	{
		if (acceptMimeType(event->mimeData()))
			event->acceptProposedAction();
	}
	void dragLeaveEvent(QDragLeaveEvent * event) override
	{
		event->accept();
	}
	void dropEvent(QDropEvent * event) override
	{
		if (dropWidget)
			dropWidget->passDropEvent(event);
	};

private:
	FileDropWidget * dropWidget = nullptr;
};

