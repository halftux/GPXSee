#include <QFile>
#include <QFileInfo>
#if defined (Q_WS_MAEMO_5)
#include <QSslSocket>
#include <QSslConfiguration>
#endif
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBasicTimer>
#include "config.h"
#include "downloader.h"


#if defined(Q_OS_LINUX) || defined(Q_WS_MAEMO_5)
#define PLATFORM_STR "Linux"
#elif defined(Q_OS_WIN32)
#define PLATFORM_STR "Windows"
#elif defined(Q_OS_MAC)
#define PLATFORM_STR "OS X"
#else
#define PLATFORM_STR "Unknown"
#endif

#define USER_AGENT \
	APP_NAME "/" APP_VERSION " (" PLATFORM_STR "; Qt " QT_VERSION_STR ")"

#define ATTR_REDIRECT QNetworkRequest::RedirectionTargetAttribute
#define ATTR_FILE     QNetworkRequest::User
#define ATTR_ORIGIN   (QNetworkRequest::Attribute)(QNetworkRequest::User + 1)
#define ATTR_LEVEL    (QNetworkRequest::Attribute)(QNetworkRequest::User + 2)

#define MAX_REDIRECT_LEVEL 5
#define TIMEOUT            30 /* s */


Authorization::Authorization(const QString &username, const QString &password)
{
	QString concatenated = username + ":" + password;
	QByteArray data = concatenated.toLocal8Bit().toBase64();
	_header = "Basic " + data;
}

class Downloader::ReplyTimeout : public QObject
{
public:
	static void setTimeout(QNetworkReply *reply, int timeout)
	{
		Q_ASSERT(reply);
		new ReplyTimeout(reply, timeout);
	}

private:
	ReplyTimeout(QNetworkReply *reply, int timeout) : QObject(reply)
	{
		_timer.start(timeout * 1000, this);
	}

	void timerEvent(QTimerEvent *ev)
	{
		if (!_timer.isActive() || ev->timerId() != _timer.timerId())
			return;
		QNetworkReply *reply = static_cast<QNetworkReply*>(parent());
		if (reply->isRunning())
			reply->close();
		_timer.stop();
	}

	QBasicTimer _timer;
};

class Downloader::Redirect
{
public:
	Redirect() : _level(0) {}
	Redirect(const QUrl &origin, int level) :
	  _origin(origin), _level(level) {}

	const QUrl &origin() const {return _origin;}
	int level() const {return _level;}

private:
	QUrl _origin;
	int _level;
};


Downloader::Downloader(QObject *parent) : QObject(parent)
{
	connect(&_manager, SIGNAL(finished(QNetworkReply*)),
	  SLOT(downloadFinished(QNetworkReply*)));
}

bool Downloader::doDownload(const Download &dl,
  const QByteArray &authorization, const Redirect *redirect)
{
	QUrl url(dl.url());

	if (_errorDownloads.contains(url))
		return false;
	if (_currentDownloads.contains(url))
		return false;

	QNetworkRequest request(url);
	request.setAttribute(ATTR_FILE, QVariant(dl.file()));
	if (redirect) {
		request.setAttribute(ATTR_ORIGIN, QVariant(redirect->origin()));
		request.setAttribute(ATTR_LEVEL, QVariant(redirect->level()));
	}
#ifdef Q_WS_MAEMO_5
	//if (url.toString().startsWith("https://"))
	//{
		//qDebug() << QSslConfiguration::supportedCiphers;
		//QSslConfiguration configSsl = request.sslConfiguration();
		QSslConfiguration configSsl = QSslConfiguration::defaultConfiguration();
		//configSsl.setProtocol(QSsl::AnyProtocol);
		configSsl.setProtocol(QSsl::TlsV1);
		QSslConfiguration::setDefaultConfiguration(configSsl);
		request.setSslConfiguration(configSsl);
	//}
#endif
	request.setRawHeader("User-Agent", USER_AGENT);
	if (!authorization.isNull())
		request.setRawHeader("Authorization", authorization);

	QNetworkReply *reply = _manager.get(request);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotsslErrors(QList<QSslError>)));
	if (reply) {
		_currentDownloads.insert(url);
		ReplyTimeout::setTimeout(reply, TIMEOUT);
	} else
		return false;

	return true;
}

void Downloader::slotError(QNetworkReply::NetworkError d)
{
	qDebug() << "conerror: " << d;
}

void Downloader::slotsslErrors(const QList<QSslError> &elist)
{
	qDebug() << "sslerror: " << elist;
}

bool Downloader::saveToDisk(const QString &filename, QIODevice *data)
{
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly)) {
		qWarning("Error writing file: %s: %s\n",
		  qPrintable(filename), qPrintable(file.errorString()));
		return false;
	}

	file.write(data->readAll());
	file.close();

	return true;
}

void Downloader::downloadFinished(QNetworkReply *reply)
{
	QUrl url = reply->request().url();

	if (reply->error()) {
		QUrl origin = reply->request().attribute(ATTR_ORIGIN).toUrl();
		if (origin.isEmpty()) {
			_errorDownloads.insert(url);
			qWarning("Error downloading file: %s: %s\n",
			  url.toEncoded().constData(), qPrintable(reply->errorString()));
		} else {
			_errorDownloads.insert(origin);
			qWarning("Error downloading file: %s -> %s: %s\n",
			  origin.toEncoded().constData(), url.toEncoded().constData(),
			  qPrintable(reply->errorString()));
		}
	} else {
		QUrl location = reply->attribute(ATTR_REDIRECT).toUrl();
		QString filename = reply->request().attribute(ATTR_FILE)
		  .toString();

		if (!location.isEmpty()) {
			QUrl origin = reply->request().attribute(ATTR_ORIGIN).toUrl();
			int level = reply->request().attribute(ATTR_LEVEL).toInt();

			if (location == url) {
				_errorDownloads.insert(url);
				qWarning("Error downloading file: %s: "
				  "redirect loop\n", url.toEncoded().constData());
			} else if (level >= MAX_REDIRECT_LEVEL) {
				_errorDownloads.insert(origin);
				qWarning("Error downloading file: %s: "
				  "redirect level limit reached\n",
				  origin.toEncoded().constData());
			} else {
				Redirect redirect(origin.isEmpty() ? url : origin, level + 1);
				Download dl(location, filename);
				doDownload(dl, reply->request().rawHeader("Authorization"),
				  &redirect);
			}
		} else
			if (!saveToDisk(filename, reply))
				_errorDownloads.insert(url);
	}

	_currentDownloads.remove(url);
	reply->deleteLater();

	if (_currentDownloads.isEmpty())
		emit finished();
}

bool Downloader::get(const QList<Download> &list,
  const Authorization &authorization)
{
	bool finishEmitted = false;

	for (int i = 0; i < list.count(); i++)
		finishEmitted |= doDownload(list.at(i), authorization.header());

	return finishEmitted;
}
