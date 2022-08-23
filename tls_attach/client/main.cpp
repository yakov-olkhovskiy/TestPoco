#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Net/Context.h"
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPSession.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <iostream>


using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;


bool doRequest(Poco::Net::HTTPSClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    std::cout << "SUCCESS 1" << std::endl;
    session.sendRequest(request);
    std::cout << "SUCCESS 2" << std::endl;
    std::istream& rs = session.receiveResponse(response);
    std::cout << response.getStatus() << " " << response.getReason() << std::endl;
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        StreamCopier::copyStream(rs, std::cout);
        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool doRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    request.write(std::cout);
    std::cout << "SUCCESS 1.1" << std::endl;
    session.sendRequest(request);
    std::cout << "SUCCESS 2.1" << std::endl;
    std::istream& rs = session.receiveResponse(response);
    std::cout << "SUCCESS 3.1" << std::endl;
    std::cout << response.getStatus() << " " << response.getReason() << std::endl;
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        StreamCopier::copyStream(rs, std::cout);
        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        StreamCopier::copyStream(rs, null);
        return false;
    }
}


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Path p(argv[0]);
        std::cout << "usage: " << p.getBaseName() << " <uri>" << std::endl;
        std::cout << "       fetches the resource identified by <uri> and print it to the standard output" << std::endl;
        return 1;
    }

    try
    {
        URI uri(argv[1]);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";

        Poco::Net::initializeSSL();

        Poco::Net::Context::Ptr context ( new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "/etc/ssl/certs") );
        
/*
        Poco::Net::SecureStreamSocket socket(context);
        socket.setPeerHostName(uri.getHost());
        HTTPClientSession session(socket);
        session.setHost(uri.getHost());
        session.setPort(uri.getPort());
      //  session.socket().connect({uri.getHost(), uri.getPort()});

        HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        HTTPResponse response;
        doRequest(session, request, response);
*/

/*
{
        HTTPClientSession session(uri.getHost());//, uri.getPort());
        session.socket().connect({uri.getHost(), uri.getPort()});

        HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        HTTPResponse response;
        doRequest(session, request, response);
return 0;
}
*/

        auto get_sec = [](const URI & uri, Poco::Net::Context::Ptr context) {
            Poco::Net::StreamSocket socket({uri.getHost(), uri.getPort()});
            return Poco::Net::SecureStreamSocket::attach(socket, uri.getHost(), context);
        };

        Poco::Net::StreamSocket socket({uri.getHost(), uri.getPort()});
        Poco::Net::SecureStreamSocket sec_socket = get_sec(uri, context);//Poco::Net::SecureStreamSocket::attach(socket, uri.getHost(), context);
        HTTPClientSession session(sec_socket);

//        HTTPClientSession session(uri.getHost(), uri.getPort());
  //      session.socket().connect({uri.getHost(), uri.getPort()});
   //     Poco::Net::SecureStreamSocket socket = Poco::Net::SecureStreamSocket::attach(session.socket(), uri.getHost(), context);

/*
        if (!socket.completeHandshake())
        {
            std::cerr << "ERROR 1" << std::endl;
            return 1;
        } else
            std::cout << "SUCCESS 1" << std::endl;
*/
        HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        HTTPResponse response;
        doRequest(session, request, response);

/*
        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        std::cout << "SUCCESS 0" << std::endl;
        HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        HTTPResponse response;
        doRequest(session, request, response);
*/

/*        if (!doRequest(session, request, response))
        {
     //       credentials.authenticate(request, response);
            if (!doRequest(session, request, response))
            {
                std::cerr << "Invalid username or password" << std::endl;
                return 1;
            }
        }
*/
    }
    catch (Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
        return 1;
    }
    return 0;
}
