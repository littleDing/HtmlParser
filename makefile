BOOST = /home/dingweijie/IDE/boost_1_49_0/
INCLUDE = -I$(BOOST) 
CFLAGS = -g -std=c++0x 
TARGETS = libhtmlparser.a sentence_counter link_extractor

output : $(TARGETS)

sentence_counter :	 sentence_counter.cpp libhtmlparser.a
	g++ $(CFLAGS) -o $@ $^
	
link_extractor :	 link_extractor.cpp libhtmlparser.a
	g++ $(CFLAGS) -o $@ $^

libhtmlparser.a : HtmlParser.o
	ar r $@ $^

HtmlParser.o : HtmlParser.cpp HtmlParser.hpp
	g++ $(CFLAGS) -c $(INCLUDE) -o $@ $<

test : link_extractor
	-./link_extractor -b www.baidu.com < input
	

clean :
	-rm *.o
	-rm $(TARGETS)
	
