#include "filedropwidget.h"

#include <QFrame>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QRegularExpression>

#include <iostream>
#include <stdio.h>

FileDropWidget::FileDropWidget(QWidget *parent) : QLabel(parent)
{
    setMinimumSize(200, 200);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    clear();
}

void FileDropWidget::setVideoLowWindowPointer(const VideoLowWindow *vlw)
{
    videoLowWindow = vlw;
    connectSlots();
}

void FileDropWidget::connectSlots(){
    QObject::connect(this, &FileDropWidget::newVideoFileDropped, videoLowWindow, &VideoLowWindow::newVideoFile);
}

void FileDropWidget::clear()
{
    if(!hasDrop)
        setText(tr("<drop content>"));
    setBackgroundRole(QPalette::Window);
}

void FileDropWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(acceptMimeType(event->mimeData())){
        event->acceptProposedAction();
        if(!hasDrop)
            setText(tr("<drop content>"));
        setBackgroundRole(QPalette::Highlight);
    }
}

void FileDropWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(acceptMimeType(event->mimeData()))
        event->acceptProposedAction();
}

void FileDropWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}

void FileDropWidget::dropEvent(QDropEvent *event)
{
    setBackgroundRole(QPalette::Window);
    const QMimeData *mimeData = event->mimeData();
    if(acceptMimeType(mimeData)){
        QString path = mimeData->text().split("file:///")[1];
        std::cout<<"New File: "<<path.toStdString()<<std::endl;
        setText(tr(breakLines(path, 40).toStdString().c_str()));
        std::string cmd = "ffprobe.exe \"" + path.toStdString() + "\" 2>&1"; //redirect stderr to stdout
        QString probe("");
        int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
        {
            FILE * f;
            f = _popen(cmd.c_str(), "r");
            char buff[128];
            if(f){
                while(fgets(buff, 128, f)){
                    probe.append(buff);
                }
            }
        }
        std::cout<<"Probe:"<<probe.toStdString()<<std::endl;
        QString pattern("Duration: \\d{2}:\\d{2}:\\d{2}.\\d{2}");
        QRegularExpression regex(pattern);
        
        auto match = regex.match(probe);
        if(match.hasMatch()){
            QString res = match.captured(0);
            std::cout<<res.toStdString()<<std::endl;
            QString splitter("Duration: ");
            QString time = res.split(splitter)[1];
            auto timesplit = time.split(":");
            hours =  timesplit[0].toInt();
            minutes = timesplit[1].toInt();
            seconds = static_cast<int>(timesplit[2].toDouble());
            milliseconds = static_cast<int>((timesplit[2].toDouble() - static_cast<double>(seconds)) * 100.0) * 10;
        }


        Video vid = {path, QTime(hours, minutes, seconds, milliseconds)};
        emit newVideoFileDropped(vid);
        hasDrop = true;
    }
}

bool FileDropWidget::acceptMimeType(QMimeData const *mimeData)
{
    if(mimeData->hasText()){
        QString text = mimeData->text();
        if(text.startsWith("file:///")){
            QString filePath = text.split("file:///")[1];
            for (QString ending : acceptedFileTypes::ALL){
                if(text.endsWith("."+ending))
                    return true;
            }
        }
    }
    return false;
}

QString FileDropWidget::breakLines(QString str, int maxChars)
{
    if(maxChars < 0)
        return str;
    if(str.length() <= maxChars){
        return str;
    }else{
        int length = str.length();
        QString newS = str;
        for(int i = maxChars; i < length ; i+=maxChars){
            newS = newS.insert(i, "\n");
        }
        return newS;
    }
}
