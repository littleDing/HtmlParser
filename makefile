BOOST = /home/dingweijie/IDE/boost_1_49_0/
INCLUDE = -I$(BOOST) 
CFLAGS = -g -std=c++0x 
TARGETS = libhtmlparser.a sentence_cnt

output : $(TARGETS)

sentence_cnt :	 sentence_cnt.cpp libhtmlparser.a
	g++ $(CFLAGS) -o $@ $^

libhtmlparser.a : HtmlParser.o
	ar r $@ $^

HtmlParser.o : HtmlParser.cpp HtmlParser.hpp
	g++ $(CFLAGS) -c $(INCLUDE) -o $@ $<

test : sentence_cnt
	-./sentence_cnt 

clean :
	-rm *.o
	-rm $(TARGETS)
	
