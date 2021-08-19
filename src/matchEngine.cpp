#include <matchEngine.hpp>
using namespace marketdata;

matchEngine::matchEngine() {
	thread_ = std::thread{ &matchEngine::processOrder, this };
}

matchEngine::~matchEngine() {
	{
		std::unique_lock<std::mutex> lock(mutex_);
		exit_ = true;
		condVar_.notify_all();
	}
	thread_.join();
}

void matchEngine::pushOrder(const order_t& newOrder) {
	std::lock_guard<std::mutex>  lock(mutex_);
	orderQueue_.push(newOrder);
	condVar_.notify_all();
}

void matchEngine::processOrder() {
	std::unique_lock<std::mutex>  lock(mutex_);
	while(true) {
		if(!exit_)
			condVar_.wait(lock);
		lock.unlock();
		while(true) {
			lock.lock();
			if(orderQueue_.empty())
				break;
			else {
				auto newOrder = orderQueue_.front();
				orderQueue_.pop();
				lock.unlock();

				if(!matchOrder(newOrder))
					addOrder(newOrder);
			}
		}
		if(exit_)
			break;
	}
	reportOpenOrders();
}

void matchEngine::addOrder(const order_t& addOrder) {
	if(!addOrder.side.compare(BUY)) {
		addBuyOrder(addOrder);
	}
	else if(!addOrder.side.compare(SELL)) {
		addSellOrder(addOrder);
	}
}

void matchEngine::addBuyOrder(const order_t& addOrder) {
	auto iter1 = bidBook_.find(addOrder.instrument);
	if(iter1 != bidBook_.end()) {
		auto iter2 = iter1->second.find(addOrder.price);
		if( iter2 != iter1->second.end()) {
			iter2->second.emplace_back(addOrder);

		}
		else {
			std::list<order_t> tempList;
			tempList.emplace_back(addOrder);
			iter1->second.emplace(addOrder.price,tempList);
		}
	}
	else {
		std::list<order_t> tempList;
		tempList.emplace_back(addOrder);
		bookvalue_t tempMap;
		tempMap.emplace(addOrder.price,tempList);
		bidBook_.emplace(addOrder.instrument,tempMap);

	}
	bidSimpleBook_.emplace(addOrder.order_id,addOrder);
}

void matchEngine::addSellOrder(const order_t& addOrder) {
	auto iter1 = askBook_.find(addOrder.instrument);
	if(iter1 != askBook_.end()) {
		auto iter2 = iter1->second.find(addOrder.price);
		if( iter2 != iter1->second.end()) {
			iter2->second.emplace_back(addOrder);

		}
		else {
			std::list<order_t> tempList;
			tempList.emplace_back(addOrder);
			iter1->second.emplace(addOrder.price,tempList);
		}
	}
	else {
		std::list<order_t> tempList;
		tempList.emplace_back(addOrder);
		bookvalue_t tempMap;
		tempMap.emplace(addOrder.price,tempList);
		askBook_.emplace(addOrder.instrument,tempMap);

	}
	askSimpleBook_.emplace(addOrder.order_id,addOrder);
}

bool matchEngine::matchOrder(const order_t& newOrder) {
	if(!newOrder.side.compare(BUY)) {
		return buyMatchOrder(newOrder);
	}
	else if(!newOrder.side.compare(SELL)) {
		return sellMatchOrder(newOrder);
	}
	return false;
}

