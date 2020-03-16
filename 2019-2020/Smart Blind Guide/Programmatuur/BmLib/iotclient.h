#ifndef IOTCLIENT_H
#define IOTCLIENT_H

#include "Arduino.h"
#include "gprs.h"
#include <vector>

typedef vector<String> StringList;

class IotClient
{
public:

	IotClient();
	~IotClient();

	bool connect( const String host, const String database, const String user, const String password);
	void close();

	// 'post' sends a post to the server.
	// You can build the post via the helper routines.
	// The post answer is reported via 'nextRow'.
	// Each time 'nextRow' gets called the columns of the next row
	// in a set are returned as a StringList.
	// When the end of the rows is reached, an empty list is returned.
	// The field names of the result are reported via 'columnHeaders'.
	bool       post( const String htmlstr);
	StringList nextRow();
	StringList columnHeaders();

    // helper routines

	// There are two versions of 'filloutPost':
	// - the first takes a StringList as value list
	// - the second takes a variable argument list (type String) for it.
	// The routine takes an html post that contains '@@n' tokens.
	// The consequetive tokens are replace by the consequetive values in the list.
	// Thus in the post "POST /api/server/@@d/value=@@v\r\n" @@d will be replaced
	// by the first value in the list and @@v by the second.
	// If a token is used more than once it will reference to the same value.
	// In the post "POST /api/server/@@d=@@v,@@d=@@v\r\n" both @@d tokens are
	// replaced by the first value (since @@d occurs first in it) and both @@v
	// tokens are replaced by the second value (since @@v occurs second).
	// NB! The number of different '@@n' tokens MUST match the number of values!
    static String filloutPost( const String htmlstr, StringList values);
    static String filloutPost( const String htmlstr, ...);

protected:

};

#endif
