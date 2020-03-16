#include "sqlclient.h"
#include <mysql/mysql.h>
#include <stdarg.h>

MYSQL* 	 	msql = NULL;
MYSQL_RES*	mresult = NULL;

SqlClient::SqlClient()
{
	msql = mysql_init( NULL);
}

SqlClient::~SqlClient()
{
	close();
}

bool SqlClient::connect( const String host, const String database, const String user, const String password)
{
	if ( !msql ) return false;
	String h = host;
	String d = database;
	String u = user;
	String p = password;
	MYSQL* m = mysql_real_connect( msql,
			h.c_str(), u.c_str(), p.c_str(), d.c_str(),
			0, NULL, 0);
	return (m == msql);
}

void SqlClient::close()
{
	if ( msql )
		mysql_close( msql);
	msql = NULL;
	mresult = NULL;
}

String SqlClient::error()
{
	return mysql_error( msql);
}

bool SqlClient::query( const String querystr)
{
	mresult = NULL;
	if ( !msql ) return false;
	String qs = querystr;
	if ( mysql_real_query( msql, qs.c_str(), qs.length()) )
		return false;
	mresult = mysql_use_result( msql);
	return true;
}

StringList SqlClient::nextRow()
{
	StringList sl;
	if ( !mresult ) return sl;
	MYSQL_ROW row = mysql_fetch_row( mresult);
	if ( row ) {
		uint fields = mysql_num_fields( mresult);
		for ( uint i = 0; i < fields; i++ )
			sl.push_back( String( row[i]));
	}
	return sl;
}

StringList SqlClient::columnHeaders()
{
	StringList sl;
	if ( !mresult ) return sl;
	MYSQL_FIELD *fields = mysql_fetch_fields( mresult);
	uint cnt = mysql_num_fields( mresult);
	for ( uint i = 0; i < cnt; i++ )
		sl.push_back( String( fields[i].name));
	return sl;
}

String SqlClient::filloutQuery( const String querystr, StringList values)
{
    int ix;
    String repl, ret;
    ret = querystr;
    for ( uint i = 0; i < values.size(); i++ ) {
        if ( (ix = ret.indexOf( "@@")) < 0 )
            break;
		repl = ret.substring( ix, ix + 3);
		ret = ret.replace( repl, values.at( i));
	}
    return ret;
}

String SqlClient::filloutQuery( const String querystr, ...)
{
    int ix, cnt = 0;
    String repl, ret = querystr;

	// count number of @@n tokens
	while ( (ix = ret.indexOf( "@@")) >= 0 ) {
		cnt++;
		repl = ret.substring( ix, ix + 3);
		ret = ret.replace( repl, "-");
	}

	// create StringList from argument list
	// number of tokens MUST match number of arguments
    StringList values;
    va_list vals;
    va_start( vals, cnt);
    for ( int i = 0; i < cnt; i++ )
        values.push_back( va_arg( vals, String));
    va_end( vals);

	return filloutQuery( querystr, values);
}
