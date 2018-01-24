# ctp_nanomsg
package ctp api with nanomsg
## CTP Edition
    * 6.3.8 20171123
# setup
## nanomsg
    * git clone from github
    * build and make and install
    * compile app with -lnanomsg
    * error: libnanomsg.so.5 no such file or directory
        * sudo vi /etc/ld.so.conf
        * add line /usr/local/lib64
        * sudo ldconfig
## thread
    * add param -std=c++11 -lpthread to compile
