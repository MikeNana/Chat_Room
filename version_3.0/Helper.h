#ifndef _Helper_H_
#define _Helper_H_

#include <string>

using std::string;

void handle_error(const string& str);
int socket_and_bind();
ssize_t read_str(int fd, std::string& in_Buffer, bool &zero);
ssize_t write_str(int fd, std::string & out_buffer);
void handle_for_signal();
int set_non_block(int fd);
#endif
