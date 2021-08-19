#include <matchEngine.hpp>
using namespace marketdata;

int main()
{
	matchEngine mymatchEngine;
	std::string line;
	std::stringstream ss;
	order_t newOrder;
	std::vector<std::string> vec(6);
	std::string word;

	while(getline(std::cin,line) && !line.empty()) {
		try {
			ss.clear();
			vec.clear();
			newOrder.clear();

			ss << line;
			while(ss >> word)
				vec.push_back(word);

			if(vec.size() == 5 ) {
				newOrder.timestamp  = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count(); 
				newOrder.order_id   = vec[0];
				newOrder.side       = vec[1];
				newOrder.instrument = vec[2];
				newOrder.quantity   = std::stoull(vec[3]);
				newOrder.price      = std::stoull(vec[4]);
				mymatchEngine.pushOrder(newOrder);

			} else {
				std::cerr<<"Invalid Message"<<std::endl;
				continue;
			}

		}
		catch(const std::exception& e) {
			std::cout<<"Caught exception:"<<e.what()<<std::endl;
		}
		catch(...) {
			std::cout<<"Caught Unknown Exception"<<std::endl;
		}
	}

	return 0;
}
