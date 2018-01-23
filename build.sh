cd release
#mkdir log
#mkdir flow
#cp ../src/config.json ./
g++ -o ctp -g ../src/hf_ctp.cpp ../src/quote.cpp ../ctp_20171123/thostmduserapi.so -lnanomsg -lpthread -std=c++11
