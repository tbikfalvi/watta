#include "preferences.h"
#include "watta.h"

#include <QSettings>

cPreferences::cPreferences()
{
    init();
}

cPreferences::cPreferences( const QString &p_qsAppName )
{
    init();
    setAppName( p_qsAppName );
    load();
}

cPreferences::~cPreferences()
{
}

void cPreferences::init()
{
    m_qsAppName         = "";
    m_qsFileName        = "";
    m_qsVersion         = "";
    m_qsWorkDayEnd      = "";
    m_qsWorkDayLength   = "";

    m_ulWorkDayEndSeconds    = 0;
    m_ulWorkDayLengthSeconds = 0;

}

void cPreferences::setAppName( const QString &p_qsAppName )
{
    m_qsAppName = p_qsAppName;
    m_qsFileName = QString( "./%1.ini" ).arg( p_qsAppName );
}

QString cPreferences::getAppName() const
{
    return m_qsAppName;
}

void cPreferences::setVersion( const QString &p_qsVersion )
{
    m_qsVersion = p_qsVersion;
}

QString cPreferences::getVersion() const
{
    return m_qsVersion;
}

void cPreferences::setLogLevels( const unsigned int p_uiConLevel,
                                 const unsigned int p_uiDBLevel,
                                 const unsigned int p_uiGUILevel,
                                 bool p_boSaveNow )
{
    g_obLogger.setMinSeverityLevels( (cSeverity::teSeverity)p_uiConLevel,
                                     (cSeverity::teSeverity)p_uiDBLevel,
                                     (cSeverity::teSeverity)p_uiGUILevel );

    if( p_boSaveNow )
    {
        QSettings  obPrefFile( m_qsFileName, QSettings::IniFormat );
        obPrefFile.setValue( QString::fromAscii( "LogLevels/ConsoleLogLevel" ), p_uiConLevel );
        obPrefFile.setValue( QString::fromAscii( "LogLevels/DBLogLevel" ), p_uiDBLevel );
        obPrefFile.setValue( QString::fromAscii( "LogLevels/GUILogLevel" ), p_uiGUILevel );
    }
}

void cPreferences::setWorkDayEnd( const QString &p_qsTime )
{
    m_qsWorkDayEnd = p_qsTime;
    m_ulWorkDayEndSeconds = timeStrToSeconds( m_qsWorkDayEnd );
}

QString cPreferences::getWorkDayEnd() const
{
    return m_qsWorkDayEnd;
}

unsigned long cPreferences::getWorkDayEndSeconds() const
{
    return m_ulWorkDayEndSeconds;
}

void cPreferences::setWorkDayLength( const QString &p_qsTime )
{
    m_qsWorkDayLength = p_qsTime;
    m_ulWorkDayLengthSeconds = timeStrToSeconds( m_qsWorkDayLength );
}

QString cPreferences::getWorkDayLength() const
{
    return m_qsWorkDayLength;
}

unsigned long cPreferences::getWorkDayLengthSeconds() const
{
    return m_ulWorkDayLengthSeconds;
}

void cPreferences::getLogLevels( unsigned int *p_poConLevel,
                                 unsigned int *p_poDBLevel,
                                 unsigned int *p_poGUILevel ) const
{
    cSeverity::teSeverity  enConLevel = cSeverity::DEBUG;
    cSeverity::teSeverity  enDBLevel  = cSeverity::DEBUG;
    cSeverity::teSeverity  enGUILevel = cSeverity::DEBUG;
    g_obLogger.getMinSeverityLevels( &enConLevel, &enDBLevel, &enGUILevel );

    if( p_poConLevel ) *p_poConLevel = enConLevel;
    if( p_poDBLevel )  *p_poDBLevel  = enDBLevel;
    if( p_poGUILevel ) *p_poGUILevel = enGUILevel;
}

void cPreferences::setDBAccess( const QString &p_qsHost, const QString &p_qsDB,
                                const QString &p_qsUser, const QString &p_qsPwd )
{
    g_poDB->setHostName( p_qsHost );
    g_poDB->setDatabaseName( p_qsDB );
    g_poDB->setUserName( p_qsUser );
    g_poDB->setPassword( p_qsPwd );
}

void cPreferences::getDBAccess( QString *p_poHost, QString *p_poDB,
                                QString *p_poUser, QString *p_poPwd ) const
{
    if( p_poHost ) *p_poHost = g_poDB->getHostName();
    if( p_poDB )   *p_poDB   = g_poDB->getDatabaseName();
    if( p_poUser ) *p_poUser = g_poDB->getUserName();
    if( p_poPwd )  *p_poPwd  = g_poDB->getPassword();
}

