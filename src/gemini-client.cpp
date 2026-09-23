#include <sys/socket.h>
#include <sys/time.h>
#include "../include/gemini-client.hpp"
#include "../include/site.hpp"
#include "../include/utils.hpp"
#include "../include/errors.hpp"
#include <openssl/ssl.h>
#include <string>

Site* GeminiClient::getNetworkedSite(Link link) {

    std::string host = link.getLinkDestination().get_host();
    std::string req  = link.getLinkDestination().to_string() + "\r\n";
    std::string conn = host + ":1965";

    if(link.getLinkDestination().get_port()) {
        conn = host + ":"  + std::to_string(link.getLinkDestination().get_port());
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    BIO* bio = BIO_new_ssl_connect(ctx);

    SSL* ssl;
    BIO_get_ssl(bio, &ssl);
    SSL_set_tlsext_host_name(ssl, host.c_str());
    BIO_set_conn_hostname(bio, conn.c_str());

    if (BIO_do_connect(bio) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return nullptr;
    }

    // timeout, 10 seconds
    int fd = -1;
    if (BIO_get_fd(bio, &fd) >= 0 && fd >= 0) {
        timeval tv{10, 0};
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
    }

    BIO_write(bio, req.data(), (int)req.size());

    std::string response;
    char buf[4096];
    int n;
    while ((n = BIO_read(bio, buf, sizeof buf)) > 0) {
        response.append(buf, n);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);


    int nl = response.find("\n");
    if (nl == -1) {
        return nullptr; 
    }

    std::size_t end = nl;

    if (end > 0 && response[end - 1] == '\r') {
        end -= 1;
    }

    std::string status = response.substr(0, end);
    std::string body   = response.substr(nl + 1);

    return new Site(status, body);
}


Site* GeminiClient::fetchSite(Link link) {

    std::string destination = link.getLinkDestination().to_string();

    // TODO: Actually handle uris
    if(isPrefixed(destination, "gemini://")) {
        try {
            return getNetworkedSite(link);
        } catch (...) {
            auto* unreach = new Site{"", ""};
            unreach->setUnreachable();
            return unreach;
        }
    } else if (isPrefixed(destination, "file://")){ // TODO: This seems wrong; it should probably be fullpath with that prefix.
        destination = destination.substr(7, destination.size() - 7);
        std::string fileStr = "";
        try {
            fileStr = readFileToString(destination);
        } catch (FileReadError e ) {
            return new Site {"51 \r\n", ""};
        }
        // TODO: how should I discern file types?
        return new Site {"20 text/gemini\r\n", fileStr};
    } else if(isPrefixed(destination, "about:")){
        return new Site {"20 text/gemini\r\n", getNewTab()};
    } else {
        throw NotImplemented();
    }

    throw NotImplemented();
}
