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
    void    sendPage( QVariant page);
    void    sendNumber( QString field, QVariant data);
    void    sendText( QString field, QVariant data);

private:
    void    send( QString data);
    int     m_fd;
};

#endif // NEXTION_H
