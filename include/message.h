#ifndef _MESSAGE_H_
#define _MESSAGE_H_

int message_send(contact *receiver, char *send_data);
void *message_receive(void *data);

#endif //_MESSAGE_H_
