all:
	g++ -Wall -fPIC -shared -o tracer.so tracer.cpp -ldl

clean:
	rm -f tracer.so

test: clean all
	echo "hello" | LD_PRELOAD=./tracer.so nc google.com 80

python: clean all
	LD_PRELOAD=./tracer.so python3 -c "import requests; requests.get('https://google.com')"