void cPreferences::load()
{
    QSettings obPrefFile( m_qsFileName, QSettings::IniFormat );

    if( obPrefFile.status() != QSettings::NoError )
    {
        g_obLogger << cSeverity::WARNING;
        g_obLogger << "Failed to load preferences from file: " << m_qsFileName.toStdString();
        g_obLogger << cQTLogger::EOM;
    }
    else
    {
        setWorkDayEnd( obPrefFile.value( QString::fromAscii( "WorkDay/EndTime" ), "00:00:00" ).toString() );
        setWorkDayLength( obPrefFile.value( QString::fromAscii( "WorkDay/Length" ), "08:30:00" ).toString() );

        unsigned int uiConsoleLevel = obPrefFile.value( QString::fromAscii( "LogLevels/ConsoleLogLevel" ), cSeverity::ERROR ).toUInt();
        if( (uiConsoleLevel >= cSeverity::MAX) ||
            (uiConsoleLevel <= cSeverity::MIN) )
        {
            uiConsoleLevel = cSeverity::DEBUG;

            g_obLogger << cSeverity::WARNING;
            g_obLogger << "Invalid ConsoleLogLevel in preferences file: " << m_qsFileName.toStdString();
            g_obLogger << cQTLogger::EOM;
        }

        unsigned int uiDBLevel = obPrefFile.value( QString::fromAscii( "LogLevels/DBLogLevel" ), cSeverity::ERROR ).toUInt();
        if( (uiDBLevel >= cSeverity::MAX) &&
            (uiDBLevel <= cSeverity::MIN) )
        {
            uiDBLevel = cSeverity::DEBUG;

            g_obLogger << cSeverity::WARNING;
            g_obLogger << "Invalid DBLogLevel in preferences file: " << m_qsFileName.toStdString();
            g_obLogger << cQTLogger::EOM;
        }

        unsigned int uiGUILevel = obPrefFile.value( QString::fromAscii( "LogLevels/GUILogLevel" ), cSeverity::ERROR ).toUInt();
        if( (uiGUILevel >= cSeverity::MAX) &&
            (uiGUILevel <= cSeverity::MIN) )
        {
            uiGUILevel = cSeverity::DEBUG;

            g_obLogger << cSeverity::WARNING;
            g_obLogger << "Invalid GUILogLevel in preferences file: " << m_qsFileName.toStdString();
            g_obLogger << cQTLogger::EOM;
        }

        setLogLevels( uiConsoleLevel, uiDBLevel, uiGUILevel );
    }
}

void cPreferences::save() const
{
    QSettings  obPrefFile( m_qsFileName, QSettings::IniFormat );

    obPrefFile.setValue( "WorkDay/EndTime", m_qsWorkDayEnd );
    obPrefFile.setValue( "WorkDay/Length", m_qsWorkDayLength );

    unsigned int  uiConLevel, uiDBLevel, uiGUILevel;
    getLogLevels( &uiConLevel, &uiDBLevel, &uiGUILevel );
    obPrefFile.setValue( QString::fromAscii( "LogLevels/ConsoleLogLevel" ), uiConLevel );
    obPrefFile.setValue( QString::fromAscii( "LogLevels/DBLogLevel" ), uiDBLevel );
    obPrefFile.setValue( QString::fromAscii( "LogLevels/GUILogLevel" ), uiGUILevel );
}

long cPreferences::timeStrToSeconds( const QString &p_qsTime )
{
    long loSeconds = 0;

    if( p_qsTime != "" )
    {
        bool boConversionOk = true;
        loSeconds = p_qsTime.section( ':', 2, 2 ).toInt( &boConversionOk, 10 );

        if( boConversionOk ) loSeconds += p_qsTime.section( ':', 1, 1 ).toInt( &boConversionOk, 10 ) * 60;
        else loSeconds = 0;

        if( boConversionOk ) loSeconds += p_qsTime.section( ':', 0, 0 ).toInt( &boConversionOk, 10 ) * 3600;
        else loSeconds = 0;
    }

    return loSeconds;
}

QString cPreferences::secondsToTimeStr( const long p_loSeconds )
{
    QString  qsTime = "";
    unsigned long ulSeconds = p_loSeconds;
    if( p_loSeconds < 0 )
    {
        qsTime = "-";
        ulSeconds = -1 * p_loSeconds;
    }
    unsigned long ulHours = ulSeconds / 3600;
    ulSeconds -= ulHours * 3600;
    unsigned long ulMinutes = ulSeconds / 60;
    ulSeconds -= ulMinutes * 60;

    qsTime += QString( "%1:%2:%3" ).arg( ulHours, 2, 10, QChar( '0' ) ).arg( ulMinutes, 2, 10, QChar( '0' ) ).arg( ulSeconds, 2, 10, QChar( '0' ) );
    return qsTime;
}
