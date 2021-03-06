#include <qtextcodec.h>
#include <qmessagebox.h>
#include "os.h"
#include "string.h"
#include "ctdrive.h"


//QCtDrive::QCtDrive(int argc,char **argv )
// : QApplication( argc, argv )//2016.11.18
#ifdef OS_WIN 
QCtDrive::QCtDrive(int argc,char **argv ) 
: QApplication( argc, argv ) 
#else 
QCtDrive::QCtDrive(int argc,char **argv, Type type ) 
: QApplication( argc, argv, type ) 
#endif 
{
	server = NULL ;
	client = NULL ;
	freshTimer = new QTimer(this);
	connect( freshTimer, SIGNAL(timeout()), this, SLOT(freshRun()) );

#ifdef OS_WIN
	setExistFile(argv[0]);
#endif

#ifdef OS_UNIX
	char *index ;
	index = rindex(argv[0],'/');
	QString filename(index);
	setExistFile(filename);
#endif
}


QCtDrive::~QCtDrive()
{

}

void QCtDrive::quitApplication()
{
	quit();
}


void QCtDrive::setExistFile(QString file)
{
#ifdef OS_WIN
	QTextCodec * codec = QTextCodec::codecForName( "GB2312" );					// add to support Chinese Path;
	QString name ,path;
	int index =0 ;
	file = file.replace(0x5c, "/");
	index = file.findRev ("/",-1,false);
	name = file.right(file.length() - index -1);
	name = name.left(name.length() - 4);
	filePath = file.left(index);
	fileName = name;
	protocol.ReadXmlFile( codec->toUnicode(filePath) ,codec->toUnicode(fileName) );
#endif

#ifdef OS_UNIX
	file = file.right(file.length()-1);
	protocol.ReadXmlFile("/home/ctstor/ctfiles" ,file);
#endif
		
    //server = new SimpleServer( 5002+protocol.iRamid ,this );
	server = new SimpleServer( 5002 + protocol.rmtPort ,this );
	connect( server, SIGNAL(newConnect(ClientSocket*)),this ,SLOT(newConnect(ClientSocket*)) );
	
	freshTimer->start(protocol.cycTime,FALSE);
	//freshTimer->start(1*100,FALSE);
}

void QCtDrive::freshRun()
{
	protocol.Run( );
}

void QCtDrive::newConnect( ClientSocket *s )
{
	client = s ;
		
	if(client->state() == QSocket::Idle )
		return ;
	Buf buf;
	buf.type = 0x03 ;
	memcpy(	buf.pBuf , (unsigned char*)&protocol.openinfo ,sizeof(protocol.openinfo) );

	int len = sizeof(buf);
	int ret = client->writeBlock((char *)&buf,len);
}

void QCtDrive::freshDisplay(Buf buf)
{
	if(client == NULL)
		return ;
	if(client->state() == QSocket::Idle )
		return ;

	int len = sizeof(buf);
	int ret = client->writeBlock((char *)&buf,len);
}
