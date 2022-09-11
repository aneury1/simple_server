
#include <memory>
#include "HttpServer.h"
#include "HttpDefaults.h"


#include <map>
#include <iostream>
#include "Router.h"


struct Tree {
	int val;
	int reps;
	Tree* left;
	Tree* right;
};
void initTree(Tree* tree, int val) {
	if (tree)
	{
		tree->val = val;
		tree->reps = 0;
	}
	else {
		tree = new Tree();
		tree->val = val;
		tree->reps = 0;
	}
}


#include <mutex>




int main() {


////std::cout << convertUrlToFormatter("/host/1/hola");
	

	 auto theServer = new HttpServer{9001};
	    theServer
	/*	 ->Get("/favicon.ico", http_send_static_html_file_favicon)
		->Get("/file", http_send_static_file)
		->Get("/mp3", http_send_static_mp3_file)
		->Get("/horse.mp3", http_send_static_mp3_file)
		->Get("/html", http_send_static_html_file) 
		*/
		->Start()
		->Listen();

	return 0;
}
