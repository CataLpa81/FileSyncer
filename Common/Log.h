#include<QString>
#include<qdebug.h>
#include<qdatetime.h>
namespace FileSyncerSystem
{
	class Log
	{
	public:
		static void Info(QString info);
		static void Warning(QString info);
		static void Error(QString info);
	private:
		static QString GetCurTime();
	};
}