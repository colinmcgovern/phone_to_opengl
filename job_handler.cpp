#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

//g++ job_handler.cpp -pthread -lboost_thread -o server

using boost::asio::ip::tcp;
using namespace std;
using std::vector;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
	public: 
		typedef boost::shared_ptr<tcp_connection> pointer;

		static pointer create(boost::asio::io_context& io_context){
			cout << "tcp_connection::pointer()" << endl; //del
			return pointer(new tcp_connection(io_context));
		}

		tcp::socket& socket(){
			cout << "socket()" << endl; //del
			return socket_;
		}

		void start(){
			cout << "start() started" << endl; //del

			cout << "Sending info to " <<
			 socket_.remote_endpoint().address().to_string() << ":" << 
			 socket_.remote_endpoint().port() <<
			endl;

			client_t new_client = {
				socket_.remote_endpoint().address().to_string(),
				socket_.remote_endpoint().port() 
			};

			clients.push_back(new_client);

			for(;;){
				cout << "loop start" << endl; //del

				boost::array<char,128> buf;
				boost::system::error_code error;
				

				if(error == boost::asio::error::eof){
					cout << 2 << endl;
					break;
				}else if(error){
					cout << 3 << endl;
					throw boost::system::system_error(error);
				}

				cout << "!!!!!!!!!!" << endl;
				cout << len << endl;
				cout << buf.data() << endl;
				cout << "!!!!!!!!!!" << endl;

				string message = "test";
				boost::asio::async_write(socket_, boost::asio::buffer(message),
				boost::bind(&tcp_connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

				cout << "loop end" << endl; //del
			}

			
			cout << "start() ended" << endl; //del
		}

	private:
		tcp_connection(boost::asio::io_context& io_context)
		: socket_(io_context)
		{
			cout << "tcp_connection()" << endl; //del
		}

		void handle_write(const boost::system::error_code& /*error*/,
		  size_t /*bytes_transferred*/)
		{
			cout << "handle_write()" << endl; //del
		}

		tcp::socket socket_;
};


class tcp_server{
	public:
		tcp_server(boost::asio::io_context& io_context) : io_context_(io_context), 
		acceptor_(io_context, tcp::endpoint(tcp::v4(), 15)){
			cout << "tcp_server() started" << endl; //del
			start_accept();
			cout << "tcp_server() ended" << endl; //del
		}

	private:
		void start_accept(){

			cout << "start_accept() started" << endl; //del
			tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

			acceptor_.async_accept(new_connection->socket(),
				boost::bind(&tcp_server::handle_accept, this, new_connection,
					boost::asio::placeholders::error));

			cout << "start_accept() ended" << endl; //del
		}

		void handle_accept(tcp_connection::pointer new_connection,
			const boost::system::error_code& error){

			cout << "handle_accept() started" << endl; //del

			if(!error){
				new_connection->start();
			}

			start_accept();

			cout << "handle_accept() ended" << endl; //del
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;
};


int main(){

	cout << "main() started" << endl; //del

	for(;;){
		
		try{
			boost::asio::io_context io_context;
			tcp_server server(io_context);
			io_context.run();
		}catch(std::exception& e){
			std::cerr << e.what() << std::endl;
		}
	}

	cout << "main() ended" << endl; //del

	return 0;
}