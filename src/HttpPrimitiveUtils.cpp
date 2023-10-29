#include "HttpPrimitiveUtils.h"
#include "IOUtils.h"

#define TEST_STATIC_FILE "C:\\Users\\aneur\\Desktop\\AHTTPSERVER\\tests\\render_test\\index.html"




std::unordered_map<std::string, std::string> parseUrlParams(std::string url){
	std::unordered_map<std::string, std::string> ret;
	if(url.find("?")!=std::string::npos){
		std::cout <<"WE need to parse this params...\n"<<url<<"\n";
	}
	return ret;
}






ParserEndpoint parseRequest(Request& request, ParserEndpoint notFoundHandler) {
	ParserEndpoint ret = nullptr;
	std::string url = request.url;
	bool found = false;
	std::unordered_map<std::string, std::string> parameters;  // Create an empty map to store the parameters

    request.urlParams = parseUrlParams(request.url);

	for (const auto& endpoint : registered_endpoint) {
		std::string pattern = endpoint.first;
		std::regex regexPattern(std::regex_replace(pattern, std::regex(":\\w+"), "(\\w+)"));
		std::smatch matches;
		if (std::regex_match(url, matches, regexPattern)) {
			ret = endpoint.second;
			found = true;

			std::regex paramRegex(":\\w+");
			std::sregex_iterator paramIterator(pattern.begin(), pattern.end(), paramRegex);

			for (std::size_t i = 1; i < matches.size(); ++i) {
				std::string paramName = paramIterator->str().substr(1); // Remove the leading ":"
				std::string paramValue = matches[i].str();
				std::cout << "name " << paramName << " Value: " << paramValue << "\n\n";
				parameters[paramName] = paramValue; // Store the parameter name-value pair in the map

				++paramIterator;
			}

			break;
		}
	}
	if (!found) {

		if (!notFoundHandler) {
			notFoundHandler = [](Request*)->Response* {
				auto response = new Response();
			    response->body = "<b>EMPTY REQUEST</b>";
				response->headers["Content-Type"]="text/html";
				return response;
		};
		}

		ret = notFoundHandler;

	}

	// Store the parameters in the Request object
	request.parameters = parameters;


	return ret;
}

std::unordered_map<std::string, std::string> parseRequest(const std::string& request) {
	std::unordered_map<std::string, std::string> requestData;

	std::istringstream iss(request);
	std::string requestLine;

	// Parse the request line
	std::getline(iss, requestLine);

	// Find the position of the first space character
	size_t verbEndPos = requestLine.find(' ');

	if (verbEndPos != std::string::npos) {
		// Extract the verb
		std::string verb = requestLine.substr(0, verbEndPos);

		// Find the position of the second space character
		size_t urlEndPos = requestLine.find(' ', verbEndPos + 1);

		if (urlEndPos != std::string::npos) {
			// Extract the URL
			std::string url = requestLine.substr(verbEndPos + 1, urlEndPos - verbEndPos - 1);

			// Save the verb and URL into the map
			requestData["verb"] = verb;
			requestData["url"] = url;
		}
	}

	return requestData;
}

std::unordered_map<std::string, std::string> parseHeaders(const std::string& request) {
	std::unordered_map<std::string, std::string> headers;

	std::istringstream iss(request);
	std::string line;

	// Skip the first line (request line)
	std::getline(iss, line);

	// Parse headers
	while (std::getline(iss, line) && !line.empty()) {
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string headerName = line.substr(0, colonPos);
			std::string headerValue = line.substr(colonPos + 2); // Skip the colon and space

			// Remove any leading or trailing whitespaces from the header value
			headerValue.erase(0, headerValue.find_first_not_of(" \t"));
			headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

			headers[headerName] = headerValue;
		}
	}

	return headers;
}
// Function to handle an individual client connection
void handleClient(ClientInfo* client) {
	// Receive and process the request
	char buffer[8194] = { 0x00 };
	int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		std::string request(buffer, bytesRead);


		auto headers = parseHeaders(request);
		auto head = parseRequest(request);
		Request thisRequest;
		thisRequest.url = head["url"];
		thisRequest.parameters = headers;
        auto res = parseRequest(thisRequest, nullptr);
		
		std::string stream;


        std::cout <<"Request to Url: "<< head["url"]<<"\n";
//        for(auto it : headers)
//		 std::cout << it.first <<": "<< it.second<<"\n";
  

#ifdef __JUST_TEST
		stream += "Http Verb and URL \n";
		stream += head["verb"] + " \n";
		stream += head["url"] + " \n\n";
		stream += "Http Headers";
		for (const auto& header : headers) {
			stream += header.first + ": " + header.second + "\n";
		}
#else
    //    if(res)
	//	  stream += 
    //    else
	//	  stream += readWholeFile(TEST_STATIC_FILE);
#endif

        auto workedResponse = res(&thisRequest);

		// Process the request and generate a response
		std::string response = "HTTP/1.1 200 OK\r\n";
		response += ("Content-Length: ")+workedResponse->headers["Content-Length"]+"\r\n";
		response += ("Content-Type: "+ workedResponse->headers["Content-Type"]);
		response += "\r\n\r\n";
		response += workedResponse->body;
		//12\r\n\r\nHello World!";

		// Send the response back to the client
		int querysend = send(client->sockfd, response.c_str(), response.length(), 0);

		//std::cout <<"Send: "<< querysend <<"\n" << ;
	}

	// Close the client connection
	closesocket(client->sockfd);
	delete client;
}

