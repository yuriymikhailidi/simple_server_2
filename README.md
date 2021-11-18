# simple-c-server
Simple C server

Name: Yuriy Mikhailidi.
Programming Assigment 2.

This is a simple server that runs html page. The request is made on local machine to port that the server accepts. The
request is parsed and the uri is treated as root, the uri is checked to make sure the file has correct path. The request
info is stored in struct. The methods use the struct to construct the packet response. The methods are used in Handler 
and implementation files to handle incoming request information and prepare response with header and file information
based of the request struct.

#make all
Make all will create the "serverapp" executable.

#usage

The server application will start with command:
./sererapp <port>

#make clean

Make clean will clean all output files and delete executable.


# simple_server_2
