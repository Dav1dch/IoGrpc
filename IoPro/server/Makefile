LDFLAGS = -L/usr/local/lib -lcPMML
CXX = g++
all : predict
predict: predict.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean :
	rm -f *.o predict
