#include "include/filedropwidget.h"

#include <include/structs.h>

#include <QFrame>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QRegularExpression>
#include <QDir>


#include <iostream>
#include <cstdio>

FileDropWidget::FileDropWidget(QWidget * parent) : QLabel(parent)
{
	setMinimumSize(200, 200);
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setAlignment(Qt::AlignCenter);
	setAcceptDrops(true);
	clear();
}



void FileDropWidget::clear()
{
	if (!hasDrop)
		setText(tr("<drop content>"));
	setBackgroundRole(QPalette::Window);
}

void FileDropWidget::dragEnterEvent(QDragEnterEvent * event)
{
	if (acceptMimeType(event->mimeData())) {
		event->acceptProposedAction();
		if (!hasDrop)
			setText(tr("<drop content>"));
		setBackgroundRole(QPalette::Highlight);
	}
}

void FileDropWidget::dragMoveEvent(QDragMoveEvent * event)
{
	if (acceptMimeType(event->mimeData()))
		event->acceptProposedAction();
}

void FileDropWidget::dragLeaveEvent(QDragLeaveEvent * event)
{
	clear();
	event->accept();
}

void FileDropWidget::dropEvent(QDropEvent * event)
{
	setBackgroundRole(QPalette::Window);
	const QMimeData * mimeData = event->mimeData();
	if (acceptMimeType(mimeData)) {
		//accept File
		QString path = mimeData->text().split("file:///")[1];
		std::cout << "New File: " << path.toStdString() << std::endl;
		setText(tr(breakLines(path, 40).toStdString().c_str()));

		//Call FFProbe to get file information
		std::string cmd = "ffprobe.exe -hide_banner -show_streams -v error -i \"" + path.toStdString() + "\" 2>&1"; //redirect stderr to stdout
		QString probe("");
		Resolution resolution = {0, 0};
		double framerate = 0.;
		QTime vidLength;
		QTime audioLength(0,0);
		double bitrate = 0., audioBitrate = 0.;
		CodecConfig codec;
		{
			{
				FILE* f;
				f = _popen(cmd.c_str(), "r");
				char buff[128];
				if (f) {
					while (fgets(buff, 128, f)) {
						probe.append(buff);
					}
				}
			}
			probe += "\n";
			std::cout << "Probe:" << probe.toStdString() << std::endl;
			//split streams
			QString videoStream = "";
			QString audioStream = "";
			auto streamSplits = probe.split("[/STREAM]");
			for (auto str : streamSplits) {
				if (str.contains("[STREAM]")) {
					if (str.contains("codec_type=audio")) {
						audioStream = str;
					}
					else if (str.contains("codec_type=video")) {
						videoStream = str;
					}
				}
				if (videoStream.compare("") != 0 && audioStream.compare("") != 0)
					break;
			}

			//extract video data
			if (videoStream != "") {
				const int num_param = 7;
				QString parameters[num_param] = { "codec_name", "width", "height", "profile", "r_frame_rate", "duration", "bit_rate" };
				std::vector<QString> results;
				for (QString const& param : parameters) {
					results.push_back(extractParameter(videoStream, param));
				}

				codec.videoCodec = VIDEO_CODEC::UNSUPPORTED_VIDEO;
				codec.name = VIDEO_CODEC_STRINGS[VIDEO_CODEC::UNSUPPORTED_VIDEO];
				for (int i = 0; i < VIDEO_CODEC::UNSUPPORTED_VIDEO; i++) {
					if (results[0].compare(VIDEO_CODEC_STRINGS[i], Qt::CaseInsensitive) == 0) {
						codec = DefaultCodecs[static_cast<VIDEO_CODEC>(i)][HARDWARE_ACCELERATION::NONE];
					}
				}

				bool ok = true;
				auto w = results[1].toInt(&ok);
				if (ok)
					resolution.width = w;
				auto h = results[2].toInt(&ok);
				if (ok)
					resolution.height = h;
				codec.profiles = { results[3] };
				codec.profile = 0;
				bool ok2 = true;
				double fr1 = results[4].split("/")[0].toDouble(&ok);
				double fr2 = results[4].split("/")[1].toDouble(&ok2);
				if (ok && ok2) {
					framerate = fr1 / fr2;
				}
				auto dur = results[5].toDouble(&ok);
				if (ok)
					vidLength = QTime::fromMSecsSinceStartOfDay((int)(dur * 1000.0));
				auto bit = results[6].toDouble(&ok);
				if (ok)
					bitrate = bit / 1024.0 / 1024.0;

				std::vector<QString> parsed;
				parsed.push_back(QString::number(codec.videoCodec));
				parsed.push_back(QString::number(resolution.width));
				parsed.push_back(QString::number(resolution.height));
				parsed.push_back(codec.profiles[codec.profile]);
				parsed.push_back(QString::number(framerate));
				parsed.push_back(vidLength.toString());
				parsed.push_back(QString::number(bitrate));
				for (int i = 0; i < num_param; i++) {
					std::cout << parameters[i].toStdString() << ": " << parsed[i].toStdString() << "(" << results[i].toStdString() << ")\n";
				}
			}
			else {
				codec.videoCodec = VIDEO_CODEC::UNSUPPORTED_VIDEO;
				codec.name = "Unsupported";
				codec.profiles = { "-" };
				codec.profile = 0;
			}

			//extract audio data
			if (videoStream != "") {
				const int num_param = 3;
				QString parameters[num_param] = { "codec_name", "duration", "bit_rate" };
				std::vector<QString> results;
				for (QString const& param : parameters) {
					results.push_back(extractParameter(audioStream, param));
				}
				codec.audioCodec = AUDIO_CODEC::UNSUPPORTED_AUDIO;	
				for (int i = 0; i < AUDIO_CODEC::UNSUPPORTED_AUDIO; i++) {
					if (results[0].compare(AUDIO_CODEC_STRINGS[i], Qt::CaseInsensitive) == 0) {
						codec.audioCodec = static_cast<AUDIO_CODEC>(i);
					}
				}
				codec.audioCodecName = AUDIO_CODEC_STRINGS[codec.audioCodec];

				bool ok = true;
				auto dur = results[1].toDouble(&ok);
				if (ok)
					audioLength = QTime::fromMSecsSinceStartOfDay((int)(dur * 1000.0));
				auto bit = results[2].toDouble(&ok);
				if (ok)
					audioBitrate = bit / 1000.0 / 1000.0;
				codec.audioBitrateIdx = 0;
				std::vector<QString> parsed;
				parsed.push_back(QString::number(codec.audioCodec));
				parsed.push_back(audioLength.toString());
				parsed.push_back(QString::number(audioBitrate));
				for (int i = 0; i < num_param; i++) {
					std::cout << parameters[i].toStdString() << ": " << parsed[i].toStdString() << "(" << results[i].toStdString() << ")\n";
				}
			}
			else {
				codec.audioCodec = AUDIO_CODEC::UNSUPPORTED_AUDIO;
				codec.audioCodecName = AUDIO_CODEC_STRINGS[AUDIO_CODEC::UNSUPPORTED_AUDIO];
				codec.audioBitrateIdx = 0;
			}
		}
		
		//thumbnail generation
		int msecsMid = vidLength.msecsSinceStartOfDay() / 2;
		QTime thumbnailTime = QTime::fromMSecsSinceStartOfDay(msecsMid > 3000 ? 3000 : msecsMid); //prefer a thumbnail at 3 seconds or at videoLength / 2, if video is shorter
        QString tempFolder("VideoLow.tmp");
        QDir::temp().mkdir(tempFolder); //create temporary folder for thumbnail
        QString thumbnail = QDir::tempPath()+QString("/"+tempFolder+"/thumb.jpg");
		QString format("hh:mm:ss.zzz");

		cmd = "ffmpeg -y -i \"" + path.toStdString() + "\" -ss " + thumbnailTime.toString(format).toStdString() + " -vf \"scale=1000:1000:force_original_aspect_ratio=decrease\" -vframes 1 \"" + thumbnail.toStdString() + "\" 2>&1"; //redirect stderr to stdout
		if (codec.videoCodec != VIDEO_CODEC::UNSUPPORTED_VIDEO) {
			QString thumb;
			{
				FILE* f;
				f = _popen(cmd.c_str(), "r");
				char buff[128];
				if (f) {
					while (fgets(buff, 128, f)) {
						thumb.append(buff);
					}
				}
			}
			//std::cout << cmd << "\n--------\n" << thumb.toStdString() << std::endl;
			QPixmap m(thumbnail);
			auto scaled = m.scaled(previewLabel->width(), previewLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			previewLabel->setText("");
			previewLabel->setPixmap(scaled);
		}
		else {
			QPixmap p;
			previewLabel->setPixmap(p);
			previewLabel->setText("Video codec not supported or no video stream found.");
		}
		
		
		Video vid = { path,  vidLength, resolution, framerate, codec, bitrate, audioBitrate, audioLength}; //TODO: check if video got detected properly
		emit newVideoFileDropped(vid);
		hasDrop = true;
	}
}

bool FileDropWidget::acceptMimeType(QMimeData const * mimeData)
{
	if (mimeData->hasText()) {
		QString text = mimeData->text();
		if (text.startsWith("file:///")) {
			QString filePath = text.split("file:///")[1];
			for (QString ending : acceptedFileTypes::ALL) {
				if (text.endsWith("." + ending))
					return true;
			}
		}
	}
	return false;
}

QString FileDropWidget::extractParameter(QString const& input, QString parameter)
{
	bool contains = input.contains(parameter);
	if (contains)
		return input.split(parameter + "=")[1].split("\n")[0].trimmed();
	else
		return QString("");
}

QString FileDropWidget::extractParameterRegex(QString const& input, QString pattern)
{
	QRegularExpression regex(pattern);

	auto match = regex.match(input);
	if (match.hasMatch()) {
		QString res = match.captured(0);
		return res;
	}
	else
		return QString("");
}

QString FileDropWidget::breakLines(QString str, int maxChars)
{
	if (maxChars < 0)
		return str;
	if (str.length() <= maxChars) {
		return str;
	}
	else {
		int length = str.length();
		QString newS = str;
		for (int i = maxChars; i < length; i += maxChars) {
			newS = newS.insert(i, "\n");
		}
		return newS;
	}
}
