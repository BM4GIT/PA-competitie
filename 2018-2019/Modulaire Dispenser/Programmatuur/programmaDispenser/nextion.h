#ifndef NEXTION_H
#define NEXTION_H

#include <QVariant>

class Nextion
{
public:
    Nextion();
    ~Nextion();

    bool connect();

    QVariant  receive();
	// this routine receives a Nextion print instruction
	// it must be:
	// - a text ending by a ~ sign, or
	// - a number prefixed by a # sign

    void    sendPage( QVariant page);
    void    sendNumber( QString field, QVariant data);
    void    sendText( QString field, QVariant data);

private:
    void    send( QString data);
    int     m_fd;
};

#endif // NEXTION_H
