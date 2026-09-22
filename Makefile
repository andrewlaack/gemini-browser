build:
	g++ src/main.cpp src/link.cpp  src/site.cpp src/utils.cpp src/gemini-client.cpp src/browser.cpp -lCatch2Main -lCatch2 -o browser.out

install:
	echo "Not implemented"

clean:
	echo "Not implemented"

browser-test:
	g++ tests/browser-test.cpp src/link.cpp src/browser.cpp src/site.cpp src/utils.cpp src/gemini-client.cpp -lCatch2Main -lCatch2 -o test.out
	./test.out
	rm test.out

fetch-test:
	g++ tests/fetch-test.cpp src/link.cpp  src/site.cpp src/utils.cpp src/gemini-client.cpp -lCatch2Main -lCatch2 -o test.out
	./test.out
	rm test.out

test: browser-test fetch-test
