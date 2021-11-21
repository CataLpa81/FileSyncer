#include "Log.h"

using namespace FileSyncerSystem;

void Log::Info(QString info)
{
	qDebug() << "[Info:"
		<< GetCurTime()
		<< "-CataLpa-@ "
		<< info
		<< "]";
		
}

void Log::Warning(QString info)
{
	qDebug() << "[Warning:"
		<< GetCurTime()
		<< "-CataLpa-@ "
		<< info
		<< "]";
}

void Log::Error(QString info)
{
	qDebug() << "[Warning:"
		<< GetCurTime()
		<< "-CataLpa-@ "
		<< info
		<< "]";
}

QString Log::GetCurTime()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
	return current_date;
}
