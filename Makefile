build:
	g++ -g src/main.cpp src/quote.cpp src/list-item.cpp src/preformatted.cpp src/format-switch.cpp src/cache.cpp src/link.cpp  src/plaintext.cpp src/site.cpp src/utils.cpp src/gemini-client.cpp src/browser.cpp src/heading.cpp -lssl -lcrypto -lncursesw -o browser.out

install:
	echo "Not implemented"

clean:
	echo "Not implemented"

browser-test:
	g++ tests/browser-test.cpp src/quote.cpp src/list-item.cpp  src/preformatted.cpp src/format-switch.cpp src/heading.cpp src/cache.cpp src/plaintext.cpp src/browser.cpp src/link.cpp src/site.cpp src/utils.cpp src/gemini-client.cpp -lCatch2Main -lCatch2 -lssl -lcrypto -o test.out
	./test.out
	rm test.out

fetch-test:
	g++ tests/fetch-test.cpp src/quote.cpp src/list-item.cpp src/link.cpp src/preformatted.cpp src/format-switch.cpp src/heading.cpp src/cache.cpp src/site.cpp src/plaintext.cpp src/utils.cpp src/gemini-client.cpp -lCatch2Main -lCatch2 -lssl -lcrypto -o test.out
	./test.out
	rm test.out

pure-test:
	g++ tests/pure-test.cpp src/quote.cpp src/list-item.cpp src/link.cpp src/preformatted.cpp src/format-switch.cpp src/heading.cpp src/cache.cpp src/site.cpp src/plaintext.cpp src/utils.cpp src/gemini-client.cpp -lCatch2Main -lCatch2 -lssl -lcrypto -o test.out
	./test.out
	rm test.out


test: pure-test browser-test fetch-test