bool matchEngine::sellMatchOrder(const order_t& newOrder) {
	auto iter1 = bidBook_.find(newOrder.instrument);
	if( iter1 == bidBook_.end())
		return false;
	auto tempQuantity = newOrder.quantity;
	bool slice = false;
	for(auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2 ) {
		if(iter2->first >= newOrder.price) {
			for(auto iter3 = iter2->second.begin();iter3 != iter2->second.end() ; ++iter3 ) {
				if(tempQuantity > iter3->quantity ) {
					auto temp = iter3->quantity;
					tempQuantity = tempQuantity - iter3->quantity;
					iter3->quantity = 0;
					slice = true;
					bidSimpleBook_.erase(iter3->order_id);
					std::cout<<"TRADE "<< newOrder.instrument << " " << newOrder.order_id << " " 
						<<  iter3->order_id << " " << temp << " " << iter3->price << std::endl;
				} else if(tempQuantity == iter3->quantity ) {
					auto temp = tempQuantity;
					tempQuantity = iter3->quantity = 0;
					bidSimpleBook_.erase(iter3->order_id);
					std::cout<<"TRADE "<< newOrder.instrument << " " << newOrder.order_id << " " 
						<<  iter3->order_id << " " << temp << " " << iter3->price << std::endl;
					return true;
				} else if(tempQuantity < iter3->quantity ) {
					auto temp = tempQuantity;
					iter3->quantity = iter3->quantity - tempQuantity;
					tempQuantity = 0;

					auto iter4 = bidSimpleBook_.find(iter3->order_id);
					if(iter4 != bidSimpleBook_.end()) {
						iter4->second = *iter3;
					}

					std::cout<<"TRADE "<< newOrder.instrument << " " << newOrder.order_id << " " 
						<<  iter3->order_id << " " << temp << " " << iter3->price << std::endl;

					return true;
				}
			}
		}
	}
	if(slice)
		return true;
	return false;
}

bool matchEngine::buyMatchOrder(const order_t& newOrder) {
	auto iter1 = askBook_.find(newOrder.instrument);
	if( iter1 == askBook_.end())
		return false;
	auto tempQuantity = newOrder.quantity;
	bool slice = false;
	for(auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2 ) {
		if(iter2->first <= newOrder.price) {
			for(auto iter3 = iter2->second.begin();iter3 != iter2->second.end() ; ++iter3 ) {
				if(tempQuantity > iter3->quantity ) {
					auto temp = iter3->quantity;
					tempQuantity = tempQuantity - iter3->quantity;
					iter3->quantity = 0;
					slice = true;
					askSimpleBook_.erase(iter3->order_id);
					std::cout<<"TRADE "<< newOrder.instrument << " " << iter3->order_id << " " 
						<< newOrder.order_id << " " << " " << temp << " " << iter3->price << std::endl;
				} else if(tempQuantity == iter3->quantity ) {
					auto temp = tempQuantity;
					tempQuantity = iter3->quantity = 0;
					askSimpleBook_.erase(iter3->order_id);
					std::cout<<"TRADE "<< newOrder.instrument << " " << iter3->order_id << " " 
						<< newOrder.order_id << " " << " " << temp << " " << iter3->price << std::endl;
					return true;

				} else if(tempQuantity < iter3->quantity ) {
					auto temp = tempQuantity;
					iter3->quantity = iter3->quantity - tempQuantity;
					tempQuantity = 0;

					auto iter4 = askSimpleBook_.find(iter3->order_id);
					if(iter4 != askSimpleBook_.end()) {
						iter4->second = *iter3;
					}

					std::cout<<"TRADE "<< newOrder.instrument << " " << iter3->order_id << " " 
						<< newOrder.order_id << " " << " " << temp << " " << iter3->price << std::endl;
					return true;
				}
			}
		}
	}
	if(slice)
		return true;
	return false;
}

void matchEngine::reportOpenOrders() const {
	std::multimap<uint64_t,order_t> temp;
	for(const auto& x:askSimpleBook_) {
		temp.emplace(x.second.timestamp,x.second);
	}

	for(const auto& x:temp) {
		std::cout<< "\n" << x.second.order_id<< " " << x.second.side << " " << x.second.instrument << " " << x.second.quantity << " " << x.second.price;
	}

	temp.clear();
	for(const auto& x:bidSimpleBook_) {
		temp.emplace(x.second.timestamp,x.second);
	}

	for(const auto& x:temp) {
		std::cout<< "\n" << x.second.order_id<< " " << x.second.side << " " << x.second.instrument << " " << x.second.quantity << " " << x.second.price;
	}
	std::cout <<"\n\n";
}
